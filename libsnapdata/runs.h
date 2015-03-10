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
#include <stdexcept>
#include "trickmodel.h"
#include "numsortitem.h"

class Runs
{
  public:
    Runs();
    Runs(const QStringList &runDirs, double startTime=0, double endTime=1.0e20);
    ~Runs();
    virtual QStringList params() const { return _params; }
    virtual QStringList runs() const { return _runs; }
    virtual QList<TrickModel*>* models(const QString& param);

    virtual QString parentDir() const { return _parentDir; }

  protected:
    virtual void _setParentDir(const QString& parentDir);
    bool _setDirs(const QStringList &dirs);

  private:
    QString _parentDir;
    QStringList _runs;
    double _startTime;
    double _stopTime;
    QStringList _params;
    QHash<QString,QList<TrickModel*>* > _paramToModels;
    QList<TrickModel*> _models;

    static QString _err_string;
    static QTextStream _err_stream;
};


#endif // RUNS_H
