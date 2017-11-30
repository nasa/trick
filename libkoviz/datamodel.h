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
                       const QString &fileName,
                       double startTime=0.0,
                       double stopTime=1.0e20,
                       QObject *parent = 0) :
        QAbstractTableModel(parent),
        _timeNames(timeNames),
        _fileName(fileName),
        _startTime(startTime),
        _stopTime(stopTime)
    {}

    ~DataModel() {}

    static DataModel* createDataModel(const QStringList& timeNames,
                                      const QString& fileName,
                                      double startTime=0.0,
                                      double stopTime=1.0e20);

    QString fileName() const { return _fileName; }
    double startTime() { return _startTime; }
    double stopTime() { return _stopTime; }

    virtual void map() = 0;
    virtual void unmap() = 0;
    virtual Parameter param(int col) const = 0;
    virtual int paramColumn(const QString& param) const = 0;
    virtual ModelIterator* begin(int tcol, int xcol, int ycol) const = 0;
    virtual int indexAtTime(double time) = 0 ;

    virtual int rowCount(const QModelIndex& pidx=QModelIndex()) const = 0;
    virtual int columnCount(const QModelIndex& pidx=QModelIndex()) const = 0;
    virtual QVariant data(const QModelIndex& idx,
                          int role=Qt::DisplayRole) const = 0;

  private:

    QStringList _timeNames;
    QString _fileName;
    double _startTime;
    double _stopTime;
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
