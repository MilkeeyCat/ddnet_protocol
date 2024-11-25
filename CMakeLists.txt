cmake_minimum_required(VERSION 3.16)
project(ddnet_protocol)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (CMAKE_VERSION VERSION_GREATER_EQUAL "3.24.0")
    cmake_policy(SET CMP0135 NEW)
endif()

FILE(GLOB LIB_SOURCES src/*.c)
add_library(ddnet_protocol ${LIB_SOURCES})

set_target_properties(
    ddnet_protocol PROPERTIES PUBLIC_HEADER
"include/ddnet_protocol/common.h;\
include/ddnet_protocol/errors.h;\
include/ddnet_protocol/token.h;\
include/ddnet_protocol/chunk.h;\
include/ddnet_protocol/packet.h;\
include/ddnet_protocol/packer.h;\
include/ddnet_protocol/huffman.h"
)

option(BLOAT "Whether to use C standard library" ON)

if (BLOAT)
    add_definitions(-DBLOAT)
endif()

### tests

if(CMAKE_TESTING_ENABLED)
    set(INSTALL_GTEST OFF)
    set(TARGET_TESTRUNNER testrunner)

    FILE(GLOB TEST_SOURCES test/*.cc)
    add_executable(${TARGET_TESTRUNNER} EXCLUDE_FROM_ALL ${TEST_SOURCES})

    target_link_libraries(${TARGET_TESTRUNNER}
        ddnet_protocol
    )

    include(FetchContent)
        FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/3d73dee972d0db344bda9b659836612aba6a3564.zip
    )

    # For Windows: Prevent overriding the parent project's compiler/linker settings
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)

    target_link_libraries(
        ${TARGET_TESTRUNNER}
        GTest::gtest_main
    )

    include(GoogleTest)
    gtest_discover_tests(${TARGET_TESTRUNNER})

    add_custom_target(run_tests
        COMMAND $<TARGET_FILE:${TARGET_TESTRUNNER}> ${TESTRUNNER_ARGS}
        COMMENT Running unit tests
        DEPENDS ${TARGET_TESTRUNNER}
        USES_TERMINAL
    )
endif()

### install

include(GNUInstallDirs)
install(TARGETS ddnet_protocol
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/ddnet_protocol)

### uninstall

configure_file(cmake/cmake_uninstall.cmake.in cmake_uninstall.cmake IMMEDIATE @ONLY)

add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake")
