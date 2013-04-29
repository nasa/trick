#include <QCoreApplication>
#include <QVector>
#include <QDebug>
#include <QtAlgorithms>

#include <stdio.h>

#include "TrickBinaryRiver.hh"
#include "blame.h"

void usage();

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    if ( argc != 2 ) {
        fprintf(stderr,"snap [error] : wrong number of arguments\n");
        usage();
    }

    //Jobs jobs(QString("/home/vetter/dev/blame/RUN_30_iss_wo_hla"));
    //Jobs jobs(QString("/home/vetter/dev/For_Keith"));
    //Jobs jobs(QString("/home/vetter/dev/sims/SIM_threads/RUN_test"));
    QString rundir(argv[1]);
    Jobs jobs(rundir);

    return 0;
}

void usage()
{
    fprintf(stderr,"usage: snap <RUN_directory> (");
    fprintf(stderr,"RUN directory should contain Trick job timing data)\n");
    exit(-1);
}
