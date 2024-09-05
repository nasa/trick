QT  += core
QT  += gui
QT  += xml
QT  += network

CONFIG -= app_bundle

include($$PWD/../koviz.pri)

release {
    QMAKE_CXXFLAGS_RELEASE -= -g
}

TARGET = koviz
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

LIBS += -L$$PWD/../lib -lkoviz

# Ubuntu libs are order dependent so put after -lkoviz
exists( /usr/include/mpv/client.h ) {
    LIBS += -lmpv
}


PRE_TARGETDEPS += $$PWD/../lib/libkoviz.a
