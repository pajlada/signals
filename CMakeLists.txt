cmake_minimum_required(VERSION 3.7...4.0)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_EXPORT_COMPILE_COMMANDS YES)

list(APPEND CMAKE_MODULE_PATH
    "${CMAKE_SOURCE_DIR}/.cmake"
    )

project(signals)

add_library(PajladaSignals INTERFACE)

target_include_directories(PajladaSignals INTERFACE include)

# Enable C++17
set(CMAKE_CXX_STANDARD 17)
# set_property(TARGET signals-test PROPERTY CXX_STANDARD 17)
# set_property(TARGET signals-test PROPERTY CXX_STANDARD_REQUIRED ON)


if (BUILD_TESTS)
     # For MSVC: Prevent overriding the parent project's compiler/linker settings
    # See https://github.com/google/googletest/blob/main/googletest/README.md#visual-studio-dynamic-vs-static-runtimes
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

    message("++ Tests enabled")

    add_subdirectory(external/googletest)

    enable_testing()

    include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

    add_executable(signals-test
        src/main.cpp
        src/signal.cpp
        src/self-disconnecting-signal.cpp
        src/connection.cpp
        src/scoped-connection.cpp
        src/signal-holder.cpp
        )

    add_definitions(-DPAJLADA_TESTING)

    target_link_libraries(signals-test gtest PajladaSignals)

    include(GoogleTest)
    gtest_discover_tests(signals-test)

    if (BUILD_COVERAGE)
        include(CodeCoverage)
        append_coverage_compiler_flags()
        message("++ Coverage enabled (${CMAKE_CXX_COMPILER_ID})")

        setup_target_for_coverage_gcovr_html(
            NAME coverage
            EXECUTABLE signals-test
            EXCLUDE "external/*" "src/*"
            )
    endif()
endif()
