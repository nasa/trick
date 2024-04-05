#ifndef CURVE_MODEL_H
#define CURVE_MODEL_H

#include <QAbstractTableModel>
#include <QString>
#include "parameter.h"
#include "datamodel.h"
#include "curvemodelparameter.h"

class CurveModel : public QAbstractTableModel
{
  Q_OBJECT

  public:

    CurveModel() ;

    explicit CurveModel(DataModel* datamodel,
                        int tcol, int xcol, int ycol,
                        QObject *parent = 0);

    virtual ~CurveModel();

    virtual CurveModelParameter* t() { return _t; }
    virtual CurveModelParameter* x() { return _x; }
    virtual CurveModelParameter* y() { return _y; }

    virtual QString fileName() const { return _datamodel->fileName(); }
    virtual QString runPath() const { return _datamodel->runPath(); }

    virtual void map() { _datamodel->map(); }
    virtual void unmap() { _datamodel->unmap(); }
    virtual ModelIterator* begin() const { return _datamodel->begin(_tcol,_xcol,_ycol);}
    virtual int indexAtTime(double time) { return _datamodel->indexAtTime(time); }

    virtual int rowCount(const QModelIndex & pidx = QModelIndex() ) const;
    virtual int columnCount(const QModelIndex & pidx = QModelIndex() ) const;
    virtual QVariant data (const QModelIndex & index,
                           int role = Qt::DisplayRole ) const ;

    double* _real; // cache for fft
    double* _imag; // cache for fft

  private:

    DataModel* _datamodel;
    int _tcol;
    int _xcol;
    int _ycol;
    CurveModelParameter* _t;
    CurveModelParameter* _x;
    CurveModelParameter* _y;

};

#endif // CURVE_MODEL_H
