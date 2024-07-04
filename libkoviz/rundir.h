#ifndef RUNDIR_H
#define RUNDIR_H

#include "run.h"
#include "mapvalue.h"
#include "datamodel_csv.h"

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
           const QString& excludePattern);

    virtual QStringList params();
    virtual DataModel *dataModel(const QString& param);

    static bool isValid(const QString& run,
                        const QStringList &timeNames,
                        const QString& filterPattern,
                        const QString& excludePattern);

private:
    const QHash<QString, QStringList> _varMap;
    QHash<QString,DataModel*> _param2model;

    static QStringList _fileList(const QString &run,
                                 const QStringList &timeNames,
                                 const QString& filterPattern,
                                 const QString& excludePattern);
};

#endif // RUNDIR_H
