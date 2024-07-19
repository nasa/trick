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
#include <QMessageBox>
#include <stdexcept>
#include "datamodel.h"
#include "curvemodel.h"
#include "numsortitem.h"
#include "mapvalue.h"
#include "rundir.h"
#include "runfile.h"

class Runs : public QObject
{
  Q_OBJECT

  public:
    Runs();
    Runs(const QStringList& timeNames,
         const QStringList &runPaths,
         const QHash<QString,QStringList> &varMap,
         const QString& filterPattern,
         const QString& excludePattern,
         bool isShowProgress);
    virtual ~Runs();
    const QStringList& params() const { return _params; }
    const QStringList& runPaths() const { return _runPaths; }
    void refresh() ;
    void clear();
    void addRun(const QString& runPath);
    void deleteRun(const QString& runPath);
    CurveModel* curveModel(int row,
                           const QString& tName,
                           const QString& xName,
                           const QString& yName) const;
    QStandardItemModel* runsModel();
    bool isValidRunPath(const QString& runPath);

  public:
    static QStringList abbreviateRunNames(const QStringList& runNames);
    static QString commonPrefix(const QStringList &names, const QString &sep);
    static QString __commonPrefix(const QString &a, const QString &b,
                                  const QString &sep);
    static QString commonSuffix(const QStringList &names, const QString &sep);
    static QString __commonSuffix(const QString &a, const QString &b,
                                  const QString &sep);

  signals:
    void runsRefreshed();

  private:
    QStringList _timeNames;
    QStringList _runPaths;
    QString _montePath;
    QHash<QString,QStringList> _varMap;
    QString _filterPattern;
    QString _excludePattern;
    bool _isShowProgress;
    QStringList _params;
    QHash<QString,QList<DataModel*>* > _paramToModels;
    QList<DataModel*> _models;
    QHash<QString,int> _rundir2row;
    QStandardItemModel* _runsModel;

    void _init();
    void _delete();
    QList<Run*> _runs;

    DataModel* _paramModel(const QString& param, const QString &run) const;
    int _paramColumn(DataModel* model, const QString& param) const;

    void _loadRunsModel(QStandardItemModel* runsModel,
                        const QString& montePath,
                        const QStringList& runPaths);
    void __loadRunsModel(QStandardItemModel* model,
                         const QStringList &runs);
    void _loadMonteInputModel(QStandardItemModel *model,
                              const QString &montePath,
                              const QStringList &runPaths);
    void _loadMonteInputModelTrick07(QStandardItemModel* model,
                                     const QString &monteInputFile,
                                     const QStringList &runs);
    void _loadMonteInputModelTrick17(QStandardItemModel* model,
                                     const QString &monteInputFile,
                                     const QStringList &runs);

    QStringList _runsSubset(const QStringList& runsList,
                           const QString& filterPattern,
                           const QString& excludePattern,
                           uint beginRun, uint endRun);

    static QString _err_string;
    static QTextStream _err_stream;
};


#endif // RUNS_H
