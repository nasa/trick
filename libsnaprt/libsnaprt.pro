QT       -= gui

TARGET = snaprt
TEMPLATE = lib

release {
    CONFIG += staticlib
    QMAKE_CXXFLAGS_RELEASE -= -g
}

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
    frame.cpp  \
    snap.cpp \
    utils.cpp \
    sjobexecthreadinfo.cpp


HEADERS += \
    job.h  \
    simobject.h  \
    snap.h  \
    thread.h  \
    frame.h \
    utils.h \
    sjobexecthreadinfo.h

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

unix:!macx:!symbian: LIBS += -L$$PWD/../lib -lsnapdata
