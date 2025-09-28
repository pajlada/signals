cmake_minimum_required(VERSION 3.7...4.0)

project(signals-test)

option(PAJLADA_SIGNALS_BUILD_COVERAGE "Build coverage" OFF)
add_feature_info("pajlada-signals coverage" PAJLADA_SIGNALS_BUILD_COVERAGE "")

# For MSVC: Prevent overriding the parent project's compiler/linker settings
# See https://github.com/google/googletest/blob/main/googletest/README.md#visual-studio-dynamic-vs-static-runtimes
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

include(FetchContent)

FetchContent_Declare(
    googletest
    URL ${CMAKE_CURRENT_LIST_DIR}/../external/googletest
    EXCLUDE_FROM_ALL
    FIND_PACKAGE_ARGS NAMES GTest
)

FetchContent_MakeAvailable(googletest)

include(GoogleTest)

enable_testing()

include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

add_executable(${PROJECT_NAME}
    src/main.cpp
    src/signal.cpp
    src/self-disconnecting-signal.cpp
    src/connection.cpp
    src/scoped-connection.cpp
    src/signalholder.cpp
    )

target_link_libraries(${PROJECT_NAME} PRIVATE gtest)
target_link_libraries(${PROJECT_NAME} PRIVATE Pajlada::Signals)

gtest_discover_tests(${PROJECT_NAME})

if (PAJLADA_SIGNALS_BUILD_COVERAGE)
    list(APPEND CMAKE_MODULE_PATH
        "${CMAKE_SOURCE_DIR}/.cmake"
    )
    include(CodeCoverage)
    append_coverage_compiler_flags()

    setup_target_for_coverage_gcovr_html(
        NAME coverage
        EXECUTABLE ${PROJECT_NAME}
        EXCLUDE "external/*" "tests/src/*"
        )
endif()
