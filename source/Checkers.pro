#-------------------------------------------------
#
# Project created by QtCreator 2015-01-03T15:33:12
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Checkers
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    optionsdialog.cpp \
    checkergame.cpp \
    checkerpicture.cpp \
    checkerstate.cpp

HEADERS  += mainwindow.h \
    optionsdialog.h \
    checkergame.h \
    checkerpicture.h \
    checkerstate.h

FORMS += \
    optionsdialog.ui \
    mainwindow.ui

RESOURCES += \
    icons.qrc
