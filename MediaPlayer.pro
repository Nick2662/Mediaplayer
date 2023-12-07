#-------------------------------------------------
#
# Project created by QtCreator 2023-10-26T16:23:24
#
#-------------------------------------------------

QT       += core gui

QT	+= multimedia      #使用多媒体模块
QT += multimediawidgets #使用QVideoWidget视频显示组件

QT +=sql

QT += network #使用网络模块

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = MediaPlayer
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
    Database.cpp \
    DownloadTool.cpp

HEADERS  += mainwindow.h \
    Main.h \
    mediaobject.h \
    database.h \
    DownloadTool.h

FORMS    += mainwindow.ui

RESOURCES += \
    icon.qrc
