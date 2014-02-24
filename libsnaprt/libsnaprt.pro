QT       -= gui

TARGET = snaprt
TEMPLATE = lib
#CONFIG += staticlib

QMAKE_EXT_CPP += .c

DESTDIR = $$PWD/../lib
BUILDDIR = $$PWD/../build/libsnaprt
OBJECTS_DIR = $$BUILDDIR/obj
MOC_DIR     = $$BUILDDIR/moc
RCC_DIR     = $$BUILDDIR/rcc
UI_DIR      = $$BUILDDIR/ui

SOURCES += \
    simobject.cpp  \
    thread.cpp  \
    job.cpp  \
    boundedtrickbinaryriver.cpp  \
    frame.cpp  \
    snap.cpp \
    utils.cpp


HEADERS += \
    job.h  \
    boundedtrickbinaryriver.h  \
    simobject.h  \
    snap.h  \
    thread.h  \
    frame.h \
    utils.h

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}

INCLUDEPATH += $$PWD/..
DEPENDPATH += $$PWD/../libsnapdata

unix:!macx:!symbian: LIBS += -L$$PWD/../lib -lriver -ltvs -lutils -lsnapdata
