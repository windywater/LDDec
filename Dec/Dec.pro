TEMPLATE = app
TARGET = Dec
DESTDIR = $${PWD}/../build
INCLUDEPATH += ../Common

QT += core network

HEADERS += ../Common/Common.h \
    ../Common/Setting.h

SOURCES += ./main_dec.cpp \
    ../Common/Common.cpp \
    ../Common/Setting.cpp
