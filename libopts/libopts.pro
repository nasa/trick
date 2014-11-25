QT       -= gui

TARGET = opts
TEMPLATE = lib

CONFIG += staticlib

release {
    QMAKE_CXXFLAGS_RELEASE -= -g
}


DESTDIR = $$PWD/../lib
BUILDDIR = $$PWD/../build/options
OBJECTS_DIR = $$BUILDDIR/obj
MOC_DIR     = $$BUILDDIR/moc
RCC_DIR     = $$BUILDDIR/rcc
UI_DIR      = $$BUILDDIR/ui

SOURCES += \
    options.cpp

HEADERS += \
    options.h

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = $$OBJECTS_DIR
    }
    INSTALLS += target
}
