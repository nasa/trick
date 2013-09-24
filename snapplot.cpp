#ifdef SNAPGUI

#include "snapplot.h"

SnapPlot::SnapPlot(QWidget* parent) :
    QCustomPlot(parent),
    _rubber_band(0)
{
    QSizePolicy sizePolicy(QSizePolicy::MinimumExpanding,
                           QSizePolicy::MinimumExpanding);
    setSizePolicy(sizePolicy);

    setNoAntialiasingOnDrag(true);

    // Title (blank for now)
    plotLayout()->insertRow(0);
    plotLayout()->addElement(0, 0, new QCPPlotTitle(this, ""));

    QFont legendFont = font();
    legendFont.setPointSize(10);
    legend->setFont(legendFont);
    legend->setSelectedFont(legendFont);
    legend->setSelectableParts(QCPLegend::spItems);
    legend->setVisible(true);

    setInteractions(QCP::iRangeDrag | QCP::iSelectAxes |
                    QCP::iSelectLegend | QCP::iSelectPlottables);
}

TrickCurve* SnapPlot::addCurve(TrickModel* model, int tcol,
                              int xcol, int ycol,
                              double valueScaleFactor )
{
    TrickCurve *curve = new TrickCurve(xAxis, yAxis);
    curve->setData(model, tcol, xcol, ycol);
    curve->setValueScaleFactor(valueScaleFactor);
    _curves.append(curve);
    addPlottable(curve);
    return curve;
}

bool SnapPlot::removeCurve(int index)
{
    if ( index < 0 || index >= _curves.size() ) {
        return false;
    }

    bool ret;
    TrickCurve* curve = _curves.at(index);
    ret = removePlottable(curve);
    _curves.removeAt(index);
    return ret;
}

void SnapPlot::zoomToFit(const QCPRange& xrange)
{
    QCPRange r0 = xAxis->range();

    if ( xrange.lower == 0 && xrange.upper == 0 ) {
        _fitXRange();
    } else {
        xAxis->setRange(xrange);
    }
    _fitYRange();
    axisRect()->setupFullAxesBox();

    if ( qAbs(xrange.lower-r0.lower) > 0.000001 ||
         qAbs(xrange.upper-r0.upper) > 0.000001 ) {
        replot();
    }
}

void SnapPlot::_fitXRange()
{
    double xmin = 1.0e20;
    double xmax = -1.0e20;
    bool isValidRange;
    foreach ( TrickCurve* curve, _curves ) {
        QCPRange xrange = curve->xRange(isValidRange);
        if ( xrange.lower < xmin ) xmin = xrange.lower;
        if ( xrange.upper > xmax ) xmax = xrange.upper;
    }

    // At least show +/- 10% of outside range
    double xp = 0.10*qAbs(xmax - xmin) ;
    xmin = xmin-xp;
    for ( int ii = 0; ii < 3; ++ii) {
        if ( xmin - ii*xp < 0 ) {
            xmin = xmin - ii*xp;
            break;
        }
    }
    xAxis->setRange(xmin,xmax+xp);
}

void SnapPlot::_fitYRange()
{
    double ymin = 1.0e20;
    double ymax = -1.0e20;
    bool isValidRange;
    foreach ( TrickCurve* curve, _curves ) {
        QCPRange yrange = curve->yRange(isValidRange);
        if ( yrange.lower < ymin ) ymin = yrange.lower;
        if ( yrange.upper > ymax ) ymax = yrange.upper;
    }

    // At least show +/- 10% of outside range
    double yp = 0.10*qAbs(ymax - ymin) ;
    ymin = ymin-yp;
    for ( int ii = 0; ii < 3 ; ++ii) {
        if ( ymin - ii*yp < 0 ) {
            ymin = ymin - ii*yp;
            break;
        }
    }
    yAxis->setRange(ymin,ymax+yp);
}


void SnapPlot::mousePressEvent(QMouseEvent *event)
{
    if ( event->button() == Qt::MidButton ){
        _origin = event->pos();
        if ( !_rubber_band ) {
            _rubber_band = new QRubberBand(QRubberBand::Rectangle,this);
        }
        _rubber_band->setGeometry(QRect(_origin, QSize()));
        _rubber_band->show();
    } else  if ( event->button() == Qt::RightButton ){
        zoomToFit();
    } else {
        // if an axis is selected, only drag in direction of that axis
        // if no axis is selected, both directions may be dragged
        if (xAxis->selectedParts().testFlag(QCPAxis::spAxis)) {
            axisRect()->setRangeDrag(xAxis->orientation());
        } else if (yAxis->selectedParts().testFlag(QCPAxis::spAxis)) {
            axisRect()->setRangeDrag(yAxis->orientation());
        } else {
            axisRect()->setRangeDrag(Qt::Horizontal|Qt::Vertical);
        }
    }

    QCustomPlot::mousePressEvent(event);
}

void SnapPlot::mouseMoveEvent(QMouseEvent *event)
{
    if ( event->button() == Qt::NoButton && _rubber_band ){
        _rubber_band->setGeometry(QRect(_origin, event->pos()).normalized());
    }
    QCustomPlot::mouseMoveEvent(event);
}

void SnapPlot::mouseReleaseEvent(QMouseEvent *event)
{
    if ( event->button() == Qt::MidButton && _rubber_band ) {
        QRect geom = _rubber_band->geometry();
        double gxmin = (double)geom.left();
        double gxmax = (double)geom.right();
        double xmin = xAxis->pixelToCoord(gxmin);
        double xmax = xAxis->pixelToCoord(gxmax);
        double gymin = (double)geom.top();
        double gymax = (double)geom.bottom();
        double ymin = yAxis->pixelToCoord(gymax); // screen flip
        double ymax = yAxis->pixelToCoord(gymin); // screen flip
        yAxis->setRange(ymin,ymax);
        xAxis->setRange(xmin,xmax);
        _rubber_band->hide();
    }

    QCustomPlot::mouseReleaseEvent(event);
}


#endif // SNAPGUI
