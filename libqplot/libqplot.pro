QT       += gui
QT       += xml

TARGET = qplot
TEMPLATE = lib

include($$PWD/../snap.pri)

CONFIG += staticlib

INCLUDEPATH += $$PWD/..

release {
    QMAKE_CXXFLAGS_RELEASE -= -g
}

DESTDIR = $$PWD/../lib
BUILDDIR = $$PWD/../build/qplot
OBJECTS_DIR = $$BUILDDIR/obj
MOC_DIR     = $$BUILDDIR/moc
RCC_DIR     = $$BUILDDIR/rcc
UI_DIR      = $$BUILDDIR/ui

SOURCES += \
    qcustomplot.cpp \
    axisrect.cpp \
    plot.cpp  \
    trickcurve.cpp \
    plotmainwindow.cpp \
    pagetitlewidget.cpp \
    rangeinput.cpp

HEADERS += \
    qcustomplot.h \
    axisrect.h \
    plot.h \
    trickcurve.h \
    plotmainwindow.h \
    pagetitlewidget.h \
    rangeinput.h



unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = $$OBJECTS_DIR
    }
    INSTALLS += target
}
