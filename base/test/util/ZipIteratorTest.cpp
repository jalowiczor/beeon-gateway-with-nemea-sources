#include <list>
#include <vector>

#include <cppunit/extensions/HelperMacros.h>

#include "cppunit/BetterAssert.h"
#include "util/ZipIterator.h"

using namespace std;

namespace BeeeOn {

class ZipIteratorTest : public CppUnit::TestFixture {
	CPPUNIT_TEST_SUITE(ZipIteratorTest);
	CPPUNIT_TEST(testEqualSizeBasicOperations);
	CPPUNIT_TEST(testEqualSizeIterate);
	CPPUNIT_TEST(testEqualSizeIterateAndCheckEnds);
	CPPUNIT_TEST(testModify);
	CPPUNIT_TEST(testFirstIsLongerBasicOperations);
	CPPUNIT_TEST(testFirstIsLongerIterate);
	CPPUNIT_TEST(testFirstIsLongerIterateAndContinue);
	CPPUNIT_TEST(testSecondIsLongerBasicOperations);
	CPPUNIT_TEST(testSecondIsLongerIterate);
	CPPUNIT_TEST(testSecondIsLongerIterateAndContinue);
	CPPUNIT_TEST_SUITE_END();
public:
	void testEqualSizeBasicOperations();
	void testEqualSizeIterate();
	void testEqualSizeIterateAndCheckEnds();
	void testModify();
	void testFirstIsLongerBasicOperations();
	void testFirstIsLongerIterate();
	void testFirstIsLongerIterateAndContinue();
	void testSecondIsLongerBasicOperations();
	void testSecondIsLongerIterate();
	void testSecondIsLongerIterateAndContinue();
};

CPPUNIT_TEST_SUITE_REGISTRATION(ZipIteratorTest);

void ZipIteratorTest::testEqualSizeBasicOperations()
{
	vector<int> v{0, 1, 2};
	const list<char> l{'a', 'b', 'c'};

	Zip<vector<int>, const list<char>> zip(v, l);

	CPPUNIT_ASSERT(zip.begin() != zip.end());
	CPPUNIT_ASSERT(!zip.begin().hasFirstEnded());
	CPPUNIT_ASSERT(!zip.begin().hasSecondEnded());
	CPPUNIT_ASSERT(!zip.begin().hasBothEnded());
	CPPUNIT_ASSERT(zip.end().hasFirstEnded());
	CPPUNIT_ASSERT(zip.end().hasSecondEnded());
	CPPUNIT_ASSERT(zip.end().hasBothEnded());
	CPPUNIT_ASSERT(zip.begin().firstIterator() == v.begin());
	CPPUNIT_ASSERT(zip.begin().secondIterator() == l.begin());
	CPPUNIT_ASSERT(zip.end().firstIterator() == v.end());
	CPPUNIT_ASSERT(zip.end().secondIterator() == l.end());
	CPPUNIT_ASSERT(zip.firstEnd() == v.end());
	CPPUNIT_ASSERT(zip.secondEnd() == l.end());
}

void ZipIteratorTest::testEqualSizeIterate()
{
	vector<int> v{0, 1, 2};
	const list<char> l{'a', 'b', 'c'};

	Zip<vector<int>, const list<char>> zip(v, l);

	vector<pair<int, char>> pairs;
	for (auto it : zip)
		pairs.push_back(it);

	CPPUNIT_ASSERT_EQUAL(3, pairs.size());
	CPPUNIT_ASSERT_EQUAL(0, pairs[0].first);
	CPPUNIT_ASSERT_EQUAL('a', pairs[0].second);
	CPPUNIT_ASSERT_EQUAL(1, pairs[1].first);
	CPPUNIT_ASSERT_EQUAL('b', pairs[1].second);
	CPPUNIT_ASSERT_EQUAL(2, pairs[2].first);
	CPPUNIT_ASSERT_EQUAL('c', pairs[2].second);
}

void ZipIteratorTest::testEqualSizeIterateAndCheckEnds()
{
	vector<int> v{0, 1, 2};
	const list<char> l{'a', 'b', 'c'};

	Zip<vector<int>, const list<char>> zip(v, l);

	vector<pair<int, char>> pairs;
	auto it = zip.begin();
	for (; it != zip.end(); ++it)
		pairs.push_back(*it);

	CPPUNIT_ASSERT_EQUAL(3, pairs.size());
	CPPUNIT_ASSERT_EQUAL(0, pairs[0].first);
	CPPUNIT_ASSERT_EQUAL('a', pairs[0].second);
	CPPUNIT_ASSERT_EQUAL(1, pairs[1].first);
	CPPUNIT_ASSERT_EQUAL('b', pairs[1].second);
	CPPUNIT_ASSERT_EQUAL(2, pairs[2].first);
	CPPUNIT_ASSERT_EQUAL('c', pairs[2].second);

	CPPUNIT_ASSERT(it.hasFirstEnded());
	CPPUNIT_ASSERT(it.hasSecondEnded());
	CPPUNIT_ASSERT(it.hasBothEnded());
}

void ZipIteratorTest::testModify()
{
	vector<int> v{0, 1};
	const list<char> l{'a', 'b'};

	Zip<vector<int>, const list<char>> zip(v, l);

	for (auto it: zip)
		it.first = 5;

	CPPUNIT_ASSERT_EQUAL(5, v[0]);
	CPPUNIT_ASSERT_EQUAL(5, v[1]);
}

void ZipIteratorTest::testFirstIsLongerBasicOperations()
{
	vector<int> v{0, 1, 2, 3};
	const list<char> l{'a', 'b'};

	Zip<vector<int>, const list<char>> zip(v, l);

	// tests basic Zip::Iterator operations
	CPPUNIT_ASSERT(zip.begin() != zip.end());
	CPPUNIT_ASSERT(!zip.begin().hasFirstEnded());
	CPPUNIT_ASSERT(!zip.begin().hasSecondEnded());
	CPPUNIT_ASSERT(!zip.begin().hasBothEnded());
	CPPUNIT_ASSERT(!zip.end().hasFirstEnded());
	CPPUNIT_ASSERT(zip.end().hasSecondEnded());
	CPPUNIT_ASSERT(!zip.end().hasBothEnded());
	CPPUNIT_ASSERT(zip.begin().firstIterator() == v.begin());
	CPPUNIT_ASSERT(zip.begin().secondIterator() == l.begin());
	CPPUNIT_ASSERT(zip.end().firstIterator() == v.begin() + 2);
	CPPUNIT_ASSERT(zip.end().secondIterator() == l.end());
	CPPUNIT_ASSERT(zip.firstEnd() == v.end());
	CPPUNIT_ASSERT(zip.secondEnd() == l.end());
}

void ZipIteratorTest::testFirstIsLongerIterate()
{
	vector<int> v{0, 1, 2, 3};
	const list<char> l{'a', 'b'};

	Zip<vector<int>, const list<char>> zip(v, l);

	vector<pair<int, char>> pairs;
	for (auto it : zip)
		pairs.push_back(it);

	CPPUNIT_ASSERT_EQUAL(2, pairs.size());
	CPPUNIT_ASSERT_EQUAL(0, pairs[0].first);
	CPPUNIT_ASSERT_EQUAL('a', pairs[0].second);
	CPPUNIT_ASSERT_EQUAL(1, pairs[1].first);
	CPPUNIT_ASSERT_EQUAL('b', pairs[1].second);
}

void ZipIteratorTest::testFirstIsLongerIterateAndContinue()
{
	vector<int> v{0, 1, 2, 3};
	const list<char> l{'a', 'b'};

	Zip<vector<int>, const list<char>> zip(v, l);

	vector<pair<int, char>> pairs;
	auto it = zip.begin();
	for(; it != zip.end(); ++it)
		pairs.push_back(*it);

	CPPUNIT_ASSERT_EQUAL(2, pairs.size());
	CPPUNIT_ASSERT_EQUAL(0, pairs[0].first);
	CPPUNIT_ASSERT_EQUAL('a', pairs[0].second);
	CPPUNIT_ASSERT_EQUAL(1, pairs[1].first);
	CPPUNIT_ASSERT_EQUAL('b', pairs[1].second);

	CPPUNIT_ASSERT(!it.hasBothEnded());
	CPPUNIT_ASSERT(!it.hasFirstEnded());
	CPPUNIT_ASSERT(it.hasSecondEnded());

	vector<int> rest;
	for (auto firstIt = it.firstIterator(); firstIt != zip.firstEnd(); ++firstIt)
		rest.push_back(*firstIt);

	CPPUNIT_ASSERT_EQUAL(2, rest.size());
	CPPUNIT_ASSERT_EQUAL(2, rest[0]);
	CPPUNIT_ASSERT_EQUAL(3, rest[1]);
}

void ZipIteratorTest::testSecondIsLongerBasicOperations()
{
	vector<int> v{0, 1};
	const list<char> l{'a', 'b', 'c', 'd'};

	Zip<vector<int>, const list<char>> zip(v, l);

	// tests basic Zip::Iterator operations
	CPPUNIT_ASSERT(zip.begin() != zip.end());
	CPPUNIT_ASSERT(!zip.begin().hasFirstEnded());
	CPPUNIT_ASSERT(!zip.begin().hasSecondEnded());
	CPPUNIT_ASSERT(!zip.begin().hasBothEnded());
	CPPUNIT_ASSERT(zip.end().hasFirstEnded());
	CPPUNIT_ASSERT(!zip.end().hasSecondEnded());
	CPPUNIT_ASSERT(!zip.end().hasBothEnded());
	CPPUNIT_ASSERT(zip.begin().firstIterator() == v.begin());
	CPPUNIT_ASSERT(zip.begin().secondIterator() == l.begin());
	CPPUNIT_ASSERT(zip.end().firstIterator() == v.end());
	CPPUNIT_ASSERT(zip.end().secondIterator() == std::next(l.begin(), 2));
	CPPUNIT_ASSERT(zip.firstEnd() == v.end());
	CPPUNIT_ASSERT(zip.secondEnd() == l.end());
}

void ZipIteratorTest::testSecondIsLongerIterate()
{
	vector<int> v{0, 1};
	const list<char> l{'a', 'b', 'c', 'd'};

	Zip<vector<int>, const list<char>> zip(v, l);

	vector<pair<int, char>> pairs;
	for (auto it : zip)
		pairs.push_back(it);

	CPPUNIT_ASSERT_EQUAL(2, pairs.size());
	CPPUNIT_ASSERT_EQUAL(0, pairs[0].first);
	CPPUNIT_ASSERT_EQUAL('a', pairs[0].second);
	CPPUNIT_ASSERT_EQUAL(1, pairs[1].first);
	CPPUNIT_ASSERT_EQUAL('b', pairs[1].second);
}

void ZipIteratorTest::testSecondIsLongerIterateAndContinue()
{
	vector<int> v{0, 1};
	const list<char> l{'a', 'b', 'c', 'd'};

	Zip<vector<int>, const list<char>> zip(v, l);

	vector<pair<int, char>> pairs;
	auto it = zip.begin();
	for(; it != zip.end(); ++it)
		pairs.push_back(*it);

	CPPUNIT_ASSERT_EQUAL(2, pairs.size());
	CPPUNIT_ASSERT_EQUAL(0, pairs[0].first);
	CPPUNIT_ASSERT_EQUAL('a', pairs[0].second);
	CPPUNIT_ASSERT_EQUAL(1, pairs[1].first);
	CPPUNIT_ASSERT_EQUAL('b', pairs[1].second);

	CPPUNIT_ASSERT(!it.hasBothEnded());
	CPPUNIT_ASSERT(it.hasFirstEnded());
	CPPUNIT_ASSERT(!it.hasSecondEnded());

	vector<char> rest;
	for (auto secondIt = it.secondIterator(); secondIt != zip.secondEnd(); ++secondIt)
		rest.push_back(*secondIt);

	CPPUNIT_ASSERT_EQUAL(2, rest.size());
	CPPUNIT_ASSERT_EQUAL('c', rest[0]);
	CPPUNIT_ASSERT_EQUAL('d', rest[1]);
}

}
