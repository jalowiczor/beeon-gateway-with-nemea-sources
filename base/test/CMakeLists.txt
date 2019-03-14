cmake_minimum_required (VERSION 2.8.11)
project (base-test CXX)

find_library (CPP_UNIT cppunit)
find_library (PTHREAD pthread)
find_library (POCO_FOUNDATION PocoFoundation)
find_library (POCO_UTIL PocoUtil)
find_library (POCO_SSL PocoNetSSL)
find_library (POCO_CRYPTO PocoCrypto)
find_library (POCO_NET PocoNet)
find_library (POCO_XML PocoXML)
find_library (POCO_JSON PocoJSON)

add_definitions(-std=c++11)
add_definitions(-Wall -pedantic -Wextra)

file(GLOB TEST_LIBRARY_SOURCES
	${PROJECT_SOURCE_DIR}/cppunit/FileTestFixture.cpp
	${PROJECT_SOURCE_DIR}/cppunit/TapOutputter.cpp
	${PROJECT_SOURCE_DIR}/cppunit/TestTimingListener.cpp
)

file(GLOB TEST_SOURCES
	${PROJECT_SOURCE_DIR}/di/DependencyInjectorTest.cpp
	${PROJECT_SOURCE_DIR}/di/DIDaemonTest.cpp
	${PROJECT_SOURCE_DIR}/di/DIWrapperTest.cpp
	${PROJECT_SOURCE_DIR}/gwmessage/GWDeviceAcceptRequestTest.cpp
	${PROJECT_SOURCE_DIR}/gwmessage/GWDeviceListResponseTest.cpp
	${PROJECT_SOURCE_DIR}/gwmessage/GWMessageTest.cpp
	${PROJECT_SOURCE_DIR}/gwmessage/GWResponseTest.cpp
	${PROJECT_SOURCE_DIR}/gwmessage/GWResponseAckingTest.cpp
	${PROJECT_SOURCE_DIR}/io/AutoCloseTest.cpp
	${PROJECT_SOURCE_DIR}/io/ConsoleTest.cpp
	${PROJECT_SOURCE_DIR}/io/FdStreamTest.cpp
	${PROJECT_SOURCE_DIR}/io/SafeWriterTest.cpp
	${PROJECT_SOURCE_DIR}/io/TCPConsoleTest.cpp
	${PROJECT_SOURCE_DIR}/l10n/SystemLocaleImplTest.cpp
	${PROJECT_SOURCE_DIR}/loop/LoopRunnerTest.cpp
	${PROJECT_SOURCE_DIR}/loop/StopControlTest.cpp
	${PROJECT_SOURCE_DIR}/loop/ToolTest.cpp
	${PROJECT_SOURCE_DIR}/math/LogicalExpressionTest.cpp
	${PROJECT_SOURCE_DIR}/math/SimpleCalcTest.cpp
	${PROJECT_SOURCE_DIR}/model/DeviceDescriptionTest.cpp
	${PROJECT_SOURCE_DIR}/model/DeviceIDTest.cpp
	${PROJECT_SOURCE_DIR}/model/DevicePrefixTest.cpp
	${PROJECT_SOURCE_DIR}/model/GatewayIDTest.cpp
	${PROJECT_SOURCE_DIR}/model/GlobalIDTest.cpp
	${PROJECT_SOURCE_DIR}/model/ModuleIDTest.cpp
	${PROJECT_SOURCE_DIR}/model/ModuleTypeTest.cpp
	${PROJECT_SOURCE_DIR}/model/OpModeTest.cpp
	${PROJECT_SOURCE_DIR}/model/RefreshTimeTest.cpp
	${PROJECT_SOURCE_DIR}/model/SensorDataTest.cpp
	${PROJECT_SOURCE_DIR}/net/IPAddressRangeTest.cpp
	${PROJECT_SOURCE_DIR}/net/MACAddressTest.cpp
	${PROJECT_SOURCE_DIR}/util/AbstractAsyncWorkTest.cpp
	${PROJECT_SOURCE_DIR}/util/ArgsParserTest.cpp
	${PROJECT_SOURCE_DIR}/util/BacktraceTest.cpp
	${PROJECT_SOURCE_DIR}/util/Base64Test.cpp
	${PROJECT_SOURCE_DIR}/util/BlockingAsyncWorkTest.cpp
	${PROJECT_SOURCE_DIR}/util/CancellableSetTest.cpp
	${PROJECT_SOURCE_DIR}/util/CastableTest.cpp
	${PROJECT_SOURCE_DIR}/util/ClassInfoTest.cpp
	${PROJECT_SOURCE_DIR}/util/DelayedAsyncWorkTest.cpp
	${PROJECT_SOURCE_DIR}/util/DAMMTest.cpp
	${PROJECT_SOURCE_DIR}/util/EnumTest.cpp
	${PROJECT_SOURCE_DIR}/util/EventSourceTest.cpp
	${PROJECT_SOURCE_DIR}/util/ExponentialBackOffTest.cpp
	${PROJECT_SOURCE_DIR}/util/FailDetectorTest.cpp
	${PROJECT_SOURCE_DIR}/util/HashedLockTest.cpp
	${PROJECT_SOURCE_DIR}/util/IncompleteTimestampTest.cpp
	${PROJECT_SOURCE_DIR}/util/JsonUtilTest.cpp
	${PROJECT_SOURCE_DIR}/util/MultiExceptionTest.cpp
	${PROJECT_SOURCE_DIR}/util/OnceTest.cpp
	${PROJECT_SOURCE_DIR}/util/ParallelExecutorTest.cpp
	${PROJECT_SOURCE_DIR}/util/RandomBackOffTest.cpp
	${PROJECT_SOURCE_DIR}/util/SecureXmlParserTest.cpp
	${PROJECT_SOURCE_DIR}/util/SequentialAsyncExecutorTest.cpp
	${PROJECT_SOURCE_DIR}/util/SingleInstanceCheckerTest.cpp
	${PROJECT_SOURCE_DIR}/util/ThreadRecursionProtectorTest.cpp
	${PROJECT_SOURCE_DIR}/util/ThreadWrapperAsyncWorkTest.cpp
	${PROJECT_SOURCE_DIR}/util/TimeIntervalTest.cpp
	${PROJECT_SOURCE_DIR}/util/TimespanParserTest.cpp
	${PROJECT_SOURCE_DIR}/util/UnsafePtrTest.cpp
	${PROJECT_SOURCE_DIR}/util/WithTraceTest.cpp
	${PROJECT_SOURCE_DIR}/util/ZipIteratorTest.cpp
	${PROJECT_SOURCE_DIR}/ssl/X509FingerprintTest.cpp
)

include_directories(
	${PROJECT_SOURCE_DIR}
	${PROJECT_SOURCE_DIR}/../src
)

add_library(BeeeOnBaseTest SHARED ${TEST_LIBRARY_SOURCES})
add_executable(test-suite-base test.cpp ${TEST_SOURCES})
add_test(COMMAND test-suite-base)

set(LIBS
	${POCO_FOUNDATION}
	${POCO_SSL}
	${POCO_CRYPTO}
	${POCO_UTIL}
	${POCO_NET}
	${POCO_XML}
	${POCO_JSON}
	${CPP_UNIT}
	${PTHREAD}
)

# Apple's linker doesn't support --whole-archive. Instead it uses -all_load.
if (APPLE)
	target_link_libraries(test-suite-base
		-Wl,-all_load
		BeeeOnBaseTest
		BeeeOnBase
		${LIBS}
	)
# The -Wl,--whole-archive is necessary for all libraries defining
# BEEEON_OBJECTs (see di/Injectable.h).
else()
	target_link_libraries(test-suite-base
		-Wl,--whole-archive
		BeeeOnBaseTest
		BeeeOnBase
		-Wl,--no-whole-archive
		${LIBS}
	)
endif()
