QT  += core
QT  += gui
QT  += xml

CONFIG -= app_bundle

include($$PWD/../snap.pri)

release {
    QMAKE_CXXFLAGS_RELEASE -= -g
}

TARGET = snap
target.path = $$PREFIX/bin
INSTALLS += target

TEMPLATE = app

DESTDIR = $$PWD/../bin
BUILDDIR = $$PWD/../build/$${TARGET}
OBJECTS_DIR = $$BUILDDIR/obj
MOC_DIR     = $$BUILDDIR/moc
RCC_DIR     = $$BUILDDIR/rcc
UI_DIR      = $$BUILDDIR/ui

SOURCES += main.cpp
    
HEADERS +=

OTHER_FILES += 


INCLUDEPATH += $$PWD/..

unix:macx:!symbian: LIBS += -L$$PWD/../lib -lsnap

PRE_TARGETDEPS += $$PWD/../lib/libsnap.a
