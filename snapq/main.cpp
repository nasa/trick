#include <QApplication>
#include <QObject>
#include <QSet>

#include <string>
using namespace std;

#include <QDebug>
#include <QDir>
#include <stdio.h>

#include "libopts/options.h"
#include "libsnapdata/runs.h"
#include "libqplot/plotmainwindow.h"

QStandardItemModel* createVarsModel(const QStringList& runDirs);

Option::FPresetQStringList presetRunDirs;
Option::FPostsetQStringList postsetRunDirs;
Option::FPresetUInt presetBeginRun;
Option::FPresetUInt presetEndRun;

class SnapOptions : public Options
{
  public:
    QStringList runDirs;
    unsigned int beginRun;
    unsigned int endRun;
    QString title1;
    QString title2;
    QString title3;
    QString title4;
};

SnapOptions opts;


int main(int argc, char *argv[])
{
    bool ok;
    int ret = -1;

    opts.add("<MONTE_dir|RUN_dirs>:+", &opts.runDirs, QStringList(),
             "MONTE_dir or RUN_directories with RUNs",
             presetRunDirs, postsetRunDirs);
    opts.add("-t1",&opts.title1,"Snap Plot", "Main title");
    opts.add("-t2",&opts.title2,"", "Subtitle");
    opts.add("-t3",&opts.title3,"", "User title");
    opts.add("-t4",&opts.title4,"", "Date title");
    opts.add("-beginRun",&opts.beginRun,0,
             "begin run (inclusive) in set of Monte carlo RUNs",
             presetBeginRun);
    opts.add("-endRun",&opts.endRun,100000,
             "end run (inclusive) in set of Monte carlo RUNs",
             presetEndRun);
    opts.parse(argc,argv, QString("snapq"), &ok);

    if ( !ok ) {
        qDebug() << opts.usage();
        return -1;
    }

    try {

        QApplication::setGraphicsSystem("raster");
        QApplication a(argc, argv);

        bool isMonte = false;
        if ( opts.runDirs.size() == 1 ) {
            QFileInfo fileInfo(opts.runDirs.at(0));
            if ( fileInfo.fileName().startsWith("MONTE_") ) {
                isMonte = true;
            }
        }

        MonteModel* monteModel = 0;
        QStandardItemModel* varsModel = 0;
        QStandardItemModel* monteInputsModel = 0;
        Runs* runs = 0;

        if ( isMonte ) {
            Monte* monte = new Monte(opts.runDirs.at(0),
                                     opts.beginRun,opts.endRun);
            monteInputsModel = monte->inputModel();
            monteModel = new MonteModel(monte);
            QString run0 =  opts.runDirs.at(0) + "/" + monte->runs().at(0);
            QStringList runDirs; runDirs << run0;
            varsModel = createVarsModel(runDirs);
        } else {
            runs = new Runs(opts.runDirs);
            monteModel = new MonteModel(runs);
            varsModel = createVarsModel(opts.runDirs);
        }

        // Any parameters common between runs?
        if ( varsModel->rowCount() == 1 ) {
            QModelIndex idx0 = varsModel->index(0,0);
            QString param = varsModel->data(idx0).toString();
            if ( param == "sys.exec.out.time" ) {
                qDebug() << "snap [error]: No common parameters found between "
                            "runs.  Aborting.";
                return -1;
            }
        }

        // Make a list of titles
        QStringList titles;
        titles << opts.title1 << opts.title2 << opts.title3 << opts.title4;

        PlotMainWindow w(opts.runDirs.at(0), titles,
                         monteModel, varsModel, monteInputsModel);
        w.show();
        ret = a.exec();

        delete varsModel;
        delete monteModel;
        delete monteInputsModel;
        delete runs;

    } catch (std::exception &e) {
        fprintf(stderr,"\n%s\n",e.what());
        fprintf(stderr,"%s\n",opts.usage().toAscii().constData());
        exit(-1);
    }

    return ret;
}

void presetRunDirs(QStringList* defRunDirs,
                   const QStringList& runDirs,bool* ok)
{
    Q_UNUSED(defRunDirs);

    foreach ( QString dirName, runDirs ) {
        QDir dir(dirName);
        if ( ! dir.exists() ) {
            fprintf(stderr,"snapq [error] : couldn't find directory: \"%s\".\n",
                    dirName.toAscii().constData());
            *ok = false;
            return;
        }
    }

    if ( runDirs.size() > 1 ) {
        foreach ( QString dirName, runDirs ) {
            QFileInfo info(dirName);
            if ( info.fileName().startsWith("MONTE_") ) {
                fprintf(stderr,"snapq [error] : when plotting a MONTE dir "
                        "only one MONTE dir can be specified with no "
                        "additional RUNs\n");
                *ok = false;
                return;
            }
        }
    }
}

// Remove trailing /s on dir names
void postsetRunDirs (QStringList* runDirs, bool* ok)
{
    Q_UNUSED(ok);
    QStringList dirs = runDirs->replaceInStrings(QRegExp("/*$"), "");
    Q_UNUSED(dirs);
}

void presetBeginRun(uint* beginRunId, uint runId, bool* ok)
{
    Q_UNUSED(beginRunId);

    if ( runId > opts.endRun ) {
        fprintf(stderr,"snap [error] : option -beginRun, set to %d, "
                "must be greater than "
                " -endRun which is set to %d\n",
                runId, opts.endRun);
        *ok = false;
    }
}

void presetEndRun(uint* endRunId, uint runId, bool* ok)
{
    Q_UNUSED(endRunId);

    if ( runId < opts.beginRun ) {
        fprintf(stderr,"snap [error] : option -endRun, set to %d, "
                "must be greater than "
                "-beginRun which is set to %d\n",
                runId,opts.beginRun);
        *ok = false;
    }
}

QStandardItemModel* createVarsModel(const QStringList& runDirs)
{
    if ( runDirs.isEmpty() ) return 0;

    QStringList commonParams;

    QSet<QString> paramSet;
    foreach ( QString runDir, runDirs ) {
        QStringList runList;
        runList << runDir;
        Runs run(runList);
        QSet<QString> runParamSet = run.params().toSet();
        if ( paramSet.isEmpty() ) {
            paramSet = runParamSet;
        } else {
            paramSet = paramSet.intersect(runParamSet);
        }
    }

    foreach ( QString param, paramSet ) {
        // Don't add sys.exec.out.time to list since
        // there's no need to plot sys.exec.out.time
        if ( param == "sys.exec.out.time" ) continue;
        commonParams << param;
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
