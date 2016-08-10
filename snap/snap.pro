QT  += core
QT  += gui
QT  += xml

CONFIG -= app_bundle

release {
    QMAKE_CXXFLAGS_RELEASE -= -g
}

include($$PWD/../snap.pri)

TARGET = snap
target.path = $$PREFIX/bin
INSTALLS += target

TEMPLATE = app

DESTDIR = $$PWD/../bin
BUILDDIR = $$PWD/../build/snap
OBJECTS_DIR = $$BUILDDIR/obj
MOC_DIR     = $$BUILDDIR/moc
RCC_DIR     = $$BUILDDIR/rcc
UI_DIR      = $$BUILDDIR/ui

SOURCES += main.cpp \
    snapwindow.cpp

    
HEADERS += \
    snapwindow.h

OTHER_FILES += 


INCLUDEPATH += $$PWD/..

unix:!macx:!symbian: LIBS += -L$$PWD/../lib -lkplot -lsnaprt -lqplot -lsnapdata

PRE_TARGETDEPS += $$PWD/../lib/libsnaprt.a
PRE_TARGETDEPS += $$PWD/../lib/libsnapdata.a
PRE_TARGETDEPS += $$PWD/../lib/libqplot.a
PRE_TARGETDEPS += $$PWD/../lib/libkplot.a
