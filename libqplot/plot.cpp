#include "plot.h"

Plot::Plot(QWidget* parent) :
    QCustomPlot(parent),
    _titleItem(0),
    _lastSelectedCurve(0),
    _lastEmittedCurve(0),
    _lastDoubleClickedCurve(0),
    _isDoubleClick(false)

{
    setFocusPolicy(Qt::ClickFocus);

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

    _titleItem = new QCPItemText(this);
    _titleItem->position->setType(QCPItemPosition::ptAxisRectRatio);
    _titleItem->position->setAxisRect(_axisrect);
    _titleItem->position->setCoords(0.5,0.025);
    _titleItem->setPositionAlignment(Qt::AlignHCenter);
    _titleItem->setText("");

    connect(this,SIGNAL(plottableClick(QCPAbstractPlottable*,QMouseEvent*)),
            this,SLOT(_slotPlottableClick(QCPAbstractPlottable*,QMouseEvent*)));
    connect(this,SIGNAL(plottableDoubleClick(QCPAbstractPlottable*,QMouseEvent*)),
            this,SLOT(_slotPlottableDoubleClick(QCPAbstractPlottable*,QMouseEvent*)));
    connect(this,SIGNAL(mouseDoubleClick(QMouseEvent*)),
            this,SLOT(_slotMouseDoubleClick(QMouseEvent*)));
}

void Plot::setTitle(const QString &title)
{
    _titleItem->setText(title);
}

void Plot::setXAxisLabel(const QString &label)
{
    _axisrect->_xAxis->setLabel(label);
}

void Plot::setYAxisLabel(const QString &label)
{
    _axisrect->_yAxis->setLabel(label);
}

void Plot::setXMinRange(double xMin)
{
    _axisrect->setXMinRange(xMin);
}

void Plot::setXMaxRange(double xMax)
{
    _axisrect->setXMaxRange(xMax);
}

void Plot::setYMinRange(double yMin)
{
    _axisrect->setYMinRange(yMin);
}

void Plot::setYMaxRange(double yMax)
{
    _axisrect->setYMaxRange(yMax);
}

void Plot::setStartTime(double startTime)
{
    _axisrect->setStartTime(startTime);
}

void Plot::setStopTime(double stopTime)
{
    _axisrect->setStopTime(stopTime);
}

void Plot::drawMe(QCPPainter *painter)
{
    QCustomPlot::draw(painter);
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
    Q_UNUSED(e);
    _lastDoubleClickedCurve = static_cast<TrickCurve*>(plottable);
}

void Plot::_slotMouseDoubleClick(QMouseEvent *event)
{
    Q_UNUSED(event);
    _lastDoubleClickedCurve = 0 ;
    _isDoubleClick = true;
}
