target_sources(PajladaSignals INTERFACE
    FILE_SET headers TYPE HEADERS FILES
    pajlada/signals.hpp
    pajlada/signals/connection.hpp
    pajlada/signals/scoped-connection.hpp
    pajlada/signals/signalholder.hpp
    pajlada/signals/signal.hpp
)

target_include_directories(PajladaSignals INTERFACE
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
    "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)
