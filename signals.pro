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

GTEST_PATH="C:/local/googletest"

# GTest
SOURCES += \
$$GTEST_PATH/src/gtest.cc \
$$GTEST_PATH/src/gtest-death-test.cc \
$$GTEST_PATH/src/gtest-filepath.cc \
$$GTEST_PATH/src/gtest-port.cc \
$$GTEST_PATH/src/gtest-printers.cc \
$$GTEST_PATH/src/gtest-test-part.cc \
$$GTEST_PATH/src/gtest-typed-test.cc

INCLUDEPATH += \
    $$GTEST_PATH/include \
    $$GTEST_PATH/
