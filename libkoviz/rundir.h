#ifndef RUNDIR_H
#define RUNDIR_H

#include "run.h"
#include "mapvalue.h"

#include <QDir>
#include <QString>
#include <QStringList>
#include <QRegExp>
#include <QHash>

class RunDir : public Run
{
public:
    RunDir(const QString& run,
           const QStringList& timeNames,
           const QHash<QString,QStringList> &varMap,
           const QString& filterPattern,
           const QString& excludePattern,
           bool isShowProgress);

    virtual QStringList params();
    virtual DataModel *dataModel(const QString& param);

private:
    const QHash<QString, QStringList> _varMap;
    QHash<QString,DataModel*> _param2model;
};

#endif // RUNDIR_H
