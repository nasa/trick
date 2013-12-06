#include "plot.h"

Plot::Plot(QWidget* parent) :
    QCustomPlot(parent),
    _lastSelectedCurve(0),
    _lastEmittedCurve(0),
    _lastDoubleClickedCurve(0),
    _isDoubleClick(false)

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
    connect(this,SIGNAL(plottableDoubleClick(QCPAbstractPlottable*,QMouseEvent*)),
            this,SLOT(_slotPlottableDoubleClick(QCPAbstractPlottable*,QMouseEvent*)));
    connect(this,SIGNAL(mouseDoubleClick(QMouseEvent*)),
            this,SLOT(_slotMouseDoubleClick(QMouseEvent*)));
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
    if ( ! _isPlottableClicked && !_isDoubleClick ) {
        if ( _lastEmittedCurve == 0 ) {
            _lastSelectedCurve = 0 ;
        }
        emit curveClicked(0);
        _lastEmittedCurve = 0 ;
    } else if ( _isPlottableClicked && !_isDoubleClick ) {
        emit curveClicked(_lastSelectedCurve);
        _lastEmittedCurve = _lastSelectedCurve ;
        _lastSelectedCurve = _lastEmittedCurve;
    } else if ( !_isPlottableClicked && _isDoubleClick ) {
        emit curveClicked(_lastSelectedCurve);
        _lastEmittedCurve = _lastSelectedCurve ;
    } else if ( _isPlottableClicked && _isDoubleClick ) {
        if ( _lastDoubleClickedCurve ) {
            emit curveClicked(_lastDoubleClickedCurve);
            _lastSelectedCurve = _lastDoubleClickedCurve;
        }
    }
    if ( _isDoubleClick ) {
        _isDoubleClick = false;
    }
}

void Plot::_slotPlottableClick(QCPAbstractPlottable *plottable, QMouseEvent *e)
{
    Q_UNUSED(plottable);
    Q_UNUSED(e);
    _isPlottableClicked = true;
    _lastSelectedCurve = static_cast<TrickCurve*>(plottable);
}

void Plot::_slotPlottableDoubleClick(QCPAbstractPlottable *plottable, QMouseEvent *e)
{
    _lastDoubleClickedCurve = static_cast<TrickCurve*>(plottable);
}

void Plot::_slotMouseDoubleClick(QMouseEvent *event)
{
    _lastDoubleClickedCurve = 0 ;
    _isDoubleClick = true;
}
