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
    videoformatspage.h \
    introwizardpage.h \
    folderwizardpage.h

FORMS += mainwindow.ui

win32:RC_FILE += qt-youtube-dl.rc

# Copy proxy.py and youtube_dl package to build dir.
#copypylib.commands = $(COPY_DIR) $$PWD/youtube_dl $$OUT_PWD
#copypylib.depends = $$TARGET
#copyproxy.commands = $(COPY_DIR) $$PWD/proxy.py $$OUT_PWD
#first.depends = $(first) copypylib copyproxy
#first.depends = $(first) copypylib
#export(first.depends)
#export(copypylib.commands)
#export(copyproxy.commands)
#QMAKE_EXTRA_TARGETS += first copypylib copyproxy

#copypylib.path = $$OUT_PWD/youtube_dl
#copypylib.files = youtube_dl/*
#INSTALLS += copypylib

#DISTFILES += \
#    proxy.py
