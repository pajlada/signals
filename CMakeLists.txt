cmake_minimum_required(VERSION 3.8)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_EXPORT_COMPILE_COMMANDS YES)

project(signals)

enable_testing()

add_definitions(-DPAJLADA_TESTING)

add_executable(signals
    src/main.cpp
    )

target_include_directories(signals PUBLIC include)

# Enable GTest
find_package(GTest REQUIRED)
target_link_libraries(signals GTest::GTest GTest::Main)
gtest_discover_tests(signals)

# Enable C++17
set_property(TARGET signals PROPERTY CXX_STANDARD 17)
set_property(TARGET signals PROPERTY CXX_STANDARD_REQUIRED ON)
