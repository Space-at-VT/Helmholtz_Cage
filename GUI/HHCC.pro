#-------------------------------------------------
#
# Project created by QtCreator 2017-03-15T14:21:18
#
#-------------------------------------------------

QT       += core gui serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = HHCC
TEMPLATE = app


SOURCES += main.cpp\
        hhcc.cpp \
    serial.cpp

HEADERS  += hhcc.h \
    serial.h

FORMS    += hhcc.ui
