#include "bookview_curves.h"

CoordArrow::CoordArrow() :
    coord(QPointF(DBL_MAX,DBL_MAX)),
    r(2.0),
    h(16.0),
    a(48.0),
    b(18.0),
    m(4.0),
    angle(M_PI/4),
    tipAngle(M_PI/8)
{
}

CoordArrow::CoordArrow(const QPointF &coord,
                       double r, double h,
                       double a, double b, double m,
                       double angle, double tipAngle) :
    coord(coord),        // math coord of pt to draw arrow to
    r(r),                // radius of circle around point
    h(h),                // isoscelese triangle arrow head height
    a(a),                // arrow tail length part between arrow head 'b'
    b(b),                // arrow tail length part between 'a' and txt
    m(m),                // distance (margin) between tail and text
    angle(angle),
    tipAngle(tipAngle)
{
}

// T is coordToPix transform
QRectF CoordArrow::boundingBox(const QPainter& painter,
                               const QTransform& T) const
{
    QRectF bbox;

    // Map math coord to window pt
    QPointF pt = T.map(coord);

    // Text bbox width and height
    double tw = painter.fontMetrics().boundingRect(txt).width();
    double th = painter.fontMetrics().boundingRect(txt).height();

    double aw = (h+a)*qAbs(cos(angle)) + b + m + tw; //total arrow width (w/txt)
    double ah = (h+a)*qAbs(sin(angle)) + th/2.0;     //total arrow height
    QPointF atl; // top left point of arrow bbox
    if ( angle > 0.0 && angle < M_PI/2.0 ) {
        // Quadrant I
        atl.setX(pt.x()+r*cos(angle));
        atl.setY(pt.y()-(r+h+a)*sin(angle)-th/2.0); // neg since +y down
    } else if ( angle > M_PI/2.0 && angle < M_PI ) {
        // Quadrant II
        atl.setX(pt.x()+(r+h+a)*cos(angle)-m-b-tw);
        atl.setY(pt.y()-(r+h+a)*sin(angle)-th/2.0);
    } else if ( angle > M_PI && angle < 3*(M_PI/2.0) ) {
        // Quadrant III
        atl.setX(pt.x()+(r+h+a)*cos(angle)-m-b-tw);
        atl.setY(pt.y()-r*sin(angle));
    } else if ( angle > 3*(M_PI/2.0) && angle < 2*M_PI ) {
        // Quadrant IV
        atl.setX(pt.x()+r*cos(angle));
        atl.setY(pt.y()-r*sin(angle));
    } else {
        fprintf(stderr,"koviz [bad scoobs]: CoorArrow::boundingBox(): "
                       "arrow angle <=0,==90,==270 or >=360. "
                       "May want to support it.  Bailing!!!\n");
        exit(-1);
    }

    bbox.setTopLeft(atl);
    bbox.setSize(QSize(aw,ah));

    return bbox;
}

// T is coordToPix transform
QRectF CoordArrow::txtBoundingBox(const QPainter& painter,
                                  const QTransform& T) const
{
    QRectF bbox;

    // Map math coord to window pt
    QPointF pt = T.map(coord);

    // Text bbox width and height
    double tw = painter.fontMetrics().boundingRect(txt).width();
    double th = painter.fontMetrics().boundingRect(txt).height();

    QPointF tl;
    if ( angle > 0.0 && angle < M_PI/2.0 ) {
        // Quadrant I
        tl.setX(pt.x() + (r+h+a)*cos(angle) + b + m);
        tl.setY(pt.y() - (r+h+a)*sin(angle) - th/2.0);
    } else if ( angle > M_PI/2.0 && angle < M_PI ) {
        // Quadrant II
        tl.setX(pt.x() + (r+h+a)*cos(angle) - b - m - tw);
        tl.setY(pt.y() - (r+h+a)*sin(angle) - th/2.0);
    } else if ( angle > M_PI && angle < 3*(M_PI/2.0) ) {
        // Quadrant III
        tl.setX(pt.x() + (r+h+a)*cos(angle) - b - m - tw);
        tl.setY(pt.y() - (r+h+a)*sin(angle) - th/2.0);
    } else if ( angle > 3*(M_PI/2.0) && angle < 2*M_PI ) {
        // Quadrant IV
        tl.setX(pt.x() + (r+h+a)*cos(angle) + b + m);
        tl.setY(pt.y() - (r+h+a)*sin(angle) - th/2.0);
    } else {
        fprintf(stderr,"koviz [bad scoobs]: CoorArrow::txtBBox(): "
                       "arrow angle <=0,==90,==270 or >=360. "
                       "May want to support it.  Bailing!!!\n");
        exit(-1);
    }

    bbox.setTopLeft(tl);
    bbox.setSize(QSize(tw,th));

    return bbox;
}

void CoordArrow::paintMe(QPainter &painter, const QTransform &T,
                         const QColor& fg, const QColor& bg) const
{
    // Save painter
    painter.save();
    QBrush origBrush = painter.brush();
    QPen origPen = painter.pen();

    // Map math coord to window pt
    QPointF pt = T.map(coord);

    double tw = painter.fontMetrics().boundingRect(txt).width();
    double th = painter.fontMetrics().boundingRect(txt).height();

    QRectF txtBox;
    QVector<QPointF> ptsArrowHead;
    QVector<QPointF> ptsArrowTail;
    if ( angle > 0.0 && angle < M_PI/2.0 ) {
        // Quadrant I
        QPointF tip(pt.x()+r*cos(angle),
                    pt.y()-r*sin(angle));  // note: minus since +y is down
        QPointF p(tip.x()+h*cos(angle+tipAngle/2.0),
                  tip.y()-h*sin(angle+tipAngle/2.0));
        QPointF q(tip.x()+h*cos(angle-tipAngle/2.0),
                  tip.y()-h*sin(angle-tipAngle/2.0));
        ptsArrowHead << tip << q << p;

        QPointF a0(tip.x()+h*cos(angle),
                   tip.y()-h*sin(angle));
        QPointF a1(a0.x()+a*cos(angle),
                   a0.y()-a*sin(angle));
        QPointF a2(a1.x()+b,a1.y());
        ptsArrowTail << a0 << a1 << a2;

        QPointF tl(a2.x()+m,
                   a2.y()-th/2.0);
        txtBox.setTopLeft(tl);
        txtBox.setSize(QSize(tw,th));

    } else if ( angle > M_PI/2.0 && angle < M_PI ) {
        // Quadrant II
        QPointF tip(pt.x()+r*cos(angle),
                    pt.y()-r*sin(angle));  // note: minus since +y is down
        QPointF p(tip.x()+h*cos(angle+tipAngle/2.0),
                  tip.y()-h*sin(angle+tipAngle/2.0));
        QPointF q(tip.x()+h*cos(angle-tipAngle/2.0),
                  tip.y()-h*sin(angle-tipAngle/2.0));
        ptsArrowHead << tip << q << p;

        QPointF a0(tip.x()+h*cos(angle),
                   tip.y()-h*sin(angle));
        QPointF a1(a0.x()+a*cos(angle),
                   a0.y()-a*sin(angle));
        QPointF a2(a1.x()-b,a1.y());
        ptsArrowTail << a0 << a1 << a2;

        QPointF tl(a2.x()-m-tw,
                   a2.y()-th/2.0);
        txtBox.setTopLeft(tl);
        txtBox.setSize(QSize(tw,th));

    } else if ( angle > M_PI && angle < 3*(M_PI/2.0) ) {
        // Quadrant III
        QPointF tip(pt.x()+r*cos(angle),
                    pt.y()-r*sin(angle));  // note: minus since +y is down
        QPointF p(tip.x()+h*cos(angle+tipAngle/2.0),
                  tip.y()-h*sin(angle+tipAngle/2.0));
        QPointF q(tip.x()+h*cos(angle-tipAngle/2.0),
                  tip.y()-h*sin(angle-tipAngle/2.0));
        ptsArrowHead << tip << q << p;

        QPointF a0(tip.x()+h*cos(angle),
                   tip.y()-h*sin(angle));
        QPointF a1(a0.x()+a*cos(angle),
                   a0.y()-a*sin(angle));
        QPointF a2(a1.x()-b,a1.y());
        ptsArrowTail << a0 << a1 << a2;

        QPointF tl(a2.x()-m-tw, a2.y()-th/2.0);
        txtBox.setTopLeft(tl);
        txtBox.setSize(QSize(tw,th));
    } else if ( angle > 3*(M_PI/2.0) && angle < 2*M_PI ) {
        // Quadrant IV
        QPointF tip(pt.x()+r*cos(angle),
                    pt.y()-r*sin(angle));  // note: minus since +y is down
        QPointF p(tip.x()+h*cos(angle+tipAngle/2.0),
                  tip.y()-h*sin(angle+tipAngle/2.0));
        QPointF q(tip.x()+h*cos(angle-tipAngle/2.0),
                  tip.y()-h*sin(angle-tipAngle/2.0));
        ptsArrowHead << tip << q << p;

        QPointF a0(tip.x()+h*cos(angle),
                   tip.y()-h*sin(angle));
        QPointF a1(a0.x()+a*cos(angle),
                   a0.y()-a*sin(angle));
        QPointF a2(a1.x()+b,a1.y());
        ptsArrowTail << a0 << a1 << a2;

        QPointF tl(a2.x()+m,a2.y()-th/2.0);
        txtBox.setTopLeft(tl);
        txtBox.setSize(QSize(tw,th));
    } else {
        fprintf(stderr,"koviz [bad scoobs]: CoorArrow::paintMe(): "
                       "arrow angle <=0,==90,==270 or >=360. "
                       "May want to support it.  Bailing!!!\n");
        exit(-1);
    }

    // Draw circle around point
    painter.setPen(fg);
    painter.setBrush(bg);
    painter.drawEllipse(pt,qRound(r),qRound(r));

    // Draw arrow head (tip on circle, not on point)
    QPolygonF arrowHead(ptsArrowHead);
    painter.setPen(fg);
    painter.setBrush(fg);
    painter.drawConvexPolygon(arrowHead);

    // Draw arrow tail (attached to triangle)
    QPolygonF polyLine(ptsArrowTail);
    painter.setPen(fg);
    painter.setBrush(fg);
    painter.drawPolyline(polyLine);

    // Draw background for text box
    painter.setPen(bg);
    painter.setBrush(bg);
    painter.drawRect(txtBox);

    // Draw coord text i.e. (x,y)
    painter.setPen(fg);
    painter.setBrush(bg);
    painter.drawText(txtBox,Qt::TextDontClip|Qt::AlignCenter,txt);

    // Restore painter
    painter.setPen(origPen);
    painter.setBrush(origBrush);
    painter.restore();
}

void CoordArrow::paintMeCenter(QPainter &painter,
                               const QTransform &T, const QRect &viewportRect,
                               const QColor &fg, const QColor &bg) const
{
    if (painter.fontMetrics().boundingRect(txt).width() > viewportRect.width()){
        return;  // don't paint if coord txt will not fit in window
    }

    // Map math coord to window pt
    QPointF pt = T.map(coord);

    if ( !viewportRect.contains(pt.toPoint()) ) {
        return; // don't paint if coord outside of window
    }

    // Save/set painter
    painter.save();
    QBrush origBrush = painter.brush();
    QPen origPen = painter.pen();

    // Init txtbox
    QRectF txtbox = painter.fontMetrics().boundingRect(txt);
    double tw = txtbox.width();
    double th = txtbox.height();

    // Set txtbox top left corner and arrow tail points
    QVector<QPointF> ptsArrowTail;
    QPointF a0;
    QPointF a1;
    QPointF a2;
    if ( pt.y() < viewportRect.height()/2 ) {
        // pt is in top half of viewport, so draw coord at bot of plot
        QPointF tl(viewportRect.width()/2.0-tw/2.0,
                   viewportRect.height()-th);
        txtbox.moveTopLeft(tl);

        a0.setX(txtbox.center().x());
        a0.setY(txtbox.top()-m);
        a1.setX(a0.x());
        a1.setY(a0.y()-b);
        a2.setX(pt.x());
        a2.setY(pt.y());

    } else {
        // pt is in bot half of viewport, so draw coord at top of plot
        QPointF tl(viewportRect.width()/2.0-tw/2.0,0);
        txtbox.moveTopLeft(tl);

        a0.setX(txtbox.center().x());
        a0.setY(txtbox.bottom()+m);
        a1.setX(a0.x());
        a1.setY(a0.y()+b);
        a2.setX(pt.x());
        a2.setY(pt.y());
    }
    ptsArrowTail << a0 << a1 << a2;

    // Draw background for text box
    painter.setPen(bg);
    painter.setBrush(bg);
    painter.drawRect(txtbox);

    // Draw coord text i.e. (x,y)
    painter.setPen(fg);
    painter.setBrush(bg);
    painter.drawText(txtbox,Qt::TextDontClip|Qt::AlignCenter,txt);

    // Draw arrow tail
    painter.setPen(fg);
    painter.setBrush(bg);
    painter.drawPolyline(ptsArrowTail);

    // Calculate arrow angle
    double dx = a2.x()-a1.x();
    double dy = -1*(a2.y()-a1.y());         // -1 since y+ is negative
    double arrowAngle = atan2(dy,dx)+M_PI;  // +pi to flip arrow head

    // Draw arrow head
    QVector<QPointF> ptsArrowHead;
    QPointF tip(pt.x()+r*cos(arrowAngle),
                pt.y()-r*sin(arrowAngle));  // note: minus since +y is down
    QPointF p(tip.x()+h*cos(arrowAngle+tipAngle/2.0),
              tip.y()-h*sin(arrowAngle+tipAngle/2.0));
    QPointF q(tip.x()+h*cos(arrowAngle-tipAngle/2.0),
              tip.y()-h*sin(arrowAngle-tipAngle/2.0));
    ptsArrowHead << tip << q << p;
    QPolygonF arrowHead(ptsArrowHead);
    painter.setPen(fg);
    painter.setBrush(fg);
    painter.drawConvexPolygon(arrowHead);

    // Draw circle around point
    painter.setPen(fg);
    painter.setBrush(bg);
    painter.drawEllipse(pt,qRound(r),qRound(r));

    // Restore painter
    painter.setPen(origPen);
    painter.setBrush(origBrush);
    painter.restore();
}

CurvesView::CurvesView(QWidget *parent) :
    BookIdxView(parent),_pixmap(0)
{
    setFocusPolicy(Qt::StrongFocus);
    setFrameShape(QFrame::NoFrame);
    _colorBandsNormal = _createColorBands(9,false);
    _colorBandsRainbow = _createColorBands(10,true);

    // Set mouse tracking to receive mouse move events when button not pressed
    setMouseTracking(true);
}

CurvesView::~CurvesView()
{
    if ( _pixmap ) {
        delete _pixmap;
    }
    foreach ( TimeAndIndex* marker, _markers ) {
        delete marker;
    }
}

void CurvesView::setCurrentCurveRunID(int runID)
{
    if ( runID < 0 ) {
        // Set current curves view index to nothing
        setCurrentIndex(QModelIndex());
        return;
    }

    QModelIndex plotIdx = rootIndex();
    QModelIndex curvesIdx = _bookModel()->getIndex(plotIdx,"Curves","Plot");
    int rc = model()->rowCount(curvesIdx);
    int nMatches = 0;
    QModelIndex curveIdx;
    for (int i = 0 ; i < rc; ++i ) {
        QModelIndex idx = model()->index(i,0,curvesIdx);
        if ( model()->data(idx).toString() == "Curve" ) {
            int curveRunID = _bookModel()->getDataInt(idx,"CurveRunID","Curve");
            if ( curveRunID == runID ) {
                ++nMatches;
                curveIdx = idx;
            }
        }
    }
    if ( nMatches == 1 ) {
        // If runID matches a single curve, make curve current
        setCurrentIndex(curveIdx);
    }
}

void CurvesView::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    if ( !model() ) return;

    QModelIndex curvesIdx = _bookModel()->getIndex(rootIndex(),"Curves","Plot");
    int nCurves = model()->rowCount(curvesIdx);

    QPainter painter(viewport());
    painter.save();

    painter.setRenderHint(QPainter::Antialiasing);

    QTransform T = _coordToPixelTransform();
    double ptSizeCurve = 0.0;
    QPen pen;
    pen.setWidthF(ptSizeCurve);
    painter.setPen(pen);

    // Draw curves
    if ( nCurves == 2 ) {
        QString plotPresentation = _bookModel()->getDataString(rootIndex(),
                                                     "PlotPresentation","Plot");
        if ( plotPresentation.isEmpty() ) {
            plotPresentation = _bookModel()->getDataString(QModelIndex(),
                                                           "Presentation");
        }

        // Plot background
        QModelIndex pageIdx = rootIndex().parent().parent();
        QColor bg = _bookModel()->pageBackgroundColor(pageIdx);
        painter.fillRect(viewport()->rect(),bg);

        _paintGrid(painter,rootIndex());

        if ( plotPresentation == "compare" ) {
            _paintCoplot(T,painter,pen);
        } else if ( plotPresentation == "error" ) {
            _paintErrorplot(T,painter,pen,rootIndex());
        } else if ( plotPresentation == "error+compare" ) {
            _paintCoplot(T,painter,pen);
            _paintErrorplot(T,painter,pen,rootIndex()); // overlay err on coplot
        } else {
            fprintf(stderr,"koviz [bad scoobs]: paintEvent() : "
                           "PlotPresentation=\"%s\" not recognized.\n",
                           plotPresentation.toLatin1().constData());
            exit(-1);
        }
    } else {
        _paintCoplot(T,painter,pen);
    }

    // Restore the painter state off the painter stack
    painter.restore();

    // Draw legend (if needed)
    _paintCurvesLegend(viewport()->rect(),curvesIdx,painter);

    // Draw markers
    _paintMarkers(painter);
}


void CurvesView::_paintCoplot(const QTransform &T,QPainter &painter,QPen &pen)
{
    Q_UNUSED(pen);

    if ( !_pixmap ) return;

    painter.save();

    // Draw pixmap of all curves
    QTransform I;
    painter.setTransform(I);
    painter.drawPixmap(viewport()->rect(),*_pixmap);

    // If curve is selected
    QModelIndex gpidx = currentIndex().parent().parent();
    QString tag = model()->data(currentIndex()).toString();
    if ( currentIndex().isValid() && tag == "Curve" && gpidx == rootIndex()) {

        // Lighten unselected curves to semi-transparent bg
        QModelIndex pageIdx = rootIndex().parent().parent();
        QColor bg = _bookModel()->pageBackgroundColor(pageIdx);
        bg.setAlpha(190);
        painter.fillRect(viewport()->rect(),bg);

        // Since grid is too light with semi-transparent bg, paint it
        _paintGrid(painter,rootIndex());

        // Paint curve (possibly with linestyle, symbols etc.)
        _paintCurve(currentIndex(),T,painter,true);
    }

    painter.restore();
}

void CurvesView::_paintCurve(const QModelIndex& curveIdx,
                             const QTransform& T,
                             QPainter& painter, bool isHighlight)
{
    painter.save();
    QPen origPen = painter.pen();

    CurveModel* curveModel = _bookModel()->getCurveModel(curveIdx);

    if ( curveModel ) {

        // Line color
        QPen pen;
        pen.setWidth(0);
        QColor color(_bookModel()->getDataString(curveIdx,
                                                 "CurveColor","Curve"));
        if ( isHighlight ) {
            QModelIndex pageIdx = curveIdx.parent().parent().parent().parent();
            QColor bg = _bookModel()->pageBackgroundColor(pageIdx);
            if ( bg.lightness() < 128 ) {
                color = color.lighter(120);
            } else {
                color = color.darker(200);
            }
        }
        pen.setColor(color);

        // Line style
        QVector<qreal> pattern = _bookModel()->getLineStylePattern(curveIdx);
        pen.setDashPattern(pattern);

        // Set pen
        painter.setPen(pen);

        // Get painter path
        QPainterPath* path = _bookModel()->getCurvePainterPath(curveIdx);

        // Scale transform (e.g. for unit axis scaling)
        double xs = _bookModel()->xScale(curveIdx);
        double ys = _bookModel()->yScale(curveIdx);
        double xb = _bookModel()->xBias(curveIdx);
        double yb = _bookModel()->yBias(curveIdx);
        QTransform Tscaled(T);
        Tscaled = Tscaled.scale(xs,ys);
        Tscaled = Tscaled.translate(xb/xs,yb/ys);
        painter.setTransform(Tscaled);

        // Draw "Flatline=#" label if curve is flat (constant)
        QRectF cbox = path->boundingRect();
        if ( cbox.height() == 0.0 ) {
            QString yval;
            double ys = _bookModel()->yScale(curveIdx);
            double yb = _bookModel()->yBias(curveIdx);
            yval = QString("Flatline=%1").arg(_format(cbox.y()*ys+yb));
            QRectF tbox = Tscaled.mapRect(cbox);
            QTransform I;
            painter.setTransform(I);
            double top = tbox.y()-fontMetrics().ascent();
            if ( top >= 0 ) {
                // Draw flatline label over curve
                painter.drawText(tbox.topLeft()-QPointF(0,5),yval);
            } else {
                // Draw flatline label under curve since it would drawn off page
                painter.drawText(tbox.topLeft()+
                                 QPointF(0,fontMetrics().ascent())
                                 +QPointF(0,5),yval);
            }
            painter.setTransform(Tscaled);
        }

        // Draw curve!
        painter.drawPath(*path);

        // Draw symbols on curve (if there are any)
        QString symbolStyle = _bookModel()->getDataString(curveIdx,
                                               "CurveSymbolStyle", "Curve");
        symbolStyle = symbolStyle.toLower();
        if ( !symbolStyle.isEmpty() && symbolStyle != "none" ) {
            pattern.clear();
            pen.setDashPattern(pattern); // plain lines for drawing symbols
            QTransform I;
            painter.setTransform(I);
            double w = pen.widthF();
            pen.setWidthF(0.0);
            painter.setPen(pen);
            QPointF pLast;
            for ( int i = 0; i < path->elementCount(); ++i ) {
                QPainterPath::Element el = path->elementAt(i);
                QPointF p(el.x,el.y);
                p = Tscaled.map(p);
                if ( i > 0 ) {
                    double r = 32.0;
                    double x = pLast.x()-r/2.0;
                    double y = pLast.y()-r/2.0;
                    QRectF R(x,y,r,r);
                    if ( R.contains(p) ) {
                        continue;
                    }
                }

                __paintSymbol(p,symbolStyle,painter);

                pLast = p;
            }
            pen.setWidthF(w);
            painter.setPen(pen);
            painter.setTransform(Tscaled);
        }
    }
    painter.setPen(origPen);
    painter.restore();
}

void CurvesView::_paintMarkers(QPainter &painter)
{
    // Return and do not draw if Options->isShowLiveCoord is off
    QModelIndex isShowIdx = _bookModel()->getDataIndex(QModelIndex(),
                                                       "IsShowLiveCoord");
    bool isShowLiveCoord = model()->data(isShowIdx).toBool();
    if ( !isShowLiveCoord ) {
        return;
    }

    // Draw in window coords
    painter.save();
    QTransform I;
    painter.setTransform(I);

    QModelIndex pageIdx = rootIndex().parent().parent();
    QColor bg = _bookModel()->pageBackgroundColor(pageIdx);
    QColor fg = _bookModel()->pageForegroundColor(pageIdx);

    TimeAndIndex* liveMarker = 0;
    QList<TimeAndIndex*> markers;
    QString tag = model()->data(currentIndex()).toString();
    QModelIndex liveIdx = _bookModel()->getDataIndex(QModelIndex(),
                                                     "LiveCoordTime");
    double liveTime = model()->data(liveIdx).toDouble();
    if ( currentIndex().isValid() && (tag == "Curve" || tag == "Plot") ) {
        TimeAndIndex* liveMarker = new TimeAndIndex(liveTime,currentIndex());
        markers << liveMarker;
    }
    markers << _markers;

    foreach ( TimeAndIndex* marker, markers ) {

        // Tag is either "Curve" or "Plot"
        QString tag = _bookModel()->data(marker->idx()).toString();

        // Scale and bias
        double xs = 1.0;
        double ys = 1.0;
        double xb = 0.0;
        double yb = 0.0;
        if ( tag == "Curve") {
            QModelIndex curveIdx = marker->idx();
            xs = _bookModel()->xScale(curveIdx);
            ys = _bookModel()->yScale(curveIdx);
            xb = _bookModel()->xBias(curveIdx);
            yb = _bookModel()->yBias(curveIdx);
        }

        // Get path
        QPainterPath* path = 0;
        if ( tag == "Curve" ) {
            QModelIndex curveIdx = marker->idx();
            path = _bookModel()->getCurvePainterPath(curveIdx);
        } else if ( tag == "Plot" ) {
            QModelIndex plotIdx = marker->idx();
            QModelIndex curvesIdx = _bookModel()->getIndex(plotIdx,
                                                           "Curves","Plot");
            path = _bookModel()->getCurvesErrorPath(curvesIdx);
        }

        // Get element index (i) for live time
        int high = path->elementCount()-1;
        int i = 0;
        if ( xb != 0.0 || xs != 1.0 ) {
            double t = (marker->time()-xb)/xs;
            ROUNDOFF(t,t);
            i = _idxAtTimeBinarySearch(path,0,high,t);
        } else {
            i = _idxAtTimeBinarySearch(path,0,high,marker->time());
        }

        // Element/coord at live time
        QPainterPath::Element el = path->elementAt(i);
        QPointF coord(el.x*xs+xb,el.y*ys+yb);

        // Init arrow struct
        CoordArrow arrow;
        arrow.coord = coord;

        // Set arrow text (special syntax for extremums)
        QString x = _format(coord.x());
        QString y = _format(coord.y());
        int rc = path->elementCount();
        if ( i > 0 && i < rc-1) {
            // First and last point not considered
            double y1 = path->elementAt(i-1).y*ys+yb;
            double y  = path->elementAt(i).y*ys+yb;
            double y2 = path->elementAt(i+1).y*ys+yb;
            if ( (y>y1 && y>y2) || (y<y1 && y<y2) ) {
                arrow.txt = QString("<%1, %2>").arg(x).arg(y);
            } else if ( y1 == y && y != y2 ) {
                arrow.txt = QString("(%1, %2]").arg(x).arg(y);
            } else if ( y1 != y && y == y2 ) {
                arrow.txt = QString("[%1, %2)").arg(x).arg(y);
            } else {
                arrow.txt = QString("(%1, %2)").arg(x).arg(y);
            }
        } else if ( i == 0 ) {
            arrow.txt = QString("init=(%1, %2)").arg(x).arg(y);
        } else if ( i == rc-1 ) {
            arrow.txt = QString("last=(%1, %2)").arg(x).arg(y);
        } else {
            fprintf(stderr,"koviz [bad scoobs]: CurvesView::_paintMarkers()\n");
            exit(-1);
        }

        // Prepend RunID to live text if more than 5 runs
        if ( tag == "Curve") {
            QModelIndex curveIdx = marker->idx();
            if ( model()->rowCount(curveIdx.parent()) > 5 ) {
                int id = _bookModel()->getDataInt(curveIdx,"CurveRunID","Curve");
                QString runID = QString("RUN_%1: ").arg(id);
                arrow.txt.prepend(runID);
            }
        }

        // Map math coord to window pt
        QTransform T = _coordToPixelTransform();

        // Try to fit arrow into viewport using different angles
        QList<double> angles;
        angles << 1*(M_PI/4) << 3*(M_PI/4) << 5*(M_PI/4) << 7*(M_PI/4);
        bool isFits = false;
        foreach ( double angle, angles ) {
            arrow.angle = angle;
            QRect arrowBBox = arrow.boundingBox(painter,T).toRect();
            if ( viewport()->rect().contains(arrowBBox) ) {
                isFits = true;
                break;
            }
        }

        if ( isFits ) {
            arrow.paintMe(painter,T,fg,bg);
        } else if ( marker->idx() == currentIndex() ) {
            arrow.paintMeCenter(painter,T,viewport()->rect(),fg,bg);
        }
    }

    painter.restore();

    if ( liveMarker ) {
        delete liveMarker;
    }
}

int CurvesView::_idxAtTimeBinarySearch(QPainterPath* path,
                                       int low, int high, double time)
{
        if (high <= 0 ) {
                return 0;
        }
        if (low >= high) {
                return ( path->elementAt(high).x > time ) ? high-1 : high;
        } else {
                int mid = (low + high)/2;
                if (time == path->elementAt(mid).x ) {
                        return mid;
                } else if ( time < path->elementAt(mid).x ) {
                        return _idxAtTimeBinarySearch(path,
                                                      low, mid-1, time);
                } else {
                        return _idxAtTimeBinarySearch(path,
                                                      mid+1, high, time);
                }
        }
}

void CurvesView::_keyPressPeriod()
{
    // If curve is selected
    QModelIndex gpidx = currentIndex().parent().parent();
    QString tag = model()->data(currentIndex()).toString();
    if ( currentIndex().isValid() && tag == "Curve" && gpidx == rootIndex()) {

        QModelIndex curveIdx = currentIndex();

        CurveModel* curveModel = _bookModel()->getCurveModel(curveIdx);
        curveModel->map();

        // Calculate liveCoord based on model liveCoordTime
        double xs = _bookModel()->xScale(curveIdx);
        double ys = _bookModel()->yScale(curveIdx);
        double xb = _bookModel()->xBias(curveIdx);
        double yb = _bookModel()->yBias(curveIdx);
        QModelIndex liveIdx = _bookModel()->getDataIndex(QModelIndex(),
                                                         "LiveCoordTime");
        double liveTime = model()->data(liveIdx).toDouble();
        int i = 0;
        if ( curveModel->x()->name() == curveModel->t()->name() ) {
            i = curveModel->indexAtTime((liveTime-xb)/xs);
        } else {
            // e.g. ball xy curve where x is position[0]
            i = curveModel->indexAtTime(liveTime);
        }
        ModelIterator* it = curveModel->begin();
        QPointF coord(it->at(i)->x()*xs+xb, it->at(i)->y()*ys+yb);
        delete it;

        QString x = _format(coord.x());
        QString y = _format(coord.y());
        fprintf(stderr,"coord=(%s,%s)\n",
                       x.toLatin1().constData(),
                       y.toLatin1().constData());

        curveModel->unmap();
    }
}

void CurvesView::_keyPressArrow(const Qt::ArrowType& arrow)
{
    if ( arrow != Qt::LeftArrow && arrow != Qt::RightArrow ) {
        fprintf(stderr, "koviz [bad scoobs]: "
                        "CurvesView::_keyPressArrow(%d)\n",arrow);
        exit(-1);
    }

    // If curve is selected
    QModelIndex gpidx = currentIndex().parent().parent();
    QString tag = model()->data(currentIndex()).toString();
    if ( currentIndex().isValid() && tag == "Curve" && gpidx == rootIndex()) {

        QModelIndex curveIdx = currentIndex();
        CurveModel* curveModel = _bookModel()->getCurveModel(curveIdx);
        curveModel->map();

        // Calculate liveCoord based on model liveCoordTime
        double xs = _bookModel()->xScale(curveIdx);
        double xb = _bookModel()->xBias(curveIdx);
        QModelIndex liveIdx = _bookModel()->getDataIndex(QModelIndex(),
                                                         "LiveCoordTime");

        double liveTime = model()->data(liveIdx).toDouble();
        int i = 0;
        bool isXTime = (curveModel->x()->name() == curveModel->t()->name());
        if ( isXTime ) {
            i = curveModel->indexAtTime((liveTime-xb)/xs);
        } else {
            // e.g. ball xy curve where x is position[0]
            i = curveModel->indexAtTime(liveTime);
        }

        double timeStamp = liveTime;
        ModelIterator* it = curveModel->begin();
        if ( arrow == Qt::LeftArrow ) {
            while ( i >= 0 ) {
                it = it->at(i);
                if ( isXTime ) {
                    timeStamp = it->x()*xs+xb;
                } else {
                    timeStamp = it->t();
                }
                if ( qAbs(timeStamp-liveTime) > 1.0e-16 ) {
                    break;
                }
                --i;
            }
        } else if ( arrow == Qt::RightArrow ) {
            it = it->at(i+1);
            while ( !it->isDone() ) {
                if ( isXTime ) {
                    timeStamp = it->x()*xs+xb;
                } else {
                    timeStamp = it->t();
                }
                if ( qAbs(timeStamp-liveTime) > 1.0e-16 ) {
                    break;
                }
                it->next();
            }
        }
        delete it;

        _bookModel()->setData(liveIdx,timeStamp);

        curveModel->unmap();
    }
}

void CurvesView::_paintErrorplot(const QTransform &T,
                                 QPainter &painter, const QPen &pen,
                                 const QModelIndex& plotIdx)
{
    painter.save();

    QModelIndex curvesIdx = _bookModel()->getIndex(plotIdx,"Curves","Plot");
    QPainterPath* errorPath = _bookModel()->getCurvesErrorPath(curvesIdx);

    QRectF ebox = errorPath->boundingRect();
    QPen ePen(pen);
    if ( ebox.height() == 0.0 && ebox.y() == 0.0 ) {
        // Color green if error plot is flatline zero
        ePen.setColor(_bookModel()->flatLineColor());
    } else {
        ePen.setColor(_bookModel()->errorLineColor());
    }
    painter.setPen(ePen);
    if ( ebox.height() == 0.0 ) {
        // Flatline
        QString yval;
        if ( ebox.y() == 0.0 ) {
            yval = yval.sprintf("Flatline=0.0");
        } else {
            yval = yval.sprintf("Flatline=%g",ebox.y());
        }
        QTransform I;
        painter.setTransform(I);
        QRectF tbox = T.mapRect(ebox);
        painter.drawText(tbox.topLeft()-QPointF(0,5),yval);
    }
    painter.setTransform(T);
    painter.drawPath(*errorPath);

    delete errorPath;

    painter.setPen(pen);
    painter.restore();
}

QSize CurvesView::minimumSizeHint() const
{
    QSize s(50,50);
    return s;
}

QSize CurvesView::sizeHint() const
{
    QSize s(200,200);
    return s;
}

void CurvesView::dataChanged(const QModelIndex &topLeft,
                             const QModelIndex &bottomRight)
{
    Q_UNUSED(bottomRight);

    if ( topLeft.column() != 1 ) return;

    QString tag = model()->data(topLeft.sibling(topLeft.row(),0)).toString();

    if ( tag == "PlotMathRect" && topLeft.parent() == rootIndex() ) {

        QRectF M = model()->data(topLeft).toRectF();

        if ( M.size().width() > 0 && M.size().height() != 0 && _lastM != M ) {
            if ( _pixmap ) {
                delete _pixmap;
            }
            _pixmap = _createLivePixmap();
        }

        _lastM = M;  // Saved so that pixmap is not recreated if M unchanged

    } else if ( tag == "PlotMathRect" && topLeft.parent() != rootIndex() ) {
        // Another plot has changed its PlotMathRect.
        // Synchronize this plot's PlotMathRect with the changed one
        // to keep zoom/pan synchronized across plots.

        // Only synchronize when x variables are time (normal case).
        if ( _bookModel()->isXTime(topLeft.parent()) &&
             _bookModel()->isXTime(rootIndex()) ) {
            QRectF M = model()->data(topLeft).toRectF();
            QModelIndex plotRectIdx = _bookModel()->getDataIndex(rootIndex(),
                                                         "PlotMathRect","Plot");
            QRectF R = model()->data(plotRectIdx).toRectF();
            if ( M.left() != R.left() || M.right() != R.right() ) {
                R.setLeft(M.left());
                R.setRight(M.right());
                double plotXMinRange = _bookModel()->getDataDouble(rootIndex(),
                                                        "PlotXMinRange","Plot");
                double plotXMaxRange = _bookModel()->getDataDouble(rootIndex(),
                                                        "PlotXMaxRange","Plot");
                if ( R.left() >= plotXMinRange && R.right() <= plotXMaxRange ) {
                    // Set R if within PlotXMin/MaxRange
                    _bookModel()->setPlotMathRect(R,rootIndex());
                }
            }
        }
    } else if ( topLeft.parent().parent().parent() == rootIndex() ) {
        if ( tag == "CurveXBias" ) {
            if ( _pixmap ) {
                delete _pixmap;
            }
            _pixmap = _createLivePixmap();
        } else if ( tag == "CurveColor") {
            if ( _pixmap ) {
                delete _pixmap;
            }
            _pixmap = _createLivePixmap();
        } else if ( tag == "CurveData") {
            if ( _pixmap ) {
                delete _pixmap;
            }
            _pixmap = _createLivePixmap();
        }
    }

    viewport()->update();
    update();
}

QPixmap* CurvesView::_createLivePixmap()
{
    QPixmap* livePixmap = new QPixmap(viewport()->rect().size());

    QPainter painter(livePixmap);
    painter.setRenderHint(QPainter::Antialiasing);

    QModelIndex pageIdx = rootIndex().parent().parent();
    QColor bg = _bookModel()->pageBackgroundColor(pageIdx);
    painter.fillRect(viewport()->rect(),bg);

    QTransform T = _coordToPixelTransform();

    _paintGrid(painter, rootIndex());

    QModelIndex curvesIdx = _bookModel()->getIndex(rootIndex(),"Curves","Plot");
    int rc = model()->rowCount(curvesIdx);
    for ( int i = 0; i < rc; ++i ) {
        QModelIndex curveIdx = model()->index(i,0,curvesIdx);
        _paintCurve(curveIdx,T,painter,false);
    }

    return livePixmap;
}

QString CurvesView::_format(double d)
{
    QString s;
    s = s.sprintf("%.9g",d);
    QVariant v(s);
    double x = v.toDouble();
    double e = qAbs(x-d);
    if ( e > 1.0e-9 ) {
        s = s.sprintf("%.9lf",d);
    }
    return s;
}

// TODO: This thing does nothing!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
void CurvesView::rowsInserted(const QModelIndex &pidx, int start, int end)
{
    if ( pidx.parent().parent() != rootIndex() ) return; // not my plot

        for ( int i = start; i <= end; ++i ) {
            QModelIndex curveIdx = model()->index(i,0,pidx);
            QModelIndex curveDataIdx = model()->index(i,1,pidx);
            QString name = model()->data(curveIdx).toString();
            if ( name == "CurveData" ) {
                QVariant v = model()->data(curveDataIdx);
                CurveModel* curveModel = QVariantToPtr<CurveModel>::convert(v);
                if ( curveModel ) {
                    //_setPlotMathRect(_currBBox);
                }
                break;
            }
        }
}

QPainterPath CurvesView::_sinPath()
{
    // Make sin(t) t=[0,2*pi]
    QPainterPath path;
    path.moveTo(0,0);
    double t = 0;
    double dt = 0.001;
    while (1) {
        t += dt;
        if ( t > 2*M_PI-0.001 ) {
            t = 2*M_PI;
        }
        path.lineTo(t,sin(t));
        if ( t == 2*M_PI ) break;
    }
    return path;
}

QPainterPath CurvesView::_stepPath()
{
    QPainterPath path;
    path.moveTo(0,0.0);
    double t = 0;
    double dt = 0.01;
    while (1) {
        path.lineTo(t,floor(t));
        if ( t > 10.0 ) break;
        t += dt;
    }
    return path;
}

QList<QColor> CurvesView::_createColorBands(int nBands, bool isRainbow)
{
    QList<QColor> colorBands;

    QColor blue(48,85,200);
    QColor red(200,30,30);
    QColor green(60,180,45);
    QColor magenta(130,15,120);
    QColor orange(183,120,71);
    QColor burntorange(177,79,0);
    QColor yellow(222,222,10);
    QColor pink(255,192,255);
    QColor gray(145,170,192);
    QColor medblue(49,140,250);
    QColor black(0,0,0);

    if ( isRainbow && nBands >= 10 ) {

        // This is for "rainbow banding" many monte carlo curves

        int hBeg = 10; int hEnd = 230;
        int dh = qRound((double)(hEnd-hBeg)/(nBands-1.0));
        int s = qRound(0.75*255);
        int v = qRound(0.87*255);
        for ( int h = hBeg; h <= hEnd; h+=dh) {
            // Rainbow
            colorBands << QColor::fromHsv(h,s,v);
        }
        colorBands.removeFirst();
        colorBands.prepend(burntorange);

    } else {

        // Handpicked band of colors for smaller number of curves
        colorBands << blue << red << magenta
                   << green << orange << black
                   << gray << pink << medblue;

        for ( int i = 0 ; i < nBands-10; ++i ) {
            colorBands.removeLast();
        }
    }

    return colorBands;
}


void CurvesView::mousePressEvent(QMouseEvent *event)
{
    if (  event->button() == Qt::LeftButton ) {
        _mousePressPos = event->pos();
        _mousePressMathTopLeft = _mathRect().topLeft();
        _mousePressMathRect = _mathRect();
        event->ignore();
    } else if (  event->button() == Qt::MidButton ) {
        event->ignore();
    } else if ( event->button() == Qt::RightButton ) {
        QModelIndex curvesIdx = _bookModel()->getIndex(rootIndex(),
                                                       "Curves","Plot");
        QRectF bbox = _bookModel()->calcCurvesBBox(curvesIdx);
        _bookModel()->setPlotMathRect(bbox,rootIndex());
        viewport()->update();
    }
}

void CurvesView::mouseReleaseEvent(QMouseEvent *event)
{
    bool isShift = false;
    Qt::KeyboardModifiers keymods = event->modifiers();
    if ( keymods & Qt::ShiftModifier ) {
        isShift = true;
    }

    if (  event->button() == Qt::LeftButton && isShift ) {
        // Toggle between single/multi views when clicking with shift key
        event->ignore();
    } else if (  event->button() == Qt::LeftButton && !isShift ) {
        double x0 = _mousePressPos.x();
        double y0 = _mousePressPos.y();
        double x1 = event->pos().x();
        double y1 = event->pos().y();
        double d = qSqrt((x1-x0)*(x1-x0)+(y1-y0)*(y1-y0));
        QString presentation = _bookModel()->getDataString(rootIndex(),
                                                   "PlotPresentation","Plot");
        if ( d < 10 && (presentation == "compare" || presentation.isEmpty()) ) {
            // d < 10, to hopefully catch click and not a drag
            QModelIndex curveIdx = _chooseCurveNearMousePoint(event->pos());
            if ( curveIdx.isValid() ) {
                // Curve found in small box around mouse click!
                if ( currentIndex() == curveIdx ) {
                    // If current curve clicked again - "deselect"
                    setCurrentIndex(QModelIndex());
                } else {
                    // If curve other than current clicked - "select" it
                    selectionModel()->setCurrentIndex(curveIdx,
                                                 QItemSelectionModel::NoUpdate);
                }
            } else if ( !curveIdx.isValid() &&  currentIndex().isValid() ) {
                // click off curves, current unset i.e. "deselect curve"
                setCurrentIndex(QModelIndex());
            } else if ( !curveIdx.isValid() &&  !currentIndex().isValid() ) {
                // click off curves when nothing selected -> toggle single/multi
                event->ignore(); // pass event to page view
            }
        } else if ( d < 10 && presentation == "error" ) {
            if ( currentIndex().isValid() ) {
                setCurrentIndex(QModelIndex());  // toggle to deselect
            } else {
                QModelIndex plotIdx = rootIndex();
                if ( !_bookModel()->isIndex(plotIdx,"Plot") ) {
                    fprintf(stderr, "koviz [bad scoobs]: "
                                    "CurvesView::mouseReleaseEvent()\n");
                    exit(-1);
                }
                setCurrentIndex(plotIdx);
            }
        } else {
            //event->ignore(); // pass event to parent view for stretch,zoom etc
        }
    } else if (  event->button() == Qt::MidButton ) {
        event->ignore();
    } else if ( event->button() == Qt::RightButton ) {
        event->ignore();
    }
}

// Choose first curve found within 12 pixel square about pt
QModelIndex CurvesView::_chooseCurveNearMousePoint(const QPoint &pt)
{
    QModelIndex idx;

    QImage img(viewport()->rect().size(),QImage::Format_Mono);

    QPainter painter(&img);
    QPen penBlack(img.colorTable().at(0));
    penBlack.setWidth(0);
    painter.setPen(penBlack);

    QTransform T = _coordToPixelTransform();  // _paintCurve sets painter tform

    int s = 12; // side length of small square around mouse click
    QRectF R(pt.x()-s/2,pt.y()-s/2,s,s);

    // Speed up test by drawing clipped to small square
    painter.setClipRect(R);

    QRectF W = viewport()->rect();
    QRectF M = _mathRect();
    double a = M.width()/W.width();
    double b = M.height()/W.height();
    double c = M.x() - a*W.x();
    double d = M.y() - b*W.y();
    QTransform U( a,    0,
                  0,    b, /*+*/ c,    d);
    M = U.mapRect(R);

    QModelIndex curvesIdx = _bookModel()->getIndex(rootIndex(),"Curves","Plot");
    int rc = model()->rowCount(curvesIdx);
    for ( int i = rc-1; i >= 0; --i ) {  // check curves from top to bottom

        // fill image with white (see help for QImage::fill(int))
        img.fill(1);

        // Get underlying path that goes with curve
        QModelIndex curveIdx = model()->index(i,0,curvesIdx);
        QPainterPath* path = _bookModel()->getCurvePainterPath(curveIdx);
        if ( !path ) {
            continue;
        }

        // Curve can be scaled by unit or scale factor
        double xs = _bookModel()->xScale(curveIdx);
        double ys = _bookModel()->yScale(curveIdx);
        double xb = _bookModel()->xBias(curveIdx);
        double yb = _bookModel()->yBias(curveIdx);
        QTransform Tscaled(T);
        Tscaled = Tscaled.scale(xs,ys);
        Tscaled = Tscaled.translate(xb/xs,yb/ys);
        painter.setTransform(Tscaled);

        // Ignore paths whose bounding box does not intersect
        // math click rect. This speeds up a special case of
        // selecting a spike which falls outside most other curves
        if ( xs == 1.0 && ys == 1.0 && xb == 0 && yb == 0 ) {
            // TODO: Lazily checking for xs==ys==1.0 because of scaling issues
            if ( !path->intersects(M) ) {
                continue;
            }
        }

        // Draw curve onto monochrome image (clipped to small square)
        painter.drawPath(*path);

        // Check, pixel by pixel, to see if the curve
        // is in small rectangle around mouse click
        bool isFound = false;
        for ( int x = pt.x()-s/2; x < pt.x()+s/2; ++x ) {
            if ( x < 0 || x >= img.width() ) {
                continue;
            }
            for ( int y = pt.y()-s/2; y < pt.y()+s/2; ++y ) {
                if ( y < 0 || y >= img.height() ) {
                    continue;
                }
                QRgb pix = img.pixel(x,y);
                if ( qRed(pix) == 0 ) {
                    // qRed(pix) is arbitrary. Pen is black and has no red.
                    isFound = true;
                    break;
                }
            }
            if ( isFound ) break;
        }
        if ( isFound ) {
            idx = curveIdx;  // choose first curve inside rect and bail
            break;
        }


    }

    return idx;
}

void CurvesView::mouseMoveEvent(QMouseEvent *mouseMove)
{
    QRectF W = viewport()->rect();
    if ( W.width() < 1 || W.height() < 1 ) return;

    double Ww = W.width();  // greater > 0, by top line
    double Wh = W.height();

    if ( mouseMove->buttons() == Qt::NoButton && currentIndex().isValid() ) {

        QString tag = model()->data(currentIndex()).toString();
        QString presentation = _bookModel()->getDataString(rootIndex(),
                                                   "PlotPresentation","Plot");

        // If shift pressed while moving the mouse, do not update
        // the live coordinate.  This saves the live coord from
        // being updated while moving the mouse.
        Qt::KeyboardModifiers keymods = mouseMove->modifiers();
        bool shiftPressed = false;
        if ( keymods & Qt::ShiftModifier ) {
            shiftPressed = true;
        }

        QRectF M = _mathRect();
        double a = M.width()/W.width();
        double b = M.height()/W.height();
        double c = M.x() - a*W.x();
        double d = M.y() - b*W.y();
        QTransform T(a, 0,
                     0, b, /*+*/ c, d);
        QPointF wPt = mouseMove->pos();
        QPointF mPt = T.map(wPt);

        if ( !shiftPressed &&
             (tag == "Curve" &&
             (presentation == "compare" || presentation.isEmpty())) ) {

            CurveModel* curveModel =
                                    _bookModel()->getCurveModel(currentIndex());
            if ( curveModel ) {

                curveModel->map();
                ModelIterator* it = curveModel->begin();
                double xs = _bookModel()->xScale(currentIndex());
                double ys = _bookModel()->yScale(currentIndex());
                double xb = _bookModel()->xBias(currentIndex());
                double yb = _bookModel()->yBias(currentIndex());
                int rc = curveModel->rowCount() ;
                QModelIndex liveTimeIdx = _bookModel()->getDataIndex(
                            QModelIndex(),
                            "LiveCoordTime");

                QString timeName = _bookModel()->getDataString(currentIndex(),
                                                               "CurveTimeName",
                                                               "Curve");
                if ( curveModel->x()->name() == timeName ) {

                    QPointF liveCoord(DBL_MAX,DBL_MAX);

                    if ( rc == 0 ) {

                        // "null" out liveCoord
                        liveCoord = QPointF(DBL_MAX,DBL_MAX);

                    } else if ( rc == 1 ) {

                        liveCoord = QPointF(it->x()*xs+xb,it->y()*ys+yb);

                    } else if ( rc == 2 ) {
                        QPointF p0(it->at(0)->x()*xs+xb,it->at(0)->y()*ys+yb);
                        QPointF p1(it->at(1)->x()*xs+xb,it->at(1)->y()*ys+yb);
                        QLineF l0(p0,mPt);
                        QLineF l1(p1,mPt);
                        if ( l0.length() < l1.length() ) {
                            liveCoord = p0;
                        } else {
                            liveCoord = p1;
                        }

                    } else if ( rc >= 3 ) {

                        int i = curveModel->indexAtTime((mPt.x()-xb)/xs);
                        QPointF p(it->at(i)->x()*xs+xb,it->at(i)->y()*ys+yb);

                        //
                        // Find dt between p and prev or next point
                        //
                        double dt = 0.0;
                        if ( i == 0 ) {
                            // Start point
                            dt = it->at(1)->x()*xs+xb - p.x();
                        } else if ( i > 0 ) {
                            dt = p.x() - (it->at(i-1)->x()*xs+xb);
                        } else {
                            fprintf(stderr,"koviz [bad scoobs]:1: "
                                           "CurvesView::mouseMoveEvent()\n");
                            exit(-1);
                        }
                        if ( dt < 1.0e-9 ) {
                            fprintf(stderr,"koviz [bad scoobs]:2: "
                                           "CurvesView::mouseMoveEvent() "
                                           "dt < 1.0e-9 \n");
                            exit(-1);
                        }

                        //
                        // Make "neighborhood" around mouse point
                        //
                        QRectF M = _plotMathRect(rootIndex());
                        QRectF W = viewport()->rect();
                        double Wr = 2.0*fontMetrics().xHeight(); // near mouse
                        double Mr = Wr*(M.width()/W.width());

                        int di = qFloor(Mr/dt);
                        int j = i-di;
                        j = (j < 0) ? 0 : j;
                        int k = i+di;
                        k = (k >= rc) ? rc-1 : k;

                        //
                        // Find local min/maxs in neighborhood
                        //
                        QList<QPointF> localMaxs;
                        QList<QPointF> localMins;
                        QList<QPointF> flatChangePOIs;
                        for (int m = j; m <= k; ++m ) {
                            QPointF pt(it->at(m)->x()*xs+xb,
                                       it->at(m)->y()*ys+yb);
                            if ( m > 0 && m < k ) {
                                double yPrev = it->at(m-1)->y()*ys+yb;
                                double y  = it->at(m)->y()*ys+yb;
                                double yNext = it->at(m+1)->y()*ys+yb;
                                if ( y > yPrev && y > yNext ) {
                                    if ( localMaxs.isEmpty() ) {
                                        localMaxs << pt;
                                    } else {
                                        if ( pt.y() > localMaxs.first().y() ) {
                                            localMaxs.prepend(pt);
                                        } else {
                                            localMaxs << pt;
                                        }
                                    }
                                } else if ( y < yPrev && y < yNext ) {
                                    if ( localMins.isEmpty() ) {
                                        localMins << pt;
                                    } else {
                                        if ( pt.y() < localMins.first().y() ) {
                                            localMins.prepend(pt);
                                        } else {
                                            localMins << pt;
                                        }
                                    }
                                } else if ( yPrev == y && y != yNext ) {
                                    if ( mPt.x() <= pt.x() ) {
                                        // mouse left of change
                                        flatChangePOIs.prepend(pt);
                                    }
                                } else if ( yPrev != y && y == yNext ) {
                                    if ( mPt.x() >= pt.x() ) {
                                        // mouse right of change
                                        flatChangePOIs.prepend(pt);
                                    }
                                }
                            }
                        }

                        //
                        // Choose live coord based on local mins/maxs
                        // and proximity to start/end points
                        //
                        if ( j == 0 || wPt.x()/W.width() < 0.02 ) {
                            // Mouse near curve start or left 2% of window,
                            // set to start pt
                            liveCoord = QPointF(it->at(0)->x()*xs+xb,
                                                it->at(0)->y()*ys+yb);
                        } else if ( k == rc-1 || wPt.x()/W.width() > 0.98 ) {
                            // Mouse near curve end or right 2% of window,
                            // set to last pt
                            liveCoord = QPointF(it->at(k)->x()*xs+xb,
                                                it->at(k)->y()*ys+yb);
                        } else {
                            bool isMaxs = localMaxs.isEmpty() ? false : true;
                            bool isMins = localMins.isEmpty() ? false : true;
                            if ( isMaxs && !isMins ) {
                                liveCoord = localMaxs.first();
                            } else if ( !isMaxs && isMins ) {
                                liveCoord = localMins.first();
                            } else if ( isMaxs && isMins ) {
                                // There are local mins and maxes
                                if ( wPt.y()/W.height() < 0.125 ) {
                                    // Mouse in top 1/8th of window
                                    liveCoord = localMaxs.first();
                                } else if ( mPt.y() > localMaxs.first().y() ) {
                                    // Mouse above curve
                                    liveCoord = localMaxs.first();
                                } else {
                                    // Mouse below curve
                                    liveCoord = localMins.first();
                                }
                            } else if ( !isMaxs && !isMins ) {
                                if ( !flatChangePOIs.isEmpty() ) {
                                    liveCoord = flatChangePOIs.first();
                                } else {
                                    liveCoord = p;
                                }
                            } else {
                                fprintf(stderr,"koviz [bad scoobs]:3: "
                                              "CurvesView::mouseMoveEvent()\n");
                                exit(-1);
                            }
                        }
                    }

                    // Set live coord in model
                    double start = _bookModel()->getDataDouble(QModelIndex(),
                                                               "StartTime");
                    double stop = _bookModel()->getDataDouble(QModelIndex(),
                                                "StopTime");
                    if ( liveCoord.x() <= start ) {
                        model()->setData(liveTimeIdx,start);
                    } else if ( liveCoord.x() >= stop ) {
                        model()->setData(liveTimeIdx,stop);
                    } else {
                        model()->setData(liveTimeIdx,liveCoord.x());
                    }

                } else {  // Curve x is not time e.g. ball xy-position

                    double liveTime = DBL_MAX;
                    double dMin = DBL_MAX;
                    while ( !it->isDone() ) { // find closest point on curve to mouse
                        QPointF p(it->x()*xs+xb,it->y()*ys+yb);
                        double d = QLineF(mPt,p).length();
                        if ( d < dMin ) {
                            dMin = d;
                            liveTime = it->t();
                        }
                        it->next();
                    }

                    // Set live coord in model
                    double start = _bookModel()->getDataDouble(QModelIndex(),
                                                             "StartTime");
                    double stop = _bookModel()->getDataDouble(QModelIndex(),
                                                            "StopTime");
                    if ( liveTime <= start ) {
                        model()->setData(liveTimeIdx,start);
                    } else if ( liveTime >= stop ) {
                        model()->setData(liveTimeIdx,stop);
                    } else {
                        model()->setData(liveTimeIdx,liveTime);
                    }
                }

                delete it;
                curveModel->unmap();
                viewport()->update();
            }

        } else if ( !shiftPressed && (tag == "Plot" || tag == "Curve") &&
                    (presentation == "error" ||
                     presentation == "error+compare")) {

            // TODO: This code block is almost a duplicate of the code block
            //       above for compare plot.  The difference is that the
            //       error data is an unscaled/biased QPainterPath.
            QModelIndex curvesIdx =  _bookModel()->getIndex(rootIndex(),
                                                            "Curves","Plot");
            QPainterPath* path = _bookModel()->getCurvesErrorPath(curvesIdx);
            QModelIndex liveTimeIdx = _bookModel()->getDataIndex(
                                                               QModelIndex(),
                                                               "LiveCoordTime");

            int rc = path->elementCount();
            QPointF liveCoord(DBL_MAX,DBL_MAX);

            if ( rc == 0 ) {

                // "null" out liveCoord
                liveCoord = QPointF(DBL_MAX,DBL_MAX);

            } else if ( rc == 1 ) {

                QPainterPath::Element el = path->elementAt(0);
                liveCoord = QPointF(el.x,el.y);

            } else if ( rc == 2 ) {

                QPainterPath::Element el0 = path->elementAt(0);
                QPainterPath::Element el1 = path->elementAt(1);
                QPointF p0(el0.x,el0.y);
                QPointF p1(el1.x,el1.y);
                QLineF l0(p0,mPt);
                QLineF l1(p1,mPt);
                if ( l0.length() < l1.length() ) {
                    liveCoord = p0;
                } else {
                    liveCoord = p1;
                }

            } else if ( rc >= 3 ) {

                int i =  _idxAtTimeBinarySearch(path,0,rc-1,mPt.x());
                QPainterPath::Element el = path->elementAt(i);
                QPointF p(el.x,el.y);

                //
                // Find dt between p and prev or next point
                //
                double dt = 0.0;
                if ( i == 0 ) {
                    // Start point
                    dt = path->elementAt(1).x - p.x();
                } else if ( i > 0 ) {
                    dt = p.x() - path->elementAt(i-1).x;
                } else {
                    fprintf(stderr,"koviz [bad scoobs]:4: "
                                   "CurvesView::mouseMoveEvent()\n");
                    exit(-1);
                }
                if ( dt < 1.0e-9 ) {
                    fprintf(stderr,"koviz [bad scoobs]:5: "
                                   "CurvesView::mouseMoveEvent() "
                                   "dt < 1.0e-9 \n");
                    exit(-1);
                }


                //
                // Make "neighborhood" around mouse point
                //
                QRectF M = _plotMathRect(rootIndex());
                QRectF W = viewport()->rect();
                double Wr = 2.0*fontMetrics().xHeight(); // near mouse
                double Mr = Wr*(M.width()/W.width());

                int di = qFloor(Mr/dt);
                int j = i-di;
                j = (j < 0) ? 0 : j;
                int k = i+di;
                k = (k >= rc) ? rc-1 : k;

                //
                // Find local min/maxs in neighborhood
                //
                QList<QPointF> localMaxs;
                QList<QPointF> localMins;
                for (int m = j; m <= k; ++m ) {
                    QPointF pt(path->elementAt(m).x,path->elementAt(m).y);
                    if ( m > 0 && m < k ) {
                        double yPrev = path->elementAt(m-1).y;
                        double y  = path->elementAt(m).y;
                        double yNext = path->elementAt(m+1).y;
                        if ( y > yPrev && y > yNext ) {
                            if ( localMaxs.isEmpty() ) {
                                localMaxs << pt;
                            } else {
                                if ( pt.y() > localMaxs.first().y() ) {
                                    localMaxs.prepend(pt);
                                } else {
                                    localMaxs << pt;
                                }
                            }
                        } else if ( y < yPrev && y < yNext ) {
                            if ( localMins.isEmpty() ) {
                                localMins << pt;
                            } else {
                                if ( pt.y() < localMins.first().y() ) {
                                    localMins.prepend(pt);
                                } else {
                                    localMins << pt;
                                }
                            }
                        }
                    }
                }

                //
                // Choose live coord based on local mins/maxs
                // and proximity to start/end points
                //
                if ( j == 0 ) {
                    // Mouse near start of curve, set to start pt
                    liveCoord = QPointF(path->elementAt(0).x,
                                        path->elementAt(0).y);
                } else if ( k == rc-1 ) {
                    // Mouse near end of curve, set to last pt
                    liveCoord = QPointF(path->elementAt(k).x,
                                        path->elementAt(k).y);
                } else {
                    bool isMaxs = localMaxs.isEmpty() ? false : true;
                    bool isMins = localMins.isEmpty() ? false : true;
                    if ( isMaxs && !isMins ) {
                        liveCoord = localMaxs.first();
                    } else if ( !isMaxs && isMins ) {
                        liveCoord = localMins.first();
                    } else if ( isMaxs && isMins ) {
                        // There are local mins and maxes
                        if ( mPt.y() > localMaxs.first().y() ) {
                            // Mouse above curve
                            liveCoord = localMaxs.first();
                        } else {
                            // Mouse below curve
                            liveCoord = localMins.first();
                        }
                    } else if ( !isMaxs && !isMins ) {
                        liveCoord = p;
                    } else {
                        fprintf(stderr,"koviz [bad scoobs]:6: "
                                       "CurvesView::mouseMoveEvent()\n");
                        exit(-1);
                    }
                }
            }

            // Set live coord in model
            double start = _bookModel()->getDataDouble(QModelIndex(),
                                                       "StartTime");
            double stop = _bookModel()->getDataDouble(QModelIndex(),
                                                      "StopTime");
            if ( mPt.x() <= start ) {
                model()->setData(liveTimeIdx,start);
            } else if ( mPt.x() >= stop ) {
                model()->setData(liveTimeIdx,stop);
            } else {
                model()->setData(liveTimeIdx,liveCoord.x());
            }

            viewport()->update();
        }

    } else if ( mouseMove->buttons() == Qt::LeftButton ) {

        QRectF M = _mathRect();
        double Mw = M.width();
        double Mh = M.height();

        QTransform T(Mw/Ww, 0.0,  // div by zero checked at top of method
                     0.0, Mh/Wh,
                     0.0, 0.0);

        QPointF wPt = mouseMove->pos()-_mousePressPos;
        QPointF mPt = _mousePressMathTopLeft-T.map(wPt);

        double k = 0.88;
        QRectF insideRect((1-k)*Ww/2.0,(1-k)*Wh/2.0,k*Ww,k*Wh);
        if ( insideRect.contains(_mousePressPos) ) {
            // Pan if mouse press pos is deeper inside window
            M.moveTo(mPt);
            _bookModel()->setPlotMathRect(M,rootIndex());
            viewport()->update();
        } else {
            // Scrunch if mouse press pos is on perifery of window
            QRectF leftRect(0,0,
                            (1-k)*Ww/2.0, Wh);
            QRectF rightRect(insideRect.topRight().x(),0,
                             (1-k)*Ww/2.0,Wh);
            QRectF topRect(insideRect.topLeft().x(), 0,
                           insideRect.width(),(1-k)*Wh/2.0);
            QRectF botRect(insideRect.bottomLeft().x(),
                           insideRect.bottomLeft().y(),
                           insideRect.width(),(1-k)*Wh/2.0);

            QModelIndex curvesIdx = _bookModel()->getIndex(rootIndex(),
                                                           "Curves","Plot");
            QRectF B = _bookModel()->calcCurvesBBox(curvesIdx);
            if ( B.topLeft().y() < B.bottomLeft().y() ) {
                // Flip if y-axis not directed "up" (this happens with bboxes)
                B = QRectF(B.bottomLeft(),B.topRight());
            }

            M = _mousePressMathRect;

            QEasingCurve ez(QEasingCurve::InOutQuad);

            double tx = wPt.x()/(viewport()->width());
            double ty = wPt.y()/(viewport()->height());
            double kx = ez.valueForProgress(qAbs(tx));
            double ky = ez.valueForProgress(qAbs(ty));

            if ( rightRect.contains(_mousePressPos) ) {
                if ( tx < 0 ) {
                    M.setRight(M.right()+kx*(B.right()-M.right()));
                }
                if ( ty > 0 ) {
                    M.setTop(M.top()+ky*(B.top()-M.top()));
                } else if ( ty < 0 ) {
                    M.setBottom(M.bottom()+ky*(B.bottom()-M.bottom()));
                }
            } else if ( leftRect.contains(_mousePressPos) ) {
                if ( tx > 0 ) {
                    M.setLeft(M.left()-kx*(M.left()-B.left()));
                }
                if ( ty > 0 ) {
                    M.setTop(M.top()+ky*(B.top()-M.top()));
                } else if ( ty < 0 ) {
                    M.setBottom(M.bottom()+ky*(B.bottom()-M.bottom()));
                }
            } else if ( topRect.contains(_mousePressPos) ) {
                if ( ty > 0 ) {
                    M.setTop(M.top()+ky*(B.top()-M.top()));
                }
                if ( tx < 0 ) {
                    M.setRight(M.right()+kx*(B.right()-M.right()));
                } else if ( tx > 0 ) {
                    M.setLeft(M.left()-kx*(M.left()-B.left()));
                }
            } else if ( botRect.contains(_mousePressPos) ) {
                if ( ty < 0 ) {
                    M.setBottom(M.bottom()+ky*(B.bottom()-M.bottom()));
                }
                if ( tx < 0 ) {
                    M.setRight(M.right()+kx*(B.right()-M.right()));
                } else if ( tx > 0 ) {
                    M.setLeft(M.left()-kx*(M.left()-B.left()));
                }
            } else {
                // shouldn't happen, but ignore in any case
            }

            _bookModel()->setPlotMathRect(M,rootIndex());
            viewport()->update();
        }
    } else {
        mouseMove->ignore();
    }
}

void CurvesView::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_Period: _keyPressPeriod();break;
    case Qt::Key_Space: _keyPressSpace();break;
    case Qt::Key_Up: _keyPressUp();break;
    case Qt::Key_Down: _keyPressDown();break;
    case Qt::Key_Left: _keyPressArrow(Qt::LeftArrow);break;
    case Qt::Key_Right: _keyPressArrow(Qt::RightArrow);break;
    case Qt::Key_Comma: _keyPressComma();break;
    default: ; // do nothing
    }
}

void CurvesView::currentChanged(const QModelIndex &current,
                                const QModelIndex &previous)
{
    Q_UNUSED(current);
    Q_UNUSED(previous);
    viewport()->update();
}

void CurvesView::resizeEvent(QResizeEvent *event)
{
    QRectF M = _plotMathRect(rootIndex());

    if ( M.size().width() > 0 && M.size().height() != 0 ) {
        if ( _pixmap ) {
            delete _pixmap;
        }
        _pixmap = _createLivePixmap();
    }
    QAbstractItemView::resizeEvent(event);
}

// For two curves hitting the spacebar will toggle between viewing
// the two curves in error, compare and error+compare views
void CurvesView::_keyPressSpace()
{
    QModelIndex curvesIdx = _bookModel()->getIndex(rootIndex(),"Curves","Plot");
    int rc = model()->rowCount(curvesIdx);
    if ( rc != 2 ) return;

    QString plotPresentation = _bookModel()->getDataString(rootIndex(),
                                                   "PlotPresentation","Plot");

    if ( plotPresentation == "error" || plotPresentation.isEmpty() ) {
        plotPresentation = "compare";
    } else if ( plotPresentation == "compare" ) {
        plotPresentation = "error+compare";
    } else if ( plotPresentation == "error+compare" ) {
        plotPresentation = "error";
    } else {
        fprintf(stderr,"koviz [bad scoobs]: _keyPressSpace() : "
                       "plotPresentation=\"%s\"\n",
                       plotPresentation.toLatin1().constData());
        exit(-1);
    }

    // Make units the same (in the book model)
    QModelIndex idx0 = model()->index(0,0,curvesIdx);
    QModelIndex idx1 = model()->index(1,0,curvesIdx);
    QString dpYUnit0 = _bookModel()->getDataString(idx0,"CurveYUnit","Curve");
    QString dpYUnit1 = _bookModel()->getDataString(idx1,"CurveYUnit","Curve");
    CurveModel* c0 = _bookModel()->getCurveModel(curvesIdx,0);
    if ( c0->y()->unit() != dpYUnit0 ) {
        if ( dpYUnit0.isEmpty() || dpYUnit0 == "--" ) {
            QModelIndex unitIdx0 = _bookModel()->getDataIndex(idx0,
                                                         "CurveYUnit", "Curve");
            model()->setData(unitIdx0,c0->y()->unit());
            dpYUnit0 = c0->y()->unit();
        }
    }
    if ( dpYUnit0 != dpYUnit1 && !dpYUnit0.isEmpty() && !dpYUnit1.isEmpty() ) {
        // Make dp units the same (if in same family)
        Unit u0(dpYUnit0.toLatin1().constData());
        Unit u1(dpYUnit1.toLatin1().constData());
        if ( u0.canConvert(u1) ) {
            QModelIndex unitIdx1 = _bookModel()->getDataIndex(idx1,
                                                         "CurveYUnit", "Curve");
            model()->setData(unitIdx1,dpYUnit0);
        }
    }


    // Set presentation
    QModelIndex plotPresentationIdx = _bookModel()->getDataIndex(rootIndex(),
                                                   "PlotPresentation","Plot");
    model()->setData(plotPresentationIdx,plotPresentation);

    // Recalculate plot math rect to contain curves in toggled presentation
    QRectF bbox = _bookModel()->calcCurvesBBox(curvesIdx);
    QRectF plotMathRect = _plotMathRect(rootIndex());
    double x0 = plotMathRect.left();
    double y0 = bbox.top();
    double w = plotMathRect.width();
    double h = bbox.height();
    QRectF R(x0,y0,w,h);
    _bookModel()->setPlotMathRect(R,rootIndex());

    viewport()->update();
}

void CurvesView::_keyPressUp()
{
    if ( _bookModel()->isIndex(currentIndex(),"Curve") ) {
        QModelIndex curveIdx = currentIndex();
        QModelIndex curvesIdx = curveIdx.parent();
        int i = curveIdx.row();
        if ( i > 0 ) {
            QModelIndex nextCurveIdx = model()->index(i-1,0,curvesIdx);
            setCurrentIndex(nextCurveIdx);
        }
    }
}

void CurvesView::_keyPressDown()
{
    if ( _bookModel()->isIndex(currentIndex(),"Curve") ) {
        QModelIndex curveIdx = currentIndex();
        QModelIndex curvesIdx = curveIdx.parent();
        int rc = model()->rowCount(curvesIdx);
        int i = curveIdx.row();
        if ( i+1 < rc ) {
            QModelIndex nextCurveIdx = model()->index(i+1,0,curvesIdx);
            setCurrentIndex(nextCurveIdx);
        }
    }
}

void CurvesView::_keyPressComma()
{
    if ( !currentIndex().isValid() ) return;

    if ( currentIndex().column() != 0 ) return;

    // Get presentation
    QString tag = _bookModel()->data(currentIndex()).toString();

    // Get live time
    QModelIndex liveIdx = _bookModel()->getDataIndex(QModelIndex(),
                                                     "LiveCoordTime");
    double liveTime = model()->data(liveIdx).toDouble();

    // Add (or remove) marker based on presentation
    if ( tag == "Curve" || tag == "Plot" ) {
        bool isRemoved = false;
        foreach (TimeAndIndex* marker, _markers ) {
            if ( liveTime == marker->time() &&
                 marker->idx().row() == currentIndex().row() ) {
                // Remove marker if same time and same curve (eg toggle off/on)
                _markers.removeOne(marker);
                delete marker;
                isRemoved = true;
                break;
            }
        }

        if ( !isRemoved ) {
            QModelIndex idx = currentIndex();
            TimeAndIndex* timeAndIdx = new TimeAndIndex(liveTime,idx);
            _markers << timeAndIdx;
        }
    }
}

TimeAndIndex::TimeAndIndex(double time, const QModelIndex &idx) :
    _time(time),
    _idx(idx)
{
}

double TimeAndIndex::time() const
{
    return _time;
}

QModelIndex TimeAndIndex::idx() const
{
    return _idx;
}
