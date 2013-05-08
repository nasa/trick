#include <QCoreApplication>

#include <string>
using namespace std;

#include <QDebug>
#include <QDir>
#include <stdio.h>

#include "TrickBinaryRiver.hh"
#include "snap.h"
#include "options.h"

void usage();
bool check_rundir(const QString& rundir);
bool check_file(const QString& fname);
Options opts;

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    bool ok;

    double t0;
    double t1;
    DougString rundir;
    opts.add(&t0,"-start",1.0, "start time of run analysis");
    opts.add(&t1,"-stop",1.0e20, "stop time of run analysis");
    opts.add(&rundir,"-run", "", "RUN_directory with job timing data");
    opts.parse(argc,argv, "snap", &ok);

    if ( !ok ) {
        usage();
    }

    QString qrundir(rundir.get().c_str());
    if ( ! check_rundir(qrundir) ) {
        usage();
    }

    Jobs jobs(qrundir);

    return 0;
}

void usage()
{
    string msg = opts.usage();
    fprintf(stderr,"%s\n",msg.c_str());
    exit(-1);
}

bool check_rundir(const QString& rundir)
{
    bool ret = true;

    QDir dir(rundir);
    if ( ! dir.exists() ) {
        fprintf(stderr,"snap [error] : couldn't find run directory: \"%s\"\n",
                       rundir.toAscii().constData());
        return false;
    }

    QString file_logframe  = rundir + QString("/log_frame.trk");
    if ( ! check_file(file_logframe) ) {
        return false;
    }
    QString file_trickjobs = rundir + QString("/log_trickjobs.trk");
    if ( ! check_file(file_trickjobs) ) {
        return false;
    }
    QString file_userjobs  = rundir + QString("/log_userjobs.trk");
    if ( ! check_file(file_userjobs) ) {
        return false;
    }

    return ret;
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
