#include <QFileInfo>
#include "datamodel.h"
#include "datamodel_trick.h"
#include "datamodel_csv.h"
#include "datamodel_optitrack_csv.h"
#include "datamodel_mot.h"

DataModel *DataModel::createDataModel(const QStringList &timeNames,
                                      const QString &runPath,
                                      const QString &fileName)
{
    DataModel* dataModel = 0;
    QFileInfo fi(fileName);
    if ( fi.suffix() == "trk") {
        dataModel = new TrickModel(timeNames,runPath,fileName);
    } else if ( fi.suffix() == "csv" ) {
        if ( _isOptiTrackCsv(fileName) ) {
            dataModel = new OptiTrackCsvModel(timeNames,runPath,fileName);
        } else {
            dataModel = new CsvModel(timeNames,runPath,fileName);
        }
    } else if ( fi.suffix() == "mot" ) {
        dataModel = new MotModel(timeNames,runPath,fileName);
    } else {
        fprintf(stderr,"koviz [error]: DataModel::createDataModel() cannot "
                       "handle file=\"%s\"\n",fileName.toLatin1().constData());
        exit(-1);
    }

    return dataModel;
}

bool DataModel::_isOptiTrackCsv(const QString &fileName)
{
    bool isOptiTrack = false;
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        fprintf(stderr,"koviz [error]: could not open %s\n",
                       fileName.toLatin1().constData());
        exit(-1);
    }
    QTextStream in(&file);
    in.setCodec("UTF-8");
    QString line0 = in.readLine();
    QString line1 = in.readLine();
    if ( line0.startsWith("Format Version") &&
         line1.startsWith("Take Name") ) {
        isOptiTrack = true;
    }
    file.close();
    return isOptiTrack;
}
