#include "datamodel.h"
#include "trickmodel.h"

DataModel *DataModel::createDataModel(const QStringList &timeNames,
                                      const QString &fileName,
                                      double startTime, double stopTime)
{
    return new TrickModel(timeNames,fileName,startTime,stopTime);
}
