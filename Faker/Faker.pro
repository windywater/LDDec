TEMPLATE = app
TARGET = Faker
DESTDIR = $${PWD}/../build
INCLUDEPATH += ../Common

QT += core network

HEADERS += ../Common/Common.h \
    ../Common/Setting.h

SOURCES += ./main_faker.cpp \
    ../Common/Common.cpp \
    ../Common/Setting.cpp
