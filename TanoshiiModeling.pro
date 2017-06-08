#-------------------------------------------------
#
# Project created by QtCreator 2014-07-15T19:57:52
#
#-------------------------------------------------

QT       += core gui opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = TanoshiiModeling
TEMPLATE = app

MOC_DIR     += ../TanoshiiModeling/tmp
OBJECTS_DIR += ../TanoshiiModeling/tmp
CONFIG      += c++11

SOURCES += main.cpp\
        mainwindow.cpp \
    appunit.cpp \
    glwidget.cpp \
    meshloader.cpp \
    meshsegment.cpp \
    meshdeform.cpp

HEADERS  += mainwindow.h \
    appunit.h \
    glwidget.h \
    meshloader.h \
    meshsegment.h \
    nanoflann.hpp \
    meshdeform.h

FORMS    += mainwindow.ui



RESOURCES += \
    resource.qrc

macx: LIBS += -L$$PWD/Development/lib/ -lassimp.3.0.0 -lGLEW.1.12.0

INCLUDEPATH += $$PWD/Development/include
DEPENDPATH += $$PWD/Development/include
