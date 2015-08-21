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
#include "libsnapdata/roundoff.h"

#include "libsnapdata/timeit_linux.h"

QStandardItemModel* createVarsModel(const QStringList& runDirs);
bool writeTrk(const QString& ftrk, const QString &timeName,
               QStringList& paramList, MonteModel* monteModel);
double leastCommonMultiple(const QList<double>& xs);
QList<double> uniquify(const QList<double>& list);

QList<double> merge(const QList<double>& listA,
                    const QList<double>& listB);

bool doubleLessThan(double a, double b)
{
    return a-b < 1.0e-9;
}

class TimeStamps {
  public:
    TimeStamps(QList<double> freqs, double stopTime);
    double at(int i);
    int count() { return _timeStamps.count(); }
  private:
    TimeStamps();
    QList<double> _freqs;
    double _stopTime;

    double _lcm;
    QList<double> _timeStampsToLCM;
    QList<double> _timeStamps;
};

TimeStamps::TimeStamps(QList<double> freqs, double stopTime)
{
    QList<double> ufreqs = uniquify(freqs);
    double lcm = leastCommonMultiple(ufreqs);

    foreach ( double f, freqs ) {
        QList<double> ts;
        double k = 1.0;
        while ( 1 ) {
            double t = k*f;
            ROUNDOFF(t,t);
            if ( t > (lcm+1.0e-9) ) {
                break;
            }
            ts << t;
            k = k + 1.0;
            ROUNDOFF(k,k);
        }
        _timeStampsToLCM = merge(_timeStampsToLCM,ts);
    }

    double zero = 0.0;
    ROUNDOFF(zero,zero);
    _timeStamps << zero;

    while ( 1 ) {
        bool isFinished = false;
        double lastTime = _timeStamps.last();
        foreach ( double t, _timeStampsToLCM ) {
            double nt = t + lastTime;
            ROUNDOFF(nt,nt);
            if ( nt > stopTime+1.0e-9 ) {
                isFinished = true;
                break;
            }
            _timeStamps << nt;
        }
        if ( isFinished ) {
            break;
        }
    }
}

double TimeStamps::at(int i )
{
    if ( i >= 0 && i < _timeStamps.size() ) {
        return _timeStamps.at(i);
    } else if ( i >= _timeStamps.size() ) {
        return _timeStamps.last();
    } else if ( i < 0 ) {
        return _timeStamps.at(0);
    }

    return 0.0;
}

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
                QString timeName = opts.timeName ;
                if ( timeName.isEmpty() ) {
                    timeName = "sys.exec.out.time" ;
                }
                //TimeItLinux timer;
                //timer.start();
                bool r = writeTrk(opts.trkOutFile,timeName,params,monteModel);
                //timer.snap("time=");
                if ( r ) {
                    ret = 0;
                } else {
                    fprintf(stderr, "snap [error]: Failed to write: %s\n",
                            opts.trkOutFile.toAscii().constData());
                    ret = -1;
                }
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
bool writeTrk(const QString& ftrk, const QString& timeName,
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
                    timeName.toAscii().constData(),
                    yParam.toAscii().constData());
            return false;
        }

        // Map Curve
        c->map();

        // Error check:   make sure curve has data
        if ( c->rowCount() == 0 ) {
            // No data
            fprintf(stderr, "snap [error]: no data found in %s\n",
                    c->tableName().toAscii().constData());
            return false;
        }

        // Error check: first timestamp should be 0.0
        int i = c->indexAtTime(0.0);
        if ( i != 0 ) {
            // 0.0 is not first time - yield error
            fprintf(stderr, "snap [error]: start time in data is not 0.0 "
                            " for the following trk\n    %s\n",
                    c->trkFile().toAscii().constData());
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
                fprintf(stderr,"snap [error]: frequency for data in %s is "
                               "irregular.\n",
                               curve->trkFile().toAscii().constData());
                return false;
            }

            if ( e > stopTime ) stopTime = e;

            ROUNDOFF(f,f);
            freqs.append(f);
            curve2freq.insert(curve,f);
        }
        curve->unmap();
    }
    freqs = uniquify(freqs);

    // Create list of timestamps based on frequency of data of each curve.
    // This assumes that each curve starts at 0.0
    // This is the first "curve"
    TimeStamps timeStamps(freqs,stopTime);

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
                                curve->trkFile().toAscii().constData(),
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

// TODO: Fix this dumb way
// If xs = (0.01, 0.010000010) this takes a long time
double leastCommonMultiple(const QList<double>& xs)
{
    double lcm = 0.0;

    if ( xs.size() == 0 ) {
        return 0.0;
    }
    if ( xs.size() == 1 ) {
        return xs.at(0);
    }

    QList<double> ks;
    foreach ( double x, xs ) {
        Q_UNUSED(x);
        ks << 1.0;
    }

    while ( 1 ) {

        int i = 0;
        double max = -1.0e20;
        foreach ( double k, ks ) {
            double x = xs.at(i);
            if ( k*x > max ) {
                max = k*x;
            }
            ++i;
        }

        bool isDone = true;
        for ( int i = 0 ; i < ks.size(); ++i ) {
            double k = ks.at(i);
            double x = xs.at(i);
            if ( max - k*x > 1.0e-9 ) {
                isDone = false;
                k = k + 1.0;
                ROUNDOFF(k,k);
                ks[i] = k;
            }
        }

        if ( isDone ) {
            lcm = max;
            ROUNDOFF(lcm,lcm);
            break;
        }
    }


    return lcm;
}

QList<double> merge(const QList<double>& listA, const QList<double>& listB)
{
    QList<double> list = listA + listB;
    list = uniquify(list);
    qSort(list.begin(),list.end(),doubleLessThan);
    return list;
}

QList<double> uniquify(const QList<double>& list)
{
    QList<double> ulist;
    foreach ( double v, list ) {
        bool isEqual = false;
        foreach ( double u, ulist ) {
            if ( qAbs(v-u) < 1.0e-9 ) {
                isEqual = true;
                break;
            }
        }
        if ( !isEqual ) {
            ulist << v;
        }
    }
    return ulist;
}
