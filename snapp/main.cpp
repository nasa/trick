#include <QApplication>
#include <QObject>
#include <QSet>

#include <string>
using namespace std;

#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <stdio.h>

#include "libopts/options.h"
#include "libsnapdata/runs.h"
#include "libqplot/plotmainwindow.h"

QStandardItemModel* createVarsModel(const QStringList& runDirs);

Option::FPresetQStringList presetRunsDPs;
Option::FPostsetQStringList postsetRunsDPs;

class SnapOptions : public Options
{
  public:
    QString presentation;
    unsigned int beginRun;
    unsigned int endRun;
    QString pdfOutFile;
    QStringList rundps;
    QString title1;
    QString title2;
    QString title3;
    QString title4;
};

SnapOptions opts;

Option::FPresetQString presetPresentation;
Option::FPresetUInt presetBeginRun;
Option::FPresetUInt presetEndRun;

int main(int argc, char *argv[])
{
    bool ok;
    int ret = -1;

    opts.add("<RUN_dirs and DP_files>:+", &opts.rundps, QStringList(),
             "List of RUN dirs and DP files",
             presetRunsDPs, postsetRunsDPs);
    opts.add("-pres",&opts.presentation,"error",
             "present plot with two curves as compare or error (default)",
             presetPresentation);
    opts.add("-beginRun",&opts.beginRun,0,
             "begin run (inclusive) in set of Monte carlo RUNs",
             presetBeginRun);
    opts.add("-endRun",&opts.endRun,100000,
             "end run (inclusive) in set of Monte carlo RUNs",
             presetEndRun);
    opts.add("-t1",&opts.title1,"", "Main title");
    opts.add("-t2",&opts.title2,"", "Subtitle");
    opts.add("-t3",&opts.title3,"", "User title");
    opts.add("-t4",&opts.title4,"", "Date title");
    opts.add("-pdf", &opts.pdfOutFile, QString(""),
             "Name of pdf output file");
    opts.parse(argc,argv, QString("snapp"), &ok);

    if ( !ok ) {
        qDebug() << opts.usage();
        return -1;
    }

    try {

        QApplication::setGraphicsSystem("raster");
        QApplication a(argc, argv);

        MonteModel* monteModel = 0;
        QStandardItemModel* varsModel = 0;
        QStandardItemModel* monteInputsModel = 0;
        Runs* runs = 0;

        bool isPdf = false;
        if ( !opts.pdfOutFile.isEmpty() ) {
            isPdf = true;
        }

        QStringList dps;
        QStringList runDirs;
        foreach ( QString f, opts.rundps ) {
            if ( f.startsWith("DP_") ) {
                dps << f;
            } else {
                runDirs << f;
            }
        }

        // If outputting to pdf, you must have a DP file
        if ( isPdf && (dps.size() == 0 || runDirs.size() == 0) ) {
            fprintf(stderr,
                    "snap [error] : when using the -pdf option you must "
                    "specify a DP product file and RUN directory\n");
            exit(-1);
        }

        bool isMonte = false;
        if ( runDirs.size() == 1 ) {
            QFileInfo fileInfo(runDirs.at(0));
            if ( fileInfo.fileName().startsWith("MONTE_") ) {
                isMonte = true;
            }
        }


        if ( isMonte ) {
            Monte* monte = new Monte(runDirs.at(0),opts.beginRun,opts.endRun);
            monteInputsModel = monte->inputModel();
            monteModel = new MonteModel(monte);
            QString run0 = runDirs.at(0) + "/" + monte->runs().at(0);
            QStringList runDirs0; runDirs0 << run0;
            varsModel = createVarsModel(runDirs0);
        } else {
            runs = new Runs(runDirs);
            monteModel = new MonteModel(runs);
            varsModel = createVarsModel(runDirs);
        }

        // Make a list of titles
        QStringList titles;
        QString subtitle = opts.title2;
        if ( subtitle.isEmpty() ) {
            foreach ( QString runDir, runDirs ) {
                subtitle += runDir + ",\n";
            }
            subtitle.chop(2);
        }
        titles << opts.title1 << subtitle << opts.title3 << opts.title4;

        if ( isPdf ) {
            PlotMainWindow w(opts.presentation, dps, titles,
                             monteModel, varsModel, monteInputsModel);
            w.savePdf(opts.pdfOutFile);
        } else {
            if ( dps.size() > 0 ) {
                PlotMainWindow w(opts.presentation, dps, titles,
                                 monteModel, varsModel, monteInputsModel);
                w.show();
                ret = a.exec();
            } else {

                PlotMainWindow w(opts.presentation, runDirs.at(0), titles,
                                 monteModel, varsModel, monteInputsModel);
                w.show();
                ret = a.exec();
            }
        }

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

void presetRunsDPs(QStringList* defRunDirs,
                   const QStringList& rundps,bool* ok)
{
    Q_UNUSED(defRunDirs);

    foreach ( QString f, rundps ) {
        QFileInfo fi(f);
        if ( !fi.exists() ) {
            fprintf(stderr,
                    "snapp [error] : couldn't find file/directory: \"%s\".\n",
                    f.toAscii().constData());
            *ok = false;
            return;
        }
    }
}

// Remove trailing /s on dir names
void postsetRunsDPs (QStringList* rundps, bool* ok)
{
    Q_UNUSED(ok);
    QStringList dirs = rundps->replaceInStrings(QRegExp("/*$"), "");
    Q_UNUSED(dirs);
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

void presetPresentation(QString* presVar, const QString& pres, bool* ok)
{
    Q_UNUSED(presVar);

    if ( pres != "error" && pres != "compare" ) {
        fprintf(stderr,"snap [error] : option -presentation, set to \"%s\", "
                "should be \"error\" or \"compare\"\n",
                pres.toAscii().constData());
        *ok = false;
    }
}
