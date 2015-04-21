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

bool convert2trk(const QString& f);
void write_binary_param(QDataStream& out, const Param& p);
void write_binary_qstring(const QString &str, QDataStream& out);


class SnapOptions : public Options
{
  public:
    QStringList csvFiles;
};

SnapOptions opts;

Option::FPresetQStringList presetCsvFiles;
Option::FPostsetQStringList postsetCsvFiles;

int main(int argc, char *argv[])
{
    bool ok;
    int ret = -1;

    opts.add("<csv file list>:+", &opts.csvFiles, QStringList(),
             "List of csv files",
             presetCsvFiles, postsetCsvFiles);
    opts.parse(argc,argv, QString("csv2trk"), &ok);

    if ( !ok ) {
        fprintf(stderr, "%s\n", opts.usage().toAscii().constData());
        return -1;
    }

    try {

        QApplication a(argc, argv);
        foreach ( QString f, opts.csvFiles ) {
            bool ret = convert2trk(f);
            if ( !ret )  {
                fprintf(stderr, "csv2trk [error]: Aborting!\n");
                exit(-1);
            }
        }

    } catch (std::exception &e) {
        fprintf(stderr,"\n%s\n",e.what());
        fprintf(stderr,"%s\n",opts.usage().toAscii().constData());
        exit(-1);
    }

    return ret;
}

void presetCsvFiles(QStringList* ignoreMe, const QStringList& csvs,bool* ok)
{
    Q_UNUSED(ignoreMe);

    foreach ( QString f, csvs ) {
        QFileInfo fi(f);
        if ( !fi.exists() ) {
            fprintf(stderr,
                    "csv2trk [error] : Couldn't find file/directory: \"%s\".\n",
                    f.toAscii().constData());
            *ok = false;
            return;
        }
    }
}

// Placeholder
void postsetCsvFiles (QStringList* csvs, bool* ok)
{
    Q_UNUSED(csvs);
    Q_UNUSED(ok);
}

bool convert2trk(const QString& f)
{
    QFile file(f);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        fprintf(stderr, "csv2trk [error]: Cannot read file %s!",
                f.toAscii().constData());
    }
    CSV csv(&file);

    // Parse first line to get param list
    QList<Param> params;
    QStringList list = csv.parseLine() ;
    if ( list.size() == 1 && list.at(0).isEmpty() ) {
        fprintf(stderr, "csv2trk [error]: Empty csv file \"%s\"",
                f.toAscii().constData());
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

    QFileInfo fi(f);
    QString ftrk = QString("%1.trk").arg(fi.baseName());
    QFileInfo ftrki(ftrk);
    if ( ftrki.exists() ) {
        fprintf(stderr, "csv2trk [error]: Will not overwrite %s\n",
                ftrk.toAscii().constData());
        return false;
    }
    // TODO: make sure we don't overwrite an existing *.trk

    QFile trk(ftrk);

    if (!trk.open(QIODevice::WriteOnly)) {
        fprintf(stderr,"Snap: [error] could not open %s\n",
                ftrk.toAscii().constData());
        return false;
    }
    QDataStream out(&trk);

    // Make it little endian
    out.setByteOrder(QDataStream::LittleEndian);

    //
    // Trick-version-endian (10 bytes)
    //
    out.writeRawData("Trick-",6) ;
    out.writeRawData("07",2) ;
    out.writeRawData("-",1) ;
    out.writeRawData("L",1) ; // little endian

    //
    // Num params recorded (4 bytes - int)
    //
    qint32 nparams = (qint32)(params.size());
    out << nparams;

    //
    // Write trick header param info
    //
    foreach ( Param p, params ) {
        write_binary_param(out,p);
    }

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
                QFileInfo fi(f);
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

void write_binary_param(QDataStream& out, const Param& p)
{
    // Write name, unit and type info
    write_binary_qstring(p.name,out);
    write_binary_qstring(p.unit,out);
    qint32 t = (qint32)p.type; out << t;
    qint32 s = (qint32)p.size; out << s;
}


void write_binary_qstring(const QString &str, QDataStream& out)
{
    qint32 size_str = (qint32) str.size();
    out << size_str;
    out.writeRawData(str.toAscii().constData(),str.size());
}
