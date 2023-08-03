#ifndef TV_MODEL_H
#define TV_MODEL_H

#include <QHash>
#include <QFile>
#include <QDataStream>
#include <QTextStream>
#include <QAbstractTableModel>
#include <QString>
#include <QStringList>
#include "datamodel.h"
#include "parameter.h"

class TVModel;
class TVModelIterator;

class TVParam : public Parameter
{
  public:
    int nValuesMissed;
    QList<QVariant> values;
};

class TVModel : public DataModel
{
  Q_OBJECT

  friend class TVModelIterator;

  public:

    explicit TVModel(const QString &host, int port,
                     QObject *parent = 0) ;

    ~TVModel();

    virtual void map();
    virtual void unmap();
    virtual const Parameter* param(int col) const;
    virtual int paramColumn(const QString& param) const;
    virtual ModelIterator* begin(int tcol, int xcol, int ycol) const;
    virtual int indexAtTime(double time);

    virtual int rowCount(const QModelIndex& pidx=QModelIndex()) const;
    virtual int columnCount(const QModelIndex& pidx=QModelIndex()) const;
    virtual QVariant data(const QModelIndex& idx,
                          int role=Qt::DisplayRole) const;

  private:

    QString _host;
    int _port;
    QList<TVParam> _params;
    int _timeCol;
    TVModelIterator* _iteratorTimeIndex;

    int _idxAtTimeBinarySearch (TVModelIterator* it,
                                int low, int high, double time);
};

class TVModelIterator : public ModelIterator
{
  public:
    TVModelIterator() : i(0) {}

    inline TVModelIterator(int row, // iterator pos
                           const TVModel* model,
                           int tcol, int xcol, int ycol):
        i(row),
        _model(model),
        _row_count(model->rowCount()),
        _tcol(tcol), _xcol(xcol), _ycol(ycol)
    {
    }

    virtual ~TVModelIterator() {}

    virtual void start();
    virtual void next();
    virtual bool isDone() const;
    virtual TVModelIterator* at(int n);
    virtual double t() const;
    virtual double x() const;
    virtual double y() const;

  private:
    qint64 i;
    const TVModel* _model;
    int _row_count;
    int _tcol;
    int _xcol;
    int _ycol;
};

#endif // TV_MODEL_H
