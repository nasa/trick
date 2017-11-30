#ifndef CURVE_MODEL_H
#define CURVE_MODEL_H

#include <QAbstractTableModel>
#include <QString>
#include "parameter.h"
#include "datamodel.h"

class CurveModel : public QAbstractTableModel
{
  Q_OBJECT

  public:

    explicit CurveModel(DataModel* datamodel,
                        int tcol, int xcol, int ycol,
                        QObject *parent = 0);

    ~CurveModel() {}

    QString fileName() const { return _datamodel->fileName(); }

    Parameter t() const { return _datamodel->param(_tcol); }
    Parameter x() const { return _datamodel->param(_xcol); }
    Parameter y() const { return _datamodel->param(_ycol); }

    void map() { _datamodel->map(); }
    void unmap() { _datamodel->unmap(); }
    ModelIterator* begin() const { return _datamodel->begin(_tcol,_xcol,_ycol);}
    int indexAtTime(double time) { return _datamodel->indexAtTime(time); }

    virtual int rowCount(const QModelIndex & pidx = QModelIndex() ) const;
    virtual int columnCount(const QModelIndex & pidx = QModelIndex() ) const;
    virtual QVariant data (const QModelIndex & index,
                           int role = Qt::DisplayRole ) const ;

  private:

    DataModel* _datamodel;
    int _tcol;
    int _xcol;
    int _ycol;
};

#endif // CURVE_MODEL_H
