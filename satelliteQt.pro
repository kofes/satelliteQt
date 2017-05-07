#-------------------------------------------------
#
# Project created by QtCreator 2017-04-23T16:12:56
#
#-------------------------------------------------

QT       += core gui

CONFIG += c++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET = satelliteQt
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += main.cpp\
        mainwindow.cpp \
    createimage.cpp \
    levels.cpp \
    vardialog.cpp \
    qcustomplot.cpp \
    graphic.cpp

HEADERS  += mainwindow.h \
    inc/Image.hpp \
    inc/Math.hpp \
    inc/Passport.hpp \
    inc/Satellite.hpp \
    createimage.h \
    levels.h \
    vardialog.h \
    qcustomplot.h \
    graphic.h \
    allin.hpp

FORMS    += mainwindow.ui \
    createimage.ui \
    levels.ui \
    vardialog.ui \
    graphic.ui

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/inc/release/ -lsatellite
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/inc/debug/ -lsatellite
else:unix: LIBS += -L$$PWD/inc/ -lsatellite

INCLUDEPATH += $$PWD/inc
DEPENDPATH += $$PWD/inc
