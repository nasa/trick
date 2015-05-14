#ifndef MONTE_MODEL_H
#define MONTE_MODEL_H

#include <QString>
#include <QTextStream>
#include <QStandardItemModel>
#include "trickcurvemodel.h"
#include "runs.h"

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
class MonteModel : public QStandardItemModel
{
  Q_OBJECT

  public:

    explicit MonteModel(Runs* runs,
                       QObject *parent = 0);
    ~MonteModel();

    TrickCurveModel* curve(const QModelIndex& xIdx,
                           const QModelIndex& yIdx,
                           double xScaleFactor=1.0,
                           double yScaleFactor=1.0) const;
    TrickCurveModel* curve(int row,
                           const QString& xparam,
                           const QString& yparam,
                           double xScaleFactor=1.0,
                           double yScaleFactor=1.0) const;

    virtual QVariant headerData(int sect,
                                Qt::Orientation orientation=Qt::Horizontal,
                                int role=Qt::DisplayRole) const;

  private:

    Runs* _runs;
    QStringList _params;
    QStringList _runDirs;

    static QString _err_string;
    static QTextStream _err_stream;

signals:

public slots:

};


#endif // MONTE_H
