#include <QApplication>
#include <QObject>
#include <QSet>


#include <string>
using namespace std;

#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QDataStream>
#include <stdio.h>

#include "libopts/options.h"
#include "libsnapdata/runs.h"
#include "libqplot/plotmainwindow.h"
#include "libsnapdata/unit.h"
#include "libsnapdata/csv.h"
#include "libsnapdata/trick_types.h"

bool convert2trk(const QString& csvFileName, const QString &trkFileName);

class SnapOptions : public Options
{
  public:
    QString csvFile;
    QString trkOutFile;
};

SnapOptions opts;

Option::FPresetQString presetCsvFile;
Option::FPostsetQString postsetCsvFile;
Option::FPresetQString presetTrkOutputFile;

int main(int argc, char *argv[])
{
    bool ok;
    int ret = -1;

    opts.add("<csv file>", &opts.csvFile, QString(""),
             "csv file to convert to a trk",
             presetCsvFile, postsetCsvFile);
    opts.add("-trk", &opts.trkOutFile, QString(""),
             "Name of trk outputfile",
             presetTrkOutputFile);
    opts.parse(argc,argv, QString("csv2trk"), &ok);

    if ( !ok ) {
        fprintf(stderr, "%s\n", opts.usage().toAscii().constData());
        return -1;
    }

    try {

        QApplication a(argc, argv);
        if ( opts.trkOutFile.isEmpty() ) {
            QFileInfo fi(opts.csvFile);
            opts.trkOutFile = QString("%1.trk").arg(fi.baseName());
        }
        bool ret = convert2trk(opts.csvFile, opts.trkOutFile);
        if ( !ret )  {
            fprintf(stderr, "csv2trk [error]: Aborting!\n");
            exit(-1);
        }

    } catch (std::exception &e) {
        fprintf(stderr,"\n%s\n",e.what());
        fprintf(stderr,"%s\n",opts.usage().toAscii().constData());
        exit(-1);
    }

    return ret;
}

void presetCsvFile(QString* ignoreMe, const QString& csv,bool* ok)
{
    Q_UNUSED(ignoreMe);

    QFileInfo fi(csv);
    if ( !fi.exists() ) {
        fprintf(stderr,
                "csv2trk [error] : Couldn't find file/directory: \"%s\".\n",
                csv.toAscii().constData());
        *ok = false;
        return;
    }
}

// Placeholder
void postsetCsvFile(QString* csvs, bool* ok)
{
    Q_UNUSED(csvs);
    Q_UNUSED(ok);
}

bool convert2trk(const QString& csvFileName, const QString& trkFileName)
{
    QFile file(csvFileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        fprintf(stderr, "csv2trk [error]: Cannot read file %s!",
                csvFileName.toAscii().constData());
    }
    CSV csv(&file);

    // Parse first line to get param list
    QList<Param> params;
    QStringList list = csv.parseLine() ;
    if ( list.size() == 1 && list.at(0).isEmpty() ) {
        fprintf(stderr, "csv2trk [error]: Empty csv file \"%s\"",
                csvFileName.toAscii().constData());
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
            if ( u.isUnit(unitString.toAscii().constData()) ) {
                p.unit = unitString;
            }
        }
        p.type = TRICK_07_DOUBLE;
        p.size = sizeof(double);
        params.append(p);
    }

    QFileInfo ftrki(trkFileName);
    if ( ftrki.exists() ) {
        fprintf(stderr, "csv2trk [error]: Will not overwrite %s\n",
                trkFileName.toAscii().constData());
        return false;
    }

    QFile trk(trkFileName);

    if (!trk.open(QIODevice::WriteOnly)) {
        fprintf(stderr,"Snap: [error] could not open %s\n",
                trkFileName.toAscii().constData());
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
                 "csv2trk [error]: Bad value \"%s\" on line %d in file %s\n",
                        s.toAscii().constData(),
                        line,
                        fi.absoluteFilePath().toAscii().constData());
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

void presetTrkOutputFile(QString* v, const QString& ftrk, bool* ok)
{
    Q_UNUSED(v);

    *ok = true;
    QFileInfo ftrki(ftrk);
    if ( ftrki.exists() ) {
        fprintf(stderr, "snapq [error]: %s exists, will not overwrite\n",
                ftrk.toAscii().constData());
        *ok = false;
    }
}
