#-------------------------------------------------
#
# Project created by QtCreator 2014-04-21T02:40:24
#
#-------------------------------------------------

QT       += widgets

TARGET = Q2048
TEMPLATE = lib

DEFINES += Q2048_LIBRARY

SOURCES += Q2048.cpp \
    MainWindow.cpp \
    core_2048.cpp

HEADERS += Q2048.hpp\
        q2048_global.hpp \
    MainWindow.hpp \
    core_2048.hpp
    core_2048.hpp

unix {
    target.path = /usr/lib
    INSTALLS += target
}

FORMS += \
    MainWindow.ui
QMAKE_CXXFLAGS += -std=c++1y
