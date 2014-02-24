QT       += gui

TARGET = qplot
TEMPLATE = lib
#CONFIG += staticlib

QMAKE_EXT_CPP += .c

DESTDIR = $$PWD/../lib
BUILDDIR = $$PWD/../build/qplot
OBJECTS_DIR = $$BUILDDIR/obj
MOC_DIR     = $$BUILDDIR/moc
RCC_DIR     = $$BUILDDIR/rcc
UI_DIR      = $$BUILDDIR/ui

SOURCES += \
    qcustomplot.cpp

HEADERS += \
    qcustomplot.h

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}
