#-------------------------------------------------
#
# Project created by QtCreator 2013-04-12T11:36:28
#
#-------------------------------------------------

CONFIG += core
CONFIG += console
CONFIG -= gui
CONFIG -= app_bundle

CONFIG += cmdline_build
#CONFIG += force_32bit   # if this set, change Projects build to Linux_FC3
#CONFIG += qtestlib
#CONFIG += snapgui
CONFIG += monte
#CONFIG += timeline  # this is a dev thing; must use RUN_iss* for 64.08s spike

qtestlib {
    DEFINES += TEST
    QT += gui
    greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport
}

snapgui {
    DEFINES += SNAPGUI
    QT += gui
    QT += xml
    CONFIG -= console
}

monte {
    DEFINES += SNAPGUI
    DEFINES += MONTECARLO
    QT += gui
    QT += xml
    CONFIG -= console

    SOURCES += \
        montewindow.cpp

    HEADERS += \
        montewindow.h
}

timeline {
    # This is dev thing
    # A set of graphs around the 64.08 second spike is hardcoded
    # inside timelineplot.cpp.
    DEFINES += SNAPGUI
    DEFINES += TIMELINE
    QT += gui
    QT += xml
    CONFIG -= console
}

snapgui|monte|timeline {

SOURCES += \
    timelineloader.cpp \
    timelineplot.cpp \
    monte.cpp \
    dp.cpp \
    trickmodel.cpp \
    trickcurve.cpp \
    axisrect.cpp \
    plot.cpp \
    plotpage.cpp \
    trickcurvemodel.cpp

HEADERS +=  \
    monte.h \
    trick_types.h \
    dp.h \
    trick_types.h \
    timelineloader.h \
    timelineplot.h \
    trickmodel.h \
    trickcurve.h \
    axisrect.h \
    plot.h \
    plotpage.h \
    trickcurvemodel.h
}

FORMS  += \
    qplot/qplotmainwindow.ui

TARGET = snap
TEMPLATE = app

cmdline_build {
    HOST_TYPE = unknownhosttype
    unix:HOST_TYPE = linux

    DESTDIR = $$PWD
    BUILDDIR = $$DESTDIR/build_$${HOST_TYPE}_$$QMAKE_HOST.arch
    OBJECTS_DIR = $$BUILDDIR/obj
    MOC_DIR     = $$BUILDDIR/moc
    RCC_DIR     = $$BUILDDIR/rcc
    UI_DIR      = $$BUILDDIR/ui
}



force_32bit {
        QMAKE_CXXFLAGS += -m32 -D__STDC_CONSTANT_MACROS
   unix:QMAKE_CXXFLAGS += -fPIC
        QMAKE_LFLAGS   += -m32 -fPIC
   unix:QMAKE_LFLAGS   += -rdynamic
   unix:HOST_CPU = Linux_FC3
  win32:HOST_CPU = Win32
} else {
    unix:HOST_CPU = Linux_FC3_x86_64
   win32:HOST_CPU = Win64 # Untested
}

      RIVER = $$PWD/river
    OPTIONS = $$PWD/options

DEFINES += NOSTAT    # this is probably not needed

INCLUDEPATH += $${RIVER}/libriver
INCLUDEPATH += $${RIVER}/libtvs
INCLUDEPATH += $${OPTIONS}

 unix:LIBS += -L$${RIVER}/lib_$${HOST_CPU} -lriver -ltvs -lutils
 unix:LIBS += -L$${OPTIONS}/lib_$${HOST_CPU} -loptions


SOURCES += \
    test/testsnap.cpp \
    main.cpp \
    snap.cpp \
    job.cpp \
    boundedtrickbinaryriver.cpp \
    thread.cpp \
    simobject.cpp \
    utils.cpp \
    frame.cpp \
    trickdatamodel.cpp \
    qplot/qcustomplot.cpp \
    qplot/qplotmainwindow.cpp \
    snaptable.cpp \
    snapwindow.cpp \
    snapplot.cpp \
    timeit_linux.cpp \
    timeit.cpp \
    timeit_win32.cpp \
    montemodel.cpp \
    plotbookview.cpp

HEADERS += \
    test/testsnap.h \
    river/libriver/DataRiver.hh \
    river/libriver/DataRiverFactory.hh \
    river/libriver/LogData.hh \
    river/libriver/var.hh \
    river/libriver/TrickBinaryRiver.hh \
    snap.h \
    job.h \
    boundedtrickbinaryriver.h \
    thread.h \
    simobject.h \
    utils.h \
    frame.h \
    trickdatamodel.h \
    qplot/qcustomplot.h \
    qplot/qplotmainwindow.h \
    snaptable.h \
    snapwindow.h \
    snapplot.h \
    timeit.h \
    timeit_linux.h \
    timeit_win32.h \
    montemodel.h \
    plotbookview.h
