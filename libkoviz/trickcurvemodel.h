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
                       const QString &tableName=QString(),
                       QObject *parent = 0);
    ~TrickCurveModel();

    QString  tableName () const { return _tableName ; }

    QString  trkFile () const { return _trickmodel->trkFile(); }

    Parameter* t() { return &_t ; }
    Parameter* x() { return &_x ; }
    Parameter* y() { return &_y ; }

    void map(){ _trickmodel->map(); }
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
    Parameter _t;
    Parameter _x;
    Parameter _y;
    int _tcol;
    int _xcol;
    int _ycol;
    QString _tableName;

signals:
    
public slots:
    
};

#endif // TRICKCURVEMODEL_H
