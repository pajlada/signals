cmake_minimum_required(VERSION 3.7...4.0)

list(APPEND CMAKE_MESSAGE_CONTEXT PajladaSignals)
project(
    PajladaSignals
    VERSION 0.1.0
    DESCRIPTION "pajlada signals library"
    HOMEPAGE_URL https://github.com/pajlada/signals
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FeatureSummary)

option(PAJLADA_SIGNALS_BUILD_TESTS "Build tests" OFF)
add_feature_info("pajlada-signals tests" PAJLADA_SIGNALS_BUILD_TESTS "")

add_library(PajladaSignals INTERFACE)
add_library(Pajlada::Signals ALIAS PajladaSignals)



if(PAJLADA_SIGNALS_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

feature_summary(WHAT ALL)

target_include_directories(PajladaSignals INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)
install(TARGETS PajladaSignals EXPORT PajladaSignalsTargets)
install(EXPORT PajladaSignalsTargets DESTINATION lib/cmake/PajladaSignals)
install(DIRECTORY include DESTINATION include)
