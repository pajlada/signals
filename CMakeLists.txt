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

include(GNUInstallDirs)
include(FeatureSummary)

option(PAJLADA_SIGNALS_BUILD_TESTS "Build tests" ${PROJECT_IS_TOP_LEVEL})
add_feature_info("pajlada-signals tests" PAJLADA_SIGNALS_BUILD_TESTS "")
option(PAJLADA_SIGNALS_INSTALL "Install pajlada-signals" ${PROJECT_IS_TOP_LEVEL})

add_library(PajladaSignals INTERFACE)
add_library(Pajlada::Signals ALIAS PajladaSignals)

set_target_properties(PajladaSignals PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
    EXPORT_NAME PajladaSignals
)

add_subdirectory(include)

if(PAJLADA_SIGNALS_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()


if(PAJLADA_SIGNALS_INSTALL)
    include(CMakePackageConfigHelpers)

    # Install header files
    install(TARGETS PajladaSignals
        EXPORT SignalsTargets
        FILE_SET headers
    )

    # Export(?) & install the Targets.cmake file
    install(EXPORT SignalsTargets
        FILE PajladaSignalsTargets.cmake
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}"
    )

    # Write the ConfigVersion.cmake and Config.cmake files and install them into lib/cmake/ProjectName/
    write_basic_package_version_file(
        ${CMAKE_CURRENT_BINARY_DIR}/PajladaSignalsConfigVersion.cmake
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY AnyNewerVersion
    )
    configure_package_config_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/cmake/PajladaSignalsConfig.cmake.in
        ${CMAKE_CURRENT_BINARY_DIR}/PajladaSignalsConfig.cmake
        INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/PajladaSignals"
        NO_CHECK_REQUIRED_COMPONENTS_MACRO
    )
    install(FILES
        ${CMAKE_CURRENT_BINARY_DIR}/PajladaSignalsConfigVersion.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/PajladaSignalsConfig.cmake
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}"
    )
endif()

if(NOT PROJECT_IS_TOP_LEVEL)
    return(PROPAGATE
        PajladaSignals_VERSION
        PajladaSignals_VERSION_MAJOR
        PajladaSignals_VERSION_MINOR
        PajladaSignals_VERSION_PATCH
        PajladaSignals_VERSION_TWEAK
    )
else()
    feature_summary(WHAT ALL)
endif()
