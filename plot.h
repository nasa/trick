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
    explicit Plot(QWidget* parent=0);
    TrickCurve *addCurve(TrickCurveModel *model);
    void setData(MonteModel *monteModel);
    void setXAxisLabel(const QString& label);
    void setYAxisLabel(const QString& label);

protected:
    void keyPressEvent(QKeyEvent *event);

signals:
    void keyPress(QKeyEvent* e);
    void curveClicked(TrickCurve* curve);

private slots:
    void _slotPlottableClick(QCPAbstractPlottable* plottable, QMouseEvent* e);

private:
    DPPlot* _dpplot;
    AxisRect* _axisrect;
    QCPLayoutElement* _keyEventElement;
};

#endif // PLOTPAGE_H
