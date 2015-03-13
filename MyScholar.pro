#-------------------------------------------------
#
# Project created by QtCreator 2015-03-11T02:47:34
#
#-------------------------------------------------

QT       += core gui webkit network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = MyScholar
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    pageparser.cpp \
    scholar.cpp \
    singlescholarwidget.cpp \
    scholarsearcher.cpp \
    longoperation.cpp \
    mynetworkaccessmanager.cpp

HEADERS  += mainwindow.h \
    pageparser.h \
    scholar.h \
    singlescholarwidget.h \
    scholarsearcher.h \
    longoperation.h \
    mynetworkaccessmanager.h

FORMS    += mainwindow.ui \
    singlescholarwidget.ui
