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
//***********************************************************************
//        timeName             yparam1    yparam2......yparamj.....yparamN
// RUN_0  curve00              curve01    curve02..................curve0N
// RUN_1  curve10              curve11    curve12..................curve1N
//  .
// RUN_i                                              curve(i,j).........
//  .
// RUN_M  curveM0              curveM1    curveM2..................curveMN
//***********************************************************************
class MonteModel : public QStandardItemModel
{
  Q_OBJECT

  public:

    explicit MonteModel(Runs* runs,
                       QObject *parent = 0);
    ~MonteModel();

    TrickCurveModel* curve(int row,
                           const QString& tparam,
                           const QString& xparam,
                           const QString& yparam) const;

    virtual QVariant headerData(int sect,
                                Qt::Orientation orientation=Qt::Horizontal,
                                int role=Qt::DisplayRole) const;

  private:

    Runs* _runs;
    QStringList _params;
    QStringList _runDirs;

signals:

public slots:

};

#endif // MONTE_H
