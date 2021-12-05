cmake_minimum_required(VERSION 3.7.2)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_EXPORT_COMPILE_COMMANDS YES)

project(signals)

add_library(PajladaSignals INTERFACE)

target_include_directories(PajladaSignals INTERFACE include)

# Enable C++17
set(CMAKE_CXX_STANDARD 17)
# set_property(TARGET signals-test PROPERTY CXX_STANDARD 17)
# set_property(TARGET signals-test PROPERTY CXX_STANDARD_REQUIRED ON)


if (BUILD_TESTS)
    message("++ Tests enabled")
    # find_package(GTest)

    add_subdirectory(external/googletest)

    enable_testing()

    include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

    add_executable(signals-test
        src/main.cpp
        src/signal.cpp
        src/self-disconnecting-signal.cpp
        src/connection.cpp
        src/scoped-connection.cpp
        )

    add_definitions(-DPAJLADA_TESTING)

    target_link_libraries(signals-test gtest PajladaSignals)

    # gtest_discover_tests(signals-test)
    add_test(UnitTests signals-test)

    set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/.cmake)

    if (BUILD_COVERAGE)

        if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
            message("++ Coverage enabled (Clang)")
            SET(coverage_compile_flags "-g -O0 -fprofile-instr-generate -fcoverage-mapping -fno-inline")

            SET(CMAKE_CXX_FLAGS "${coverage_compile_flags}")
            SET(CMAKE_C_FLAGS "${coverage_compile_flags}")
        else()
            message("++ Coverage enabled (GCC)")
            SET(coverage_compile_flags "-g -O0 -fprofile-arcs -ftest-coverage -fno-inline")

            include(CodeCoverage)

            setup_target_for_coverage(signals-test-coverage signals-test coverage)
            SET(CMAKE_CXX_FLAGS "${coverage_compile_flags}")
            SET(CMAKE_C_FLAGS "${coverage_compile_flags}")
        endif()
    endif()
endif()

