#include <QApplication>
#include <QObject>
#include <QThread>

#include <string>
using namespace std;

#include <QDebug>
#include <QDir>
#include <stdio.h>

#include "libopts2/options2.h"
#include "libsnapdata/monte.h"
#include "libsnapdata/montemodel.h"
#include "libqplot/dp.h"
#include "libqplot/plotmainwindow.h"

QStandardItemModel* createVarsModel(MonteModel *mm);

Option::FPresetQString presetMontedir;
Option::FPresetUInt presetBeginRun;
Option::FPresetUInt presetEndRun;

class SnapOptions : public Options2
{
  public:
    QString montedir;
    uint beginRun;
    uint endRun;
};
SnapOptions opts;


int main(int argc, char *argv[])
{

    bool ok;

    opts.add("<MONTE_dir>", &opts.montedir, "",
             "MONTE_directory with RUNs",
             presetMontedir);
    opts.add("-beginRun", &opts.beginRun,0,
             "begin run (inclusive) in set of RUNs to plot",
             presetBeginRun);
    opts.add("-endRun", &opts.endRun,1.0e8,
             "end run (inclusive) in set of RUNs to plot",
             presetEndRun);

    opts.parse(argc,argv, QString("monte"), &ok);

    if ( !ok ) {
        fprintf(stderr,"%s\n",opts.usage().toAscii().constData());
        exit(-1);
    }

    try {
        QApplication::setGraphicsSystem("raster");
        QApplication a(argc, argv);
        QString monteDir(opts.montedir);
        Monte monte(monteDir,opts.beginRun,opts.endRun);
        MonteModel monteModel(&monte);
        QStandardItemModel* varsModel = createVarsModel(&monteModel);
        PlotMainWindow w(monteDir, &monteModel, varsModel, monte.inputModel());
        w.show();
        return a.exec();
    } catch (std::exception &e) {
        fprintf(stderr,"\n%s\n",e.what());
        fprintf(stderr,"%s\n",opts.usage().toAscii().constData());
        exit(-1);
    }

    return 0;
}

void presetMontedir(QString* beginMonteDir,
                      const QString& monteDir, bool* ok)
{
    Q_UNUSED(beginMonteDir);

    QDir dir(monteDir);
    if ( ! dir.exists() ) {
        fprintf(stderr,"snap [error] : couldn't find monte directory: \"%s\".\n",
                       monteDir.toAscii().constData());
        *ok = false;
        return;
    }
}

void presetBeginRun(uint* beginRunId, uint runId, bool* ok)
{
    Q_UNUSED(beginRunId);

    if ( *ok ) {
        // beginRun should be greater than endRun
        if ( runId > opts.endRun ) {
            fprintf(stderr,"snap [error] : option -beginRun, set to %d, "
                    "must be greater than "
                    " -endRun which is set to %d\n",
                    runId, opts.endRun);
            *ok = false;
        }
    }
}

void presetEndRun(uint* endRunId, uint runId, bool* ok)
{
    Q_UNUSED(endRunId);

    *ok = true;

    if ( *ok ) {
        // Start time should be less than stop time
        if ( runId < opts.beginRun ) {
            fprintf(stderr,"snap [error] : option -endRun, set to %d, "
                    "must be greater than "
                    "-beginRun which is set to %d\n",
                    runId,opts.beginRun);
            *ok = false;
        }
    }
}

//
// Just a simple list of vars grabbed off the MonteModel col headerData
//
QStandardItemModel* createVarsModel(MonteModel *mm)
{
    QStandardItemModel* pm = new QStandardItemModel(0,1);

    QStringList varList;
    for ( int c = 1; c < mm->columnCount(); ++c) {
        QString var = mm->headerData(c,Qt::Horizontal).toString();
        varList.append(var);
    }

    varList.sort();

    QStandardItem *rootItem = pm->invisibleRootItem();
    for ( int i = 0; i < varList.size(); ++i) {
        QStandardItem *varItem = new QStandardItem(varList.at(i));
        rootItem->appendRow(varItem);
    }

    return pm;
}
