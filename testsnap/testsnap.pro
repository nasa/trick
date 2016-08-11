QT  += core
QT  += gui

CONFIG += qtestlib
CONFIG -= app_bundle

release {
    QMAKE_CXXFLAGS_RELEASE -= -g
}

TARGET = testsnap

TEMPLATE = app

DESTDIR = $$PWD/../bin
BUILDDIR = $$PWD/../build/testsnap
OBJECTS_DIR = $$BUILDDIR/obj
MOC_DIR     = $$BUILDDIR/moc
RCC_DIR     = $$BUILDDIR/rcc
UI_DIR      = $$BUILDDIR/ui

SOURCES += testsnap.cpp \
           trickdatamodel.cpp

HEADERS += \ 
    trickdatamodel.h

OTHER_FILES += 


INCLUDEPATH += $$PWD/..

unix:!macx:!symbian: LIBS += -L$$PWD/../lib  -lsnap

PRE_TARGETDEPS += $$PWD/../lib/libsnap.a
