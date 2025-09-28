option(PAJLADA_SIGNALS_BUILD_COVERAGE "pajlada signals build coverage" OFF)

# For MSVC: Prevent overriding the parent project's compiler/linker settings
# See https://github.com/google/googletest/blob/main/googletest/README.md#visual-studio-dynamic-vs-static-runtimes
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

message("++ Tests enabled")

add_subdirectory(${CMAKE_SOURCE_DIR}/external/googletest googletest)

enable_testing()

include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

add_executable(signals-test
    src/main.cpp
    src/signal.cpp
    src/self-disconnecting-signal.cpp
    src/connection.cpp
    src/scoped-connection.cpp
    src/signalholder.cpp
    )

target_link_libraries(signals-test gtest PajladaSignals)

include(GoogleTest)
gtest_discover_tests(signals-test)

if (PAJLADA_SIGNALS_BUILD_COVERAGE)
    list(APPEND CMAKE_MODULE_PATH
        "${CMAKE_SOURCE_DIR}/.cmake"
    )
    include(CodeCoverage)
    append_coverage_compiler_flags()
    message("++ Coverage enabled (${CMAKE_CXX_COMPILER_ID})")

    setup_target_for_coverage_gcovr_html(
        NAME coverage
        EXECUTABLE signals-test
        EXCLUDE "external/*" "tests/src/*"
        )
endif()
