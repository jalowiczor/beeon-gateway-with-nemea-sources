#pragma once

#include <iterator>

namespace BeeeOn {

/**
 * @brief The class provides an elegant way to iterate over 2 containers,
 * which size can be different. Both const and non-const containers are
 * supported. Zip::Iterator provides special functions to find out of which
 * of the iterators did not come to an end. Dereferencing of Zip::Iterator
 * returns std::pair formed by the references to values of containers.
 * The iterators of containers can be also returned from Zip::Iterator.
 *
 * The begin method returns the Zip::Iterator formed by begins of iterators, but
 * the end method is different. If one container is smaller than other,
 * the method returns Zip::Iterator formed by end iterator of smaller container
 * and begin iterator of larger container incremented by distance between
 * begin and end of smaller iterator.
 *
 * This class is not intended to iterate over large containers due to linear
 * complexity of the std::distance. It is used to determine which of the
 * containers is larger to calculate Zip.end();
 *
 * <pre>
 * vector<int> v{0, 1, 2};
 * const list<char> l{'a', 'b'};
 *
 * Zip<vector<int>, const list<char>> zip(v, l);
 * auto it = zip.begin();
 * for (; it != zip.end(); ++it)
 *   cout << (*it).first << " " << (*it).second << endl;
 *
 * if (it.hasBothEnded())
 *   return;
 *
 * if (!it.hasFirstEnded()) {
 *   for (auto firstIt = it.firstIterator(); firstIt != zip.firstEnd(); ++firstIt)
 *     cout << *firstIt << endl;
 * }
 * </pre>
 */
template <typename Container1, typename Container2>
class Zip {
public:
	using Iterator1 = decltype(std::declval<Container1&>().begin());
	using Iterator2 = decltype(std::declval<Container2&>().begin());
	using Value1 = decltype(*std::declval<Iterator1>());
	using Value2 = decltype(*std::declval<Iterator2>());

	Zip(Container1 &container1, Container2 &container2) {
		m_beginC1 = container1.begin();
		m_beginC2 = container2.begin();

		m_endC1 = container1.end();
		m_endC2 = container2.end();

		auto distanceC1 = std::distance(m_beginC1, m_endC1);
		auto distanceC2 = std::distance(m_beginC2, m_endC2);

		if (distanceC1 == distanceC2) {
			m_zipEndC1 = m_endC1;
			m_zipEndC2 = m_endC2;
		}
		else if (distanceC1 < distanceC2) {
			m_zipEndC1 = m_endC1;
			m_zipEndC2 = std::next(m_beginC2, distanceC1);
		}
		else {
			m_zipEndC2 = m_endC2;
			m_zipEndC1 = std::next(m_beginC1, distanceC2);
		}
	}

	class Iterator {
	public:
		Iterator(
				Iterator1 currentIt1,
				Iterator2 currentIt2,
				Iterator1 endIt1,
				Iterator2 endIt2):
			m_currentIt1(currentIt1),
			m_currentIt2(currentIt2),
			m_endIt1(endIt1),
			m_endIt2(endIt2)
		{
		}

		Iterator& operator++()
		{
			m_currentIt1++;
			m_currentIt2++;
			return *this;
		}

		Iterator operator++(int)
		{
			Iterator copy = *this;
			m_currentIt1++;
			m_currentIt2++;
			return copy;
		}

		bool operator==(const Iterator &other) const
		{
			return m_currentIt1 == other.m_currentIt1
				&& m_currentIt2 == other.m_currentIt2;
		}

		bool operator!=(const Iterator &other) const
		{
			return m_currentIt1 != other.m_currentIt1
				|| m_currentIt2 != other.m_currentIt2;
		}

		std::pair<Value1, Value2> operator*()
		{
			return std::pair<Value1, Value2>(*m_currentIt1, *m_currentIt2);
		}

		Iterator1 firstIterator() const
		{
			return m_currentIt1;
		}

		Iterator2 secondIterator() const
		{
			return m_currentIt2;
		}

		bool hasFirstEnded() const
		{
			return m_currentIt1 == m_endIt1;
		}

		bool hasSecondEnded() const
		{
			return m_currentIt2 == m_endIt2;
		}

		bool hasBothEnded() const
		{
			return m_currentIt1 == m_endIt1 && m_currentIt2 == m_endIt2;
		}

	private:
		Iterator1 m_currentIt1;
		Iterator2 m_currentIt2;

		Iterator1 m_endIt1;
		Iterator2 m_endIt2;
	};

	Zip<Container1, Container2>::Iterator begin() const
	{
		return Iterator(m_beginC1, m_beginC2, m_endC1, m_endC2);
	}

	Zip<Container1, Container2>::Iterator end() const
	{
		return Iterator(m_zipEndC1, m_zipEndC2, m_endC1, m_endC2);
	}

	Iterator1 firstEnd() const
	{
		return m_endC1;
	}

	Iterator2 secondEnd() const
	{
		return m_endC2;
	}

private:
	Iterator1 m_beginC1;
	Iterator2 m_beginC2;
	Iterator1 m_endC1;
	Iterator2 m_endC2;
	Iterator1 m_zipEndC1;
	Iterator2 m_zipEndC2;
};

}
