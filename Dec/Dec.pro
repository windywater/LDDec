TEMPLATE = app
TARGET = Dec
DESTDIR = ../build
INCLUDEPATH += ../Common

QT += core network

HEADERS += ../Common.h \
    ../Setting.h

SOURCES += ./main_dec.cpp \
    ../Common.cpp \
    ../Setting.cpp