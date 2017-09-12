#-------------------------------------------------
#
# Project created by QtCreator 2017-09-06T13:56:01
#
#-------------------------------------------------

QT       += core gui
QT       += network
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

INCLUDEPATH    += /home/ember/opt/rasp-pi-rootfs/usr/local/include
LIBS += -L/home/ember/opt/rasp-pi-rootfs/usr/local/lib -lwiringPi


TARGET = Room_Timetable
TEMPLATE = app


SOURCES += main.cpp\
        timetable.cpp

HEADERS  += timetable.h

FORMS    += timetable.ui
