QT       += gui
QT       += xml

TARGET = qplot
TEMPLATE = lib

CONFIG += staticlib

INCLUDEPATH += $$PWD/..

release {
    QMAKE_CXXFLAGS_RELEASE -= -g
}

QMAKE_EXT_CPP += .c

DESTDIR = $$PWD/../lib
BUILDDIR = $$PWD/../build/qplot
OBJECTS_DIR = $$BUILDDIR/obj
MOC_DIR     = $$BUILDDIR/moc
RCC_DIR     = $$BUILDDIR/rcc
UI_DIR      = $$BUILDDIR/ui

SOURCES += \
    qcustomplot.cpp \
    axisrect.cpp \
    dp.cpp  \
    plot.cpp  \
    trickcurve.cpp \
    plotbookview.cpp

HEADERS += \
    qcustomplot.h \
    axisrect.h \
    dp.h \
    plot.h \
    trickcurve.h \
    plotbookview.h



unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}
