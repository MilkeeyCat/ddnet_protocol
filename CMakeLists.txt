cmake_minimum_required(VERSION 3.16)
project(ddnet_protocol)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (CMAKE_VERSION VERSION_GREATER_EQUAL "3.24.0")
    cmake_policy(SET CMP0135 NEW)
endif()

FILE(GLOB LIB_SOURCES src/*.c)
add_library(ddnet_protocol ${LIB_SOURCES})

option(BLOAT "Whether to use C standard library" ON)

if (BLOAT)
    add_definitions(-DBLOAT)
endif()

if(CMAKE_TESTING_ENABLED)
    set(TARGET_TESTRUNNER testrunner)

    FILE(GLOB TEST_SOURCES test/*.cc)
    add_executable(${TARGET_TESTRUNNER} EXCLUDE_FROM_ALL ${TEST_SOURCES})

    target_link_libraries(${TARGET_TESTRUNNER}
        ddnet_protocol
    )

    include(FetchContent)
        FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
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

configure_file(cmake/cmake_uninstall.cmake.in cmake_uninstall.cmake IMMEDIATE @ONLY)

add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake")
