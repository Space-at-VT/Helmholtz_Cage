TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG += qt

SOURCES += main.cpp \
    serial.cpp

HEADERS += \
    visa.h \
    visatype.h \
    vpptype.h \
    serial.h
