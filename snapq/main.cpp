#include <QApplication>
#include <QObject>
#include <QSet>
#include <QHash>
#include <QString>

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
bool writeTrk(const QString& ftrk,
              QStringList& paramList, MonteModel* monteModel);

Option::FPresetQStringList presetRunsDPs;
Option::FPostsetQStringList postsetRunsDPs;

class SnapOptions : public Options
{
  public:
    QString presentation;
    unsigned int beginRun;
    unsigned int endRun;
    QString pdfOutFile;
    QString trkOutFile;
    QStringList rundps;
    QString title1;
    QString title2;
    QString title3;
    QString title4;
    QString timeName;
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
    opts.add("-trk", &opts.trkOutFile, QString(""),
             "Produce *.trk with variables from DP_Products");
    opts.add("-timeName", &opts.timeName, QString(""),
             "Time name for RUN data");
    opts.parse(argc,argv, QString("snapq"), &ok);

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

        bool isTrk = false;
        if ( !opts.trkOutFile.isEmpty() ) {
            isTrk = true;
        }

        bool isPdf = false;
        if ( !opts.pdfOutFile.isEmpty() ) {
            isPdf = true;
        }

        if ( isPdf && isTrk ) {
            fprintf(stderr,
                    "snap [error] : you may not use the -pdf and -trk "
                    "options together.");
            exit(-1);
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

        // If outputting to pdf, you must have a DP file and RUN dir
        if ( isPdf && (dps.size() == 0 || runDirs.size() == 0) ) {
            fprintf(stderr,
                    "snap [error] : when using the -pdf option you must "
                    "specify a DP product file and RUN directory\n");
            exit(-1);
        }

        // If outputting to trk, you must have a DP file and RUN dir
        if ( isTrk && (dps.size() == 0 || runDirs.size() == 0) ) {
            fprintf(stderr,
                    "snap [error] : when using the -trk option you must "
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
            PlotMainWindow w(opts.timeName,
                             opts.presentation, QString(), dps, titles,
                             monteModel, varsModel, monteInputsModel);
            w.show();
            w.savePdf(opts.pdfOutFile);
        } else if ( isTrk ) {

            QStringList params = DPProduct::paramList(dps);

            if ( monteModel->rowCount() == 1 ) {
                writeTrk(opts.trkOutFile, params, monteModel);
            } else {
                fprintf(stderr, "snap [error]: Only one RUN allowed with "
                                 "the -trk option.\n");
                exit(-1);
            }

        } else {
            if ( dps.size() > 0 ) {
                PlotMainWindow w(opts.timeName,
                                 opts.presentation, ".", dps, titles,
                                 monteModel, varsModel, monteInputsModel);
                w.show();
                ret = a.exec();
            } else {

                PlotMainWindow w(opts.timeName,
                               opts.presentation, runDirs.at(0), QStringList(),
                               titles, monteModel, varsModel, monteInputsModel);
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
                    "snapq [error] : couldn't find file/directory: \"%s\".\n",
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

bool writeTrk(const QString& ftrk,
              QStringList& paramList, MonteModel* monteModel)
{
    QFileInfo ftrki(ftrk);
    if ( ftrki.exists() ) {
        fprintf(stderr, "snapq [error]: Will not overwrite %s\n",
                ftrk.toAscii().constData());
        return false;
    }

    fprintf(stdout, "\nsnap [info]: extracting the following params "
                    "into %s:\n\n",
                    ftrk.toAscii().constData());
    foreach ( QString param, paramList ) {
        fprintf(stdout, "    %s\n", param.toAscii().constData());
    }
    fprintf(stdout, "\n");

    //
    // Make param list
    //
    QString timeParam = paramList.first();
    QList<Param> params;
    foreach ( QString yParam, paramList ) {
        Param p;
        p.name = yParam;
        TrickCurveModel* c = monteModel->curve(0,timeParam,timeParam,yParam);
        if ( !c) {
            fprintf(stderr, "snap [error]: could not find param \"%s\"\n",
                    yParam.toAscii().constData());
            exit(-1);
        }
        QString unit = c->y()->unit();
        p.unit = unit;
        p.type = TRICK_07_DOUBLE;
        p.size = sizeof(double);
        params.append(p);
    }
    if ( params.size() < 2 ) {
        fprintf(stderr,"snap: [error] Could not find any params in RUN that "
                       "are in DP files\n\n");
        return false;
    }

    // Open trk file for writing
    QFile trk(ftrk);
    if (!trk.open(QIODevice::WriteOnly)) {
        fprintf(stderr,"snap: [error] could not open %s\n",
                ftrk.toAscii().constData());
        return false;
    }
    QDataStream out(&trk);

    // Write Trk Header
    TrickModel::writeTrkHeader(out,params);

    // Make list of curves and create hash of next times
    QString yParam0 = paramList.at(1);
    QList<TrickCurveModel*> curves;
    QHash<TrickCurveModel*,int> curve2idx;
    QHash<TrickCurveModel*,int> curve2rc;
    int j = 0;
    foreach ( QString yParam, paramList ) {
        TrickCurveModel* c = 0;
        if ( j == 0 ) {
            // Timestamp is in first curve
            c = monteModel->curve(0,timeParam,timeParam,yParam0);
        } else {
            c = monteModel->curve(0,timeParam,timeParam,yParam);
        }
        curves.append(c);
        curve2idx.insert(c,-1);
        int rc = c->rowCount();
        if ( rc == 0 ) {
            // No data
            fprintf(stderr, "snap [error]: no data found in %s\n",
                    c->tableName().toAscii().constData());
            return false;
        }
        curve2rc.insert(c,c->rowCount());
        ++j;
    }

    while ( 1 ) {

        //
        // Calc next time
        //
        bool isFinished = true;
        double nextTime = 1.0e20;
        foreach ( TrickCurveModel* c, curves ) {
            c->map();
            TrickModelIterator it = c->begin();
            int i = curve2idx.value(c);
            double t;
            if ( i+1 < curve2rc.value(c) ) {
                t = it[i+1].t();
                isFinished = false;
            } else {
                t = 1.0e30;
            }
            if ( t < nextTime ) {
                nextTime = t;
            }
            c->unmap();
        }
        if ( isFinished ) {
            break;
        }

        // Increment curve idx if curve's next time matches nextTime
        foreach ( TrickCurveModel* c, curves ) {
            c->map();
            int i = curve2idx.value(c);
            TrickModelIterator it = c->begin();
            if ( i+1 < curve2rc.value(c) ) {
                double t = it[i+1].t();
                if ( qAbs(nextTime-t) < 1.0e-9 ) {
                    curve2idx[c] = i+1;
                }
            }
            c->unmap();
        }

        // Write the values
        int j = 0;
        foreach ( TrickCurveModel* c, curves ) {
            c->map();
            TrickModelIterator it = c->begin();
            int i = curve2idx.value(c);
            if ( i < 0 ) {
                // This is for the case when curves begin at different times
                i = 0;
            }
            double val = 0.0;
            if ( j == 0 ) {
                val = nextTime;
            } else {
                val = it[i].y();
            }
            out << val;
            c->unmap();
            ++j;
        }
    }

    //
    // Clean up
    //
    trk.close();

    return true;
}
