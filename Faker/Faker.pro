TEMPLATE = app
TARGET = Faker
DESTDIR = ../build
INCLUDEPATH += ../Common

QT += core network

HEADERS += ../Common.h \
    ../Setting.h

SOURCES += ./main_faker.cpp \
    ../Common.cpp \
    ../Setting.cpp