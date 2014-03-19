#ifndef PLOT_H
#define PLOT_H

#include "libqplot/qcustomplot.h"
#include "axisrect.h"
#include "libsnapdata/trickmodel.h"
#include "libsnapdata/montemodel.h"
#include "libsnapdata/monte.h"
#include "trickcurve.h"

class Plot : public QCustomPlot
{
    Q_OBJECT

public:
    explicit Plot(QWidget* parent=0);
    void setXAxisLabel(const QString& label);
    void setYAxisLabel(const QString& label);
    AxisRect* axisRect() { return _axisrect; }
    void drawMe(QCPPainter *painter);

protected:
    void keyPressEvent(QKeyEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);

signals:
    void keyPress(QKeyEvent* e);
    void curveClicked(TrickCurve* curve);

private slots:
    void _slotPlottableClick(QCPAbstractPlottable* plottable, QMouseEvent* e);
    void _slotPlottableDoubleClick(QCPAbstractPlottable* plottable,QMouseEvent*e);
    void _slotMouseDoubleClick(QMouseEvent *event);

private:
    AxisRect* _axisrect;
    QCPLayoutElement* _keyEventElement;
    bool _isPlottableClicked;
    TrickCurve* _lastSelectedCurve ;
    TrickCurve* _lastEmittedCurve ;
    TrickCurve* _lastDoubleClickedCurve ;
    bool _isDoubleClick;
};

#endif // PLOTPAGE_H
