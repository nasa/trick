#include "plot.h"

Plot::Plot(DPPlot* plot, QWidget* parent) :
    QCustomPlot(parent),
    _dpplot(plot)
{
    setFocusPolicy(Qt::StrongFocus);

    setInteractions(QCP::iRangeDrag | QCP::iSelectAxes |
                    QCP::iSelectLegend | QCP::iSelectPlottables);

    QSizePolicy sizePolicy(QSizePolicy::MinimumExpanding,
                           QSizePolicy::MinimumExpanding);
    setSizePolicy(sizePolicy);

    setNoAntialiasingOnDrag(true);
    setAutoAddPlottableToLegend(false);

    _axisrect = new AxisRect(plot,this);

    plotLayout()->clear();
    plotLayout()->addElement(0,0,_axisrect);
}

void Plot::setData(MonteModel *monteModel)
{
    _axisrect->setData(monteModel);
    _axisrect->zoomToFit();
}

void Plot::keyPressEvent(QKeyEvent *event)
{
  _axisrect->keyPressEvent(event);
  QWidget::keyPressEvent(event);
}
