#ifndef TRICK_CURVEMODEL_H
#define TRICK_CURVEMODEL_H

#include <QDebug>
#include <QDataStream>
#include "snaptable.h"
#include "trickmodel.h"

class TrickCurveModel;

class TrickCurveModel : public SnapTable
{
  Q_OBJECT

  public:

    explicit TrickCurveModel(TrickModel* trickmodel,
                             int tcol, int xcol, int ycol,
                       const QString &tableName=QString(),
                             double yScaleFactor=1.0,
                       QObject *parent = 0);
    ~TrickCurveModel();

    void map(){ _trickmodel->map(); }
    void unmap() { _trickmodel->unmap(); }
    int indexAtTime(double time) { return _trickmodel->indexAtTime(time); }

    TrickModelIterator begin() const;
    TrickModelIterator end() const;

    virtual int rowCount(const QModelIndex & pidx = QModelIndex() ) const;
    virtual int columnCount(const QModelIndex & pidx = QModelIndex() ) const;

    virtual QVariant data (const QModelIndex & index,
                           int role = Qt::DisplayRole ) const
    {
        return _trickmodel->data(index,role);
    }

    virtual bool setData (const QModelIndex & idx,
                  const QVariant & value,
                  int role = Role::EditNoEmitDataChange )
    {
        return _trickmodel->setData(idx,value,role);
    }

    virtual bool insertRows(int row, int count,
                       const QModelIndex &pidx = QModelIndex());
    virtual bool removeRows(int row, int count,
                       const QModelIndex &pidx = QModelIndex());
    virtual bool insertColumns(int column, int count,
                       const QModelIndex &pidx = QModelIndex());
    virtual bool removeColumns(int column, int count, const QModelIndex &pidx
                                                               = QModelIndex());
  protected:
    bool _hasColumnRoles() { return true; }
    bool _hasRowRoles() { return false; }
    Role* _createColumnRole();

  private:

    TrickModel* _trickmodel;
    int _tcol;
    int _xcol;
    int _ycol;
    double _yScaleFactor;

signals:
    
public slots:
    
};

#endif // TRICKCURVEMODEL_H
