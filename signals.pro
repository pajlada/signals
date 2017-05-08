TEMPLATE = app
CONFIG += c++14 console
CONFIG -= qt app_bundle

linux {
    QMAKE_CXXFLAGS += --std=c++14
}

INCLUDEPATH += ./include

HEADERS += \
    include/pajlada/signals/signal.hpp

SOURCES += \
    src/main.cpp
