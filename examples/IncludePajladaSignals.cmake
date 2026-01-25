option(EXAMPLE_IMPORT_METHOD "Method to use when trying to import pajlada-signals. Valid options are: SOURCE_DIR (default), FIND_PACKAGE, and GIT.")
string(TOUPPER "${EXAMPLE_IMPORT_METHOD}" _example_import_method)

if(_example_import_method STREQUAL "SOURCE_DIR" OR NOT EXAMPLE_IMPORT_METHOD)
    message(STATUS "Using SOURCE_DIR import method")
    FetchContent_Declare(
        PajladaSignals
        SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/../
        EXCLUDE_FROM_ALL
    )
elseif(_example_import_method STREQUAL "GIT")
    message(STATUS "Using GIT import method")
    FetchContent_Declare(
        PajladaSignals
        GIT_REPOSITORY https://github.com/pajlada/signals
        GIT_TAG master
        EXCLUDE_FROM_ALL
    )
elseif(_example_import_method STREQUAL "FIND_PACKAGE")
    message(STATUS "Using FIND_PACKAGE import method")
    FetchContent_Declare(
        PajladaSignals
        EXCLUDE_FROM_ALL
        FIND_PACKAGE_ARGS CONFIG REQUIRED
    )
else()
    message(FATAL_ERROR "Unknown example import method '${_example_import_method}'")
endif()

