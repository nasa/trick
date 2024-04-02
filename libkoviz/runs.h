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
#include <QRegExp>
#include <stdexcept>
#include "datamodel.h"
#include "curvemodel.h"
#include "numsortitem.h"
#include "mapvalue.h"
#include "rundir.h"
#include "runfile.h"

class Runs
{
  public:
    Runs();
    Runs(const QStringList& timeNames,
         const QStringList &runDirs,
         const QHash<QString,QStringList> &varMap,
         const QString& filterPattern,
         const QString& excludePattern,
         bool isShowProgress);
    virtual ~Runs();
    virtual QStringList params() const { return _params; }
    virtual QStringList runPaths() const { return _runPaths; }
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
    QStringList _runPaths;
    QHash<QString,QStringList> _varMap;
    QString _filterPattern;
    QString _excludePattern;
    bool _isShowProgress;
    QStringList _params;
    QHash<QString,QList<DataModel*>* > _paramToModels;
    QList<DataModel*> _models;
    QHash<QString,int> _rundir2row;

    void _init();
    QList<Run*> _runs;

    DataModel* _paramModel(const QString& param, const QString &run) const;
    int _paramColumn(DataModel* model, const QString& param) const;

    static QString _err_string;
    static QTextStream _err_stream;
};


#endif // RUNS_H
