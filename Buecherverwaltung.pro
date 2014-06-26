#-------------------------------------------------
#
# Project created by QtCreator 2013-11-17T19:56:56
#
#-------------------------------------------------

QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Bücherverwaltung
TEMPLATE = app


SOURCES += main.cpp\
    deletedlg.cpp \
    insertdlg.cpp \
    selectdlg.cpp \
    importdlg.cpp \
    settingsdlg.cpp \
    tableview.cpp \
    listview.cpp \
    basedlg.cpp \
    view.cpp

HEADERS  += \
    deletedlg.h \
    insertdlg.h \
    main.h \
    selectdlg.h \
    importdlg.h \
    settingsdlg.h \
    tableview.h \
    listview.h \
    basedlg.h \
    view.h

FORMS +=

CONFIG += c++11

RESOURCES += \
    Buecherverwaltung.qrc
