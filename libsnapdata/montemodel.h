#ifndef MONTE_MODEL_H
#define MONTE_MODEL_H

#include <QString>
#include <QTextStream>
#include <QHash>
#include "trickcurvemodel.h"
#include "monte.h"

//
// Monte carlo model
//
// Rows are RUNs
// Columns are curves of the yparam where t=x=sys.exec.out.time
//
//***********************************************************************
//        sys.exec.out.time    yparam1    yparam2......yparamj.....yparamN
// RUN_0  curve00              curve01    curve02..................curve0N
// RUN_1  curve10              curve11    curve12..................curve1N
//  .
// RUN_i                                              curve(i,j).........
//  .
// RUN_N  curveN0              curveN1    curveN2..................curveNN
//***********************************************************************
//
// curve(QModelIdx) returns a TrickCurveModel* for RUN,yparam (t=x=time)
// e.g.
// QModelIdx idx(runi,yparamj)
// TrickCurveModel* curve = monteModel->curve(idx);
//
class MonteModel : public QAbstractItemModel
{
  Q_OBJECT

  public:

    explicit MonteModel(Monte* monte,
                       QObject *parent = 0);
    ~MonteModel();

    virtual QModelIndex parent(const QModelIndex & index) const;
    virtual QModelIndex index(int row, int column,
                      const QModelIndex &pidx = QModelIndex()) const;

    virtual int rowCount(const QModelIndex & pidx = QModelIndex() ) const;
    virtual int columnCount(const QModelIndex & pidx = QModelIndex() ) const;
    virtual QVariant data (const QModelIndex & idx,
                           int role = Qt::DisplayRole ) const;

    // See comment at top of file
    TrickCurveModel* curve(const QModelIndex& idx) const;
    TrickCurveModel* curve(int row, const QString& param) const;

    // Convenience for getting column by param name
    int paramColumn(const QString& param);

    virtual QVariant headerData(int sect,
                                Qt::Orientation orientation=Qt::Horizontal,
                                int role=Qt::DisplayRole) const;

  private:

    Monte* _monte;
    QStringList _params;
    QHash<QString,int> _param2column;
    QStringList _runs;

    QString _err_string;
    QTextStream _err_stream;

signals:

public slots:

};


#endif // MONTE_H
