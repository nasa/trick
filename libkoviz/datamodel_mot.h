#ifndef MOT_MODEL_H
#define MOT_MODEL_H

#include <stdio.h>
#include <stdlib.h>
#include <QString>
#include <QStringList>
#include <QVariant>
#include <QTextStream>
#include <QProgressDialog>
#include <QFileInfo>
#include <stdexcept>

#include "datamodel.h"
#include "parameter.h"
#include "unit.h"
#include "timeit_linux.h"

class MotModel;
class MotModelIterator;

class MotModel : public DataModel
{
  Q_OBJECT

  friend class MotModelIterator;

  public:

    explicit MotModel(const QStringList &timeNames,
                      const QString &runPath,
                      const QString &motfile,
                      QObject *parent = 0);
    ~MotModel();

    virtual const Parameter* param(int col) const ;
    virtual void map();
    virtual void unmap();
    virtual int paramColumn(const QString& paramName) const ;
    virtual ModelIterator* begin(int tcol, int xcol, int ycol) const ;
    int indexAtTime(double time);

    virtual int rowCount(const QModelIndex & pidx = QModelIndex() ) const;
    virtual int columnCount(const QModelIndex & pidx = QModelIndex() ) const;
    virtual QVariant data (const QModelIndex & index,
                           int role = Qt::DisplayRole ) const;

  private:

    QStringList _timeNames;
    QString _motfile;

    int _nrows;
    int _ncols;
    int _timeCol;

    QHash<int,Parameter*> _col2param;
    QHash<QString,int> _paramName2col;
    MotModelIterator* _iteratorTimeIndex;

    double* _data;

    static QString _err_string;
    static QTextStream _err_stream;

    void _init();
    int _idxAtTimeBinarySearch (MotModelIterator *it,
                               int low, int high, double time);

    inline double _convert(const QString& s);
};

class MotModelIterator : public ModelIterator
{
  public:

    inline MotModelIterator(): i(0) {}

    inline MotModelIterator(int row, // iterator pos
                            const MotModel* model,
                            int tcol, int xcol, int ycol):
        i(row),
        _model(model),
        _tcol(tcol), _xcol(xcol), _ycol(ycol)
    {
    }

    virtual ~MotModelIterator() {}

    virtual void start()
    {
        i = 0;
    }

    virtual void next()
    {
        ++i;
    }

    virtual bool isDone() const
    {
        return ( i >= _model->rowCount() ) ;
    }

    virtual MotModelIterator* at(int n)
    {
        i = n;
        return this;
    }

    inline double t() const
    {
        return _model->_data[i*_model->_ncols+_tcol];
    }

    inline double x() const
    {
        return _model->_data[i*_model->_ncols+_xcol];
    }

    inline double y() const
    {
        return _model->_data[i*_model->_ncols+_ycol];
    }

  private:

    int i;
    const MotModel* _model;
    int _tcol;
    int _xcol;
    int _ycol;
};


#endif // MOTMODEL_H
