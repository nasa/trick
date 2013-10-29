#ifndef PLOT_H
#define PLOT_H

#include "qplot/qcustomplot.h"
#include "axisrect.h"
#include "dp.h"
#include "trickmodel.h"
#include "monte.h"

class Plot : public QCustomPlot
{
    Q_OBJECT

public:
    explicit Plot(DPPlot *plot, QWidget* parent=0);
    explicit Plot(const QModelIndex& plotIdx, QWidget* parent=0);
    TrickCurve *addCurve(TrickCurveModel *model);
    void setData(MonteModel *monteModel);

protected:
    void keyPressEvent(QKeyEvent *event);

private:
    DPPlot* _dpplot;
    AxisRect* _axisrect;
    QCPLayoutElement* _keyEventElement;

    
signals:
    
public slots:
    
};

#endif // PLOTPAGE_H
