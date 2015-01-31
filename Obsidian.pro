#-------------------------------------------------
#
# Project created by QtCreator 2013-11-17T19:56:56
#
#-------------------------------------------------

QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = usr/bin/Obsidian
TEMPLATE = app


SOURCES += src/main.cpp\
    src/deletedlg.cpp \
    src/insertdlg.cpp \
    src/selectdlg.cpp \
    src/importdlg.cpp \
    src/settingsdlg.cpp \
    src/tableview.cpp \
    src/listview.cpp \
    src/basedlg.cpp \
    src/view.cpp

HEADERS  += \
    src/deletedlg.h \
    src/insertdlg.h \
    src/main.h \
    src/selectdlg.h \
    src/importdlg.h \
    src/settingsdlg.h \
    src/tableview.h \
    src/listview.h \
    src/basedlg.h \
    src/view.h

FORMS +=

CONFIG += c++11

RESOURCES += \
    src/Obsidian.qrc
