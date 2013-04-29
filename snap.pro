#-------------------------------------------------
#
# Project created by QtCreator 2013-04-12T11:36:28
#
#-------------------------------------------------

QT  += core
QT  -= gui

TARGET = snap
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


 unix:DEFINES += __LINUX__

 unix:DEV  = $(DEV) # UNIX env e.g. in ~/.cshrc setenv DEV /home/vetter/dev
win32:DEV  = $(DEV) # Windows system environment variable
      RIVER = $${DEV}/snap/river

      QMAKE_CXXFLAGS += -m32 -D__STDC_CONSTANT_MACROS
 unix:QMAKE_CXXFLAGS += -fPIC
      QMAKE_LFLAGS   += -m32 -fPIC
 unix:QMAKE_LFLAGS   += -rdynamic

INCLUDEPATH += $${RIVER}/libriver
INCLUDEPATH += $${RIVER}/libtvs

 unix:HOST_CPU = Linux_FC3
win32:HOST_CPU = Win32
 unix:LIBS += -L$${RIVER}/lib_$${HOST_CPU} -lriver -ltvs -lutils

DEFINES += NOSTAT

SOURCES += main.cpp \
    snap.cpp

HEADERS += DataRiver.hh \
DataRiverFactory.hh \
    ../river/libriver/LogData.hh \
    ../river/libriver/var.hh \
    ../libriver/TrickBinaryRiver.hh \
    snap.h
