#-------------------------------------------------
#
# Project created by QtCreator 2016-01-24T19:18:44
#
#-------------------------------------------------

QT       += core gui
QT       += serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = DJIModel
TEMPLATE = app

DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x000000
DEFINES += PLATFORM_QT

SOURCES += main.cpp\
        mainwindow.cpp \
    DJI_LIB/DJI_Pro_App.cpp \
    DJI_LIB/DJI_Pro_Codec.cpp \
    DJI_LIB/DJI_Pro_Hw.cpp \
    DJI_LIB/DJI_Pro_Link.cpp \
    DJI_LIB/DJI_Pro_Rmu.cpp \
    OUR_DJI/OurDJI_FUNC.cpp \
    OUR_DJI/DebugTool.cpp \
    serial/qextserialbase.cpp \
    serial/qextserialport.cpp \
    serial/win_qextserialport.cpp \
    DJI_DEMO/DJI_Pro_Sample.cpp \
    DJI_DEMO/MyDemo.cpp

HEADERS  += mainwindow.h \
    DJI_LIB/DJI_Pro_App.h \
    DJI_LIB/DJI_Pro_Codec.h \
    DJI_LIB/DJI_Pro_Config.h \
    DJI_LIB/DJI_Pro_Hw.h \
    DJI_LIB/DJI_Pro_Link.h \
    DJI_LIB/DJI_Pro_Rmu.h \
    OUR_DJI/OurDJI_FUNC.h \
    OUR_DJI/DebugTool.h \
    serial/qextserialbase.h \
    serial/qextserialport.h \
    serial/win_qextserialport.h \
    DJI_DEMO/DJI_Pro_Sample.h \
    DJI_DEMO/MyDemo.h

FORMS    += mainwindow.ui
