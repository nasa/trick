#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <stdio.h>

#include "TrickBinaryRiver.hh"
#include "snap.h"

void usage();
bool check_rundir(const QString& rundir);
bool check_file(const QString& fname);

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    if ( argc != 2 ) {
        fprintf(stderr,"snap [error] : wrong number of arguments\n");
        usage();
    }

    QString rundir(argv[1]);

    if ( ! check_rundir(rundir) ) {
        usage();
    }

    //Jobs jobs(QString("/home/vetter/dev/blame/RUN_30_iss_wo_hla"));
    //Jobs jobs(QString("/home/vetter/dev/For_Keith"));
    //Jobs jobs(QString("/home/vetter/dev/sims/SIM_threads/RUN_test"));
    Jobs jobs(rundir);

    return 0;
}

void usage()
{
    fprintf(stderr,"usage: snap <RUN_directory> (");
    fprintf(stderr,"RUN directory should contain Trick job timing data)\n");
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
