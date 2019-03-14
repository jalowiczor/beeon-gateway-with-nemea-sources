cmake_minimum_required (VERSION 2.8.11)
project (beeeon-base CXX)

if(NOT DEFINED LIB_INSTALL_DIR)
set(LIB_INSTALL_DIR "lib")
endif()

if(NOT DEFINED AR_INSTALL_DIR)
set(AR_INSTALL_DIR "lib")
endif()

add_subdirectory(src)

if(NOT DEFINED ENABLE_TESTS OR ENABLE_TESTS)
add_subdirectory(test)
endif()

install(FILES dtd/factory.dtd
	DESTINATION "share/beeeon/dtd"
	COMPONENT core)
