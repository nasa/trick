#include <QApplication>
#include <QObject>
#include <QThread>
#include "snapwindow.h"

#include <string>
using namespace std;

#include <QDebug>
#include <QDir>
#include <stdio.h>

#include "libkplot/snap.h"
#include "libkplot/options.h"

Option::FPresetDouble preset_start;
Option::FPresetDouble preset_stop;
Option::FPresetQString preset_rundir;
bool check_file(const QString& fname);
class SnapOptions : public Options
{
  public:
    bool textReport;
    double start;
    double stop;
    QString rundir;
};

SnapOptions opts;


int main(int argc, char *argv[])
{

    bool ok;

    opts.add("-textReport:{0,1}", &opts.textReport, false, "print text report");
    opts.add("-start", &opts.start, 1.0, "start time of run analysis",
             preset_start);
    opts.add("-stop", &opts.stop,1.0e20, "stop time of run analysis",
             preset_stop);
    opts.add("<RUN_dir>", &opts.rundir,"",
             "RUN_directory with job timing data",
             preset_rundir);
    opts.parse(argc,argv, "snap", &ok);

    if ( !ok ) {
        fprintf(stderr,"%s\n",opts.usage().toAscii().constData());
        exit(-1);
    }

    try {
        if ( opts.textReport ) {
            Snap snap(opts.rundir,opts.start,opts.stop);
            SnapReport rpt(snap);
            fprintf(stderr,"%s",rpt.report().toAscii().constData());
        } else {
            QApplication::setGraphicsSystem("raster");
            QApplication a(argc, argv);
            SnapWindow w(opts.rundir,opts.start,opts.stop);
            w.show();
            return a.exec();
        }
    } catch (std::exception &e) {
        fprintf(stderr,"\n%s\n",e.what());
        fprintf(stderr,"%s\n",opts.usage().toAscii().constData());
        exit(-1);
    }

    return 0;
}

void preset_start(double* time, double new_time, bool* ok)
{
    *ok = true;

    if ( *ok ) {
        // Start time should be less than stop time
        if ( new_time > opts.stop ) {
            fprintf(stderr,"snap [error] : Trying to set option -start to "
                    "%g; however stop time is %g.  Start should be less than "
                    "stop time.  Current start time is t=%g.\n",
                    new_time, opts.stop,*time);
            *ok = false;
        }
    }
}

void preset_stop(double* time, double new_time, bool* ok)
{
    *ok = true;

    if ( *ok ) {
        // Stop time should be greater than start time
        if ( new_time < opts.start ) {
            fprintf(stderr,"snap [error] : Trying to set option -stop to "
                    "%g; however start time is %g.  Start should be less than "
                    "stop time.  -stop is currently t=%g.\n",
                    new_time, opts.start,*time);
            *ok = false;
        }
    }
}

void preset_rundir(QString* curr_rundir, const QString& rundir, bool* ok)
{
    Q_UNUSED(curr_rundir);

    *ok = true;

    QDir dir(rundir);
    if ( ! dir.exists() ) {
        fprintf(stderr,"snap [error] : couldn't find run directory: \"%s\".\n",
                       rundir.toAscii().constData());
        *ok = false;
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
