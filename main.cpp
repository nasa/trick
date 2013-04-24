#include <QCoreApplication>
#include <QVector>
#include <QDebug>
#include <QtAlgorithms>

#include <stdio.h>

#include "TrickBinaryRiver.hh"
#include "blame.h"

#define TO_SECONDS(X) X/1000000.0
double max_val(double* vals, int nvals);

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    //Jobs jobs(QString("/home/vetter/dev/blame/RUN_30_iss_wo_hla"));
    Jobs jobs(QString("/home/vetter/dev/For_Keith"));
    //Jobs jobs(QString("/home/vetter/dev/sims/SIM_threads/RUN_test"));

    const QList<QPair<QString,long> > rts = jobs.jobtimes(30);
    for ( int ii = 0 ; ii < rts.length() && ii < 10; ++ii ) {
        QPair<QString,long> rt = rts.at(ii);
        //qDebug() << rt;
    }

#if 0
    string trk("/home/vetter/dev/blame/RUN_30_iss_wo_hla/log_userjobs.trk");
    TrickBinaryRiver river(const_cast<char*>(trk.c_str()));
    std::vector<LOG_PARAM> params = river.getParamList();
    int nvals = river.getNumPoints();
    QVector<double> maxes;
    for ( unsigned int ii = 0 ; ii < params.size(); ++ii ) {
        LOG_PARAM param = params.at(ii);
        double* vals = river.getVals(const_cast<char*> (param.data.name.c_str()));
        double max = max_val(vals,nvals);
        maxes.push_back(max);
        fprintf(stderr, "%d %lf %s\n",
                ii,
                TO_SECONDS(max),
                const_cast<char*> (param.data.name.c_str()));
    }
#if 0
    qSort(maxes.begin(),maxes.end());
    for ( int ii = 0 ; ii < maxes.size(); ++ii ) {
        fprintf(stderr, "%lf\n", TO_SECONDS(maxes.at(ii)));
    }
#endif

#if 0
    int nvals = river.getNumPoints();
    LOG_PARAM param = params.at(822);
        fprintf(stderr, "%s\n",
                const_cast<char*> (param.data.name.c_str()));
    //double* vals = river.getVals(const_cast<char*> (param.data.name.c_str()));
    double* vals = river.getVals(const_cast<char*> (param.data.name.c_str()));
    double* ts = river.getTimeStamps();
    for ( int ii = 0 ; ii < 10 ; ii++ ) {
        fprintf(stderr, "%lf %lf\n", ts[ii],vals[ii]/1000000);
    }
#endif
#endif

    //return a.exec();
    return 0;
}

double max_val(double* vals, int nvals)
{
    double max = 0;
    for ( int ii = 0 ; ii < nvals ; ii++ ) {
        if ( max < vals[ii] ) max = vals[ii];
    }

    return max;
}
