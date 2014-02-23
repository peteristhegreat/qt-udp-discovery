#-------------------------------------------------
#
# Project created by QtCreator 2013-12-19T08:51:48
#
#-------------------------------------------------

QT       += core gui svg network multimedia

QTPLUGIN += qavfmediaplayer qtaudio_coreaudio qtmedia_audioengine qavfcamera

RC_FILE = myapp.rc

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

#QT       -= gui

ICON = jotto.icns

TARGET = Jotto
#CONFIG   += console
#CONFIG   -= app_bundle

TEMPLATE = app

#QMAKE_CXXFLAGS += -std=c++0x
#CONFIG+=c++0x

SOURCES += main.cpp \
    server.cpp \
    mainstack.cpp \
    dictionary.cpp \
    letterbutton.cpp \
    flowlayout.cpp \
    highlighter.cpp \
    shuffle.cpp \
    overlay.cpp \
    slidingstackedwidget.cpp \
    aspectratiosvgwidget.cpp \
    globals.cpp \
    mainstack_2.cpp

HEADERS += \
    server.h \
    mainstack.h \
    dictionary.h \
    letterbutton.h \
    flowlayout.h \
    version.h \
    highlighter.h \
    shuffle.h \
    overlay.h \
    slidingstackedwidget.h \
    aspectratiosvgwidget.h \
    globals.h

RESOURCES += \
    qresources.qrc

OTHER_FILES += \
    myapp.rc \
    style.qss \
    android/AndroidManifest.xml

ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android
