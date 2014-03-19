QT  += core
QT  += gui
QT  += xml

CONFIG -= app_bundle

release {
    QMAKE_CXXFLAGS_RELEASE -= -g
}

TARGET = snapx

QMAKE_EXT_CPP += .c

TEMPLATE = app

DESTDIR = $$PWD/../bin
BUILDDIR = $$PWD/../build/snapx
OBJECTS_DIR = $$BUILDDIR/obj
MOC_DIR     = $$BUILDDIR/moc
RCC_DIR     = $$BUILDDIR/rcc
UI_DIR      = $$BUILDDIR/ui

SOURCES += main.cpp \
    timelineloader.cpp \
    timelineplot.cpp \
    snapwindow.cpp

    
HEADERS += \
    timelineloader.h \
    timelineplot.h \
    snapwindow.h

OTHER_FILES += 


INCLUDEPATH += $$PWD/..

unix:!macx:!symbian: LIBS += -L$$PWD/../lib -lsnaprt -lqplot -lsnapdata -lopts

PRE_TARGETDEPS += $$PWD/../lib/libsnaprt.a
PRE_TARGETDEPS += $$PWD/../lib/libsnapdata.a
PRE_TARGETDEPS += $$PWD/../lib/libopts.a
PRE_TARGETDEPS += $$PWD/../lib/libqplot.a
