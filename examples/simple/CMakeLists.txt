cmake_minimum_required(VERSION 3.15...4.2.1)

project(PajladaSignalsSimple)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(${PROJECT_NAME} main.cpp)
set_target_properties(${PROJECT_NAME} PROPERTIES
    OUTPUT_NAME "out"
)

include(FetchContent)

include(${CMAKE_CURRENT_LIST_DIR}/../IncludePajladaSignals.cmake)

FetchContent_MakeAvailable(PajladaSignals)

if (TARGET PajladaSignals)
    message(STATUS "Found PajladaSignals target")
else()
    message(STATUS "No PajladaSignals target found")
endif ()

target_link_libraries(${PROJECT_NAME} PRIVATE PajladaSignals)
