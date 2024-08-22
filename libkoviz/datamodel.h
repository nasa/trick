#ifndef DATA_MODEL_H
#define DATA_MODEL_H

#include <QHash>
#include <QFile>
#include <QDataStream>
#include <QTextStream>
#include <QAbstractTableModel>
#include <QString>
#include <QStringList>
#include "parameter.h"

class DataModel;
class ModelIterator;

class DataModel : public QAbstractTableModel
{
  Q_OBJECT

  public:

    explicit DataModel(const QStringList &timeNames,
                       const QString &runPath,
                       const QString &fileName,
                       QObject *parent = 0) :
        QAbstractTableModel(parent),
        _timeNames(timeNames),
        _runPath(runPath),
        _fileName(fileName)
    {}

    ~DataModel() {}

    static DataModel* createDataModel(const QStringList& timeNames,
                                      const QString& runPath,
                                      const QString& fileName);

    QString runPath() const { return _runPath; }
    QString fileName() const { return _fileName; }

    virtual void map() = 0;
    virtual void unmap() = 0;
    virtual const Parameter* param(int col) const = 0;
    virtual int paramColumn(const QString& param) const = 0;
    virtual ModelIterator* begin(int tcol, int xcol, int ycol) const = 0;
    virtual int indexAtTime(double time) = 0 ;

    virtual int rowCount(const QModelIndex& pidx=QModelIndex()) const = 0;
    virtual int columnCount(const QModelIndex& pidx=QModelIndex()) const = 0;
    virtual QVariant data(const QModelIndex& idx,
                          int role=Qt::DisplayRole) const = 0;

  private:

    QStringList _timeNames;
    QString _runPath;
    QString _fileName;
};

class ModelIterator
{
  public:
    ModelIterator() {}

    virtual ~ModelIterator() {}

    virtual void start() = 0;
    virtual void next() = 0;
    virtual bool isDone() const = 0;
    virtual ModelIterator* at(int n) = 0;
    virtual double t() const = 0;
    virtual double x() const = 0;
    virtual double y() const = 0;
};

#endif // DATA_MODEL_H
