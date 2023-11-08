#ifndef TV_MODEL_H
#define TV_MODEL_H

#include <QHash>
#include <QFile>
#include <QDataStream>
#include <QTextStream>
#include <QAbstractTableModel>
#include <QString>
#include <QStringList>
#include <QTcpSocket>
#include "datamodel.h"
#include "parameter.h"

class TVModel;
class TVModelIterator;

class TVParam : public Parameter
{
  public:
    TVParam(const QString& name, const QString& unit) : Parameter(name,unit) {}
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
    virtual int paramColumn(const QString& paramName) const;
    virtual ModelIterator* begin(int tcol, int xcol, int ycol) const;
    virtual int indexAtTime(double time);

    virtual int rowCount(const QModelIndex& pidx=QModelIndex()) const;
    virtual int columnCount(const QModelIndex& pidx=QModelIndex()) const;
    virtual QVariant data(const QModelIndex& idx,
                          int role=Qt::DisplayRole) const;

    virtual bool insertRows(int row, int count,
                            const QModelIndex &parent = QModelIndex());

    void addParam(const QString& paramName, const QString &unit);

  private:

    QString _host;
    int _port;
    QTcpSocket _vsSocket;
    QList<TVParam*> _params;
    int _timeCol;
    TVModelIterator* _iteratorTimeIndex;

    void _init();
    int _idxAtTimeBinarySearch (TVModelIterator* it,
                                int low, int high, double time);
    QList<QVariant> _vsReadLine();

  private slots:
    void _vsRead();
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

    virtual void start()
    {
        i = 0;
        _row_count = _model->rowCount();
    }

    virtual void next()
    {
        ++i;
    }

    virtual bool isDone() const
    {
        return ( i >= _row_count ) ;
    }

    virtual TVModelIterator* at(int n)
    {
        i = n;
        return this;
    }

    inline double t() const
    {
        return _getValueAtColumn(_tcol);
    }

    inline double x() const
    {
        return _getValueAtColumn(_xcol);
    }

    inline double y() const
    {
        return _getValueAtColumn(_ycol);
    }

  private:
    qint64 i;
    const TVModel* _model;
    int _row_count;
    int _tcol;
    int _xcol;
    int _ycol;

    inline double _getValueAtColumn(int col) const
    {
        double val;

        int max = 0;
        foreach (TVParam* param, _model->_params) {
            if ( param->values.size() > max ) {
                max = param->values.size();
            }
        }

        int offset = max - _model->_params.at(col)->values.size();

        if ( i-offset >= 0 ) {
            bool ok = false;
            val = _model->_params.at(col)->values.at(i-offset).toDouble(&ok);
            if ( !ok ) {
                val = std::numeric_limits<double>::quiet_NaN();
            }
        } else {
            val = std::numeric_limits<double>::quiet_NaN();
        }

        return val;
    }

};

#endif // TV_MODEL_H
