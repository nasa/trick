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

bool convert2csv(const QString& f);

class SnapOptions : public Options
{
  public:
    QStringList trkFiles;
};

SnapOptions opts;

Option::FPresetQStringList presetTrkFiles;
Option::FPostsetQStringList postsetTrkFiles;

int main(int argc, char *argv[])
{
    bool ok;

    opts.add("<trk file list>:+", &opts.trkFiles, QStringList(),
             "List of trk files",
             presetTrkFiles, postsetTrkFiles);
    opts.parse(argc,argv, QString("trk2csv"), &ok);

    if ( !ok ) {
        fprintf(stderr, "%s\n", opts.usage().toAscii().constData());
        return -1;
    }

    try {

        QApplication a(argc, argv);
        foreach ( QString f, opts.trkFiles ) {
            bool ret = convert2csv(f);
            if ( !ret )  {
                fprintf(stderr, "trk2csv [error]: Aborting!\n");
                exit(-1);
            }
        }

    } catch (std::exception &e) {
        fprintf(stderr,"\n%s\n",e.what());
        fprintf(stderr,"%s\n",opts.usage().toAscii().constData());
        exit(-1);
    }

    return 0;
}

void presetTrkFiles(QStringList* ignoreMe, const QStringList& trks,bool* ok)
{
    Q_UNUSED(ignoreMe);

    foreach ( QString f, trks ) {
        QFileInfo fi(f);
        if ( !fi.exists() ) {
            fprintf(stderr,
                    "trk2csv [error] : Couldn't find file: \"%s\".\n",
                    f.toAscii().constData());
            *ok = false;
            return;
        }
    }
}

// Placeholder
void postsetTrkFiles (QStringList* trks, bool* ok)
{
    Q_UNUSED(trks);
    Q_UNUSED(ok);
}

bool convert2csv(const QString& f)
{
    TrickModel m(f);

    // Make csv file name based on trk file name
    QFileInfo fi(f);
    QString fcsv = QString("%1.csv").arg(fi.baseName());
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
        QString pName = m.headerData(i,Qt::Horizontal,Param::Name).toString();
        QString pUnit = m.headerData(i,Qt::Horizontal,Param::Unit).toString();
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
