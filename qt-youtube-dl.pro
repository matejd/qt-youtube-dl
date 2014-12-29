QT += core gui concurrent
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = qt-youtube-dl
TEMPLATE = app
CONFIG += c++11
CONFIG += embed_manifest_exe

SOURCES += main.cpp\
           mainwindow.cpp \
           youtubedl.cpp

HEADERS += mainwindow.h \
           youtubedl.h \
           wizardintropage.h \
           wizardformatspage.h \
           wizardfolderpage.h

FORMS += mainwindow.ui

win32:RC_FILE += qt-youtube-dl.rc
