#include <cppunit/extensions/HelperMacros.h>

#include <Poco/Exception.h>
#include <Poco/NamedMutex.h>
#include <Poco/Process.h>

#include "cppunit/BetterAssert.h"
#include "util/SingleInstanceChecker.h"

using namespace std;
using namespace Poco;

namespace BeeeOn {

class SingleInstanceCheckerTest : public CppUnit::TestFixture {
	CPPUNIT_TEST_SUITE(SingleInstanceCheckerTest);
	CPPUNIT_TEST(testCheckPass);
	CPPUNIT_TEST(testFailUnlocksIfSuccessful);
	CPPUNIT_TEST(testFailWouldFailAndNotUnlock);
	CPPUNIT_TEST(testIgnoreDoesNotUnlock);
	CPPUNIT_TEST(testRecoverAutomaticallyUnlocks);
	CPPUNIT_TEST_SUITE_END();
public:
	void setUp();

	void testCheckPass();
	void testFailUnlocksIfSuccessful();
	void testFailWouldFailAndNotUnlock();
	void testIgnoreDoesNotUnlock();
	void testRecoverAutomaticallyUnlocks();

private:
	string name() const;
};

CPPUNIT_TEST_SUITE_REGISTRATION(SingleInstanceCheckerTest);

string SingleInstanceCheckerTest::name() const
{
	return to_string(Process::id());
}

void SingleInstanceCheckerTest::setUp()
{
	NamedMutex lock(name());
	CPPUNIT_ASSERT(lock.tryLock());
	lock.unlock();
}

/**
 * @brief Test the very basic scenario. We pass and after
 * that, the lock is still held.
 */
void SingleInstanceCheckerTest::testCheckPass()
{
	SingleInstanceChecker checker;
	checker.setName(name());
	checker.setMode("fail");

	CPPUNIT_ASSERT_NO_THROW(checker.check());

	NamedMutex lock(name());
	CPPUNIT_ASSERT(!lock.tryLock());
}

/**
 * @brief Test we can lock after SingleInstanceChecker is destroyed.
 */
void SingleInstanceCheckerTest::testFailUnlocksIfSuccessful()
{
	NamedMutex lock(name());

	{
		SingleInstanceChecker checker;
		checker.setName(name());
		checker.setMode("fail");

		CPPUNIT_ASSERT_NO_THROW(checker.check());
		CPPUNIT_ASSERT(!lock.tryLock());
	}

	CPPUNIT_ASSERT(lock.tryLock());
	lock.unlock();
}

/**
 * @brief When the checker fails in fail mode, it does not unlock.
 */
void SingleInstanceCheckerTest::testFailWouldFailAndNotUnlock()
{
	NamedMutex lock(name());

	{
		CPPUNIT_ASSERT(lock.tryLock());

		SingleInstanceChecker checker;
		checker.setName(name());
		checker.setMode("fail");

		CPPUNIT_ASSERT_THROW(checker.check(), IllegalStateException);
	}

	CPPUNIT_ASSERT(!lock.tryLock());
	lock.unlock();
}

/**
 * @brief Test that SingleInstanceChecker in ignore mode
 * does not unlock when it is destroyed.
 */
void SingleInstanceCheckerTest::testIgnoreDoesNotUnlock()
{
	NamedMutex lock(name());

	{
		CPPUNIT_ASSERT(lock.tryLock());

		SingleInstanceChecker checker;
		checker.setName(name());
		checker.setMode("ignore");

		CPPUNIT_ASSERT_NO_THROW(checker.check());
		CPPUNIT_ASSERT(!lock.tryLock()); // still locked
	} // no unlock expected

	CPPUNIT_ASSERT(!lock.tryLock());
	lock.unlock();
}

/**
 * @brief Test that SingleInstanceChecker in recover mode does
 * unlock when it is destroyed.
 */
void SingleInstanceCheckerTest::testRecoverAutomaticallyUnlocks()
{
	NamedMutex lock(name());

	{
		CPPUNIT_ASSERT(lock.tryLock());

		SingleInstanceChecker checker;
		checker.setName(name());
		checker.setMode("recover");

		CPPUNIT_ASSERT_NO_THROW(checker.check());
		CPPUNIT_ASSERT(!lock.tryLock()); // locked
	} // unlock would occur

	CPPUNIT_ASSERT(lock.tryLock());
	lock.unlock();
}

}
