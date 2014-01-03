#-------------------------------------------------
#
# Project created by QtCreator 2013-12-19T08:51:48
#
#-------------------------------------------------

QT       += core gui svg network

RC_FILE = myapp.rc

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

#QT       -= gui

TARGET = Jotto
#CONFIG   += console
#CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    server.cpp \
    mainstack.cpp \
    dictionary.cpp \
    letterbutton.cpp \
    flowlayout.cpp \
    highlighter.cpp

HEADERS += \
    server.h \
    mainstack.h \
    dictionary.h \
    letterbutton.h \
    flowlayout.h \
    version.h \
    highlighter.h

RESOURCES += \
    qresources.qrc

OTHER_FILES += \
    myapp.rc
