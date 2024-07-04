#ifndef RUNFILE_H
#define RUNFILE_H

#include "run.h"
#include "mapvalue.h"

#include <QDir>
#include <QString>
#include <QStringList>
#include <QHash>

class RunFile : public Run
{
public:
    RunFile(const QString& run,
           const QStringList& timeNames,
           const QHash<QString,QStringList> &varMap);

    virtual QStringList params();
    virtual DataModel *dataModel(const QString& param);

    static bool isValid(const QString& run);
private:
    const QHash<QString, QStringList> _varMap;
    DataModel* _model;
    QStringList _params;
};

#endif // RUNFILE_H
