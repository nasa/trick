#include <QApplication>
#include <QObject>
#include <QSet>
#include <QHash>
#include <QString>
#include <QDate>

#include <string>
using namespace std;

#include <QDir>
#include <QFileInfo>
#include <QTextStream>
#include <stdio.h>

#include "libkoviz/options.h"
#include "libkoviz/runs.h"
#include "libkoviz/plotmainwindow.h"
#include "libkoviz/roundoff.h"
#ifdef __linux
#include "libkoviz/timeit_linux.h"
#endif
#include "libkoviz/timestamps.h"
#include "libkoviz/tricktablemodel.h"
#include "libkoviz/dp.h"
#include "libkoviz/snap.h"
#include "libkoviz/csv.h"

QStandardItemModel* createVarsModel(Runs* runs);
bool writeTrk(const QString& ftrk, const QString &timeName,
              double start, double stop, double timeShift,
              QStringList& paramList, MonteModel* monteModel);
bool writeCsv(const QString& fcsv, const QStringList& timeNames,
              DPTable* dpTable, const QString &runDir);
bool convert2csv(const QStringList& timeNames,
                 const QString& ftrk, const QString& fcsv);
bool convert2trk(const QString& csvFileName, const QString &trkFileName);
QHash<QString,QVariant> getShiftHash(const QString& shiftString,
                                const QStringList &runDirs);
QHash<QString,QStringList> getVarMap(const QString& mapString);
QHash<QString,QStringList> getVarMapFromFile(const QString& mapFileName);
QStringList getTimeNames(const QString& timeName);
QStandardItemModel* monteInputModel(const QString &monteDir,
                                    const QStringList &runs);
QStandardItemModel* runsInputModel(const QStringList &runs);
QStandardItemModel* monteInputModelTrick07(const QString &monteInputFile,
                                           const QStringList &runs);
QStandardItemModel* monteInputModelTrick17(const QString &monteInputFile,
                                           const QStringList &runs);
QStringList runsSubset(const QStringList& runsList, uint beginRun, uint endRun);

QStringList sessionFileRuns(const QString& sessionFile);
QStringList sessionFileDPs(const QString& sessionFile);
QString sessionFileDevice(const QString& sessionFile);
QString sessionFilePresentation(const QString& sessionFile);
double sessionFileTimeMatchTolerance(const QString& sessionFile);
double sessionFileFrequency(const QString& sessionFile);

Option::FPresetQString presetExistsFile;
Option::FPresetDouble preset_start;
Option::FPresetDouble preset_stop;
Option::FPresetQStringList presetRunsDPs;
Option::FPostsetQStringList postsetRunsDPs;
Option::FPresetQString presetPresentation;
Option::FPresetUInt presetBeginRun;
Option::FPresetUInt presetEndRun;
Option::FPresetQString presetOutputFile;
Option::FPresetQString presetOrientation;

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
    QString dp2trkOutFile;
    QString csvOutFile;
    QString title1;
    QString title2;
    QString title3;
    QString title4;
    QString timeName;
    QString trk2csvFile;
    QString csv2trkFile;
    QString outputFileName;
    QString shiftString;
    QString map;
    QString mapFile;
    bool isDebug;
    QString legend1;
    QString legend2;
    QString legend3;
    QString legend4;
    QString legend5;
    QString legend6;
    QString legend7;
    QString orient;
    QString sessionFile;
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
             "present plot with two curves as compare,error or error+compare",
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
    opts.add("-timeName", &opts.timeName, QString("sys.exec.out.time"),
             "Time variable (e.g. -timeName sys.exec.out.time=mySimTime)");
    opts.add("-pdf", &opts.pdfOutFile, QString(""),
             "Name of pdf output file");
    opts.add("-dp2trk", &opts.dp2trkOutFile, QString(""),
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
    opts.add("-shift", &opts.shiftString, QString(""),
             "time shift run curves by value "
             "(e.g. -shift \"RUN_a:1.075,RUN_b:2.0\")");
    opts.add("-map", &opts.map, QString(""),
             "variable mapping (e.g. -map trick.x=spots.x)");
    opts.add("-mapFile", &opts.mapFile, QString(""),
             "variable mapping file (e.g. -mapFile myMapFile.txt)");
    opts.add("-debug:{0,1}",&opts.isDebug,false, "Show book model tree etc.");
    opts.add("-l1",&opts.legend1,"", "Legend label 1");
    opts.add("-l2",&opts.legend2,"", "Legend label 2");
    opts.add("-l3",&opts.legend3,"", "Legend label 3");
    opts.add("-l4",&opts.legend4,"", "Legend label 4");
    opts.add("-l5",&opts.legend5,"", "Legend label 5");
    opts.add("-l6",&opts.legend6,"", "Legend label 6");
    opts.add("-l7",&opts.legend7,"", "Legend label 7");
    opts.add("-orient",&opts.orient,"landscape",
             "PDF page orientation - landscape (default) and portrait",
             presetOrientation);
    opts.add("-session",&opts.sessionFile,"","session file name",
             presetExistsFile);

    opts.parse(argc,argv, QString("koviz"), &ok);

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
        } else if ( fi.isDir() ) {
            runDirs << f;
        }
    }
    if ( !opts.sessionFile.isEmpty() ) {
        runDirs << sessionFileRuns(opts.sessionFile);

        if ( runDirs.isEmpty() ) {
            fprintf(stderr,"koviz [error]: no RUNs "
                           "specified in session_file=\"%s\"\n",
                    opts.sessionFile.toLatin1().constData());
            exit(-1);
        }
        dps << sessionFileDPs(opts.sessionFile);
    }

    if ( opts.rundps.isEmpty() && opts.sessionFile.isEmpty() ) {
        if ( opts.trk2csvFile.isEmpty() && opts.csv2trkFile.isEmpty() ) {
            fprintf(stderr,"koviz [error] : no RUNs specified\n");
            fprintf(stderr,"%s\n",opts.usage().toLatin1().constData());
            exit(-1);
        }
    }
    if ( runDirs.isEmpty() &&
         opts.trk2csvFile.isEmpty() && opts.csv2trkFile.isEmpty()) {
        fprintf(stderr, "koviz [error]: no RUNs specified.\n"
                "       Possible causes:\n"
                "         1) RUNs not specified on commandline\n"
                "         2) Forgot to use -session option for session file\n");
        exit(-1);
    }

    if ( !opts.map.isEmpty() && !opts.mapFile.isEmpty() ) {
        fprintf(stderr,"koviz [error] : the -map and -mapFile cannot be "
                       "used together.  Please use one or the other.\n");
        exit(-1);
    }
    QHash<QString,QStringList> varMap;
    if ( !opts.map.isEmpty() ) {
        varMap = getVarMap(opts.map);
    } else if ( !opts.mapFile.isEmpty() ) {
        varMap = getVarMapFromFile(opts.mapFile);
    }

    QHash<QString,QVariant> shifts = getShiftHash(opts.shiftString, runDirs);
    QStringList timeNames = getTimeNames(opts.timeName);

    if ( !opts.trk2csvFile.isEmpty() ) {
        QString csvOutFile = opts.outputFileName;
        if ( csvOutFile.isEmpty() ) {
            QFileInfo fi(opts.trk2csvFile);
            csvOutFile = fi.absolutePath() + "/" +
                         QString("%1.csv").arg(fi.baseName());
        }
        bool ret;
        try {
            ret = convert2csv(timeNames,opts.trk2csvFile, csvOutFile);
        } catch (std::exception &e) {
            fprintf(stderr,"\n%s\n",e.what());
            fprintf(stderr,"%s\n",opts.usage().toLatin1().constData());
            exit(-1);
        }
        if ( !ret )  {
            fprintf(stderr, "koviz [error]: Aborting trk to csv conversion!\n");
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
            fprintf(stderr, "koviz [error]: Aborting csv to trk conversion!\n");
            return -1;
        }
    }

    try {
        if ( opts.isReportRT ) {
            foreach ( QString run, runDirs ) {
                Snap snap(run,timeNames,opts.start,opts.stop);
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
        if ( !opts.dp2trkOutFile.isEmpty() ) {
            isTrk = true;
        }

        QString pdfOutFile;
        bool isPdf = false;
        if ( !opts.pdfOutFile.isEmpty() ) {
            pdfOutFile = opts.pdfOutFile;
            isPdf = true;
        } else if ( !opts.sessionFile.isEmpty() ) {
            QString device = sessionFileDevice(opts.sessionFile);
            if ( device != "terminal" ) {
                pdfOutFile = device;
                isPdf = true;
            }
        }

        bool isCsv = false;
        if ( !opts.csvOutFile.isEmpty() ) {
            isCsv = true;
        }

        if ( (isPdf && isTrk) || (isPdf && isCsv) || (isTrk && isCsv) ) {
            fprintf(stderr,
                    "koviz [error] : you may not use the -pdf, -trk and -csv "
                    "options together.");
            exit(-1);
        }

        // If outputting to pdf, you must have a DP file and RUN dir
        if ( isPdf && (dps.size() == 0 || runDirs.size() == 0) ) {
            fprintf(stderr,
                    "koviz [error] : when using the -pdf option you must "
                    "specify a DP product file and RUN directory\n");
            exit(-1);
        }

        // If outputting to trk, you must have a DP file and RUN dir
        if ( isTrk && (dps.size() == 0 || runDirs.size() == 0) ) {
            fprintf(stderr,
                    "koviz [error] : when using the -trk option you must "
                    "specify a DP product file and RUN directory\n");
            exit(-1);
        }

        // If outputting to csv, you must have a DP file and RUN dir
        if ( isCsv && (dps.size() == 0 || runDirs.size() == 0) ) {
            fprintf(stderr,
                    "koviz [error] : when using the -csv option you must "
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

            QDir monteDir(runDirs.at(0));
            if ( ! monteDir.exists() ) {
                fprintf(stderr,
                        "koviz [error]: couldn't find monte directory: %s\n",
                        runDirs.at(0).toLatin1().constData());
                exit(-1);
            }

            // Make list of RUNs in the monte directory
            QStringList filters;
            filters << "RUN_*";
            monteDir.setNameFilters(filters);
            QStringList monteRuns = monteDir.entryList(filters, QDir::Dirs);
            if ( monteRuns.empty() ) {
                fprintf(stderr, "koviz [error]: no RUN dirs in %s \n",
                        runDirs.at(0).toLatin1().constData());
                exit(-1);
            }

            QStringList runsList = runsSubset(monteRuns,
                                              opts.beginRun,opts.endRun);
            QStringList monteRunsList;
            foreach ( QString run, runsList ) {
                monteRunsList << runDirs.at(0) + "/" + run;
            }

            runs = new Runs(timeNames,monteRunsList,varMap);
            monteInputsModel = monteInputModel(monteDir.absolutePath(),
                                               runsList);
        } else {
            QStringList runsList = runsSubset(runDirs,
                                              opts.beginRun,opts.endRun);
            runs = new Runs(timeNames,runsList,varMap);
            monteInputsModel = runsInputModel(runsList);
        }
        monteModel = new MonteModel(runs);
        varsModel = createVarsModel(runs);

        // Make a list of titles
        QStringList titles;
        QString title = opts.title1;
        if ( title.isEmpty() ) {
            title = "koviz ";
            if ( isMonte ) {
                title += runDirs.at(0);
            } else {
                if ( runDirs.size() == 1 ) {
                    title += runDirs.at(0);
                } else if ( runDirs.size() == 2 ) {
                    title += runDirs.at(0) + " " + runDirs.at(1);
                } else if ( runDirs.size() > 2 ) {
                    title += runDirs.at(0) + " " + runDirs.at(1) + "...";
                }
            }
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

        // Presentation
        QString presentation = opts.presentation;
        if ( presentation.isEmpty() && !opts.sessionFile.isEmpty() ) {
            presentation = sessionFilePresentation(opts.sessionFile);
        }
        if ( presentation.isEmpty() ){
            presentation = "compare";
            int rc = monteModel->rowCount();
            if ( rc == 2 ) {
                presentation = "error";
            }
        }

        // Make a list of legend labels
        QStringList legends;
        legends << opts.legend1 << opts.legend2 << opts.legend3
                << opts.legend4 << opts.legend5 << opts.legend6 << opts.legend7;

        // Time match tolerance
        double tolerance = 0.000001;
        if ( !opts.sessionFile.isEmpty() ) {
            tolerance = sessionFileTimeMatchTolerance(opts.sessionFile);
        }

        // Frequency
        double frequency = 0.0;
        if ( !opts.sessionFile.isEmpty() ) {
            frequency = sessionFileFrequency(opts.sessionFile);
        }

        if ( isPdf ) {
            PlotMainWindow w(opts.isDebug,
                             timeNames, opts.start, opts.stop,
                             tolerance, frequency,
                             shifts,
                             presentation, QString(), dps, titles, legends,
                             opts.orient,
                             monteModel, varsModel, monteInputsModel);
            w.savePdf(pdfOutFile);

        } else if ( isTrk ) {

            QStringList params = DPProduct::paramList(dps);

            if ( monteModel->rowCount() == 1 ) {
                QHash<QString,QVariant> shifts = getShiftHash(opts.shiftString,
                                                              runDirs);
                double timeShift = 0.0;
                if ( shifts.size() == 1 ) {
                    bool ok;
                    timeShift = shifts.values().at(0).toDouble(&ok);
                    if ( !ok ) {
                        fprintf(stderr, "koviz [bad scoobs]: -shift <value> "
                                        "cannot be converted to a double.\n");
                        exit(-1);
                    }
                }
                bool r = writeTrk(opts.dp2trkOutFile,
                                  opts.timeName,
                                  opts.start,
                                  opts.stop,
                                  timeShift,
                                  params,monteModel);
                if ( r ) {
                    ret = 0;
                } else {
                    fprintf(stderr, "koviz [error]: Failed to write: %s\n",
                            opts.dp2trkOutFile.toLatin1().constData());
                    ret = -1;
                }
            } else {
                fprintf(stderr, "koviz [error]: Only one RUN allowed with "
                                 "the -trk and -dp2trk options.\n");
                exit(-1);
            }

        } else if ( isCsv ) {


            if ( runDirs.size() != 1 ) {
                fprintf(stderr, "koviz [error]: Exactly one RUN dir must be "
                                "specified with the -csv option.\n");
                exit(-1);
            }

            // If there are no tables, print a message
            int nTables = 0;
            foreach ( QString dpFileName, dps ) {
                DPProduct dp(dpFileName);
                nTables += dp.tables().size();
            }
            if ( nTables == 0 ) {
                fprintf(stderr, "koviz [error]: In order to create csv files "
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

                    bool r = writeCsv(fname,timeNames,dpTable,runDirs.at(0));
                    if ( r ) {
                        ret = 0;
                    } else {
                        fprintf(stderr, "koviz [error]: Failed to write: %s\n",
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
                PlotMainWindow w(opts.isDebug,
                                 timeNames,
                                 opts.start, opts.stop,
                                 tolerance, frequency,
                                 shifts,
                                 presentation, ".", dps, titles, legends,
                                 opts.orient,
                                 monteModel, varsModel, monteInputsModel);
#ifdef __linux
                timer.snap("time=");
#endif
                w.show();
                ret = a.exec();
            } else {

                PlotMainWindow w(opts.isDebug,
                                 timeNames,
                                 opts.start, opts.stop,
                                 tolerance, frequency,
                                 shifts,
                                 presentation, runDirs.at(0), QStringList(),
                                 titles, legends, opts.orient, monteModel,
                                 varsModel, monteInputsModel);
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
                    "koviz [error] : couldn't find file/directory: \"%s\".\n",
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

QStandardItemModel* createVarsModel(Runs* runs)
{
    if ( runs == 0 ) return 0;

    QStandardItemModel* varsModel = new QStandardItemModel(0,1);

    QStringList params = runs->params();
    params.sort();
    QStandardItem *rootItem = varsModel->invisibleRootItem();
    foreach (QString param, params) {
        if ( param == "sys.exec.out.time" ) continue;
        QStandardItem *varItem = new QStandardItem(param);
        rootItem->appendRow(varItem);
    }

    return varsModel;
}

void presetBeginRun(uint* beginRunId, uint runId, bool* ok)
{
    Q_UNUSED(beginRunId);

    if ( runId > opts.endRun ) {
        fprintf(stderr,"koviz [error] : option -beginRun, set to %d, "
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
        fprintf(stderr,"koviz [error] : option -endRun, set to %d, "
                "must be greater than "
                "-beginRun which is set to %d\n",
                runId,opts.beginRun);
        *ok = false;
    }
}

void presetPresentation(QString* presVar, const QString& pres, bool* ok)
{
    Q_UNUSED(presVar);

    if ( !pres.isEmpty() && pres != "compare" && pres != "error" &&
         pres != "error+compare" ) {
        fprintf(stderr,"koviz [error] : option -presentation, set to \"%s\", "
                "should be \"compare\", \"error\" or \"error+compare\"\n",
                pres.toLatin1().constData());
        *ok = false;
    }
}

void presetOutputFile(QString* presVar, const QString& fname, bool* ok)
{
    Q_UNUSED(presVar);

    QFileInfo fi(fname);
    if ( fi.exists() ) {
        fprintf(stderr, "koviz [error]: Will not overwrite %s\n",
                fname.toLatin1().constData());
        *ok = false;
    }
}

void presetOrientation(QString* presVar, const QString& orient, bool* ok)
{
    Q_UNUSED(presVar);

    if ( orient != "landscape" && orient != "portrait" && !orient.isEmpty() ) {
        fprintf(stderr, "koviz [error]: option -orient set to \"%s\", "
                        "should be \"landscape\" or \"portrait\"\n",
                orient.toLatin1().constData());
        *ok = false;
    }
}

bool writeTrk(const QString& ftrk, const QString& timeName,
              double start, double stop, double timeShift,
              QStringList& paramList, MonteModel* monteModel)
{
    QFileInfo ftrki(ftrk);
    if ( ftrki.exists() ) {
        fprintf(stderr, "koviz [error]: Will not overwrite %s\n",
                ftrk.toLatin1().constData());
        return false;
    }

    // Print message
    fprintf(stderr, "\nkoviz [info]: extracting the following params "
                    "into %s:\n\n",
                    ftrk.toLatin1().constData());
    foreach ( QString param, paramList ) {
        fprintf(stderr, "    %s\n", param.toLatin1().constData());
    }
    fprintf(stderr, "\n");

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
            fprintf(stderr, "koviz [error]: could not find curve: \n    ("
                    "%s,%s)\n",
                    timeName.toLatin1().constData(),
                    yParam.toLatin1().constData());
            foreach ( TrickCurveModel* curveModel, curves ) {
                delete curveModel;
            }
            delete c;
            return false;
        }

        // Map Curve
        c->map();

        // Error check:   make sure curve has data
        if ( c->rowCount() == 0 ) {
            // No data
            fprintf(stderr, "koviz [error]: no data found in %s\n",
                    c->fileName().toLatin1().constData());
            foreach ( TrickCurveModel* curveModel, curves ) {
                delete curveModel;
            }
            delete c;
            return false;
        }

        // Make a Param (for trk header)
        Param p;
        p.name = yParam;
        p.unit = c->y().unit();
        p.type = TRICK_07_DOUBLE;
        p.size = sizeof(double);

        // Make params/curves lists (lazily mapping params to curves)
        params.append(p);
        curves.append(c);

        // Unmap Curve
        c->unmap();
    }
    if ( params.size() < 2 ) {
        fprintf(stderr,"koviz [error]: Could not find any params in RUN that "
                       "are in DP files\n\n");
        return false;
    }

    // Make time stamps list
    QList<double> timeStamps;
    foreach ( TrickCurveModel* curve, curves ) {

        if ( !curve ) continue ;

        curve->map();

        ModelIterator* it = curve->begin();
        while ( !it->isDone() ) {
            double t = it->t();
            if ( t < start ) {
                it->next();
                continue;
            }
            if ( t > stop ) {
                break;
            }
            TimeStamps::insert(t,timeStamps);
            it->next();
        }
        delete it;
        curve->unmap();
    }

    // Open trk file for writing
    QFile trk(ftrk);
    if (!trk.open(QIODevice::WriteOnly)) {
        fprintf(stderr,"koviz: [error] could not open %s\n",
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

    int nTimeStamps = timeStamps.count();

    // Write time stamps and data
    int i = 0;
    foreach ( TrickCurveModel* curve, curves ) {

        if ( !curve ) {
            // write time stamps
            for ( int j = 0 ; j < nTimeStamps; ++j ) {
                qint64 recordOffset = j*recordSize;
                qint64 paramOffset = 0;
                qint64 offset = headerSize + recordOffset + paramOffset;
                trk.seek(offset);
                out << timeStamps.at(j)+timeShift;
            }
        } else {
            // write curve data
            curve->map();
            ModelIterator* it = curve->begin();
            for ( int j = 0 ; j < nTimeStamps; ++j ) {

                double timeStamp = timeStamps.at(j);
                int k = curve->indexAtTime(timeStamp);
                double v = it->at(k)->y();

                qint64 recordOffset = j*recordSize;
                qint64 paramOffset = i*sizeof(double);
                qint64 offset = headerSize + recordOffset + paramOffset;
                trk.seek(offset);
                out << v;
            }
            delete it;
            curve->unmap();
        }
        ++i;
    }

    //
    // Clean up
    //
    trk.close();
    foreach ( TrickCurveModel* curveModel, curves ) {
        delete curveModel;
    }

    return true;
}

bool writeCsv(const QString& fcsv, const QStringList& timeNames,
              DPTable* dpTable, const QString& runDir)
{
    QFileInfo fcsvi(fcsv);
    if ( fcsvi.exists() ) {
        fprintf(stderr, "koviz [error]: Will not overwrite %s\n",
                fcsv.toLatin1().constData());
        return false;
    }

    // Open trk file for writing
    QFile csv(fcsv);
    if (!csv.open(QIODevice::WriteOnly)) {
        fprintf(stderr,"koviz: [error] could not open %s\n",
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
    header = timeNames.at(0) + ",";
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


    TrickTableModel ttm(timeNames, runDir, params);
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
            fprintf(stderr,"koviz [error] : Trying to set option -start to "
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
            fprintf(stderr,"koviz [error] : Trying to set option -stop to "
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
                "koviz [error] : Couldn't find file: \"%s\".\n",
                fname.toLatin1().constData());
        *ok = false;
        return;
    }
}

bool convert2csv(const QStringList& timeNames,
                 const QString& ftrk, const QString& fcsv)
{
    TrickModel m(timeNames, ftrk);

    QFileInfo fcsvi(fcsv);
    if ( fcsvi.exists() ) {
        fprintf(stderr, "koviz [error]: Will not overwrite %s\n",
                fcsv.toLatin1().constData());
        return false;
    }

    // Open csv file stream
    QFile csv(fcsv);
    if (!csv.open(QIODevice::WriteOnly)) {
        fprintf(stderr,"koviz: [error] could not open %s\n",
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
        fprintf(stderr, "koviz [error]: Cannot read file %s!",
                csvFileName.toLatin1().constData());
    }
    CSV csv(&file);

    // Parse first line to get param list
    QList<Param> params;
    QStringList list = csv.parseLine() ;
    if ( list.isEmpty() ) {
        fprintf(stderr, "koviz [error]: Empty csv file \"%s\"",
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
        fprintf(stderr, "koviz [error]: Will not overwrite %s\n",
                trkFileName.toLatin1().constData());
        return false;
    }

    QFile trk(trkFileName);

    if (!trk.open(QIODevice::WriteOnly)) {
        fprintf(stderr,"koviz [error]: could not open %s\n",
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
        if ( list.isEmpty() ) break;  // end of file, hopefully!!!
        foreach ( QString s, list ) {
            bool ok;
            double val = s.toDouble(&ok);
            if ( !ok ) {
                QStringList vals = s.split(":");
                if (vals.length() == 3 ) {
                    // Try converting to a utc timestamp
                    val = 3600.0*vals.at(0).toDouble(&ok);
                    if ( ok ) {
                        val += 60.0*vals.at(1).toDouble(&ok);
                        if ( ok ) {
                            val += vals.at(2).toDouble(&ok);
                        }
                    }
                }
            }
            if ( !ok ) {
                QFileInfo fi(csvFileName);
                fprintf(stderr,
                 "koviz [error]: Bad value \"%s\" on line %d in file %s\n",
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

// shiftString has the form "[RUN_0:]val0[,RUN_1:val1,...]"
// This function returns a hash RUN_0->val0, RUN_1->val1...
QHash<QString,QVariant> getShiftHash(const QString& shiftString,
                                     const QStringList& runDirs)
{
    QHash<QString,QVariant> shifts;

    if (shiftString.isEmpty() || runDirs.isEmpty() ) return shifts; // empty map

    QStringList shiftStrings = shiftString.split(',',QString::SkipEmptyParts);
    foreach ( QString s, shiftStrings ) {

        s = s.trimmed();
        QString shiftRunFullPath;
        double shiftVal;
        if ( s.contains(':') ) {
            // e.g. koviz RUN_a RUN_b -shift "RUN_a:0.00125"
            QString shiftRun       = s.split(':').at(0).trimmed();
            QString shiftValString = s.split(':').at(1).trimmed();
            if ( shiftRun.isEmpty() || shiftValString.isEmpty() ) {
                fprintf(stderr,"koviz [error] : -shift option value \"%s\""
                               "is malformed.\n"
                               "Use this syntax -shift \"<run>:<val>\"\n",
                        opts.shiftString.toLatin1().constData());
                exit(-1);
            }

            bool isFound = false;
            QFileInfo fi(shiftRun);
            shiftRunFullPath = fi.absoluteFilePath();
            foreach ( QString runDir, runDirs ) {
                QFileInfo fir(runDir);
                QString runDirFullPath = fir.absoluteFilePath();
                if ( runDirFullPath == shiftRunFullPath ) {
                    isFound = true;
                    break;
                }
            }
            if ( !isFound ) {
                fprintf(stderr,"koviz [error] : -shift option \"%s\" "
                               "does not specify a valid run to shift.\n"
                               "Use this syntax -shift \"<run>:<val>\" "
                               "where <run> is one of the runs in the \n"
                               "commandline set of runs e.g. %s.\n",
                        s.toLatin1().constData(),
                        runDirs.at(0).toLatin1().constData());
                exit(-1);
            }

            QVariant q(shiftValString);
            bool ok;
            shiftVal = q.toDouble(&ok);
            if ( !ok ) {
                fprintf(stderr,"koviz [error] : option -shift \"%s\" "
                               "does not specify a valid shift value.\n"
                               "Use this syntax -shift \"[<run>:]<val>\"\n",
                        s.toLatin1().constData());
                exit(-1);
            }

        } else {
            // e.g. koviz RUN_a -shift 0.00125
            if ( runDirs.size() != 1 ) {
                fprintf(stderr,"koviz [error] : option -shift \"%s\" "
                               "does not specify a valid shift string.\n"
                               "Use the run:val syntax when there are "
                               "multiple runs.\n"
                               "Use this syntax -shift \"[<run>:]<val>\"\n",
                        s.toLatin1().constData());
                exit(-1);
            }
            QVariant q(s);
            bool ok;
            shiftVal = q.toDouble(&ok);
            if ( !ok ) {
                fprintf(stderr,"koviz [error] : option -shift \"%s\" "
                               "does not specify a valid shift value.\n"
                               "Use this syntax -shift \"[<run>:]<val>\"\n",
                        s.toLatin1().constData());
                exit(-1);
            }

            QFileInfo fi(runDirs.at(0));
            shiftRunFullPath = fi.absoluteFilePath();
        }

        shifts.insert(shiftRunFullPath,shiftVal);

    }

    return shifts;
}

// An example mapString is "px=trick.pos[0]=spots.posx,trick.pos[1]=spots.posy"
// In this example, getVarMap would return the following hash:
//                 px->[trick.pos[0],spots.posx]
//                 trick.pos[1]->[spots.posy]
QHash<QString,QStringList> getVarMap(const QString& mapString)
{
    QHash<QString,QStringList> varMap;

    if (mapString.isEmpty() ) return varMap; // empty map

    QStringList maps = mapString.split(',',QString::SkipEmptyParts);
    foreach ( QString s, maps ) {
        s = s.trimmed();
        if ( s.contains('=') ) {
            QStringList list = s.split('=');
            QString key = list.at(0).trimmed();
            if ( key.isEmpty() ) {
                fprintf(stderr,"koviz [error] : -map option value \"%s\""
                        "is malformed.\n"
                        "Use this syntax -map \"key=val1=val2...,key=val...\"\n",
                        mapString.toLatin1().constData());
                exit(-1);
            }
            QStringList vals;
            for (int i = 1; i < list.size(); ++i ) {
                QString val = list.at(i).trimmed();
                if ( val.isEmpty() ) {
                    fprintf(stderr,"koviz [error] : -map option value \"%s\""
                       "is malformed.\n"
                       "Use this syntax -map \"key=val1=val2...,key=val...\"\n",
                       mapString.toLatin1().constData());
                    exit(-1);
                }
                vals << val;
            }
            varMap.insert(key,vals);
        } else {
            // error
            fprintf(stderr,"koviz [error] : -map option value \"%s\""
                       "is malformed.\n"
                       "Use this syntax -map \"key=val1=val2...,key=val...\"\n",
                       mapString.toLatin1().constData());
            exit(-1);
        }

    }

    return varMap;
}

QHash<QString,QStringList> getVarMapFromFile(const QString& mapFileName)
{
    QHash<QString,QStringList> varMap;

    if (mapFileName.isEmpty() ) return varMap; // empty map

    QFile file(mapFileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        fprintf(stderr, "koviz [error]: Cannot read map file \"%s\".\n",
                mapFileName.toLatin1().constData());
        fprintf(stderr, "Aborting!!!\n");
        exit(-1);
    }

    QTextStream in(&file);

    QString mapString;
    while (!in.atEnd()) {
        QString line = in.readLine();
        mapString += line;
    }

    file.close();

    varMap = getVarMap(mapString);

    return varMap;
}

QStringList getTimeNames(const QString& timeName)
{
    QStringList timeNames;
    QStringList names = timeName.split('=',QString::SkipEmptyParts);
    foreach ( QString s, names ) {
        timeNames << s.trimmed();
    }
    return timeNames;
}

//
// Create table model from monte_runs file
//
//       Var0        Var1                         Varn
// Run0  val00       val01                        val0n
// Run1  val10       val11                        val1n
// ...
// Runm  valm0       valm1                        valmn
//
QStandardItemModel* monteInputModel(const QString &monteDir,
                                    const QStringList &runs)
{
    QStandardItemModel* m = 0 ;

    QString monteInputFile("monte_runs");
    monteInputFile.prepend("/");
    monteInputFile.prepend(monteDir);

    QFile file(monteInputFile);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        fprintf(stderr, "koviz [error]: could not open %s\n",
                         monteInputFile.toLatin1().constData());
        exit(-1);
    }
    QTextStream in(&file);

    bool isTrick07 = false;
    while (!in.atEnd()) {
        QString line = in.readLine();
        if ( line.startsWith("NUM_RUNS:") ) {
            isTrick07 = true;
            break;
        }
    }
    file.close();

    if ( isTrick07 ) {
        m = monteInputModelTrick07(monteInputFile,runs);
    } else {
        m = monteInputModelTrick17(monteInputFile,runs);
    }

    return m;
}

QStandardItemModel* monteInputModelTrick07(const QString &monteInputFile,
                                           const QStringList& runs)
{
    QStandardItemModel* m = 0;

    QFile file(monteInputFile);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        fprintf(stderr, "koviz [error]: could not open %s\n",
                        monteInputFile.toLatin1().constData());
        exit(-1);
    }
    QTextStream in(&file);

    //
    // Get Num Runs
    //
    bool isNumRuns = false;
    while (!in.atEnd()) {
        QString line = in.readLine();
        if ( line.startsWith("NUM_RUNS:") ) {
            line.split(':').at(1).trimmed().toInt(&isNumRuns);
            break;
        }
    }
    if ( !isNumRuns ) {
        fprintf(stderr, "koviz [error]: error parsing monte carlo input file: "
                        "%s.   Couldn't find/parse \"NUM_RUNS:\" line.\n",
                        monteInputFile.toLatin1().constData());
        exit(-1);
    }

    //
    // Get Vars
    //
    QStringList vars;
    bool isVars = false;
    bool isData = false;
    while ( !in.atEnd() ) {
        QString line = in.readLine();
        if ( line.startsWith("VARS:") ) {
            isVars = true;
            break;
        }
    }
    if ( ! isVars ) {
        fprintf(stderr, "koviz [error]: error parsing monte carlo input file: "
                        "%s.   Couldn't find/parse \"VARS:\" line.\n",
                        monteInputFile.toLatin1().constData());
        exit(-1);
    }
    while ( !in.atEnd() ) {
        QString var = in.readLine();
        if ( var.startsWith('#')) continue;
        if ( !var.startsWith("DATA:") ) {
            int delim = var.indexOf(' ');
            var = var.mid(0,delim);
            var = var.split('.').last();
            if ( ! var.isEmpty() ) {
                vars.append(var);
            }
        } else {
            isData = true;
            break;
        }
    }
    if ( !isData ) {
        fprintf(stderr, "koviz [error]: error parsing monte carlo input file: "
                        "%s.   Couldn't find/parse \"DATA:\" line.\n",
                        monteInputFile.toLatin1().constData());
        exit(-1);
    }

    //
    // Allocate table items
    //
    int nRuns = runs.size();
    m = new QStandardItemModel(nRuns,vars.size());

    //
    // Set table header for rows and get begin/end run ids
    //
    int beginRun = INT_MAX;
    int endRun = 0;
    int r = 0 ;
    foreach ( QString run, runs ) {
        int runId = run.mid(4).toInt(); // 4 is for RUN_
        if ( runId < beginRun ) {
            beginRun = runId;
        }
        if ( runId > endRun ) {
            endRun = runId;
        }
        QString runName = QString("%0").arg(runId);
        m->setHeaderData(r,Qt::Vertical,runName);
        r++;
    }

    //
    // Get Data
    //
    int nDataLines = 0 ;
    while (!in.atEnd()) {

        QString dataLine = in.readLine();

        //
        // Get runId
        //
        int hashIdx = dataLine.indexOf('#');
        if ( hashIdx < 0 ) {
            fprintf(stderr, "koviz [error]: Each dataline in monte_runs should "
                            "have a runId following a hash mark.\n"
                            "The following line does not:\n    \"%s\"\n",
                            dataLine.toLatin1().constData());
            exit(-1);
        }
        int runId = dataLine.mid(hashIdx+1).trimmed().toInt();
        if ( runId < beginRun || runId > endRun ) {
            continue;
        }

        //
        // Get RUN_<nnnn> dir
        //
        QString runName= QString("%0").arg(runId);
        runName = runName.rightJustified(5, '0');
        runName.prepend("RUN_");
        if ( ! runs.contains(runName) ) {
            fprintf(stderr,
                    "koviz [error]: error parsing monte carlo "
                    "input file. RunID for Run=%s"
                    " is in the monte carlo input file, but "
                    " can't find the RUN_ directory. "
                    "Look in file:"
                    "\n\n"
                    "File:%s\n",
                    runName.toLatin1().constData(),
                    monteInputFile.toLatin1().constData());
            exit(-1);
        }
        int c = 0;
        QString runIdString ;
        runIdString = runIdString.sprintf("%d",runId);
        NumSortItem *runIdItem = new NumSortItem(runIdString);
        m->setItem(nDataLines,c,runIdItem);
        m->setHeaderData(c,Qt::Horizontal,"RunId");

        //
        // Monte carlo input data values
        //
        dataLine = dataLine.mid(0,hashIdx).trimmed();
        QStringList dataVals = dataLine.split(' ',QString::SkipEmptyParts);
        if ( dataVals.size() != vars.size() ) {
            fprintf(stderr,
                    "koviz [error]: error parsing monte carlo input file.  "
                    "There are %d variables in VARS list, but only %d fields "
                    "on line number %d.\n\n"
                    "File:%s\n"
                    "Line:%s\n",
                    vars.size(), dataVals.size(),
                    nDataLines,
                    monteInputFile.toLatin1().constData(),
                    dataLine.toLatin1().constData());
            exit(-1);
        }
        c = 1;
        foreach ( QString val, dataVals ) {
            double v = val.toDouble();
            val = val.sprintf("%.4lf",v);
            NumSortItem *item = new NumSortItem(val);
            m->setItem(nDataLines,c,item);
            m->setHeaderData(c,Qt::Horizontal,vars.at(c-1));
            ++c;
        }

        nDataLines++;
    }

    file.close();

    return m;
}

QStandardItemModel* monteInputModelTrick17(const QString &monteInputFile,
                                           const QStringList& runs)
{
    QStandardItemModel* m = 0;

    QFile file(monteInputFile);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        fprintf(stderr,"koviz [error]: could not open %s\n",
                       monteInputFile.toLatin1().constData());
        exit(-1);
    }
    QTextStream in(&file);

    QString line = in.readLine();

    //
    // Sanity check num runs
    //
    int nRuns = 0 ;
    bool ok = false;
    int lineNum = 1;
    while (!in.atEnd()) {
        QString line = in.readLine();
        nRuns = line.split(' ').at(0).trimmed().toInt(&ok) + 1;
        if ( !ok || nRuns != lineNum ) {
            fprintf(stderr,"koviz [error]: error parsing %s "
                           " around line number %d\n",
                           monteInputFile.toLatin1().constData(),lineNum);
            exit(-1);
        }
        ++lineNum;
    }

    //
    // Get Vars
    //
    in.seek(0); // Go to beginning of file
    line = in.readLine();
    QStringList vars = line.split(' ',QString::SkipEmptyParts);
    vars.replace(0,"RunId");

    //
    // Allocate table items
    //
    nRuns = runs.size(); // If beginRun,endRun set, runs could be a subset
    m = new QStandardItemModel(nRuns,vars.size());

    //
    // Set table header for rows and get begin/end run ids
    //
    int beginRun = INT_MAX;
    int endRun = 0;
    int r = 0 ;
    foreach ( QString run, runs ) {
        int runId = run.mid(4).toInt(); // 4 is for RUN_
        if ( runId < beginRun ) {
            beginRun = runId;
        }
        if ( runId > endRun ) {
            endRun = runId;
        }
        QString runName = QString("%0").arg(runId);
        m->setHeaderData(r,Qt::Vertical,runName);
        r++;
    }

    //
    // Data
    //
    lineNum = 0;
    while (!in.atEnd()) {
        ++lineNum;
        line = in.readLine();
        QStringList vals = line.split(' ',QString::SkipEmptyParts);
        if ( vals.size() != vars.size() ) {
            fprintf(stderr, "koviz [error]: error parsing %s.  There "
                            "are %d variables specified in top line, "
                            "but only %d values on line number %d.\n",
                           monteInputFile.toLatin1().constData(),
                           vars.size(),vals.size(),lineNum);
            exit(-1);
        }

        int runId = vals.at(0).toInt();
        if ( runId < beginRun || runId > endRun ) {
            continue;
        }

        int nv = vals.size();
        for ( int c = 0; c < nv; ++c) {
            QString val = vals.at(c) ;
            double v = val.toDouble();
            if ( c == 0 ) {
                int ival = val.toInt();
                val = val.sprintf("%d",ival);
            } else {
                val = val.sprintf("%.4lf",v);
            }
            NumSortItem *item = new NumSortItem(val);
            m->setItem(lineNum-1,c,item);
            m->setHeaderData(c,Qt::Horizontal,vars.at(c));
        }
    }

    file.close();

    return m;
}

QStandardItemModel* runsInputModel(const QStringList &runs)
{
    QStandardItemModel* m = 0;

    bool ok;

    bool isRunNamesNumeric = true;
    foreach ( QString run, runs ) {
        int i = run.lastIndexOf("RUN_");
        int runId = run.mid(i+4).toInt(&ok); // 4 is for RUN_
        Q_UNUSED(runId);
        if ( !ok ) {
            isRunNamesNumeric = false;
            break;
        }
    }

    if ( isRunNamesNumeric ) {
        m = new QStandardItemModel(runs.size(),1);
        int r = 0 ;
        m->setHeaderData(0,Qt::Horizontal,"RunId");
        foreach ( QString run, runs ) {
            int i = run.lastIndexOf("RUN_");
            int runId = run.mid(i+4).toInt(&ok); // 4 is for RUN_
            QString runName = QString("%0").arg(runId);
            runName = runName.rightJustified(5, '0');
            runName.prepend("RUN_");
            m->setHeaderData(r,Qt::Vertical,runName);

            QString runIdString ;
            runIdString = runIdString.sprintf("%d",runId);
            NumSortItem *runIdItem = new NumSortItem(runIdString);
            m->setItem(r,0,runIdItem);

            r++;
        }
    } else {
        m = new QStandardItemModel(runs.size(),2);
        int r = 0 ;
        m->setHeaderData(0,Qt::Horizontal,"RunId");
        m->setHeaderData(1,Qt::Horizontal,"RunName");
        foreach ( QString run, runs ) {
            QString runName = run.split('/').last();
            m->setHeaderData(r,Qt::Vertical,runName);
            QString runIdString ;
            runIdString = runIdString.sprintf("%d",r);
            NumSortItem *runIdItem = new NumSortItem(runIdString);
            QStandardItem* runNameItem = new QStandardItem(runName);
            m->setItem(r,0,runIdItem);
            m->setItem(r,1,runNameItem);
            r++;
        }
    }

    return m;
}

// Make subset of runs based on beginRun and endRun option
QStringList runsSubset(const QStringList& runsList, uint beginRun, uint endRun)
{
    QStringList subset;

    foreach ( QString run, runsList ) {
        bool ok = false;
        QString runName = QFileInfo(run).fileName();
        unsigned int runId = runName.mid(4).toInt(&ok);
        if ( ok && (runId < beginRun || runId > endRun) ) {
            continue;
        }
        subset.append(run);
    }
    if ( subset.empty() ) {
        fprintf(stderr,"koviz [error]: main::runsSubset() is empty.  "
                       "-beginRun -endRun options may be at fault.\n"
                       "Aborting!!!");
        exit(-1);
    }

    return subset;
}

QStringList sessionFileRuns(const QString& sessionFile)
{
    QStringList runs;

    QFile file(sessionFile);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        fprintf(stderr, "koviz [error]: Cannot read session file %s!\n",
                sessionFile.toLatin1().constData());
        exit(-1);
    }

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();
        if ( line.contains("RUN:",Qt::CaseInsensitive) ) {
            int i = line.indexOf("RUN:",0,Qt::CaseInsensitive);
            QString run = line.mid(i+4).trimmed();
            run = run.remove("\"");
            if ( run.isEmpty() ) {
                fprintf(stderr,"koviz [error]: empty run specification in "
                               "session file %s.\n",
                               sessionFile.toLatin1().constData());
                exit(-1);
            }
            runs << run.split(" ",QString::SkipEmptyParts).at(0);
        }
    }

    file.close();

    return runs;
}

QStringList sessionFileDPs(const QString& sessionFile)
{
    QStringList dps;

    QFile file(sessionFile);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        fprintf(stderr, "koviz [error]: Cannot read session file %s!\n",
                sessionFile.toLatin1().constData());
        exit(-1);
    }

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();
        if ( line.contains("PRODUCT:",Qt::CaseInsensitive) ) {
            int i = line.indexOf("PRODUCT:",0,Qt::CaseInsensitive);
            QString dp = line.mid(i+8).trimmed();
            if ( dp.startsWith("\"") ) {
                dp = dp.mid(1);
            }
            if ( dp.endsWith("\"") ) {
                dp.chop(1);
            }
            dps << dp;
        }
    }

    file.close();

    return dps;
}

// If device is "file", return filename; otherwise return "terminal"
QString sessionFileDevice(const QString& sessionFile)
{
    QString device("terminal");

    QFile file(sessionFile);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        fprintf(stderr, "koviz [error]: Cannot read session file %s!\n",
                sessionFile.toLatin1().constData());
        exit(-1);
    }

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();
        if ( line.contains("DEVICE:",Qt::CaseInsensitive) ) {
            int i = line.indexOf("DEVICE:",0,Qt::CaseInsensitive);
            device = line.mid(i+7).trimmed();
            device = device.remove("\"");
            if ( device.isEmpty() ) {
                fprintf(stderr,"koviz [error]: empty device specification in "
                               "session file %s.\n",
                               sessionFile.toLatin1().constData());
                exit(-1);
            }
            QStringList list = device.split(" ",QString::SkipEmptyParts);
            if ( !QString::compare(list.at(0),"FILE",Qt::CaseInsensitive) ) {
                device = list.at(1); // device is filename
                if ( device == "terminal") { //just in case filename=="terminal"
                    device = "terminal.pdf";
                }
            } else {
                if ( !device.compare("terminal",Qt::CaseInsensitive) ) {
                    device = "terminal";
                } else {
                    // device is considered to be a filename
                }
            }
            break;
        }
    }

    file.close();

    return device;
}

QString sessionFilePresentation(const QString& sessionFile)
{
    QString presentation;

    QFile file(sessionFile);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        fprintf(stderr, "koviz [error]: Cannot read session file %s!\n",
                sessionFile.toLatin1().constData());
        exit(-1);
    }

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();
        if ( line.contains("PRESENTATION:",Qt::CaseInsensitive) ) {
            int i = line.indexOf("PRESENTATION:",0,Qt::CaseInsensitive);
            presentation = line.mid(i+13).trimmed();
            if ( presentation.startsWith("\"") ) {
                presentation = presentation.mid(1);
            }
            if ( presentation.endsWith("\"") ) {
                presentation.chop(1);
            }
            if ( presentation == "single") {
                fprintf(stderr,"koviz [error]: session file has presentation "
                               "set to \"single\".  For now, koviz only "
                               "supports \"compare\", \"error\" and "
                               "\"error+compare\"\n");
                exit(-1);
            }
            break;
        }
    }

    file.close();

    return presentation;
}

double sessionFileTimeMatchTolerance(const QString& sessionFile)
{
    double tolerance = 0.000001;

    QFile file(sessionFile);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        fprintf(stderr, "koviz [error]: Cannot read session file %s!\n",
                sessionFile.toLatin1().constData());
        exit(-1);
    }

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();
        if ( line.contains("TIME_MATCH_TOLERANCE:",Qt::CaseInsensitive) ) {
            int i = line.indexOf("TIME_MATCH_TOLERANCE:",0,Qt::CaseInsensitive);
            bool ok;
            tolerance = line.mid(i+21).trimmed().toDouble(&ok);
            if ( !ok ) {
                fprintf(stderr,"koviz [error]: time match tolerance in session"
                               "file %s is corrupt.\n",
                        sessionFile.toLatin1().constData());
                exit(-1);
            }
            break;
        }
    }

    file.close();

    return tolerance;
}

double sessionFileFrequency(const QString& sessionFile)
{
    double frequency = 0.0;

    QFile file(sessionFile);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        fprintf(stderr, "koviz [error]: Cannot read session file %s!\n",
                sessionFile.toLatin1().constData());
        exit(-1);
    }

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();
        if ( line.contains("FREQUENCY:",Qt::CaseInsensitive) ) {
            int i = line.indexOf("FREQUENCY:",0,Qt::CaseInsensitive);
            bool ok;
            frequency = line.mid(i+10).trimmed().toDouble(&ok);
            if ( !ok ) {
                fprintf(stderr,"koviz [error]: frequency spec in session"
                               "file %s is corrupt.\n",
                        sessionFile.toLatin1().constData());
                exit(-1);
            }
            break;
        }
    }

    file.close();

    return frequency;
}
