#pragma once

#include <string>

#include <Poco/RWLock.h>

namespace BeeeOn {

/**
 * @brief IOStats represents common I/O statistics
 * that are usually measured for performance monitoring.
 *
 * IOStats is thread-safe.
 */
class IOStats {
public:
	struct Data {
		uint64_t writtenPdu;
		uint64_t writtenBytes;
		uint64_t readPdu;
		uint64_t readBytes;
		uint64_t lostPdu;
		uint64_t lostBytes;

		std::string toString() const;
	};

	IOStats();

	/**
	 * @brief Notify that the given amount of bytes
	 * have been written to an output device as a single
	 * PDU.
	 */
	void written(size_t bytes);

	/**
	 * @brief Notify that the given amount of bytes
	 * have been read from an input device as a single
	 * PDU.
	 */
	void read(size_t bytes);

	/**
	 * @brief Notify that the given amount of bytes
	 * representing a single PDU have been lost because
	 * of too low processing capacities or because of
	 * some policing strategy.
	 */
	void lost(size_t bytes);

	/**
	 * Provide the current statistics data.
	 */
	Data data() const;

private:
	Data m_data;
	mutable Poco::RWLock m_lock;
};

}
