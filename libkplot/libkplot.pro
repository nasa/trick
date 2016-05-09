#-------------------------------------------------
#
# Project created by QtCreator 2015-11-23T14:52:08
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = kplot
TEMPLATE = lib

include($$PWD/../snap.pri)

CONFIG += staticlib

INCLUDEPATH += $$PWD/..

release {
    QMAKE_CXXFLAGS_RELEASE -= -g
}

DESTDIR = $$PWD/../lib
BUILDDIR = $$PWD/../build/kplot
OBJECTS_DIR = $$BUILDDIR/obj
MOC_DIR     = $$BUILDDIR/moc
RCC_DIR     = $$BUILDDIR/rcc
UI_DIR      = $$BUILDDIR/ui


SOURCES += koviplot.cpp \
           kplot.cpp \
           kplotmodel.cpp \
           labeledruler.cpp \
           linedruler.cpp \
           plotcorner.cpp \
           verticallabel.cpp \
           pagetitleview.cpp \
           plotview.cpp \
    bookview.cpp \
    pageview.cpp \
    plottitleview.cpp \
    bookidxview.cpp \
    linedrulerview.cpp \
    curvesview.cpp \
    cornerview.cpp \
    labeledrulerview.cpp

HEADERS  += koviplot.h \
            kplot.h \
            kplotmodel.h \
            labeledruler.h \
            linedruler.h \
            plotcorner.h \
            verticallabel.h \
            pagetitleview.h \
            plotview.h \
    bookview.h \
    pageview.h \
    plottitleview.h \
    bookidxview.h \
    linedrulerview.h \
    curvesview.h \
    cornerview.h \
    labeledrulerview.h
