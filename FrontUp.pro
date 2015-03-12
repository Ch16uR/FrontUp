#-------------------------------------------------
#
# Project created by QtCreator 2015-02-24T14:45:48
#
#-------------------------------------------------

QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

VERSION = 0.9.3
TARGET = FrontUp
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    parserxch.cpp

HEADERS  += mainwindow.h \
    parserxch.h

FORMS    += mainwindow.ui

RESOURCES +=
RC_FILE = frontup.rc
