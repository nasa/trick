#-------------------------------------------------
#
# Project created by QtCreator 2015-11-23T14:52:08
#
#-------------------------------------------------

QT       += core gui network
QT       += xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = koviz
TEMPLATE = lib

include($$PWD/../koviz.pri)

CONFIG += staticlib

INCLUDEPATH += $$PWD/..

release {
    QMAKE_CXXFLAGS_RELEASE -= -g
}

DESTDIR = $$PWD/../lib
BUILDDIR = $$PWD/../build/libkoviz
OBJECTS_DIR = $$BUILDDIR/obj
MOC_DIR     = $$BUILDDIR/moc
RCC_DIR     = $$BUILDDIR/rcc
UI_DIR      = $$BUILDDIR/ui


SOURCES += bookmodel.cpp \
           bookview_pagetitle.cpp \
           bookview_plot.cpp \
           bookview.cpp \
           bookview_page.cpp \
           bookview_plottitle.cpp \
           bookidxview.cpp \
           bookview_linedruler.cpp \
           bookview_curves.cpp \
           bookview_corner.cpp \
           bookview_labeledruler.cpp \
           bookview_yaxislabel.cpp \
           bookview_xaxislabel.cpp \
           bookview_tablepage.cpp \
           bookview_table.cpp \
           curvemodel_copy.cpp \
           datamodel_optitrack_csv.cpp \
           dp.cpp \
           run.cpp \
           rundir.cpp \
           runfile.cpp \
           runswidget.cpp \
           runtv.cpp \
           sie_listmodel.cpp \
           trickvarsortfilterproxymodel.cpp \
           varswidget.cpp \
           dptreewidget.cpp \
           monteinputsview.cpp \
           dpfilterproxymodel.cpp \
           options.cpp \
           frame.cpp \
           job.cpp \
           simobject.cpp \
           sjobexecthreadinfo.cpp \
           snap.cpp \
           thread.cpp \
           utils.cpp \
           unit.cpp \
           versionnumber.cpp \
           csv.cpp \
           monte.cpp \
           parameter.cpp \
           runs.cpp \
           snaptable.cpp \
           timestamps.cpp \
           datamodel.cpp \
           datamodel_trick.cpp \
           datamodel_csv.cpp \
           tricktablemodel.cpp \
           plotmainwindow.cpp \
           timeinput.cpp \
           curvemodel.cpp \
           programmodel.cpp \
           session.cpp \
           plotlayout.cpp \
           pagelayout.cpp \
           timecom.cpp \
           videowindow.cpp \
           layoutitem_pagetitle.cpp \
           layoutitem_yaxislabel.cpp \
           layoutitem_plotcorner.cpp \
           layoutitem_plottitle.cpp \
           layoutitem_ticlabels.cpp \
           layoutitem_xaxislabel.cpp \
           layoutitem_curves.cpp \
           layoutitem_plottics.cpp \
           layoutitem_paintable.cpp \
           mapvalue.cpp \
           curvemodelparameter.cpp \
           datamodel_mot.cpp \
           bookview_tabwidget.cpp \
           fft.cpp \
           curvemodel_fft.cpp \
           curvemodel_ifft.cpp \
           filter.c \
           curvemodel_bw.cpp \
           curvemodel_sg.cpp \
           filter_sgolay.cpp \
           coord_arrow.cpp \
           curvemodel_deriv.cpp \
           curvemodel_integ.cpp \
           curvemodel_painterpath.cpp \
           trickview.cpp \
           datamodel_tv.cpp

HEADERS  += bookmodel.h \
            bookidxview.h \
            bookview.h \
            bookview_corner.h \
            bookview_curves.h \
            bookview_labeledruler.h \
            bookview_linedruler.h \
            bookview_page.h \
            bookview_pagetitle.h \
            bookview_plot.h \
            bookview_plottitle.h \
            bookview_xaxislabel.h \
            bookview_yaxislabel.h \
            bookview_tablepage.h \
            bookview_table.h \
            curvemodel_copy.h \
            datamodel_optitrack_csv.h \
            dp.h \
            run.h \
            rundir.h \
            runfile.h \
            runswidget.h \
            runtv.h \
            sie_listmodel.h \
            trickvarsortfilterproxymodel.h \
            varswidget.h \
            dptreewidget.h \
            monteinputsview.h \
            dpfilterproxymodel.h \
            options.h \
            frame.h \
            job.h \
            simobject.h \
            sjobexecthreadinfo.h \
            snap.h \
            thread.h \
            utils.h \
            unit.h \
            versionnumber.h \
            csv.h \
            monte.h \
            numsortitem.h \
            parameter.h \
            role.h \
            roundoff.h \
            runs.h \
            snaptable.h \
            timestamps.h \
            trick_types.h \
            tricktablemodel.h \
            plotmainwindow.h \
            timeinput.h \
            datamodel.h \
            datamodel_csv.h \
            datamodel_trick.h \
            curvemodel.h \
            programmodel.h \
            session.h \
            plotlayout.h \
            pagelayout.h \
            timecom.h \
            videowindow.h \
            layoutitem_pagetitle.h \
            layoutitem_yaxislabel.h \
            layoutitem_plotcorner.h \
            layoutitem_plottitle.h \
            layoutitem_ticlabels.h \
            layoutitem_xaxislabel.h \
            layoutitem_curves.h \
            layoutitem_plottics.h \
            layoutitem_paintable.h \
            mapvalue.h \
            curvemodelparameter.h \
            datamodel_mot.h \
            bookview_tabwidget.h \
            fft.h \
            curvemodel_fft.h \
            curvemodel_ifft.h \
            filter.h \
            curvemodel_bw.h \
            curvemodel_sg.h \
            filter_sgolay.h \
            coord_arrow.h \
            curvemodel_deriv.h \
            curvemodel_integ.h \
            curvemodel_painterpath.h \
            trickview.h \
            datamodel_tv.h

unix {
    FLEX  = flex
    BISON = bison
}
win32 {
    FLEX  = c:/Qt/5.15.2/Src/gnuwin32/bin/flex.exe
    BISON = c:/Qt/5.15.2/Src/gnuwin32/bin/bison.exe

    !exists($$FLEX) {
        error("Error: $$FLEX not found. Modify the FLEX path in libkoviz.pro")
    }
    !exists($$BISON) {
        error("Error: $$BISON not found. Modify the BISON path in libkoviz.pro")
    }
}

FLEX_COMMAND  = $$FLEX \
                    --header-file=$$PWD/${QMAKE_FILE_BASE}.h \
                    -o $$PWD/${QMAKE_FILE_BASE}.cpp ${QMAKE_FILE_IN}
BISON_COMMAND = $$BISON \
                    -d --defines=$$PWD/${QMAKE_FILE_BASE}.h \
                    -o $$PWD/${QMAKE_FILE_BASE}.cpp ${QMAKE_FILE_IN}

FLEXSOURCES = product_lexer.l
BISONSOURCES = product_parser.y

flexsource.input = FLEXSOURCES
flexsource.output = $$PWD/${QMAKE_FILE_BASE}.cpp
flexsource.commands = $$FLEX_COMMAND
flexsource.variable_out = FLEX_DUMMY_SOURCE
flexsource.name = Flex Sources ${QMAKE_FILE_IN}
flexsource.CONFIG += target_predeps
QMAKE_EXTRA_COMPILERS += flexsource

flexheader.input = FLEXSOURCES
flexheader.output = $$PWD/${QMAKE_FILE_BASE}.h
flexheader.commands = $$FLEX_COMMAND
flexheader.variable_out = HEADERS
flexheader.name = Flex Headers ${QMAKE_FILE_IN}
flexheader.CONFIG += target_predeps no_link
QMAKE_EXTRA_COMPILERS += flexheader

# This block used to compile product_lexer.cpp with -Wno-sign-compare
# to get around flex genning code that has gcc sign compare warning
FLEX_CPP = product_lexer.cpp
FLEX_OBJ = $$OBJECTS_DIR/product_lexer.o
compileflex.input = $$FLEX_CPP
compileflex.output = $$FLEX_OBJ
compileflex.commands = $(CXX) -c $(CXXFLAGS) -Wno-sign-compare \
                              $(INCPATH) -o $$FLEX_OBJ $$FLEX_CPP
QMAKE_EXTRA_COMPILERS += compileflex
unix {
    QMAKE_POST_LINK += $$quote(ar cqs $$DESTDIR/lib$${TARGET}.a $$FLEX_OBJ)
} else:win32 {
    QMAKE_POST_LINK += $$quote(lib /OUT:$$DESTDIR/lib$${TARGET}.lib $$FLEX_OBJ)
}

bisonsource.input = BISONSOURCES
bisonsource.output = $$PWD/${QMAKE_FILE_BASE}.cpp
bisonsource.commands = $$BISON_COMMAND
bisonsource.variable_out = SOURCES
bisonsource.name = Bison Sources ${QMAKE_FILE_IN}
bisonsource.CONFIG += target_predeps
QMAKE_EXTRA_COMPILERS += bisonsource

bisonheader.input = BISONSOURCES
bisonheader.output = $$PWD/${QMAKE_FILE_BASE}.h
bisonheader.commands = $$BISON_COMMAND
bisonheader.variable_out = HEADERS
bisonheader.name = Bison Headers ${QMAKE_FILE_IN}
bisonheader.CONFIG += target_predeps no_link
QMAKE_EXTRA_COMPILERS += bisonheader

OTHER_FILES += \
    $$FLEXSOURCES \
    $$BISONSOURCES
