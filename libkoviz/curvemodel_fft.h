#ifndef CURVE_MODEL_FFT_H
#define CURVE_MODEL_FFT_H

#include <QAbstractTableModel>
#include <QString>
#include <QtMath>
#include "parameter.h"
#include "datamodel.h"
#include "curvemodelparameter.h"
#include "curvemodel.h"
#include "fft.h"

class CurveModelFFT;
class FFTModelIterator;

class CurveModelFFT : public CurveModel
{
  Q_OBJECT

  friend class FFTModelIterator;

  public:

    explicit CurveModelFFT(CurveModel* curveModel, double xb, double xs,
                           double begX, double endX);

    ~CurveModelFFT();

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
    double _begX;
    double _endX;
    double _xb;
    double _xs;
    double* _data;
    int _ncols;
    int _nrows;
    int _tcol;
    int _xcol;
    int _ycol;
    CurveModelParameter* _t;
    CurveModelParameter* _x;
    CurveModelParameter* _y;

    FFTModelIterator* _iteratorTimeIndex;

    void _init(CurveModel *curveModel);
    int _idxAtTimeBinarySearch (FFTModelIterator *it,
                                int low, int high, double time);
};

class FFTModelIterator : public ModelIterator
{
  public:

    inline FFTModelIterator():
        i(0),
        _tcol(0),
        _xcol(1),
        _ycol(2)
    {
    }

    inline FFTModelIterator(const CurveModelFFT* model) :
        i(0),
        _tcol(0),
        _xcol(1),
        _ycol(2),
        _model(model)
    {
    }

    virtual ~FFTModelIterator() {}

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

    virtual FFTModelIterator* at(int n)
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
    const CurveModelFFT* _model;
};

#endif // CURVE_MODEL_H
