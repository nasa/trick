#include "plot.h"

Plot::Plot(QWidget* parent) :
    QCustomPlot(parent)
{
    setFocusPolicy(Qt::StrongFocus);

    setInteractions(QCP::iRangeDrag | QCP::iSelectAxes |
                    QCP::iSelectLegend);

    QSizePolicy sizePolicy(QSizePolicy::MinimumExpanding,
                           QSizePolicy::MinimumExpanding);
    setSizePolicy(sizePolicy);

    setNoAntialiasingOnDrag(true);
    setAutoAddPlottableToLegend(false);

    _axisrect = new AxisRect(this);

    plotLayout()->clear();
    plotLayout()->addElement(0,0,_axisrect);

    connect(this,SIGNAL(plottableClick(QCPAbstractPlottable*,QMouseEvent*)),
            this,SLOT(_slotPlottableClick(QCPAbstractPlottable*,QMouseEvent*)));
}

TrickCurve *Plot::addCurve(TrickCurveModel *model)
{
    TrickCurve* curve = _axisrect->addCurve(model);
    _axisrect->zoomToFit();
    return curve;
}

void Plot::setData(MonteModel *monteModel)
{
    _axisrect->setData(monteModel);
    _axisrect->zoomToFit();
}

void Plot::setXAxisLabel(const QString &label)
{
    _axisrect->_xAxis->setLabel(label);
}

void Plot::setYAxisLabel(const QString &label)
{
    _axisrect->_yAxis->setLabel(label);
}

void Plot::keyPressEvent(QKeyEvent *event)
{
    emit keyPress(event);
    _axisrect->keyPressEvent(event);
    QWidget::keyPressEvent(event);
}

void Plot::mouseReleaseEvent(QMouseEvent *event)
{
    _isPlottableClicked = false;
    QCustomPlot::mouseReleaseEvent(event);
    if ( ! _isPlottableClicked ) {
        emit curveClicked(0);
    }
}

void Plot::_slotPlottableClick(QCPAbstractPlottable *plottable, QMouseEvent *e)
{
    Q_UNUSED(plottable);
    Q_UNUSED(e);
    _isPlottableClicked = true;
    TrickCurve* curve = static_cast<TrickCurve*>(plottable);
    emit curveClicked(curve);
}
