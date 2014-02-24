#ifndef MONTE_H
#define MONTE_H

#include <QString>
#include <QDir>
#include <QFile>
#include <QPair>
#include <QTextStream>
#include <QSet>
#include <QHash>
#include <QList>
#include <QStandardItemModel>
#include <stdexcept>
#include "trickmodel.h"
#include "numsortitem.h"

class Monte
{
  public:
    Monte(const QString &dirname, int beginRun=0, int endRun=1.0e6);
    ~Monte();
    bool setDir(const QString& montedir);
    QStringList params() const { return _params; }
    QStringList runs() const { return _runsSubset; }
    QList<TrickModel*>* models(const QString& param);
    QStandardItemModel* inputModel();   // rows:monteinputvars cols:values

  private:
    QString _montedir;
    int _beginRun;
    int _endRun;
    QStringList _runs;
    QStringList _runsSubset; // subset bounds are [_beginRun,_endRun]
    QMap<QString,QString> _param2ftrk;
    QStringList _params;
    QHash<QString,QList<TrickModel*>* > _ftrk2models;
    QString _err_string;
    QTextStream _err_stream;
};


#endif // MONTE_H
