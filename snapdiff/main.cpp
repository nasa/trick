#include <QApplication>
#include <QObject>
#include <QThread>

#include <string>
using namespace std;

#include <QDebug>
#include <QDir>
#include <stdio.h>

#include "snapd.h"
#include "libsnapdata/runs.h"
#include "libqplot/plotmainwindow.h"

void dircheck(const QString& dirName);
void usage(const QString& msg=QString());
QStandardItemModel* createVarsModel(const QString& run1, const QString& run2);

int main(int argc, char *argv[])
{
    bool ok;

    try {
        QApplication::setGraphicsSystem("raster");
        QApplication a(argc, argv);
        if ( argc != 3 ) {
            usage("wrong number of arguments");
        }
        QString run1(argv[1]);
        QString run2(argv[2]);
        dircheck(run1);
        dircheck(run2);

        QStringList runList;
        runList.append(run1);
        runList.append(run2);
        Runs runs(runList);
        MonteModel monteModel(&runs);

        QStandardItemModel* varsModel = createVarsModel(run1,run2);

        PlotMainWindow w(run1, &monteModel, varsModel);

        w.show();

        return a.exec();
    } catch (std::exception &e) {
        fprintf(stderr,"\n%s\n",e.what());
        exit(-1);
    }

    return 0;
}

void usage(const QString& msg)
{
    qDebug() << "usage: snapdiff <RUN_1> <RUN_2>" ;
    if ( !msg.isEmpty() ) {
        qDebug() << " msg: " << msg;
    }
    exit(-1);
}

void dircheck(const QString& dirName)
{
    QDir dir(dirName);
    if ( ! dir.exists() ) {
        fprintf(stderr,"snapdiff [error] : directory does not exist: \"%s\".\n",
                       dirName.toAscii().constData());
        usage();
    }
}

//
// List of vars from the MonteModel col headerData
//
QStandardItemModel* createVarsModel(const QString& run1, const QString& run2)
{
    QStringList commonParams;
    QStringList s1; s1 << run1 ;
    Runs runs1(s1);
    QStringList s2; s2 << run2 ;
    Runs runs2(s2);
    QStringList params1 = runs1.params();
    foreach ( QString param2, runs2.params() ) {
        if ( params1.contains(param2) ) {
            commonParams.append(param2);
        }
    }
    commonParams.sort();

    QStandardItemModel* pm = new QStandardItemModel(0,1);

    QStandardItem *rootItem = pm->invisibleRootItem();
    for ( int i = 0; i < commonParams.size(); ++i) {
        QStandardItem *varItem = new QStandardItem(commonParams.at(i));
        rootItem->appendRow(varItem);
    }

    return pm;
}
