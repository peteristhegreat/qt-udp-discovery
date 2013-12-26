#-------------------------------------------------
#
# Project created by QtCreator 2013-12-19T08:51:48
#
#-------------------------------------------------

QT       += core gui svg network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

#QT       -= gui

TARGET = PeerToPeerChat
#CONFIG   += console
#CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    server.cpp \
    mainstack.cpp \
    dictionary.cpp

HEADERS += \
    server.h \
    mainstack.h \
    dictionary.h

RESOURCES += \
    qresources.qrc
