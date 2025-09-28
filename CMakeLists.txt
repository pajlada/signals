cmake_minimum_required(VERSION 3.7...4.0)

include(FeatureSummary)

option(PAJLADA_SIGNALS_BUILD_TESTS "Build tests" OFF)
add_feature_info("pajlada-signals tests" PAJLADA_SIGNALS_BUILD_TESTS "")

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
    enable_testing()
    add_subdirectory(tests)
endif()

feature_summary(WHAT ALL)
