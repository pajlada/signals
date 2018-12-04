cmake_minimum_required(VERSION 3.8)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_EXPORT_COMPILE_COMMANDS YES)

project(signals)

include_directories(include)

# Enable C++17
set(CMAKE_CXX_STANDARD 17)
# set_property(TARGET signals-test PROPERTY CXX_STANDARD 17)
# set_property(TARGET signals-test PROPERTY CXX_STANDARD_REQUIRED ON)


if (BUILD_TESTS)
    # find_package(GTest)

    add_subdirectory(external/googletest)

    enable_testing()

    include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

    add_executable(signals-test
        src/main.cpp
        )

    add_definitions(-DPAJLADA_TESTING)

    target_link_libraries(signals-test gtest gtest_main)


    # gtest_discover_tests(signals-test)
    add_test(UnitTests signals-test)
endif()

