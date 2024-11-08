cmake_minimum_required(VERSION 3.25)
project(ddnet_protocol)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(ddnet_protocol
    src/token.c
    src/packet.c
    src/control_packet.c
)

if(CMAKE_TESTING_ENABLED)
    set(TARGET_TESTRUNNER testrunner)

    add_executable(${TARGET_TESTRUNNER} EXCLUDE_FROM_ALL
        test/control_packet.cc
    )

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
