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
    QString pdfOutFile;
    QStringList rundps;
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


    opts.add("<RUN_dirs and DP_files>:*", &opts.rundps, QStringList(),
             "List of RUN dirs and DP files",
             presetRunsDPs, postsetRunsDPs);
    opts.add("-t1",&opts.title1,"Snap Plot", "Main title");
    opts.add("-t2",&opts.title2,"", "Subtitle");
    opts.add("-t3",&opts.title3,"", "User title");
    opts.add("-t4",&opts.title4,"", "Date title");
    opts.add("-pdf", &opts.pdfOutFile, QString("out.pdf"),
             "Name of pdf output file");
    opts.parse(argc,argv, QString("snapp"), &ok);

    if ( !ok ) {
        qDebug() << opts.usage();
        return -1;
    }

    try {

        QApplication::setGraphicsSystem("raster");
        QApplication a(argc, argv);

        QStringList dps;
        QStringList runDirs;
        foreach ( QString f, opts.rundps ) {
            if ( f.startsWith("DP_") ) {
                dps << f;
            } else {
                runDirs << f;
            }
        }
        MonteModel* monteModel = 0;
        QStandardItemModel* varsModel = 0;
        QStandardItemModel* monteInputsModel = 0;
        Runs* runs = 0;

        runs = new Runs(runDirs);
        monteModel = new MonteModel(runs);
        varsModel = createVarsModel(runDirs);

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

        PlotMainWindow w(dps, titles,
                         monteModel, varsModel, monteInputsModel);
        w.savePdf(opts.pdfOutFile);

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
