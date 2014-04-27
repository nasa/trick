
QT  += core
QT  -= gui

CONFIG -= app_bundle

release {
    QMAKE_CXXFLAGS_RELEASE -= -g
}

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

SOURCES += main.cpp 

HEADERS += 

OTHER_FILES += 


INCLUDEPATH += $$PWD/..
DEPENDPATH += $$PWD/../libsnaprt
DEPENDPATH += $$PWD/../libsnapdata

unix:!macx:!symbian: LIBS += -L$$PWD/../lib  -lopts -lsnaprt -lsnapdata

PRE_TARGETDEPS += $$PWD/../lib/libsnaprt.a
PRE_TARGETDEPS += $$PWD/../lib/libsnapdata.a
PRE_TARGETDEPS += $$PWD/../lib/libopts.a
PRE_TARGETDEPS += $$PWD/../lib/libqplot.a
