#ifndef RUN_H
#define RUN_H

#include <QStringList>
#include "datamodel.h"


class Run
{
public:
    Run();
    virtual ~Run(){};
    virtual QStringList params() = 0;
    virtual DataModel* dataModel(const QString& param) = 0;
};

#endif // RUN_H
