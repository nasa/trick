#ifndef TRICK_CURVEMODEL_H
#define TRICK_CURVEMODEL_H

#include <QAbstractTableModel>
#include <QDataStream>
#include "snaptable.h"
#include "trickmodel.h"
#include "parameter.h"

class TrickCurveModel;

class TrickCurveModel : public QAbstractTableModel
{
  Q_OBJECT

  public:

    explicit TrickCurveModel(TrickModel* trickmodel,
                             int tcol, int xcol, int ycol,
                             QObject *parent = 0);
    ~TrickCurveModel();

    QString fileName() const { return _trickmodel->trkFile(); }

    Parameter t() const { return _trickmodel->param(_tcol); }
    Parameter x() const { return _trickmodel->param(_xcol); }
    Parameter y() const { return _trickmodel->param(_ycol); }

    void map() { _trickmodel->map(); }
    void unmap() { _trickmodel->unmap(); }
    TrickModelIterator begin() const;
    TrickModelIterator end() const;
    int indexAtTime(double time) { return _trickmodel->indexAtTime(time); }

    virtual int rowCount(const QModelIndex & pidx = QModelIndex() ) const;
    virtual int columnCount(const QModelIndex & pidx = QModelIndex() ) const;
    virtual QVariant data (const QModelIndex & index,
                           int role = Qt::DisplayRole ) const ;

  private:

    TrickModel* _trickmodel;
    int _tcol;
    int _xcol;
    int _ycol;
};

#endif // TRICKCURVEMODEL_H
