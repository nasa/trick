#ifdef SNAPGUI
#include <QApplication>
#include <QObject>
#include <QThread>
#include "snapwindow.h"
#include "montewindow.h"
#else
#include <QCoreApplication>
#endif

#ifdef TEST
// To include test, snap.pro CONFIG += qtestlib
#include <QtTest/QtTest>
#include "test/testsnap.h"
#endif

#include <string>
using namespace std;

#include <QDebug>
#include <QDir>
#include <stdio.h>

#include "TrickBinaryRiver.hh"
#include "snap.h"
#include "options.h"
#ifdef MONTECARLO
#include "dp.h"
#endif


#ifdef MONTECARLO
Options::func_presetCB_DougString preset_montedir;
class SnapOptions : public Options
{
  public:
    DougString montedir;
};
#else
Options::func_presetCB_double preset_start;
Options::func_presetCB_double preset_stop;
Options::func_presetCB_DougString preset_rundir;
bool check_file(const QString& fname);
class SnapOptions : public Options
{
  public:
    double start;
    double stop;
    DougString rundir;
};
#endif

SnapOptions opts;


#ifndef TEST
int main(int argc, char *argv[])
{
#ifndef SNAPGUI
    QCoreApplication a(argc, argv);
#endif

    bool ok;

#ifdef MONTECARLO
    opts.add(&opts.montedir,"<MONTE_dir>", "",
             "MONTE_directory with RUNs",
             preset_montedir);
#else
    opts.add(&opts.start,"-start",1.0, "start time of run analysis",
             preset_start);
    opts.add(&opts.stop,"-stop",1.0e20, "stop time of run analysis",
             preset_stop);
    opts.add(&opts.rundir,"<RUN_dir>", "",
             "RUN_directory with job timing data",
             preset_rundir);
#endif
    opts.parse(argc,argv, "snap", &ok);

    if ( !ok ) {
        fprintf(stderr,"%s\n",opts.usage().c_str());
        exit(-1);
    }

#ifndef MONTECARLO
    QString rundir(opts.rundir.get().c_str());
#endif

    try {
#ifdef SNAPGUI
        QApplication::setGraphicsSystem("raster");
        QApplication a(argc, argv);
#ifdef MONTECARLO
        QString montedir(opts.montedir.get().c_str());
        MonteWindow* w = new MonteWindow(montedir);
#else
        SnapWindow* w = new SnapWindow(rundir,opts.start,opts.stop);
#endif
        w->show();
        return a.exec();
#else
        Snap snap(rundir,opts.start,opts.stop);
        SnapReport rpt(snap);
        fprintf(stderr,"%s",rpt.report().toAscii().constData());
#endif
    } catch (std::exception &e) {
        fprintf(stderr,"\n%s\n",e.what());
        fprintf(stderr,"%s\n",opts.usage().c_str());
        exit(-1);
    }

    return 0;
}
#endif

#ifdef MONTECARLO
void preset_montedir(DougString* curr_montedir,const char* new_montedir,int* cok)
{
    Q_UNUSED(curr_montedir);

    *cok = (int) true;

    QString montedir(new_montedir);

    QDir dir(montedir);
    if ( ! dir.exists() ) {
        fprintf(stderr,"snap [error] : couldn't find monte directory: \"%s\".\n",
                       montedir.toAscii().constData());
        *cok = (int)false;
        return;
    }
}
#else
void preset_start(double* time, const char* sval, int* cok)
{
    *cok = (int) true;


    bool ok;
    double new_time;
    Options::str_to_double(sval, &new_time, &ok);

    if ( ok ) {
        // Start time should be less than stop time
        if ( new_time > opts.stop ) {
            fprintf(stderr,"snap [error] : Trying to set option -start to "
                    "%g; however stop time is %g.  Start should be less than "
                    "stop time.  Current start time is t=%g.\n",
                    new_time, opts.stop,*time);
            *cok = false;
        }
    }
}

void preset_stop(double* time, const char* sval, int* cok)
{
    *cok = (int) true;

    bool ok;
    double new_time;
    Options::str_to_double(sval, &new_time, &ok);

    if ( ok ) {
        // Stop time should be greater than start time
        if ( new_time < opts.start ) {
            fprintf(stderr,"snap [error] : Trying to set option -stop to "
                    "%g; however start time is %g.  Start should be less than "
                    "stop time.  -stop is currently t=%g.\n",
                    new_time, opts.start,*time);
            *cok = false;
        }
    }
}

void preset_rundir(DougString* curr_rundir, const char* new_rundir, int* cok)
{
    Q_UNUSED(curr_rundir);

    *cok = (int) true;

    QString rundir(new_rundir);

    QDir dir(rundir);
    if ( ! dir.exists() ) {
        fprintf(stderr,"snap [error] : couldn't find run directory: \"%s\".\n",
                       rundir.toAscii().constData());
        *cok = (int)false;
        return;
    }

    QString file_logframe  = rundir + QString("/log_frame.trk");
    if ( ! check_file(file_logframe) ) {
        *cok = false;
        return;
    }
    QString file_trickjobs = rundir + QString("/log_trickjobs.trk");
    if ( ! check_file(file_trickjobs) ) {
        *cok = false;
        return;
    }
    QString file_userjobs  = rundir + QString("/log_userjobs.trk");
    if ( ! check_file(file_userjobs) ) {
        *cok = false;
        return;
    }
}

bool check_file(const QString& fname)
{
    QFile file(fname);
    if ( ! file.exists() || !(file.permissions()&QFile::ReadUser) ) {
        fprintf(stderr,"snap [error] : can't read or find file %s\n",
                         fname.toAscii().constData());
        return false;
    }

    return true;
}
#endif
