#-------------------------------------------------
#
# Project created by QtCreator 2013-12-19T08:51:48
#
#-------------------------------------------------

QT       += core gui svg network multimedia multimediawidgets widgets

QTPLUGIN += qavfmediaplayer qtaudio_coreaudio qtmedia_audioengine qavfcamera

RC_FILE = myapp.rc

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

#QT       -= gui
macx {
    QMAKE_MAC_SDK = macosx10.12
    #!host_build:QMAKE_MAC_SDK = macosx10.11
    ICON = jotto.icns
}

ios {
# http://www.appiconsizes.com/

BUNDLE_DATA.files = $$system("find $$PWD/icon/ios/ -name '*.png'")
QMAKE_BUNDLE_DATA += BUNDLE_DATA
}
QMAKE_INFO_PLIST = custom.plist

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
    mainstack_2.cpp \
    overlaydialogbox.cpp \
    mytime.cpp \
    winbox.cpp

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
    globals.h \
    overlaydialogbox.h \
    mytime.h \
    winbox.h

RESOURCES += \
    qresources.qrc \
    word_lists.qrc \
    styles.qrc

OTHER_FILES += \
    myapp.rc \
    style.qss \
    android/AndroidManifest.xml \
    custom.plist

ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android
