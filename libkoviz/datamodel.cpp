#include "datamodel.h"
#include "trickmodel.h"

DataModel *DataModel::createDataModel(const QStringList &timeNames,
                                      const QString &fileName)
{
    return new TrickModel(timeNames,fileName);
}
