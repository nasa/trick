QT  += core
QT  += gui

CONFIG += qtestlib
CONFIG -= app_bundle

TARGET = testsnap

QMAKE_EXT_CPP += .c

TEMPLATE = app

DESTDIR = $$PWD/../bin
BUILDDIR = $$PWD/../build/testsnap
OBJECTS_DIR = $$BUILDDIR/obj
MOC_DIR     = $$BUILDDIR/moc
RCC_DIR     = $$BUILDDIR/rcc
UI_DIR      = $$BUILDDIR/ui

SOURCES += \
           testsnap.cpp \
    trickdatamodel.cpp

HEADERS += \ 
    trickdatamodel.h

OTHER_FILES += 


INCLUDEPATH += $$PWD/..
DEPENDPATH += $$PWD/../libsnaprt
DEPENDPATH += $$PWD/../libsnapdata

unix:!macx:!symbian: LIBS += -L$$PWD/../lib  -lopts -lsnaprt -lsnapdata
