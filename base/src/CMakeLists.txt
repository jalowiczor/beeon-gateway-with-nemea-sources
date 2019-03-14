cmake_minimum_required (VERSION 2.8.11)
project (base-library CXX)

find_library (POCO_FOUNDATION PocoFoundation)
find_library (POCO_SSL PocoNetSSL)
find_library (POCO_CRYPTO PocoCrypto)
find_library (POCO_UTIL PocoUtil)
find_library (POCO_NET PocoNet)
find_library (POCO_XML PocoXML)
find_library (POCO_JSON PocoJSON)

add_definitions(-std=c++11)
add_definitions(-Wall -pedantic -Wextra)

if(ENABLE_UCLIBCXX_FIXES)
message(STATUS "fixing uclibc++ flaws")

add_definitions(-DNO_std_to_string)
set(CMAKE_CXX_FLAGS "-I${PROJECT_SOURCE_DIR}/uclibc++ ${CMAKE_CXX_FLAGS}")

file(GLOB UCLIBCXX_FIXES
	${PROJECT_SOURCE_DIR}/uclibc++/string_impl.cpp
)
endif()

file(GLOB SOURCES
	${PROJECT_SOURCE_DIR}/di/DependencyInjector.cpp
	${PROJECT_SOURCE_DIR}/di/DIApplicationConfigurationLoader.cpp
	${PROJECT_SOURCE_DIR}/di/DIDaemon.cpp
	${PROJECT_SOURCE_DIR}/di/DIFactory.cpp
	${PROJECT_SOURCE_DIR}/di/DIWrapper.cpp
	${PROJECT_SOURCE_DIR}/di/DIXmlLoader.cpp
	${PROJECT_SOURCE_DIR}/gwmessage/GWAck.cpp
	${PROJECT_SOURCE_DIR}/gwmessage/GWDeviceAcceptRequest.cpp
	${PROJECT_SOURCE_DIR}/gwmessage/GWDeviceListRequest.cpp
	${PROJECT_SOURCE_DIR}/gwmessage/GWDeviceListResponse.cpp
	${PROJECT_SOURCE_DIR}/gwmessage/GWGatewayAccepted.cpp
	${PROJECT_SOURCE_DIR}/gwmessage/GWGatewayRegister.cpp
	${PROJECT_SOURCE_DIR}/gwmessage/GWMessage.cpp
	${PROJECT_SOURCE_DIR}/gwmessage/GWMessageType.cpp
	${PROJECT_SOURCE_DIR}/gwmessage/GWLastValueRequest.cpp
	${PROJECT_SOURCE_DIR}/gwmessage/GWLastValueResponse.cpp
	${PROJECT_SOURCE_DIR}/gwmessage/GWListenRequest.cpp
	${PROJECT_SOURCE_DIR}/gwmessage/GWNewDeviceGroupRequest.cpp
	${PROJECT_SOURCE_DIR}/gwmessage/GWNewDeviceRequest.cpp
	${PROJECT_SOURCE_DIR}/gwmessage/GWNoticeRequest.cpp
	${PROJECT_SOURCE_DIR}/gwmessage/GWRequest.cpp
	${PROJECT_SOURCE_DIR}/gwmessage/GWResponse.cpp
	${PROJECT_SOURCE_DIR}/gwmessage/GWResponseWithAck.cpp
	${PROJECT_SOURCE_DIR}/gwmessage/GWSearchRequest.cpp
	${PROJECT_SOURCE_DIR}/gwmessage/GWSensorDataConfirm.cpp
	${PROJECT_SOURCE_DIR}/gwmessage/GWSensorDataExport.cpp
	${PROJECT_SOURCE_DIR}/gwmessage/GWSetValueRequest.cpp
	${PROJECT_SOURCE_DIR}/gwmessage/GWUnpairRequest.cpp
	${PROJECT_SOURCE_DIR}/gwmessage/GWUnpairResponse.cpp
	${PROJECT_SOURCE_DIR}/io/AutoClose.cpp
	${PROJECT_SOURCE_DIR}/io/Console.cpp
	${PROJECT_SOURCE_DIR}/io/FdStream.cpp
	${PROJECT_SOURCE_DIR}/io/IOStats.cpp
	${PROJECT_SOURCE_DIR}/io/Printable.cpp
	${PROJECT_SOURCE_DIR}/io/SafeWriter.cpp
	${PROJECT_SOURCE_DIR}/io/SerialPort.cpp
	${PROJECT_SOURCE_DIR}/io/SimpleConsole.cpp
	${PROJECT_SOURCE_DIR}/io/StdConsole.cpp
	${PROJECT_SOURCE_DIR}/io/StreamConsole.cpp
	${PROJECT_SOURCE_DIR}/io/TCPConsole.cpp
	${PROJECT_SOURCE_DIR}/l10n/Locale.cpp
	${PROJECT_SOURCE_DIR}/l10n/LocaleImpl.cpp
	${PROJECT_SOURCE_DIR}/l10n/LocaleManager.cpp
	${PROJECT_SOURCE_DIR}/l10n/NoTranslator.cpp
	${PROJECT_SOURCE_DIR}/l10n/SystemLocaleManager.cpp
	${PROJECT_SOURCE_DIR}/l10n/Translator.cpp
	${PROJECT_SOURCE_DIR}/loop/LoopRunner.cpp
	${PROJECT_SOURCE_DIR}/loop/StoppableLoop.cpp
	${PROJECT_SOURCE_DIR}/loop/StoppableRunnable.cpp
	${PROJECT_SOURCE_DIR}/loop/StopControl.cpp
	${PROJECT_SOURCE_DIR}/loop/Tool.cpp
	${PROJECT_SOURCE_DIR}/math/LogicalExpression.cpp
	${PROJECT_SOURCE_DIR}/math/SimpleCalc.cpp
	${PROJECT_SOURCE_DIR}/model/CustomTypeID.cpp
	${PROJECT_SOURCE_DIR}/model/DeviceCriteria.cpp
	${PROJECT_SOURCE_DIR}/model/DeviceDescription.cpp
	${PROJECT_SOURCE_DIR}/model/DeviceID.cpp
	${PROJECT_SOURCE_DIR}/model/DevicePrefix.cpp
	${PROJECT_SOURCE_DIR}/model/GatewayID.cpp
	${PROJECT_SOURCE_DIR}/model/GlobalID.cpp
	${PROJECT_SOURCE_DIR}/model/ModuleID.cpp
	${PROJECT_SOURCE_DIR}/model/ModuleType.cpp
	${PROJECT_SOURCE_DIR}/model/OpMode.cpp
	${PROJECT_SOURCE_DIR}/model/RefreshTime.cpp
	${PROJECT_SOURCE_DIR}/model/SensorValue.cpp
	${PROJECT_SOURCE_DIR}/model/SensorData.cpp
	${PROJECT_SOURCE_DIR}/model/Severity.cpp
	${PROJECT_SOURCE_DIR}/model/SimpleID.cpp
	${PROJECT_SOURCE_DIR}/model/TokenID.cpp
	${PROJECT_SOURCE_DIR}/net/HTTPEntireResponse.cpp
	${PROJECT_SOURCE_DIR}/net/HTTPUtil.cpp
	${PROJECT_SOURCE_DIR}/net/IPAddressRange.cpp
	${PROJECT_SOURCE_DIR}/net/MACAddress.cpp
	${PROJECT_SOURCE_DIR}/ssl/RejectCertificateHandler.cpp
	${PROJECT_SOURCE_DIR}/ssl/SSLClient.cpp
	${PROJECT_SOURCE_DIR}/ssl/SSLFacility.cpp
	${PROJECT_SOURCE_DIR}/ssl/SSLServer.cpp
	${PROJECT_SOURCE_DIR}/ssl/X509Fingerprint.cpp
	${PROJECT_SOURCE_DIR}/util/AnyAsyncWork.cpp
	${PROJECT_SOURCE_DIR}/util/ApplicationConfigurationLoader.cpp
	${PROJECT_SOURCE_DIR}/util/ArgsParser.cpp
	${PROJECT_SOURCE_DIR}/util/AsyncExecutor.cpp
	${PROJECT_SOURCE_DIR}/util/AutoConfigurationExplorer.cpp
	${PROJECT_SOURCE_DIR}/util/Backtrace.cpp
	${PROJECT_SOURCE_DIR}/util/BackOff.cpp
	${PROJECT_SOURCE_DIR}/util/Cancellable.cpp
	${PROJECT_SOURCE_DIR}/util/CancellableSet.cpp
	${PROJECT_SOURCE_DIR}/util/ClassInfo.cpp
	${PROJECT_SOURCE_DIR}/util/ConfigurationExplorer.cpp
	${PROJECT_SOURCE_DIR}/util/ConfigurationLoader.cpp
	${PROJECT_SOURCE_DIR}/util/ConfigurationSaver.cpp
	${PROJECT_SOURCE_DIR}/util/CryptoConfig.cpp
	${PROJECT_SOURCE_DIR}/util/CryptoParams.cpp
	${PROJECT_SOURCE_DIR}/util/DAMM.cpp
	${PROJECT_SOURCE_DIR}/util/Exception.cpp
	${PROJECT_SOURCE_DIR}/util/ExponentialBackOff.cpp
	${PROJECT_SOURCE_DIR}/util/FailDetector.cpp
	${PROJECT_SOURCE_DIR}/util/FileLoader.cpp
	${PROJECT_SOURCE_DIR}/util/HavingThreadPool.cpp
	${PROJECT_SOURCE_DIR}/util/Joiner.cpp
	${PROJECT_SOURCE_DIR}/util/JsonUtil.cpp
	${PROJECT_SOURCE_DIR}/util/LambdaTimerTask.cpp
	${PROJECT_SOURCE_DIR}/util/Loggable.cpp
	${PROJECT_SOURCE_DIR}/util/MultiException.cpp
	${PROJECT_SOURCE_DIR}/util/NonAsyncExecutor.cpp
	${PROJECT_SOURCE_DIR}/util/Occasionally.cpp
	${PROJECT_SOURCE_DIR}/util/Once.cpp
	${PROJECT_SOURCE_DIR}/util/ParallelExecutor.cpp
	${PROJECT_SOURCE_DIR}/util/PeriodicRunner.cpp
	${PROJECT_SOURCE_DIR}/util/PosixSignal.cpp
	${PROJECT_SOURCE_DIR}/util/RandomBackOff.cpp
	${PROJECT_SOURCE_DIR}/util/SAXHelper.cpp
	${PROJECT_SOURCE_DIR}/util/SecureXmlParser.cpp
	${PROJECT_SOURCE_DIR}/util/SequentialAsyncExecutor.cpp
	${PROJECT_SOURCE_DIR}/util/SingleInstanceChecker.cpp
	${PROJECT_SOURCE_DIR}/util/ThreadNamer.cpp
	${PROJECT_SOURCE_DIR}/util/ThreadRecursionProtector.cpp
	${PROJECT_SOURCE_DIR}/util/TimeInterval.cpp
	${PROJECT_SOURCE_DIR}/util/TimespanParser.cpp
	${PROJECT_SOURCE_DIR}/util/ValueGenerator.cpp
	${PROJECT_SOURCE_DIR}/util/WaitCondition.cpp
	${PROJECT_SOURCE_DIR}/util/WithTrace.cpp
)

include_directories(
	${PROJECT_SOURCE_DIR}
)

if(UCLIBCXX_FIXES_ENABLE)
	list(APPEND SOURCES ${UCLIBCXX_FIXES})
endif()

add_library(BeeeOnBase SHARED ${SOURCES})

install(TARGETS BeeeOnBase
	COMPONENT core
       LIBRARY DESTINATION "${LIB_INSTALL_DIR}"
       ARCHIVE DESTINATION "${AR_INSTALL_DIR}")
