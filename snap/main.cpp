#include <QApplication>
#include <QObject>
#include <QSet>
#include <QHash>
#include <QString>
#include <QDate>

#include <string>
using namespace std;

#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QTextStream>
#include <stdio.h>

#include "libsnap/options.h"
#include "libsnap/runs.h"
#include "libsnap/plotmainwindow.h"
#include "libsnap/roundoff.h"
#ifdef __linux
#include "libsnap/timeit_linux.h"
#endif
#include "libsnap/timestamps.h"
#include "libsnap/tricktablemodel.h"
#include "libsnap/dp.h"
#include "libsnap/snap.h"
#include "libsnap/csv.h"

QStandardItemModel* createVarsModel(const QStringList& runDirs);
bool writeTrk(const QString& ftrk, const QString &timeName,
               QStringList& paramList, MonteModel* monteModel);
bool writeCsv(const QString& fcsv, const QString& timeName,
              DPTable* dpTable, const QString &runDir);
bool convert2csv(const QString& ftrk, const QString& fcsv);
bool convert2trk(const QString& csvFileName, const QString &trkFileName);

Option::FPresetQString presetExistsFile;
Option::FPresetDouble preset_start;
Option::FPresetDouble preset_stop;
Option::FPresetQStringList presetRunsDPs;
Option::FPostsetQStringList postsetRunsDPs;
Option::FPresetQString presetPresentation;
Option::FPresetUInt presetBeginRun;
Option::FPresetUInt presetEndRun;
Option::FPresetQString presetOutputFile;

class SnapOptions : public Options
{
  public:
    QStringList rundps;
    double start;
    double stop;
    bool isReportRT;
    QString presentation;
    unsigned int beginRun;
    unsigned int endRun;
    QString pdfOutFile;
    QString trkOutFile;
    QString csvOutFile;
    QString title1;
    QString title2;
    QString title3;
    QString title4;
    QString timeName;
    QString trk2csvFile;
    QString csv2trkFile;
    QString outputFileName;
};

SnapOptions opts;


int main(int argc, char *argv[])
{
    bool ok;
    int ret = -1;

    opts.add("[RUNs and DPs:{0,2000}]",
             &opts.rundps, QStringList(),
             "List of RUN dirs and DP files",
             presetRunsDPs, postsetRunsDPs);
    opts.add("-rt:{0,1}",&opts.isReportRT,false, "print realtime text report");
    opts.add("-start", &opts.start, 0.0, "start time", preset_start);
    opts.add("-stop", &opts.stop,1.0e20, "stop time", preset_stop);
    opts.add("-pres",&opts.presentation,"",
             "present plot with two curves as coplot,error or error+coplot",
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
    opts.add("-timeName", &opts.timeName, QString(""),
             "Time name for RUN data");
    opts.add("-pdf", &opts.pdfOutFile, QString(""),
             "Name of pdf output file");
    opts.add("-dp2trk", &opts.trkOutFile, QString(""),
             "Produce *.trk with variables from DP_Products");
    opts.add("-dp2csv", &opts.csvOutFile, QString(""),
             "Produce *.csv tables from from DP_Product tables");
    opts.add("-trk2csv", &opts.trk2csvFile, QString(""),
             "Name of trk file to convert to csv (fname subs trk with csv)",
             presetExistsFile);
    opts.add("-csv2trk", &opts.csv2trkFile, QString(""),
             "Name of csv file to convert to trk (fname subs csv with trk)",
             presetExistsFile);
    opts.add("-o", &opts.outputFileName, QString(""),
             "Name of file to output with trk2csv and csv2trk options",
             presetOutputFile);

    opts.parse(argc,argv, QString("snap"), &ok);

    if ( !ok ) {
        fprintf(stderr,"%s\n",opts.usage().toLatin1().constData());
        return -1;
    }

    QStringList dps;
    QStringList runDirs;
    foreach ( QString f, opts.rundps ) {
        QFileInfo fi(f);
        if ( fi.fileName().startsWith("DP_") ) {
            dps << f;
        } else {
            runDirs << f;
        }
    }

    if ( opts.rundps.isEmpty() ) {
        if ( opts.trk2csvFile.isEmpty() && opts.csv2trkFile.isEmpty() ) {
            fprintf(stderr,"snap [error] : no RUNs specified\n");
            fprintf(stderr,"%s\n",opts.usage().toLatin1().constData());
            return -1;
        }
    }

    if ( !opts.trk2csvFile.isEmpty() ) {
        QString csvOutFile = opts.outputFileName;
        if ( csvOutFile.isEmpty() ) {
            QFileInfo fi(opts.trk2csvFile);
            csvOutFile = fi.absolutePath() + "/" +
                         QString("%1.csv").arg(fi.baseName());
        }
        bool ret = convert2csv(opts.trk2csvFile, csvOutFile);
        if ( !ret )  {
            fprintf(stderr, "snap [error]: Aborting trk to csv conversion!\n");
            return -1;
        }
    }

    if ( !opts.csv2trkFile.isEmpty() ) {
        QString trkOutFile = opts.outputFileName;
        if ( trkOutFile.isEmpty() ) {
            QFileInfo fi(opts.csv2trkFile);
            trkOutFile = fi.absolutePath() + "/" +
                         QString("%1.trk").arg(fi.baseName());
        }
        bool ret = convert2trk(opts.csv2trkFile, trkOutFile);
        if ( !ret )  {
            fprintf(stderr, "snap [error]: Aborting csv to trk conversion!\n");
            return -1;
        }
    }

    try {
        if ( opts.isReportRT ) {
            foreach ( QString run, runDirs ) {
                Snap snap(run,opts.start,opts.stop);
                SnapReport rpt(snap);
                fprintf(stderr,"%s",rpt.report().toLatin1().constData());
            }
        }
    } catch (std::exception &e) {
        fprintf(stderr,"\n%s\n",e.what());
        fprintf(stderr,"%s\n",opts.usage().toLatin1().constData());
        exit(-1);
    }


    if ( opts.isReportRT || !opts.csv2trkFile.isEmpty()
         || !opts.trk2csvFile.isEmpty() ) {
        return 0;
    }

    try {

        //QApplication::setGraphicsSystem("raster");
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

        bool isCsv = false;
        if ( !opts.csvOutFile.isEmpty() ) {
            isCsv = true;
        }

        if ( (isPdf && isTrk) || (isPdf && isCsv) || (isTrk && isCsv) ) {
            fprintf(stderr,
                    "snap [error] : you may not use the -pdf, -trk and -csv "
                    "options together.");
            exit(-1);
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

        // If outputting to csv, you must have a DP file and RUN dir
        if ( isCsv && (dps.size() == 0 || runDirs.size() == 0) ) {
            fprintf(stderr,
                    "snap [error] : when using the -csv option you must "
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
        QString title = opts.title1;
        if ( title.isEmpty() ) {
            title  = "Snap Plots!";
        }
        titles << title;

        // Default title2 to RUN names
        title = opts.title2;
        if ( title.isEmpty() ) {
            title = "(";
            foreach ( QString runDir, runDirs ) {
                title += runDir + ",\n";
            }
            title.chop(2);
            title += ")";
        }
        titles << title;

        // Default title3 to username
        title = opts.title3;
        if ( title.isEmpty() ) {
            QFileInfo f(".");
            QString userName = f.owner();
            title = "User: " + userName;
        }
        titles << title;

        // Default title4 to date
        title = opts.title4;
        if ( title.isEmpty() ) {
            QDate date = QDate::currentDate();
            QString fmt("Date: MMMM d, yyyy");
            QString dateStr = date.toString(fmt);
            title = dateStr;
        }
        titles << title;

        // Default presentation
        QString presentation = opts.presentation;
        if ( opts.presentation.isEmpty() ) {
            presentation = "coplot";
            int rc = monteModel->rowCount();
            if ( rc == 2 ) {
                presentation = "error";
            }
        }

        if ( isPdf ) {
            // TODO: Shouldn't timeName use opts.timeName if specified?
            PlotMainWindow w(opts.timeName, opts.start, opts.stop,
                             presentation, QString(), dps, titles,
                             monteModel, varsModel, monteInputsModel);
            w.savePdf(opts.pdfOutFile);

        } else if ( isTrk ) {

            QStringList params = DPProduct::paramList(dps);

            if ( monteModel->rowCount() == 1 ) {
                QString timeName = opts.timeName ;
                if ( timeName.isEmpty() ) {
                    timeName = "sys.exec.out.time" ;
                }
                bool r = writeTrk(opts.trkOutFile,timeName,params,monteModel);
                if ( r ) {
                    ret = 0;
                } else {
                    fprintf(stderr, "snap [error]: Failed to write: %s\n",
                            opts.trkOutFile.toLatin1().constData());
                    ret = -1;
                }
            } else {
                fprintf(stderr, "snap [error]: Only one RUN allowed with "
                                 "the -trk option.\n");
                exit(-1);
            }

        } else if ( isCsv ) {


            if ( runDirs.size() != 1 ) {
                fprintf(stderr, "snap [error]: Exactly one RUN dir must be "
                                "specified with the -csv option.\n");
                exit(-1);
            }

            // TODO: This code for timeName repeated from above (consolidate)
            QString timeName = opts.timeName ;
            if ( timeName.isEmpty() ) {
                timeName = "sys.exec.out.time" ;
            }

            // If there are no tables, print a message
            int nTables = 0;
            foreach ( QString dpFileName, dps ) {
                DPProduct dp(dpFileName);
                nTables += dp.tables().size();
            }
            if ( nTables == 0 ) {
                fprintf(stderr, "snap [error]: In order to create csv files "
                        "using the -dp2csv option, there must be "
                        "data product TABLEs in the DP files.\n");
                exit(-1);
            }

            int i = 0;
            foreach ( QString dpFileName, dps ) {
                DPProduct dp(dpFileName);
                foreach ( DPTable* dpTable, dp.tables() ) {
                    QString fname = opts.csvOutFile;
                    if ( dp.tables().size() > 1 ) {
                        // Multiple files to output, so index the name
                        QString dpName = QFileInfo(dpFileName).baseName();
                        QFileInfo fi(fname);
                        QString extension("csv");
                        if ( !fi.suffix().isEmpty() ) {
                            extension = fi.suffix();
                        }
                        fname = fi.completeBaseName() +
                                "_" +
                                dpName +
                                QString("_%1.").arg(i) +
                                extension;
                        ++i;
                    }

                    bool r = writeCsv(fname,timeName,dpTable,runDirs.at(0));
                    if ( r ) {
                        ret = 0;
                    } else {
                        fprintf(stderr, "snap [error]: Failed to write: %s\n",
                                fname.toLatin1().constData());
                        ret = -1;
                        break;
                    }
                }
                if ( ret == -1 ) {
                    break;
                }
            }


        } else {
            if ( dps.size() > 0 ) {
#ifdef __linux
                TimeItLinux timer;
                timer.start();
#endif
                PlotMainWindow w(opts.timeName, opts.start, opts.stop,
                                 presentation, ".", dps, titles,
                                 monteModel, varsModel, monteInputsModel);
#ifdef __linux
                timer.snap("time=");
#endif
                w.show();
                ret = a.exec();
            } else {

                PlotMainWindow w(opts.timeName, opts.start, opts.stop,
                               presentation, runDirs.at(0), QStringList(),
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
        fprintf(stderr,"%s\n",opts.usage().toLatin1().constData());
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
                    f.toLatin1().constData());
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

    if ( !pres.isEmpty() && pres != "coplot" && pres != "error" &&
         pres != "error+coplot" ) {
        fprintf(stderr,"snap [error] : option -presentation, set to \"%s\", "
                "should be \"coplot\", \"error\" or \"error+coplot\"\n",
                pres.toLatin1().constData());
        *ok = false;
    }
}

void presetOutputFile(QString* presVar, const QString& fname, bool* ok)
{
    Q_UNUSED(presVar);

    QFileInfo fi(fname);
    if ( fi.exists() ) {
        fprintf(stderr, "snap [error]: Will not overwrite %s\n",
                fname.toLatin1().constData());
        *ok = false;
    }
}

bool writeTrk(const QString& ftrk, const QString& timeName,
               QStringList& paramList, MonteModel* monteModel)
{
    QFileInfo ftrki(ftrk);
    if ( ftrki.exists() ) {
        fprintf(stderr, "snapq [error]: Will not overwrite %s\n",
                ftrk.toLatin1().constData());
        return false;
    }

    fprintf(stdout, "\nsnap [info]: extracting the following params "
                    "into %s:\n\n",
                    ftrk.toLatin1().constData());
    foreach ( QString param, paramList ) {
        fprintf(stdout, "    %s\n", param.toLatin1().constData());
    }
    fprintf(stdout, "\n");

    //
    // Make param list
    // And make curves list (based on param list)
    //
    QList<Param> params;
    QList<TrickCurveModel*> curves;

    // Time is first "param"
    Param timeParam;
    timeParam.name = timeName;
    timeParam.unit = "s";
    timeParam.type = TRICK_07_DOUBLE;
    timeParam.size = sizeof(double);
    params << timeParam;

    // Each param gets a curve. Make the first curve null since
    // there is no actual curve to go with timeStamps
    curves << 0;

    foreach ( QString yParam, paramList ) {

        // If time is in param list, then skip it  since timestamps
        // are generated and inserted into the first column of the trk
        if ( yParam == timeName ) {
            continue;
        }

        TrickCurveModel* c = monteModel->curve(0,timeName,timeName,yParam);

        // Error check: see if MonteModel could not find curve (timeName,yParam)
        if ( !c) {
            fprintf(stderr, "snap [error]: could not find curve: \n    ("
                    "%s,%s)\n",
                    timeName.toLatin1().constData(),
                    yParam.toLatin1().constData());
            return false;
        }

        // Map Curve
        c->map();

        // Error check:   make sure curve has data
        if ( c->rowCount() == 0 ) {
            // No data
            fprintf(stderr, "snap [error]: no data found in %s\n",
                    c->tableName().toLatin1().constData());
            return false;
        }

        // Error check: first timestamp should be 0.0
        int i = c->indexAtTime(0.0);
        if ( i != 0 ) {
            // 0.0 is not first time - yield error
            fprintf(stderr, "snap [error]: start time in data is not 0.0 "
                            " for the following trk\n    %s\n",
                    c->trkFile().toLatin1().constData());
            return false;
        }

        // Make a Param (for trk header)
        Param p;
        p.name = yParam;
        p.unit = c->y()->unit();
        p.type = TRICK_07_DOUBLE;
        p.size = sizeof(double);

        // Make params/curves lists (lazily mapping params to curves)
        params.append(p);
        curves.append(c);

        // Unmap Curve
        c->unmap();
    }
    if ( params.size() < 2 ) {
        fprintf(stderr,"snap [error]: Could not find any params in RUN that "
                       "are in DP files\n\n");
        return false;
    }


    // Guess frequency foreach curve
    // Also get min start and max stop time
    // Use first 10 time stamps
    //
    // Assuming (checked above) that first timestamp == 0.0
    //    If each curve can begin at its own time, there is addeded complexity.
    //    For example:
    //    f0=0.25 c0 = (-0.65, -0.4, -0.15, 0.1, 0.35...)
    //    f1=0.10 c1 = (0.0, 0.1, 0.2, 0.3...)
    //    If one begins at -0.65, f1's genned timestamps would be off:
    //             f1's timestamps (-0.65, -0.55,..., -0.05, 0.05...) is bad
    //
    double stopTime = -1.0e20;
    QHash<TrickCurveModel*, double> curve2freq;
    QList<double> freqs;
    foreach ( TrickCurveModel* curve, curves ) {

        if ( !curve ) continue ; // skip time "curve"

        curve->map();
        TrickModelIterator it = curve->begin();

        int rc = curve->rowCount();
        if ( rc > 1 ) {
            double f = it[1].t();  // since it[0].t() == 0 and assume reg freq
            double s = it[0].t();
            double e = it[rc-1].t();
            double d = e - s ;    // Actual delta time
            double a = (rc-1)*f;  // approximately delta time using freqency

            if ( qAbs(d-a) > 1.0e-6 ) {
                fprintf(stderr,
                        "snap [error]: frequency for data in %s is "
                        "irregular.  RUN timeline is [%.8lf-%.8lf].  "
                        "Frequency is assumed to be t[1] == %.8lf.  "
                        "Number of data points, np == %d.  "
                        "Assuming regular frequency, the following should be "
                        "true: (np-1)*f == endTime-startTime.  It is not.  "
                        "(%d-1)*%.8lf == %.8lf != %.8lf\n",
                        curve->trkFile().toLatin1().constData(),
                        s,e,f,rc,rc,f,d,a);
                return false;
            }

            if ( e > stopTime ) stopTime = e;

            ROUNDOFF(f,f);
            freqs.append(f);
            curve2freq.insert(curve,f);
        }
        curve->unmap();
    }
    TimeStamps::usort(freqs);

    // Create list of timestamps based on frequency of data of each curve.
    // This assumes that each curve starts at 0.0
    // This is the first "curve"
    TimeStamps timeStamps(freqs,stopTime);

    // Open trk file for writing
    QFile trk(ftrk);
    if (!trk.open(QIODevice::WriteOnly)) {
        fprintf(stderr,"snap: [error] could not open %s\n",
                ftrk.toLatin1().constData());
        return false;
    }
    QDataStream out(&trk);

    // Write Trk Header
    TrickModel::writeTrkHeader(out,params);
    trk.flush();

    // Resize Trk file to fit all the data + header
    qint64 headerSize = trk.size();
    qint64 nParams = params.size();
    qint64 recordSize = nParams*sizeof(double);
    qint64 nRecords = timeStamps.count();
    qint64 dataSize = nRecords*recordSize;
    qint64 fileSize = headerSize + dataSize;
    trk.resize(fileSize);

    int i = 0;
    foreach ( TrickCurveModel* curve, curves ) {

        int nTimeStamps = timeStamps.count();

        if ( !curve ) {
            // write time stamps
            for ( int j = 0 ; j < nTimeStamps; ++j ) {
                qint64 recordOffset = j*recordSize;
                qint64 paramOffset = 0;
                qint64 offset = headerSize + recordOffset + paramOffset;
                trk.seek(offset);
                out << timeStamps.at(j);
            }
        } else {
            curve->map();
            TrickModelIterator it = curve->begin();
            TrickModelIterator e = curve->end();
            double lastValue = 0.0; // if j == 0, lastValue inited at bottom
            double lastTime = 0.0;  // start time assumed to be 0.0
            for ( int j = 0 ; j < nTimeStamps; ++j ) {
                double timeStamp = timeStamps.at(j);
                double t = it.t();
                double v;
                if ( t-timeStamp < 1.0e-9 ) {
                    // Curve time is on or just greater than timeline timestamp
                    if ( it != e ) {
                        v = it.y();
                    } else {
                        // The timeline's stoptime is > than curve stoptime,
                        // let value be last value in curve
                        int rc = curve->rowCount();
                        v = it[rc-1].y();
                    }
                    ++it;
                } else {
                    // Curve time is not on timeline
                    double fExpected = curve2freq.value(curve);
                    double fActual = t-lastTime;
                    if ( qAbs(t-lastTime) > 1.0e-9 &&
                         qAbs(fExpected-fActual) > 1.0e-9 ) {
                        fprintf(stderr,
                                "snap [error]: File %s has bad timestamp "
                                " %.8lf.  This timestamp is not a multiple "
                                "of the expected frequency %.8lf. \n",
                                curve->trkFile().toLatin1().constData(),
                                t, fExpected);
                        curve->unmap();
                        trk.close();
                        return false;
                    }
                    v = lastValue;
                }
                qint64 recordOffset = j*recordSize;
                qint64 paramOffset = i*sizeof(double);
                qint64 offset = headerSize + recordOffset + paramOffset;
                trk.seek(offset);
                out << v;
                lastValue = v;
                lastTime = t;
            }
            curve->unmap();
        }
        ++i;
    }

    //
    // Clean up
    //
    trk.close();

    return true;
}

bool writeCsv(const QString& fcsv, const QString& timeName,
              DPTable* dpTable, const QString& runDir)
{
    QFileInfo fcsvi(fcsv);
    if ( fcsvi.exists() ) {
        fprintf(stderr, "snapq [error]: Will not overwrite %s\n",
                fcsv.toLatin1().constData());
        return false;
    }

    // Open trk file for writing
    QFile csv(fcsv);
    if (!csv.open(QIODevice::WriteOnly)) {
        fprintf(stderr,"snap: [error] could not open %s\n",
                fcsv.toLatin1().constData());
        return false;
    }
    QTextStream out(&csv);

    // Format output
    out.setFieldAlignment(QTextStream::AlignRight);
    out.setFieldWidth(12);
    out.setPadChar(' ');
    out.setRealNumberPrecision(8);

    // Csv header
    QString header;
    header = timeName + ",";
    foreach ( DPVar* var, dpTable->vars() ) {
        QString unit("");
        //unit = " {--}"; // TODO: Unit name and unit conversion
        header += var->name() +  unit + ",";
    }
    header.chop(1);
    out << header;
    out << "\n\n";

    // Csv body
    QStringList params;
    foreach ( DPVar* var, dpTable->vars() ) {
        params << var->name() ;
    }


    TrickTableModel ttm(timeName, runDir, params);
    int rc = ttm.rowCount();
    int cc = ttm.columnCount();
    for ( int r = 0 ; r < rc; ++r ) {
        for ( int c = 0 ; c < cc; ++c ) {
            QModelIndex idx = ttm.index(r,c);
            double v = ttm.data(idx).toDouble();
            out << v;
            if ( c < cc-1 ) {
                int fw = out.fieldWidth();
                out.setFieldWidth(0);
                out << ",";
                out.setFieldWidth(fw);
            }
        }
        if ( r < rc-1 ) {
            out << "\n";
        }
    }

    // Clean up
    csv.close();

    return true;
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

void presetExistsFile(QString* ignoreMe, const QString& fname, bool* ok)
{
    Q_UNUSED(ignoreMe);

    QFileInfo fi(fname);
    if ( !fi.exists() ) {
        fprintf(stderr,
                "snap [error] : Couldn't find file: \"%s\".\n",
                fname.toLatin1().constData());
        *ok = false;
        return;
    }
}

bool convert2csv(const QString& ftrk, const QString& fcsv)
{
    TrickModel m("sys.exec.out.time", ftrk);

    QFileInfo fcsvi(fcsv);
    if ( fcsvi.exists() ) {
        fprintf(stderr, "snap [error]: Will not overwrite %s\n",
                fcsv.toLatin1().constData());
        return false;
    }

    // Open csv file stream
    QFile csv(fcsv);
    if (!csv.open(QIODevice::WriteOnly)) {
        fprintf(stderr,"snap: [error] could not open %s\n",
                fcsv.toLatin1().constData());
        return false;
    }
    QTextStream out(&csv);

    // Write csv param list (top line in csv file)
    int cc = m.columnCount();
    for ( int i = 0; i < cc; ++i) {
        QString pName = m.param(i).name();
        QString pUnit = m.param(i).unit();
        out << pName << " {" << pUnit << "}";
        if ( i < cc-1 ) {
            out << ",";
        }
    }
    out << "\n";

    //
    // Write param values
    //
    int rc = m.rowCount();
    for ( int r = 0 ; r < rc; ++r ) {
        for ( int c = 0 ; c < cc; ++c ) {
            QModelIndex idx = m.index(r,c);
            out << m.data(idx).toString();
            if ( c < cc-1 ) {
                out << ",";
            } else {
                out << "\n";
            }
        }
    }

    // Clean up
    csv.close();

    return true;
}

bool convert2trk(const QString& csvFileName, const QString& trkFileName)
{
    QFile file(csvFileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        fprintf(stderr, "csv2trk [error]: Cannot read file %s!",
                csvFileName.toLatin1().constData());
    }
    CSV csv(&file);

    // Parse first line to get param list
    QList<Param> params;
    QStringList list = csv.parseLine() ;
    if ( list.size() == 1 && list.at(0).isEmpty() ) {
        fprintf(stderr, "csv2trk [error]: Empty csv file \"%s\"",
                csvFileName.toLatin1().constData());
        return false;
    }
    foreach ( QString s, list ) {
        Param p;
        QStringList plist = s.split(" ", QString::SkipEmptyParts);
        p.name = plist.at(0);
        if ( plist.size() > 1 ) {
            QString unitString = plist.at(1);
            if ( unitString.startsWith('{') ) {
                unitString = unitString.remove(0,1);
            }
            if ( unitString.endsWith('}') ) {
                unitString.chop(1);
            }
            Unit u;
            if ( u.isUnit(unitString.toLatin1().constData()) ) {
                p.unit = unitString;
            }
        }
        p.type = TRICK_07_DOUBLE;
        p.size = sizeof(double);
        params.append(p);
    }

    QFileInfo ftrki(trkFileName);
    if ( ftrki.exists() ) {
        fprintf(stderr, "snap [error]: Will not overwrite %s\n",
                trkFileName.toLatin1().constData());
        return false;
    }

    QFile trk(trkFileName);

    if (!trk.open(QIODevice::WriteOnly)) {
        fprintf(stderr,"snap [error]: could not open %s\n",
                trkFileName.toLatin1().constData());
        return false;
    }
    QDataStream out(&trk);

    // Write trk header
    TrickModel::writeTrkHeader(out,params);

    //
    // Write param values
    //
    int line = 1;
    while ( 1 ) {
        ++line;
        QStringList list = csv.parseLine() ;
        if ( list.size() == 1 && list.at(0).isEmpty() ) break;
        foreach ( QString s, list ) {
            bool ok;
            double val = s.toDouble(&ok);
            if ( !ok ) {
                QFileInfo fi(csvFileName);
                fprintf(stderr,
                 "snap [error]: Bad value \"%s\" on line %d in file %s\n",
                        s.toLatin1().constData(),
                        line,
                        fi.absoluteFilePath().toLatin1().constData());
                file.close();
                trk.remove();
                return false;
            }
            out << val;
        }
    }

    file.close();

    return true;
}
