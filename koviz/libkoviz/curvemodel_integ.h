#ifndef CURVE_MODEL_INTEG_H
#define CURVE_MODEL_INTEG_H

#include <QAbstractTableModel>
#include <QString>
#include <QtMath>
#include <cmath>
#include "parameter.h"
#include "datamodel.h"
#include "curvemodelparameter.h"
#include "curvemodel.h"
#include "unit.h"

class CurveModelIntegral;
class IntegralModelIterator;

class CurveModelIntegral : public CurveModel
{
  Q_OBJECT

  friend class IntegralModelIterator;

  public:

    explicit CurveModelIntegral(CurveModel* curveModel, double initial_value);

    ~CurveModelIntegral();

    CurveModelParameter* t() { return _t; }
    CurveModelParameter* x() { return _x; }
    CurveModelParameter* y() { return _y; }

    QString fileName() const { return _fileName; }

    void map() {}
    void unmap() {}
    ModelIterator* begin() const ;
    int indexAtTime(double time);

    virtual int rowCount(const QModelIndex & pidx = QModelIndex() ) const;
    virtual int columnCount(const QModelIndex & pidx = QModelIndex() ) const;
    virtual QVariant data (const QModelIndex & index,
                           int role = Qt::DisplayRole ) const ;

  private:

    QString _fileName;
    double* _data;
    int _ncols;
    int _nrows;
    int _tcol;
    int _xcol;
    int _ycol;
    CurveModelParameter* _t;
    CurveModelParameter* _x;
    CurveModelParameter* _y;

    IntegralModelIterator* _iteratorTimeIndex;

    void _init(CurveModel *curveModel, double initial_value);
    int _idxAtTimeBinarySearch (IntegralModelIterator *it,
                                int low, int high, double time);
};

class IntegralModelIterator : public ModelIterator
{
  public:

    inline IntegralModelIterator():
        i(0),
        _tcol(0),
        _xcol(1),
        _ycol(2)
    {
    }

    inline IntegralModelIterator(const CurveModelIntegral* model) :
        i(0),
        _tcol(0),
        _xcol(1),
        _ycol(2),
        _model(model)
    {
    }

    virtual ~IntegralModelIterator() {}

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

    virtual IntegralModelIterator* at(int n)
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
    int _tcol;
    int _xcol;
    int _ycol;
    const CurveModelIntegral* _model;
};

#endif // CURVE_MODEL_INTEG
