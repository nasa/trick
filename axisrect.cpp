#ifdef SNAPGUI

#include "axisrect.h"
#include "timeit_linux.h"

AxisRect::AxisRect(DPPlot* dpplot, QCustomPlot *plotwidget) :
    QCPAxisRect(plotwidget),
    _plotwidget(plotwidget),
    _dpplot(dpplot),
    _rubber_band(0),
    _isXRangeCalculated(false),
    _isYRangeCalculated(false),
    _keyPressMoveFactor(.08)
{
    _xAxis = axis(QCPAxis::atBottom);
    _yAxis = axis(QCPAxis::atLeft);

    // If a single curve, make axis labels from curve name
    if ( dpplot->curves().size() == 1 ) {
        QString xlabel = _abbreviate(dpplot->curves().at(0)->x()->label());
        _xAxis->setLabel(xlabel);
        QString ylabel = _abbreviate(dpplot->curves().at(0)->y()->label());
        _yAxis->setLabel(ylabel);
    }
}

AxisRect::~AxisRect()
{
}

QString AxisRect::_abbreviate(const QString &label, int maxlen)
{
    if ( label == "sys.exec.out.time" ) {
        return "Time";
    }

    QString abbr = label.right(maxlen);
    int idx = label.size()-1;
    while ( idx > 0 ) {
        idx = label.lastIndexOf('.',idx);
        QString str = label.mid(idx+1) ;
        idx--;
        if ( str.size() > maxlen ) {
            break;
        }
        abbr = str;
    }

    return abbr;
}


void AxisRect::setData(Monte *monte)
{
    foreach (DPCurve* curve, _dpplot->curves() ) {
        QString yparam = curve->y()->name();
        QList<TrickModel*>* models = monte->models(yparam);
        foreach (TrickModel* model, *models ) {
            addCurve(model,yparam);
        }
    }
}


TrickCurve* AxisRect::addCurve(TrickModel* model, const QString& yparam,
                              double valueScaleFactor )
{
    TrickCurve* curve = 0 ;

    int ncols = model->columnCount();
    for ( int col = 0; col < ncols; ++col) {
        if (yparam ==
            model->headerData(col,Qt::Horizontal,Param::Name).toString())  {
            curve = addCurve(model,0,0,col,valueScaleFactor);
            break;
        }
    }

    return curve;
}

TrickCurve* AxisRect::addCurve(TrickModel* model, int tcol,
                              int xcol, int ycol,
                              double valueScaleFactor )
{
    TrickCurve *curve = new TrickCurve(_xAxis,_yAxis);
    curve->setData(model, tcol, xcol, ycol);
    curve->setValueScaleFactor(valueScaleFactor);
    _curves.append(curve);
    _plotwidget->addPlottable(curve);

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
                    if ( is_drag_motion_left ) {
                        if ( upper < diffdata*1.02 ) {
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
                        double dlower = -diffdata*0.02;
                        if ( dlower < lower ) {
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
    case Qt::Key_Left: _keyPressLeft(); break;
    case Qt::Key_Right: _keyPressRight();break;
    case Qt::Key_Up:_keyPressUp(); break;
    case Qt::Key_Down: _keyPressDown();break;
    case Qt::Key_Plus: _keyPressZoomIn();break;
    case Qt::Key_Minus: _keyPressZoomOut();break;
    case Qt::Key_H: _keyPressLeft();break;
    case Qt::Key_L: _keyPressRight();break;
    case Qt::Key_J: _keyPressDown();break;
    case Qt::Key_K: _keyPressUp();break;
    case Qt::Key_I: _keyPressZoomIn();break;
    case Qt::Key_O: _keyPressZoomOut();break;
    case Qt::Key_A: _keyPressZoomIn();break;
    case Qt::Key_S: _keyPressZoomOut();break;
    case Qt::Key_Space: zoomToFit(); mParentPlot->replot(); break;
    case Qt::Key_Escape: zoomToFit(); mParentPlot->replot(); break;
    default: ; // do nothing
    }
}

void AxisRect::_keyPressLeft()
{
    QCPRange _xrange = _xAxis->range();
    double sz = _xrange.size();
    double dx = sz*_keyPressMoveFactor;

    _xAxis->setRange(_xrange.lower-dx,_xrange.upper-dx);
    mParentPlot->replot();
}

void AxisRect::_keyPressRight()
{
    QCPRange _xrange = _xAxis->range();
    double sz = _xrange.size();
    double dx = sz*_keyPressMoveFactor;
    _xAxis->setRange(_xrange.lower+dx,_xrange.upper+dx);
    mParentPlot->replot();
}

void AxisRect::_keyPressUp()
{
    QCPRange _yrange = _yAxis->range();
    double sz = _yrange.size();
    double dy = sz*_keyPressMoveFactor;
    _yAxis->setRange(_yrange.lower+dy,_yrange.upper+dy);
    mParentPlot->replot();
}

void AxisRect::_keyPressDown()
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



#endif // SNAPGUI
