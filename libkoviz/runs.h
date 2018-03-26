#ifndef RUNS_H
#define RUNS_H

#include <QString>
#include <QStringList>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QPair>
#include <QTextStream>
#include <QSet>
#include <QHash>
#include <QList>
#include <QStandardItemModel>
#include <QProgressDialog>
#include <stdexcept>
#include "datamodel.h"
#include "curvemodel.h"
#include "numsortitem.h"

class Runs
{
  public:
    Runs();
    Runs(const QStringList& timeNames,
         const QStringList &runDirs,
         const QHash<QString,QStringList> &varMap,
         bool isShowProgress);
    virtual ~Runs();
    virtual QStringList params() const { return _params; }
    virtual QStringList runDirs() const { return _runDirs; }
    CurveModel* curveModel(int row,
                      const QString& tName,
                      const QString& xName,
                      const QString& yName) const;

    static QStringList abbreviateRunNames(const QStringList& runNames);
    static QString commonPrefix(const QStringList &names, const QString &sep);
    static QString __commonPrefix(const QString &a, const QString &b,
                                  const QString &sep);
    static QString commonSuffix(const QStringList &names, const QString &sep);
    static QString __commonSuffix(const QString &a, const QString &b,
                                  const QString &sep);

  private:
    QStringList _timeNames;
    QStringList _runDirs;
    QHash<QString,QStringList> _varMap;
    bool _isShowProgress;
    QStringList _params;
    QHash<QString,QList<DataModel*>* > _paramToModels;
    QList<DataModel*> _models;

    void _init();

    static QString _err_string;
    static QTextStream _err_stream;
};


#endif // RUNS_H
