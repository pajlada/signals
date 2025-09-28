cmake_minimum_required(VERSION 3.7...4.0)

option(PAJLADA_SIGNALS_BUILD_TESTS "pajlada signals build tests" OFF)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_EXPORT_COMPILE_COMMANDS YES)

project(signals)

add_library(PajladaSignals INTERFACE)

target_include_directories(PajladaSignals INTERFACE include)

# Enable C++17
set(CMAKE_CXX_STANDARD 17)
# set_property(TARGET signals-test PROPERTY CXX_STANDARD 17)
# set_property(TARGET signals-test PROPERTY CXX_STANDARD_REQUIRED ON)

if(PAJLADA_SIGNALS_BUILD_TESTS)
    add_subdirectory(tests)
endif()
