#ifndef CURVE_MODEL_SG_H
#define CURVE_MODEL_SG_H

#include <QAbstractTableModel>
#include <QString>
#include <QtMath>
#include "parameter.h"
#include "datamodel.h"
#include "curvemodelparameter.h"
#include "curvemodel.h"
#include "filter.h"
#include "filter_sgolay.h"

class SGModelIterator;

class CurveModelSG : public CurveModel
{
  Q_OBJECT

  friend class SGModelIterator;

  public:

    explicit CurveModelSG(CurveModel* curveModel,int window,int degree);

    ~CurveModelSG();

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
    int _window;       // SGolay window (num points to create poly fit)
    int _degree;       // Degree of polynomial for curve fitting
    double* _data;
    int _ncols;
    int _nrows;
    int _tcol;
    int _xcol;
    int _ycol;
    CurveModelParameter* _t;
    CurveModelParameter* _x;
    CurveModelParameter* _y;
    SGModelIterator* _iteratorTimeIndex;

    void _init(CurveModel* curveModel);
    int _idxAtTimeBinarySearch (SGModelIterator* it,
                                int low, int high, double time);
};

class SGModelIterator : public ModelIterator
{
  public:

    inline SGModelIterator():
        i(0),
        _tcol(0),
        _xcol(1),
        _ycol(2)
    {
    }

    inline SGModelIterator(const CurveModelSG* model) :
        i(0),
        _tcol(0),
        _xcol(1),
        _ycol(2),
        _model(model)
    {
    }

    virtual ~SGModelIterator() {}

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

    virtual SGModelIterator* at(int n)
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
    const CurveModelSG* _model;
};

#endif // CURVE_MODEL_SG_H
