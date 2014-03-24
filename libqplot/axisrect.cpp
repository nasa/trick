#include "axisrect.h"
#include "libsnapdata/timeit_linux.h"

AxisRect::AxisRect(QCustomPlot* plotwidget) :
    QCPAxisRect(plotwidget),
    _plotwidget(plotwidget),
    _rubber_band(0),
    _isXRangeCalculated(false),
    _isYRangeCalculated(false),
    _keyPressMoveFactor(.08)
{
    _xAxis = axis(QCPAxis::atBottom);
    _yAxis = axis(QCPAxis::atLeft);
    _colorBandsNormal = _createColorBands(9,false);
    _colorBandsMonte = _createColorBands(10,true);
}

AxisRect::~AxisRect()
{
    foreach ( TrickCurveModel* cm, _curve_models ) {
        delete cm;
    }
}

TrickCurve* AxisRect::addCurve(TrickCurveModel* model)
{
    TrickCurve *curve = new TrickCurve(_xAxis,_yAxis);
    curve->setData(model);
    _curves.append(curve);
    _plotwidget->addPlottable(curve);
    int nCurves = _curves.size();

    // Color curves
    // Make bands of color for plots with lots of curves
    QList<QColor> colorBands = _colorBandsMonte ;
    if ( nCurves < 10 ) {
        colorBands = _colorBandsNormal;
    }
    int nBands = colorBands.size();
    int nCurvesPerBand =  qRound((double)nCurves/(double)nBands);
    nCurvesPerBand = ( nCurvesPerBand == 0 ) ? 1 : nCurvesPerBand;
    int cnt = 0 ;
    int currBand = 0 ;
    foreach ( TrickCurve* curve, _curves ) {
        if ( cnt % nCurvesPerBand ==  0 ) {
            if ( currBand % nBands == 0 ) {
                currBand = 0 ;
            }
            _currPen.setColor(colorBands.at(currBand));
            currBand++;
        }
        curve->setPen(_currPen);
        cnt++;
    }

    // Reset ranges
    bool isValid;
    double xmin = curve->xRange(isValid).lower;
    double xmax = curve->xRange(isValid).upper;
    double ymin = curve->yRange(isValid).lower;
    double ymax = curve->yRange(isValid).upper;
    int sz = _curves.size();
    if ( sz == 1 || xmin < _xDataRange.lower ) _xDataRange.lower = xmin;
    if ( sz == 1 || xmax > _xDataRange.upper ) _xDataRange.upper = xmax;
    if ( sz == 1 || ymin < _yDataRange.lower ) _yDataRange.lower = ymin;
    if ( sz == 1 || ymax > _yDataRange.upper ) _yDataRange.upper = ymax;
    _isXRangeCalculated = true;
    _isYRangeCalculated = true;

    return curve;
}

bool AxisRect::removeCurve(int index)
{
    if ( index < 0 || index >= _curves.size() ) {
        return false;
    }

    bool ret;
    TrickCurve* curve = _curves.at(index);
    ret = _plotwidget->removePlottable(curve);
    _curves.removeAt(index);
    _isXRangeCalculated = false ;
    _isYRangeCalculated = false ;
    return ret;
}

void AxisRect::zoomToFit(const QCPRange& xrange)
{
    QCPRange r0 = _xAxis->range();

    if ( xrange.lower == 0 && xrange.upper == 0 ) {
        _fitXRange();
    } else {
        _xAxis->setRange(xrange);
    }
    _fitYRange();
    setupFullAxesBox();
}

void AxisRect::_fitXRange()
{
    bool isValidRange;
    QCPRange xrange = xDataRange(isValidRange);
    double xmin = xrange.lower;
    double xmax = xrange.upper;

    // At least show +/- 10% of outside range
    double xp = 0.10*qAbs(xmax - xmin) ;
    xmin = xmin-xp;
    for ( int ii = 0; ii < 3; ++ii) {
        if ( xmin - ii*xp < 0 ) {
            xmin = xmin - ii*xp;
            break;
        }
    }
    _xAxis->setRange(xmin,xmax+xp);
}

QCPRange AxisRect::xDataRange(bool& isValidRange)
{
    if ( ! _isXRangeCalculated ) {
        _isXRangeCalculated = true;
        double xmin = 1.0e20;
        double xmax = -1.0e20;
        foreach ( TrickCurve* curve, _curves ) {
            QCPRange range = curve->xRange(isValidRange);
            if ( range.lower < xmin ) xmin = range.lower;
            if ( range.upper > xmax ) xmax = range.upper;
        }
        _xDataRange = QCPRange(xmin,xmax);
    }

    return _xDataRange;
}

void AxisRect::_fitYRange()
{
    bool isValidRange;
    QCPRange yrange = yDataRange(isValidRange);
    double ymin = yrange.lower;
    double ymax = yrange.upper;

    // At least show +/- 10% of outside range
    double yp = 0.10*qAbs(ymax - ymin) ;
    ymin = ymin-yp;
    for ( int ii = 0; ii < 3 ; ++ii) {
        if ( ymin - ii*yp < 0 ) {
            ymin = ymin - ii*yp;
            break;
        }
    }
    _yAxis->setRange(ymin,ymax+yp);
}

QCPRange AxisRect::yDataRange(bool& isValidRange)
{
    if ( ! _isYRangeCalculated ) {
        _isYRangeCalculated = true;
        double ymin = 1.0e20;
        double ymax = -1.0e20;
        foreach ( TrickCurve* curve, _curves ) {
            QCPRange range = curve->yRange(isValidRange);
            if ( range.lower < ymin ) ymin = range.lower;
            if ( range.upper > ymax ) ymax = range.upper;
        }
        _yDataRange = QCPRange(ymin,ymax);
    }

    return _yDataRange;
}

void AxisRect::mousePressEvent(QMouseEvent *event)
{
    if ( event->button() == Qt::MidButton ){
        _origin = event->pos();
        if ( !_rubber_band ) {
            _rubber_band = new QRubberBand(QRubberBand::Rectangle,_plotwidget);
        }
        _rubber_band->setGeometry(QRect(_origin, QSize()));
        _rubber_band->show();
    } else  if ( event->button() == Qt::RightButton ){
        zoomToFit();
    } else {
        // if an axis is selected, only drag in direction of that axis
        // if no axis is selected, both directions may be dragged
        if (_xAxis->selectedParts().testFlag(QCPAxis::spAxis)) {
            setRangeDrag(_xAxis->orientation());
        } else if (_yAxis->selectedParts().testFlag(QCPAxis::spAxis)) {
            setRangeDrag(_yAxis->orientation());
        } else {
            setRangeDrag(Qt::Horizontal|Qt::Vertical);
        }
    }

    QCPAxisRect::mousePressEvent(event);
}

void AxisRect::mouseMoveEvent(QMouseEvent *event)
{
    if ( event->buttons() == Qt::MidButton && _rubber_band ){
        _rubber_band->setGeometry(QRect(_origin, event->pos()).normalized());
    }

    // Mouse range dragging interaction:
    if (mDragging && mParentPlot->interactions().testFlag(QCP::iRangeDrag))
    {
        if (mRangeDrag.testFlag(Qt::Horizontal))
        {
            if (QCPAxis *rangeDragHorzAxis = mRangeDragHorzAxis.data())
            {
                // Keith changed this for squishing axis
                // (instead of just panning) when grabbing extremity of plot.
                if (rangeDragHorzAxis->mScaleType == QCPAxis::stLinear) {
                    double drag_start_xpix = mDragStart.x();
                    double drag_end_xpix = event->pos().x();
                    double drag_start_xcoord = rangeDragHorzAxis->
                            pixelToCoord(drag_start_xpix);
                    double drag_start_xaxis_lower_coord =
                            mDragStartHorzRange.lower;
                    double drag_start_xaxis_upper_coord =
                            mDragStartHorzRange.upper;
                    double drag_end_xcoord = rangeDragHorzAxis->
                            pixelToCoord(drag_end_xpix);
                    double drag_diff_xcoord = drag_start_xcoord-drag_end_xcoord;
                    bool is_drag_motion_left =
                            ( drag_diff_xcoord > 0 ) ? true : false;
                    double lower = drag_start_xaxis_lower_coord+drag_diff_xcoord;
                    double upper = drag_start_xaxis_upper_coord+drag_diff_xcoord;
                    double diffdata = _xDataRange.upper-_xDataRange.lower;
                    double epsilon = diffdata*0.02;
                    if ( is_drag_motion_left ) {
                        if ( upper < _xDataRange.upper+epsilon ) {
                            // Quit dragging axis where data stops
                            if ( drag_start_xpix > left()+width()*0.88 ) {
                                // If drag begins on right extremity,
                                // fix lower range and drag in right
                                rangeDragHorzAxis->setRange(
                                            drag_start_xaxis_lower_coord,upper);
                            } else {
                                rangeDragHorzAxis->setRange(lower,upper);
                            }
                        }
                    } else {
                        if ( _xDataRange.lower-epsilon < lower ) {
                            // Quit dragging axis where data stops
                            if (drag_start_xpix < left()+width()*0.12 ) {
                                // If drag begins on left extremity,
                                // fix right range and drag in left
                                rangeDragHorzAxis->setRange(
                                            lower,drag_start_xaxis_upper_coord);
                            } else {
                                rangeDragHorzAxis->setRange(lower,upper);
                            }
                        }
                    }
                } else if (rangeDragHorzAxis->mScaleType ==
                           QCPAxis::stLogarithmic) {
                    double diff = rangeDragHorzAxis->pixelToCoord(
                            mDragStart.x()) /
                            rangeDragHorzAxis->pixelToCoord(event->pos().x());
                    rangeDragHorzAxis->setRange(
                            mDragStartHorzRange.lower*diff,
                            mDragStartHorzRange.upper*diff);
                }
            }
        }
        if (mRangeDrag.testFlag(Qt::Vertical))
        {
            if (QCPAxis *rangeDragVertAxis = mRangeDragVertAxis.data())
            {
                if (rangeDragVertAxis->mScaleType == QCPAxis::stLinear)
                {
                    double diff =
                            rangeDragVertAxis->pixelToCoord(mDragStart.y())
                            - rangeDragVertAxis->pixelToCoord(event->pos().y());
                    rangeDragVertAxis->setRange(
                            mDragStartVertRange.lower+diff,
                                mDragStartVertRange.upper+diff);
                } else if (rangeDragVertAxis->mScaleType ==
                           QCPAxis::stLogarithmic)
                {
                    double diff = rangeDragVertAxis->pixelToCoord(
                            mDragStart.y()) /
                            rangeDragVertAxis->pixelToCoord(event->pos().y());
                    rangeDragVertAxis->setRange(mDragStartVertRange.lower*diff,
                                                mDragStartVertRange.upper*diff);
                }
            }
        }
        if (mRangeDrag != 0) // if vert or horiz drag enabled,  do a replot
        {
            if (mParentPlot->noAntialiasingOnDrag())
                mParentPlot->setNotAntialiasedElements(QCP::aeAll);
            mParentPlot->replot();
        }
    }
}

void AxisRect::mouseReleaseEvent(QMouseEvent *event)
{

    if ( event->button() == Qt::MidButton && _rubber_band ) {
        QRect geom = _rubber_band->geometry();
        if ( geom.width() > 20 && geom.height() > 20 ) {
            // If rubberband too small, normally by accidental click, don't zoom
            double gxmin = (double)geom.left();
            double gxmax = (double)geom.right();
            double xmin = _xAxis->pixelToCoord(gxmin);
            double xmax = _xAxis->pixelToCoord(gxmax);
            double gymin = (double)geom.top();
            double gymax = (double)geom.bottom();
            double ymin = _yAxis->pixelToCoord(gymax); // screen flip
            double ymax = _yAxis->pixelToCoord(gymin); // screen flip
            _yAxis->setRange(ymin,ymax);
            _xAxis->setRange(xmin,xmax);
        }
        _rubber_band->hide();
    }

    QCPAxisRect::mouseReleaseEvent(event);
}

void AxisRect::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_Plus: _keyPressZoomIn();break;
    case Qt::Key_Minus: _keyPressZoomOut();break;
    case Qt::Key_H: _keyPressMoveLeft();break;
    case Qt::Key_L: _keyPressMoveRight();break;
    case Qt::Key_J: _keyPressMoveDown();break;
    case Qt::Key_K: _keyPressMoveUp();break;
    case Qt::Key_I: _keyPressZoomIn();break;
    case Qt::Key_O: _keyPressZoomOut();break;
    case Qt::Key_A: _keyPressZoomIn();break;
    case Qt::Key_S: _keyPressZoomOut();break;
    case Qt::Key_Space: zoomToFit(); mParentPlot->replot(); break;
    case Qt::Key_Escape: zoomToFit(); mParentPlot->replot(); break;
    default: ; // do nothing
    }
}

void AxisRect::wheelEvent(QWheelEvent *event)
{
    int delta = event->delta();
    QCPRange xrange = _xAxis->range();
    QCPRange yrange = _yAxis->range();
    double sx = xrange.size();
    double sy = yrange.size();
    double dx = sx*_keyPressMoveFactor;
    double dy = sy*_keyPressMoveFactor;
    if ( delta < 0 ) {
        // Zoom out
        dx = -dx;
        dy = -dy;
    }
    _xAxis->setRange(xrange.lower+dx,xrange.upper-dx);
    _yAxis->setRange(yrange.lower+dy,yrange.upper-dy);

    mParentPlot->replot();
}

QList<QColor> AxisRect::_createColorBands(int nBands, bool isMonte)
{
    QList<QColor> colorBands;

    QColor blue(48,85,200);
    QColor green(0,240,0);
    QColor magenta(222,77,203);
    QColor aqua(119,214,222);
    QColor orange(183,120,71);
    QColor burntorange(177,79,0);
    QColor yellow(222,222,10);
    QColor pink(255,192,255);
    QColor gray(145,170,192);
    QColor medblue(49,140,250);
    QColor black(0,0,0);

    if ( isMonte && nBands >= 10 ) {

        // This is for banding a monte carlo which has many curves

        int hBeg = 10; int hEnd = 230;
        int dh = qRound((double)(hEnd-hBeg)/(nBands-1.0));
        int s = qRound(0.75*255);
        int v = qRound(0.87*255);
        for ( int h = hBeg; h <= hEnd; h+=dh) {
            colorBands << QColor::fromHsv(h,s,v);
            //colorBands << blue;
        }
        colorBands.removeFirst();
        colorBands.prepend(burntorange);

    } else {

        // This is for a smaller number of curves
        colorBands << blue << green << magenta
                   << aqua << orange << yellow
                   << gray << pink << medblue;

        for ( int i = 0 ; i < nBands-10; ++i ) {
            colorBands.removeLast();
        }
    }

    return colorBands;
}

void AxisRect::_keyPressMoveLeft()
{
    QCPRange _xrange = _xAxis->range();
    double sz = _xrange.size();
    double dx = sz*_keyPressMoveFactor;

    _xAxis->setRange(_xrange.lower-dx,_xrange.upper-dx);
    mParentPlot->replot();
}

void AxisRect::_keyPressMoveRight()
{
    QCPRange _xrange = _xAxis->range();
    double sz = _xrange.size();
    double dx = sz*_keyPressMoveFactor;
    _xAxis->setRange(_xrange.lower+dx,_xrange.upper+dx);
    mParentPlot->replot();
}

void AxisRect::_keyPressMoveUp()
{
    QCPRange _yrange = _yAxis->range();
    double sz = _yrange.size();
    double dy = sz*_keyPressMoveFactor;
    _yAxis->setRange(_yrange.lower+dy,_yrange.upper+dy);
    mParentPlot->replot();
}

void AxisRect::_keyPressMoveDown()
{
    QCPRange _yrange = _yAxis->range();
    double sz = _yrange.size();
    double dy = sz*_keyPressMoveFactor;
    _yAxis->setRange(_yrange.lower-dy,_yrange.upper-dy);
    mParentPlot->replot();
}

void AxisRect::_keyPressZoomIn()
{
    QCPRange _xrange = _xAxis->range();
    QCPRange _yrange = _yAxis->range();
    double sx = _xrange.size();
    double sy = _yrange.size();
    double dx = sx*_keyPressMoveFactor;
    double dy = sy*_keyPressMoveFactor;
    _xAxis->setRange(_xrange.lower+dx,_xrange.upper-dx);
    _yAxis->setRange(_yrange.lower+dy,_yrange.upper-dy);
    mParentPlot->replot();
}

void AxisRect::_keyPressZoomOut()
{
    QCPRange _xrange = _xAxis->range();
    QCPRange _yrange = _yAxis->range();
    double sx = _xrange.size();
    double sy = _yrange.size();
    double dx = sx*_keyPressMoveFactor;
    double dy = sy*_keyPressMoveFactor;
    _xAxis->setRange(_xrange.lower-dx,_xrange.upper+dx);
    _yAxis->setRange(_yrange.lower-dy,_yrange.upper+dy);
    mParentPlot->replot();
}
