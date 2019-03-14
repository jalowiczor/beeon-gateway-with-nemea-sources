#include <unistd.h>

#include <cppunit/extensions/HelperMacros.h>

#include <Poco/Event.h>
#include <Poco/RunnableAdapter.h>
#include <Poco/Thread.h>
#include <Poco/Timestamp.h>

#include "cppunit/BetterAssert.h"
#include "io/FdStream.h"

using namespace std;
using namespace Poco;

namespace BeeeOn {

class FdStreamTest : public CppUnit::TestFixture {
	CPPUNIT_TEST_SUITE(FdStreamTest);
	CPPUNIT_TEST(testNonBlockingRead);
	CPPUNIT_TEST(testBlockingRead);
	CPPUNIT_TEST(testPollPipe);
	CPPUNIT_TEST_SUITE_END();
public:
	void testNonBlockingRead();
	void testBlockingRead();
	void testPollPipe();
};

CPPUNIT_TEST_SUITE_REGISTRATION(FdStreamTest);

/**
 * Create pipe descriptors and transfer some data
 * through the pipe. The non-blocking read would
 * report eof() (and sometimes fail()) when no data
 * is immediately available.
 */
void FdStreamTest::testNonBlockingRead()
{
	int pipefd[2];
	CPPUNIT_ASSERT(::pipe(pipefd) >= 0);

	FdInputStream in(pipefd[0]);
	FdOutputStream out(pipefd[1]);

	in.setBlocking(false);

	CPPUNIT_ASSERT(!in.eof());
	CPPUNIT_ASSERT(!in.fail());

	string buffer;

	in >> buffer;
	CPPUNIT_ASSERT(buffer.empty());
	CPPUNIT_ASSERT(!in.bad());
	CPPUNIT_ASSERT(in.eof());
	// immediatelly eof -> fail
	CPPUNIT_ASSERT(in.fail());

	in.clear();

	out << "12345678" << flush;

	in >> buffer;
	CPPUNIT_ASSERT_EQUAL(8, buffer.size());
	CPPUNIT_ASSERT(in.eof());
	// read successful but with eof -> no fail
	CPPUNIT_ASSERT(!in.fail());

	CPPUNIT_ASSERT_EQUAL("12345678", buffer);
}

class AsyncWriter {
public:
	AsyncWriter(FdOutputStream &output):
		m_output(output),
		m_written(0)
	{
	}

	void doWrite()
	{
		m_wait.wait(10000);
		Thread::sleep(100); // give some change for the read to block
		m_output << "async data$" << flush;
		m_written = true;
	}

	Event m_wait;
	FdOutputStream &m_output;
	AtomicCounter m_written;
};

/**
 * Test reading in blocking mode. Read until the '$'
 * character to avoid further blocking. The 100 ms
 * timeout is used to make some space for the read
 * to really block. This is however non-deterministic.
 */
void FdStreamTest::testBlockingRead()
{
	int pipefd[2];
	CPPUNIT_ASSERT(::pipe(pipefd) >= 0);

	FdInputStream in(pipefd[0]);
	FdOutputStream out(pipefd[1]);

	in.setBlocking(true);

	AsyncWriter asyncWriter(out);
	RunnableAdapter<AsyncWriter> writer(
		asyncWriter, &AsyncWriter::doWrite);
	Thread writerThread;

	CPPUNIT_ASSERT(!in.eof());
	CPPUNIT_ASSERT(!in.fail());

	writerThread.start(writer);

	CPPUNIT_ASSERT(!asyncWriter.m_written);

	Timestamp before;
	asyncWriter.m_wait.set();

	string buffer;
	getline(in, buffer, '$');

	CPPUNIT_ASSERT(before.elapsed() >= 100 * Timespan::MILLISECONDS);

	CPPUNIT_ASSERT_EQUAL(10, buffer.size());
	CPPUNIT_ASSERT(!in.fail());
	CPPUNIT_ASSERT(!in.eof());

	CPPUNIT_ASSERT_EQUAL("async data", buffer);

	CPPUNIT_ASSERT(writerThread.tryJoin(10000));
}

void FdStreamTest::testPollPipe()
{
	const Timespan DONT_BLOCK = 0;

	int pipefd[2];
	CPPUNIT_ASSERT(::pipe(pipefd) >= 0);

	FdInputStream in(pipefd[0]);
	FdOutputStream out(pipefd[1]);

	in.setBlocking(false);
	out.setBlocking(false);

	CPPUNIT_ASSERT(!in.eof());
	CPPUNIT_ASSERT(!in.fail());
	CPPUNIT_ASSERT(!out.fail());

	// nothing to read
	CPPUNIT_ASSERT(!in.poll(DONT_BLOCK));
	// possible to write
	CPPUNIT_ASSERT(out.poll(DONT_BLOCK));

	out << "123" << flush;
	CPPUNIT_ASSERT(!out.fail());

	// pipe is readable now
	CPPUNIT_ASSERT(in.poll(DONT_BLOCK));
	// pipe is still writable
	CPPUNIT_ASSERT(out.poll(DONT_BLOCK));

	string data;
	in >> data;

	CPPUNIT_ASSERT(in.eof());
	CPPUNIT_ASSERT(!in.fail());
	CPPUNIT_ASSERT_EQUAL("123", data);

	// nothing to read
	CPPUNIT_ASSERT(!in.poll(DONT_BLOCK));
	// possible to write
	CPPUNIT_ASSERT(out.poll(DONT_BLOCK));

	out.close();
	CPPUNIT_ASSERT(out.eof());
	CPPUNIT_ASSERT(!out.bad());

	// not readable but POLLHUP should wake as up
	CPPUNIT_ASSERT(in.poll(DONT_BLOCK));

	in >> data;
	CPPUNIT_ASSERT(in.eof());
	CPPUNIT_ASSERT(!in.bad());
	CPPUNIT_ASSERT_EQUAL("123", data);

	// not sure way it returns false even when the fd is -1
	CPPUNIT_ASSERT(!out.poll(DONT_BLOCK));

	out << "test" << flush;
	CPPUNIT_ASSERT(out.eof());
	CPPUNIT_ASSERT(!out.bad());
}

}
