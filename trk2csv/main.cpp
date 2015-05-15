#include <QApplication>
#include <QObject>
#include <QSet>


#include <string>
using namespace std;

#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QTextStream>
#include <stdio.h>

#include "libopts/options.h"
#include "libsnapdata/trickmodel.h"

//#include "libsnapdata/runs.h"
//#include "libqplot/plotmainwindow.h"
//#include "libsnapdata/unit.h"
//#include "libsnapdata/csv.h"
//#include "libsnapdata/trick_types.h"

bool convert2csv(const QString& ftrk, const QString &fcsv);

class SnapOptions : public Options
{
  public:
    QString trkFile;
    QString csvOutFile;
};

SnapOptions opts;

Option::FPresetQString presetTrkFile;
Option::FPostsetQString postsetTrkFile;
Option::FPresetQString presetCsvFile;

int main(int argc, char *argv[])
{
    bool ok;

    opts.add("<trk file>", &opts.trkFile, QString(),
             "Trk file to convert to csv",
             presetTrkFile, postsetTrkFile);
    opts.add("-csv", &opts.csvOutFile, QString(),
             "Name of csv outputfile",
             presetCsvFile);
    opts.parse(argc,argv, QString("trk2csv"), &ok);

    if ( !ok ) {
        fprintf(stderr, "%s\n", opts.usage().toAscii().constData());
        return -1;
    }

    try {

        QApplication a(argc, argv);
        if ( opts.csvOutFile.isEmpty() ) {
            QFileInfo fi(opts.trkFile);
            opts.csvOutFile = QString("%1.csv").arg(fi.baseName());
        }
        bool ret = convert2csv(opts.trkFile, opts.csvOutFile);
        if ( !ret )  {
            fprintf(stderr, "trk2csv [error]: Aborting!\n");
            exit(-1);
        }

    } catch (std::exception &e) {
        fprintf(stderr,"\n%s\n",e.what());
        fprintf(stderr,"%s\n",opts.usage().toAscii().constData());
        exit(-1);
    }

    return 0;
}

void presetTrkFile(QString* ignoreMe, const QString& trk, bool* ok)
{
    Q_UNUSED(ignoreMe);

    QFileInfo fi(trk);
    if ( !fi.exists() ) {
        fprintf(stderr,
                "trk2csv [error] : Couldn't find file: \"%s\".\n",
                trk.toAscii().constData());
        *ok = false;
        return;
    }
}

// Placeholder
void postsetTrkFile (QString* trk, bool* ok)
{
    Q_UNUSED(trk);
    Q_UNUSED(ok);
}

void presetCsvFile(QString* v, const QString& fcsv, bool* ok)
{

    Q_UNUSED(v);

    *ok = true;
    QFileInfo fcsvi(fcsv);
    if ( fcsvi.exists() ) {
        fprintf(stderr, "snapq [error]: %s exists, will not overwrite\n",
                fcsv.toAscii().constData());
        *ok = false;
    }
}

bool convert2csv(const QString& ftrk, const QString& fcsv)
{
    TrickModel m(ftrk);

    QFileInfo fcsvi(fcsv);
    if ( fcsvi.exists() ) {
        fprintf(stderr, "trk2csv [error]: Will not overwrite %s\n",
                fcsv.toAscii().constData());
        return false;
    }

    // Open csv file stream
    QFile csv(fcsv);
    if (!csv.open(QIODevice::WriteOnly)) {
        fprintf(stderr,"Snap: [error] could not open %s\n",
                fcsv.toAscii().constData());
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
