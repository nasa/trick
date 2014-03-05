
QT  += core
QT  += gui
QT  += xml

CONFIG -= app_bundle

release {
    QMAKE_CXXFLAGS_RELEASE -= -g
}

TARGET = monte

QMAKE_EXT_CPP += .c

TEMPLATE = app

DESTDIR = $$PWD/../bin
BUILDDIR = $$PWD/../build/monte
OBJECTS_DIR = $$BUILDDIR/obj
MOC_DIR     = $$BUILDDIR/moc
RCC_DIR     = $$BUILDDIR/rcc
UI_DIR      = $$BUILDDIR/ui

SOURCES += main.cpp \
    trickcurve.cpp \
    axisrect.cpp \
    montewindow.cpp \
    dp.cpp \
    plot.cpp \
    plotbookview.cpp \
    dpfilterproxymodel.cpp

    
HEADERS += \
    plot.h \
    dp.h \
    dpfilterproxymodel.h \
    axisrect.h \
    plotbookview.h \
    trickcurve.h \
    montewindow.h

OTHER_FILES += 


INCLUDEPATH += $$PWD/..
DEPENDPATH += $$PWD/../libsnapdata

unix:!macx:!symbian: LIBS += -L$$PWD/../lib -lsnapdata -lopts -lqplot
