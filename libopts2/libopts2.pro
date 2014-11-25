QT       -= gui

TARGET = opts2
TEMPLATE = lib

CONFIG += staticlib

release {
    QMAKE_CXXFLAGS_RELEASE -= -g
}


DESTDIR = $$PWD/../lib
BUILDDIR = $$PWD/../build/options2
OBJECTS_DIR = $$BUILDDIR/obj
MOC_DIR     = $$BUILDDIR/moc
RCC_DIR     = $$BUILDDIR/rcc
UI_DIR      = $$BUILDDIR/ui

SOURCES += \
    options2.cpp

HEADERS += \
    options2.h

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = $$OBJECTS_DIR
    }
    INSTALLS += target
}
