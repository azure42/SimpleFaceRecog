#-------------------------------------------------
#
# Project created by QtCreator 2017-03-01T18:34:41
#
#-------------------------------------------------

#DEFINES += RUN_ON_EMBEDDEDSYSTEM
QT       += core gui
CONFIG += C++11


greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = face2
TEMPLATE = app


SOURCES += main.cpp\
        widget.cpp \
    detect.cpp \
    train.cpp \
    collect.cpp

HEADERS  += widget.h \
    detect.h \
    stable.h \
    train.h \
    collect.h

FORMS    += widget.ui

PRECOMPILED_HEADER = stable.h

if(contains(DEFINES,RUN_ON_EMBEDDEDSYSTEM)){#contains和{必须在同一行，否则报错
INCLUDEPATH += /usr/local/include \
                /usr/local/include/opencv \
                /usr/local/include/opencv2
LIBS += /usr/local/lib/libopencv_*.so \

}else{
INCLUDEPATH+=D:/OpenCV/include \
             D:/OpenCV/include/opencv \
             D:/OpenCV/include/opencv2
LIBS += -L D:/OpenCV/lib/lib*.a
}


