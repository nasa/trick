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
#include "trickmodel.h"
#include "trickcurvemodel.h"
#include "numsortitem.h"

class Runs
{
  public:
    Runs();
    Runs(const QStringList& timeNames,
         const QStringList &runDirs, const QHash<QString,QStringList> &varMap,
         int beginRun=0, int endRun=1.0e6);
    virtual ~Runs();
    virtual QStringList params() const { return _params; }
    virtual QStringList runs() const { return _runs; }
    TrickCurveModel* curve(int row,
                           const QString& tparam,
                           const QString& xparam,
                           const QString& yparam) const;

  private:
    QStringList _timeNames;
    QStringList _runs;
    QHash<QString,QStringList> _varMap;
    int _beginRun;
    int _endRun;
    QStringList _params;
    QHash<QString,QList<TrickModel*>* > _paramToModels;
    QList<TrickModel*> _models;

    void _init();

    static QString _err_string;
    static QTextStream _err_stream;
};


#endif // RUNS_H
