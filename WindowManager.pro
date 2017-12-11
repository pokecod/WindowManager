#-------------------------------------------------
#
# Project created by QtCreator 2017-12-10T01:32:44
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = WindowManager
TEMPLATE = app


SOURCES += main.cpp\
        Window.cpp \
    EventsHandler.cpp

HEADERS  += Window.h \
    EventsHandler.h

FORMS    += Window.ui
