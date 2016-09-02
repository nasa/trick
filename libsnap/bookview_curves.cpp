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

CoordArrow::CoordArrow(const QString &txt, const QPointF &coord,
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
        qDebug() << "snap [bad scoobs]: CoorArrow::boundingBox(): "
                    "arrow angle <=0,==90,==270 or >=360. "
                    "May want to support it.  Bailing!!!";
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
        qDebug() << "snap [bad scoobs]: CoorArrow::txtBBox(): "
                    "arrow angle <=0,==90,==270 or >=360. "
                    "May want to support it.  Bailing!!!";
        exit(-1);
    }

    bbox.setTopLeft(tl);
    bbox.setSize(QSize(tw,th));

    return bbox;
}

void CoordArrow::paintMe(QPainter &painter, const QTransform &T) const
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
        qDebug() << "snap [bad scoobs]: CoorArrow::paintMe(): "
                    "arrow angle <=0,==90,==270 or >=360. "
                    "May want to support it.  Bailing!!!";
        exit(-1);
    }

    // Draw circle around point
    painter.drawEllipse(pt,qRound(r),qRound(r));

    // Draw arrow head (tip on circle, not on point)
    QPolygonF arrowHead(ptsArrowHead);
    QBrush brush(painter.pen().color());
    painter.setBrush(brush);
    painter.drawConvexPolygon(arrowHead);

    // Draw arrow tail (attached to triangle)
    QPolygonF polyLine(ptsArrowTail);
    painter.drawPolyline(polyLine);

    // Draw background for text box (plain white for now)
    painter.setPen(QPen(Qt::white));
    painter.setBrush(QBrush(Qt::white));
    painter.drawRect(txtBox);
    painter.setBrush(origBrush);
    painter.setPen(origPen);

    // Draw coord text i.e. (x,y)
    painter.drawText(txtBox,Qt::AlignCenter,txt);

    // Restore painter
    painter.setPen(origPen);
    painter.setBrush(origBrush);
    painter.restore();
}


CurvesView::CurvesView(QWidget *parent) :
    BookIdxView(parent),
    _errorPath(0)
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
    foreach ( QPainterPath* path, _curve2path.values() ) {
        delete path;
    }
    _curve2path.clear();

    if ( _errorPath ) {
        delete _errorPath;
        _errorPath = 0;
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
    for (int i = 0 ; i < rc; ++i ) {
        QModelIndex idx = model()->index(i,0,curvesIdx);
        if ( model()->data(idx).toString() == "Curve" ) {
            int curveRunID = _bookModel()->getDataInt(idx,"CurveRunID","Curve");
            if ( curveRunID == runID ) {
                setCurrentIndex(idx);
                break;
            }
        }
    }
}

void CurvesView::paintEvent(QPaintEvent *event)
{
    if ( !model() ) return;

    QModelIndex curvesIdx = _bookModel()->getIndex(rootIndex(),"Curves","Plot");
    int nCurves = model()->rowCount(curvesIdx);

    QPainter painter(viewport());
    painter.save();

    painter.setRenderHint(QPainter::Antialiasing);

    QTransform T = _coordToPixelTransform();
    double ptSizeCurve = _curvePointSize();
    QPen pen;
    pen.setWidthF(ptSizeCurve);
    painter.setPen(pen);

    // Draw!
    painter.setTransform(T);
    if ( nCurves == 2 ) {
        QString plotPresentation = _bookModel()->getDataString(rootIndex(),
                                                     "PlotPresentation","Plot");
        if ( plotPresentation == "coplot" ) {
            _paintCoplot(T,painter,pen);
        } else if (plotPresentation == "error" || plotPresentation.isEmpty()) {
            _paintErrorplot(painter,pen,_errorPath,rootIndex());
        } else if ( plotPresentation == "error+coplot" ) {
            _paintErrorplot(painter,pen,_errorPath,rootIndex());
            _paintCoplot(T,painter,pen);
        } else {
            qDebug() << "snap [bad scoobs]: paintEvent() : PlotPresentation="
                     << plotPresentation << "not recognized.";
            exit(-1);
        }
    } else {
        _paintCoplot(T,painter,pen);
    }

#if 0
    // TODO

    // Draw symbol on each curve point
    /* if  ( _scatterShape != QCPScatterStyle::ssNone ) { QCPScatterStyle s; s.setShape(_scatterShape);
        s.setSize(_scatterSize);
        QTransform t = _coordToPixelTransform();
        for ( int i = 0; i < _painterPath.elementCount(); ++i ) {
            QPainterPath::Element el = _painterPath.elementAt(i);
            QPointF p(el.x,el.y);
            p = t.map(p);
            s.drawShape(painter,p);
        }
    }
    */

    // Draw the curve
    painter->setTransform(_coordToPixelTransform());
    painter->drawPath(_painterPath);
#endif

    // Restore the painter state off the painter stack
    painter.restore();
}


void CurvesView::_paintCoplot(const QTransform &T,QPainter &painter,QPen &pen)
{
    QModelIndex clickedCurveIdx;
    QModelIndex curvesIdx = _bookModel()->getIndex(rootIndex(),"Curves","Plot");
    int rc = model()->rowCount(curvesIdx);
    for ( int i = 0; i < rc; ++i ) {
        QModelIndex curveIdx = model()->index(i,0,curvesIdx);
        if ( curveIdx != currentIndex() ) {
            _paintCurve(curveIdx,T,painter,pen);
        } else {
            clickedCurveIdx = curveIdx;
        }
    }
    if ( clickedCurveIdx.isValid() ) {
        // Paint clicked curve on top of other curves so it shows
        _paintCurve(clickedCurveIdx,T,painter,pen);
    }
}

void CurvesView::_paintCurve(const QModelIndex& curveIdx,
                             const QTransform& T,
                             QPainter& painter, QPen& pen)
{
    painter.save();

    QModelIndex curveDataIdx = _bookModel()->getDataIndex(curveIdx,
                                                          "CurveData","Curve");
    QVariant v = model()->data(curveDataIdx);
    TrickCurveModel* curveModel =QVariantToPtr<TrickCurveModel>::convert(v);

    if ( curveModel ) {

        // Color curves
        QColor color( _bookModel()->getDataString(curveIdx,
                                                  "CurveColor","Curve"));
        if ( curveIdx == currentIndex() ) {
            color = color.darker(200);
        } else if ( currentIndex().isValid() ) {
            color = color.lighter(180);
        }
        pen.setColor(color);
        painter.setPen(pen);

        // Get painter path
        QPainterPath* path = _curve2path.value(curveModel);

        // Scale transform (e.g. for unit axis scaling)
        double xs = _xScale(curveModel,curveIdx);
        double ys = _yScale(curveModel,curveIdx);
        QTransform Tscaled(T);
        Tscaled = Tscaled.scale(xs,ys);
        painter.setTransform(Tscaled);

        // Draw curve!
        painter.drawPath(*path);

        // Draw coordinate arrow (arrow with (x,y) label) if needed
        QRectF M = _plotMathRect(rootIndex());
        if ( curveIdx == currentIndex() &&
             M.width() > 0 && qAbs(M.height()) > 0 ) {
            _paintLiveCoordArrow(curveModel,curveIdx,painter);
        }
    }
    painter.restore();
}

void CurvesView::_paintLiveCoordArrow(TrickCurveModel* curveModel,
                                      const QModelIndex& curveIdx,
                                      QPainter& painter)
{
    painter.save();
    curveModel->map();

    // Draw in window coords (+y axis down which accounts for -r*sin(ang) etc)
    QTransform I;
    painter.setTransform(I);

    // Map math coord to window pt
    QTransform T = _coordToPixelTransform();

    // Initial arrow

    // Calculate liveCoord based on model liveCoordTime
    QModelIndex liveIdx = _bookModel()->getDataIndex(QModelIndex(),
                                                     "LiveCoordTime");
    double liveTime = model()->data(liveIdx).toDouble();
    int i = curveModel->indexAtTime(liveTime);
    TrickModelIterator it = curveModel->begin();
    double xs = _xScale(curveModel,curveIdx);
    double ys = _yScale(curveModel,curveIdx);
    QPointF coord(it[i].x()*xs, it[i].y()*ys);

    // Init arrow struct
    CoordArrow arrow;
    arrow.coord = coord;

    //
    // If live coord is an extremum, set flag for painting
    //
    int rc = curveModel->rowCount();
    QString s;
    if ( i > 0 && i < rc-1) {
        // First and last point not considered
        double ya = it[i-1].y()*ys;
        double y  = it[i].y()*ys;
        double yb = it[i+1].y()*ys;
        if ( (y>ya && y>yb) || (y<ya && y<yb) ) {
            arrow.txt = s.sprintf("<%g, %g>", coord.x(),coord.y());
        } else {
            arrow.txt = s.sprintf("(%g, %g)", coord.x(),coord.y());
        }
    } else if ( i == 0 ) {
            arrow.txt = s.sprintf("init=(%g, %g)", coord.x(),coord.y());
    } else if ( i == rc-1 ) {
            arrow.txt = s.sprintf("last=(%g, %g)", coord.x(),coord.y());
    } else {
        qDebug() << "snap [bad scoobs]: CurvesView::_paintLiveCoordArrow";
        exit(-1);
    }

    // Try to fit arrow into viewport using 45,135,225 and 335 degree angles
    // off of horiz (counterclockwise)
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
        /*
        if ( _lastArrow ==  ) {
        } else {
        }
        */
        arrow.paintMe(painter,T);
    }

    curveModel->unmap();
    painter.restore();
}

void CurvesView::_paintErrorplot(QPainter &painter, const QPen &pen,
                                 QPainterPath* errorPath,
                                 const QModelIndex& plotIdx)
{
    QModelIndex curvesIdx = _bookModel()->getIndex(plotIdx,"Curves","Plot");
    int rc = model()->rowCount(curvesIdx);
    if ( rc != 2 ) {
        qDebug() << "snap [bad scoobs]: BookView::_printErrorplot()";
        exit(-1);
    }

    QModelIndex i0 = model()->index(0,0,curvesIdx);
    QModelIndex i1 = model()->index(1,0,curvesIdx);

    QString curveXName0 = _bookModel()->getDataString(i0,"CurveXName","Curve");
    QString curveXUnit0 = _bookModel()->getDataString(i0,"CurveXName","Curve");
    QString curveYName0 = _bookModel()->getDataString(i0,"CurveYName","Curve");
    QString curveYUnit0 = _bookModel()->getDataString(i0,"CurveYName","Curve");

    QString curveXName1 = _bookModel()->getDataString(i1,"CurveXName","Curve");
    QString curveXUnit1 = _bookModel()->getDataString(i1,"CurveXName","Curve");
    QString curveYName1 = _bookModel()->getDataString(i1,"CurveYName","Curve");
    QString curveYUnit1 = _bookModel()->getDataString(i1,"CurveYName","Curve");


    // TODO: indicate if error curve is flat,
    // TODO: need to put name of time in model
    //       so user has settable timename (not hardcoded "sys.exec.out.time")
    // TODO: error plot when x is not time e.g. x/y position
    //       d=sqrt((x0-x1)*(x0-x1)+(y0-y1)*(y0-y1))
    QString timeName("sys.exec.out.time");
    if ( curveXName0 == timeName &&
         curveXName0 == curveXName1 && curveXUnit0 == curveXUnit1 &&
         curveYName0 == curveYName1 && curveYUnit0 == curveYUnit1 ) {
        //
        // Normal case: x and y params/units match and x is time
        //

        // Magenta for difference/error curve
        QPen magentaPen(pen);
        magentaPen.setColor(_bookModel()->createCurveColors(3).at(2));
        painter.setPen(magentaPen);

        // Draw error curve!
        painter.drawPath(*errorPath);

    } else {
        // TODO:!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        qDebug() << "snap [todo]: Handle error plot when name/units different "
                    "or x is not time.";
        exit(-1);
    }
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
    QModelIndex topLeft_ppp = topLeft.parent().parent().parent();

    if ( tag == "CurveData" && topLeft_ppp == rootIndex() ) {
        QVariant v = model()->data(topLeft);
        TrickCurveModel* curveModel =QVariantToPtr<TrickCurveModel>::convert(v);
        if ( curveModel ) {
            QModelIndex curveIdx = topLeft.parent();
            _updateAxisLabelUnits(curveIdx,'x');
            _updateAxisLabelUnits(curveIdx,'y');
            if ( _curve2path.contains(curveModel) ) {
                delete _curve2path.value(curveModel);
            }
            QPainterPath* path = _createPainterPath(curveModel);
            _curve2path.insert(curveModel,path);

            QModelIndex curvesIdx = topLeft.parent().parent();
            int nCurves = model()->rowCount(curvesIdx);
            if ( nCurves == 1 || nCurves >= 4 ) {
                _currBBox = _currBBox.united(
                            _curveBBox(curveModel,curveIdx));
            } else if ( nCurves == 2 ) {
                QModelIndex otherCurveIdx;
                if ( curveIdx.row() == 1 ) {
                    otherCurveIdx = model()->index(0,0,curvesIdx);
                    if ( _errorPath ) delete _errorPath;
                    _errorPath = _createErrorPath(otherCurveIdx,curveIdx);
                } else if ( curveIdx.row() == 0 ) {
                    otherCurveIdx = model()->index(1,0,curvesIdx);
                    if ( _errorPath ) delete _errorPath;
                    _errorPath = _createErrorPath(curveIdx,otherCurveIdx);
                } else {
                    qDebug() << "snap [bad scoobs]: "
                                "CurvesView::dataChanged(), nCurves==2";
                    exit(-1);
                }
                _currBBox = _errorPath->boundingRect();
            } else if ( nCurves == 3 ) {
                // not an error plot, must redo bbox using path 0,1,2
                _currBBox = _calcBBox();
            } else {
                qDebug() << "snap [bad scoobs]: CurvesView::rowsInserted";
                exit(-1);
            }
            _setPlotMathRect(_currBBox);
        }
    } else if ( tag == "CurveXUnit" && topLeft_ppp == rootIndex()) {
        QModelIndex curveIdx = topLeft.parent();
        _updateAxisLabelUnits(curveIdx,'x');
        QRectF prevBBox = _currBBox;
        _currBBox = _calcBBox();
        if ( prevBBox.width() > 0 ) {
            double xs = _currBBox.width()/prevBBox.width();
            QRectF M = _plotMathRect(rootIndex());
            double w = M.width();
            double h = M.height();
            QPointF topLeft(xs*M.topLeft().x(), M.topLeft().y());
            QRectF scaledM(topLeft,QSizeF(xs*w,h));
            _setPlotMathRect(scaledM);
        }
    } else if ( tag == "CurveYUnit" && topLeft_ppp == rootIndex()) {
        QModelIndex curveIdx = topLeft.parent();
        _updateAxisLabelUnits(curveIdx,'y');
        QRectF prevBBox = _currBBox;
        _currBBox = _calcBBox();
        if ( prevBBox.height() > 0 ) {
            double ys = _currBBox.height()/prevBBox.height();
            QRectF M = _plotMathRect(rootIndex());
            double w = M.width();
            double h = M.height();
            QPointF topLeft(M.topLeft().x(), ys*M.topLeft().y());
            QRectF scaledM(topLeft,QSizeF(w,ys*h));
            _setPlotMathRect(scaledM);
        }
    } else if ( tag == "PlotPresentation" && topLeft.parent() == rootIndex() ) {
        if ( _bookModel()->isChildIndex(rootIndex(),"Plot","Curves") ) {
            QModelIndex curvesIdx = _bookModel()->getIndex(rootIndex(),
                                                           "Curves","Plot");
            int nCurves = model()->rowCount(curvesIdx);
            if ( nCurves == 2 ) {
                QString presentation = model()->data(topLeft).toString();
                if ( presentation == "error" || presentation.isEmpty() ) {
                    _currBBox = _errorPath->boundingRect();
                } else if ( presentation == "coplot" ) {
                    _currBBox = _calcBBox();
                } else if ( presentation == "error+coplot" ) {
                    _currBBox = _calcBBox();
                    _currBBox = _currBBox.united(_errorPath->boundingRect());
                }
                _setPlotMathRect(_currBBox);
            } else {
                // ignore
            }
        }
    } else {
        // ignore (but still update below)
    }

    // Schedule paint()!!!
    viewport()->update();
}

void CurvesView::rowsInserted(const QModelIndex &pidx, int start, int end)
{
    if ( pidx.parent().parent() != rootIndex() ) return; // not my plot

    QModelIndex curvesIdx = pidx.parent();

    for ( int i = start; i <= end; ++i ) {
        QModelIndex curveIdx = model()->index(i,0,pidx);
        QModelIndex curveDataIdx = model()->index(i,1,pidx);
        QString name = model()->data(curveIdx).toString();
        if ( name == "CurveData" ) {
            QVariant v = model()->data(curveDataIdx);
            TrickCurveModel* curveModel =
                                     QVariantToPtr<TrickCurveModel>::convert(v);
            if ( curveModel ) {
                _updateAxisLabelUnits(curveIdx,'x');
                _updateAxisLabelUnits(curveIdx,'y');
                QPainterPath* path = _createPainterPath(curveModel);
                _curve2path.insert(curveModel,path);
                int nCurves = model()->rowCount(curvesIdx);
                if ( nCurves == 1 || nCurves >= 4 ) {
                    _currBBox = _currBBox.united(
                                              _curveBBox(curveModel,curveIdx));
                } else if ( nCurves == 2 ) {
                    QModelIndex curveIdx0 = model()->index(0,1,curvesIdx);
                    QModelIndex curveIdx1 = model()->index(1,1,curvesIdx);
                    if ( _errorPath ) delete _errorPath;
                    _errorPath = _createErrorPath(curveIdx0,curveIdx1);
                    _currBBox = _errorPath->boundingRect();
                } else if ( nCurves == 3 ) {
                    // not an error plot, must redo bbox using path 0,1,2
                    _currBBox.setTopLeft(QPointF(0,0));
                    _currBBox.setSize(QSizeF(0,0));
                    foreach ( QPainterPath* mypath, _curve2path.values() ) {
                        _currBBox = _currBBox.united(mypath->boundingRect());
                    }
                } else {
                    qDebug() << "snap [bad scoobs]: CurvesView::rowsInserted";
                    exit(-1);
                }
                _setPlotMathRect(_currBBox);
            }
            break;
        }
    }
}

QPainterPath* CurvesView::_createPainterPath(TrickCurveModel *curveModel)
{
#if 0
   QPainterPath path = _sinPath();
   //QPainterPath path = _stepPath();
    QPainterPath* ppath = new QPainterPath(path);
    return ppath;
#endif

    //_isPainterPathCreated = true;
    //_painterPath = QPainterPath();
    QPainterPath* path = new QPainterPath;

    curveModel->map();

    TrickModelIterator it = curveModel->begin();
    const TrickModelIterator e = curveModel->end();

    path->moveTo(it.x(),it.y());
    while (it != e) {
            path->lineTo(it.x(),it.y());
            ++it;
    }
    // TODO: start/stop time, scale and bias
#if 0
    if ( _startTime == -DBL_MAX && _stopTime == DBL_MAX ) {
        _painterPath.moveTo(it.x()*_xsf+_xbias,it.y()*_ysf+_ybias);
        while (it != e) {
            _painterPath.lineTo(it.x()*_xsf+_xbias,it.y()*_ysf+_ybias);
            ++it;
        }
    } else {
        while (it != e) {
            double t = it.t();
            if ( t >= _startTime ) {
                _painterPath.moveTo(it.x()*_xsf+_xbias,it.y()*_ysf+_ybias);
                break;
            }
            ++it;
        }
        while (it != e) {
            double t = it.t();
            if ( t <= _stopTime ) {
                _painterPath.lineTo(it.x()*_xsf+_xbias,it.y()*_ysf+_ybias);
            } else {
                break;
            }
            ++it;
        }
    }
#endif

    curveModel->unmap();

    return path;
}

// curveIdx0/1 are child indices of "Curves" with tagname "Curve"
//
// returned path is scaled
//
QPainterPath *CurvesView::_createErrorPath(const QModelIndex &curveIdx0,
                                            const QModelIndex &curveIdx1)
{
    QPainterPath* path = new QPainterPath;

    if ( !_bookModel()->isIndex(curveIdx0,"Curve") ) {
        qDebug() << "snap [bad scoobies]:1:BookIdxView::_createErrorPath():"
                    " curveIdx0's tag should be \"Curve\", instead "
                    "it is tag="
               << model()->data(curveIdx0).toString()
               << ".  curveIdx0=" << curveIdx0;
        exit(-1);
    }
    if ( !_bookModel()->isIndex(curveIdx1,"Curve") ) {
        qDebug() << "snap [bad scoobies]:2:BookIdxView::_createErrorPath():"
                    " curveIdx1's tag should be \"Curve\", instead "
                    "it is tag="
               << model()->data(curveIdx1).toString()
               << ".  curveIdx1=" << curveIdx1;
        exit(-1);
    }

    QModelIndex idx0 =_bookModel()->getDataIndex(curveIdx0,"CurveData","Curve");
    QModelIndex idx1 =_bookModel()->getDataIndex(curveIdx1,"CurveData","Curve");

    QVariant v0 = model()->data(idx0);
    TrickCurveModel* c0 = QVariantToPtr<TrickCurveModel>::convert(v0);

    QVariant v1 = model()->data(idx1);
    TrickCurveModel* c1 = QVariantToPtr<TrickCurveModel>::convert(v1);

    if ( c0 == 0 || c1 == 0 ) {
        qDebug() << "snap [bad scoobies]:3: BookIdxView::_createErrorPath().  "
                 << "null curveModel!!!! "
                 << "curveDataIdx0=" << curveIdx0
                 << "curveDataIdx1=" << curveIdx1
                 << "curveModel0="   << (void*) c0
                 << "curveModel1="   << (void*) c1;
    }

    double k0 = _bookModel()->getDataDouble(curveIdx0,"CurveYScale","Curve");
    double k1 = _bookModel()->getDataDouble(curveIdx1,"CurveYScale","Curve");
    double ys0 = _yScale(c0,curveIdx0);
    double ys1 = (k1/k0)*_yScale(c1,curveIdx0); // curveIdx0 for same unit
                                                // k1/k0 to correct book ysf

    if ( c0 != 0 && c1 != 0 ) {

        c0->map();
        c1->map();

        TrickModelIterator i0 = c0->begin();
        TrickModelIterator i1 = c1->begin();
        const TrickModelIterator e0 = c0->end();
        const TrickModelIterator e1 = c1->end();
        bool isFirst = true;
        while (i0 != e0 && i1 != e1) {
            double t0 = i0.t();
            double t1 = i1.t();
            if ( qAbs(t1-t0) < 0.000001 ) {
                double d = ys0*i0.y() - ys1*i1.y();
                if ( isFirst ) {
                    path->moveTo(t0,d);
                    isFirst = false;
                } else {
                    path->lineTo(t0,d);
                }
                ++i0;
                ++i1;
            } else {
                if ( t0 < t1 ) {
                    ++i0;
                } else if ( t1 < t0 ) {
                    ++i1;
                } else {
                    qDebug() << "snap [bad scoobs]:4:_createErrorPath()";
                    exit(-1);
                }
            }
        }

        c0->unmap();
        c1->unmap();

    } else {
    }

    return path;
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
                   << green << orange << yellow
                   << gray << pink << medblue;

        for ( int i = 0 ; i < nBands-10; ++i ) {
            colorBands.removeLast();
        }
    }

    return colorBands;
}

void CurvesView::_updateAxisLabelUnits(const QModelIndex &curveIdx,
                                       QChar axis) const
{
    QModelIndex axisLabelIdx;
    QModelIndex curveUnitIdx;
    if ( axis == 'x' ) {
        axisLabelIdx = _bookModel()->getDataIndex(rootIndex(),
                                                  "PlotXAxisLabel", "Plot");
        curveUnitIdx = _bookModel()->getDataIndex(curveIdx,
                                                  "CurveXUnit","Curve");
    } else if ( axis == 'y' ) {
        axisLabelIdx = _bookModel()->getDataIndex(rootIndex(),
                                                  "PlotYAxisLabel", "Plot");
        curveUnitIdx = _bookModel()->getDataIndex(curveIdx,
                                                  "CurveYUnit","Curve");
    } else {
        qDebug() << "snap [bad scoobs]: CurvesView::_updateUnits() called "
                    "with bad axis=" << axis;
        exit(-1);
    }

    // Extract what is inside curly brackets in Plot[XY]AxisLabel
    // e.g. if PlotXAxisLabel == "Time {s}" return "s"
    QString axisLabelUnit;
    QString axisLabel = model()->data(axisLabelIdx).toString();
    int openCurly = axisLabel.indexOf('{');
    if ( openCurly >= 0 ) {
        int closeCurly = axisLabel.indexOf('}');
        if ( closeCurly > openCurly+1 ) {
            // e.g. for "Time {s}" ->  openCurly=5 closeCurly=7
            axisLabelUnit = axisLabel.mid(openCurly+1,closeCurly-openCurly-1);
        }
    }

    // Get unit in the book model with tag Curve[XY]Unit
    QString curveUnit = model()->data(curveUnitIdx).toString();

    if ( curveUnit != axisLabelUnit ) {
        // Replace axis label unit in curlies
        // This check speeds things up with monte carlo when
        // lots of curves with same unit
        int openCurly = axisLabel.indexOf('{');
        if ( openCurly >= 0 ) {
            int closeCurly = axisLabel.indexOf('}');
            int n = closeCurly-openCurly-1;
            axisLabel.replace(openCurly+1,n,_curvesUnit(rootIndex(),axis));
        } else {
            axisLabel += " {" + _curvesUnit(rootIndex(),axis) + "}";
        }
        model()->setData(axisLabelIdx,axisLabel);
    }
}

void CurvesView::mousePressEvent(QMouseEvent *event)
{
    if (  event->button() == Qt::LeftButton ) {
        _mousePressPos = event->pos();
        _mousePressMathTopLeft = _mathRect().topLeft();
        _mousePressMathRect = _mathRect();
        _mousePressCurrentIndex = currentIndex();
        event->ignore();
    } else if (  event->button() == Qt::MidButton ) {
        event->ignore();
    } else if ( event->button() == Qt::RightButton ) {
        _setPlotMathRect(_currBBox);
        viewport()->update();
    }
}

void CurvesView::mouseReleaseEvent(QMouseEvent *event)
{
    if (  event->button() == Qt::LeftButton ) {
        double x0 = _mousePressPos.x();
        double y0 = _mousePressPos.y();
        double x1 = event->pos().x();
        double y1 = event->pos().y();
        double d = qSqrt((x1-x0)*(x1-x0)+(y1-y0)*(y1-y0));
        QString presentation = _bookModel()->getDataString(rootIndex(),
                                                   "PlotPresentation","Plot");
        if ( d < 10 && (presentation == "coplot" || presentation.isEmpty()) ) {
            // d < 10, to hopefully catch click and not a drag
            int s = 24; // side length for rect around mouse click
            QList<QModelIndex> curveIdxs;

            while ( 1 ) {

                curveIdxs = _curvesInsideMouseRect(QRectF(x1-s/2,y1-s/2,s,s));

                if ( curveIdxs.size() == 1 ) {
                    // Single curve found in small box around mouse click!
                    QModelIndex curveIdx = curveIdxs.first();
                    selectionModel()->setCurrentIndex(curveIdx,
                                                 QItemSelectionModel::NoUpdate);
                    break;
                } else if ( curveIdxs.isEmpty() &&  currentIndex().isValid() ) {
                    // click off curves, current unset
                    setCurrentIndex(QModelIndex());
                    break;
                } else if ( curveIdxs.isEmpty() && !currentIndex().isValid() ) {
                    // click off, toggle between single and multiplot views
                    // pass event to ancestors (page view handles toggle)
                    event->ignore();
                    break;
                } else if ( curveIdxs.size() > 1 ) {
                    // Multiple curves found in mouse rect, refine search
                    s /= 2;
                    if ( s < 2 ) {
                        // Choose closest curve to point and bail
                        selectionModel()->setCurrentIndex(curveIdxs.first(),
                                                          QItemSelectionModel::NoUpdate);
                        break;
                    }
                } else {
                    qDebug() << "snap [bad scoobs]: "
                                "CurvesView::mouseReleaseEvent()";
                }
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


QList<QModelIndex> CurvesView::_curvesInsideMouseRect(const QRectF& R)
{
    QList<QModelIndex> curveIdxs;

    QRectF  W = viewport()->rect();
    QRectF M = _mathRect();
    double a = M.width()/W.width();
    double b = M.height()/W.height();
    double c = M.x() - a*W.x();
    double d = M.y() - b*W.y();
    QTransform T( a,    0,
                  0,    b, /*+*/ c,    d);
    QRectF mathClickRect = T.mapRect(R);
    QModelIndex curvesIdx = _bookModel()->getIndex(rootIndex(),"Curves","Plot");
    int nCurves = model()->rowCount(curvesIdx);

    double t0 = mathClickRect.left();
    double t1 = mathClickRect.right();

    double dMin = DBL_MAX;

    for (int i = 0; i < nCurves; ++i) {

        QModelIndex curveIdx = model()->index(i,0,curvesIdx);
        TrickCurveModel* curveModel =_bookModel()->getTrickCurveModel(curveIdx);

        if ( !curveModel ) continue;

        curveModel->map();
        TrickModelIterator it = curveModel->begin();

        QString xName = _bookModel()->getDataString(curveIdx,
                                                    "CurveXName","Curve");
        bool isXTime = false;
        if ( xName == "sys.exec.out.time" ) { // TODO: Time name
            isXTime = true;
            int i0 = curveModel->indexAtTime(t0);
            if ( i0 > 0 ) {
                --i0; // start one step back
            }
            it = it[i0];
        }

        double xs = _xScale(curveModel,curveIdx);
        double ys = _yScale(curveModel,curveIdx);
        QPointF q(xs*it.x(),ys*it.y());
        ++it;
        TrickModelIterator e = curveModel->end();
        while ( it != e ) {
            double t = it.t();
            QPointF p(xs*it.x(),ys*it.y());
            QLineF qp(q,p);
            QLineF ll(mathClickRect.topLeft(), mathClickRect.bottomLeft());
            QLineF rr(mathClickRect.topRight(), mathClickRect.bottomRight());
            QLineF tt(mathClickRect.topLeft(), mathClickRect.topRight());
            QLineF bb(mathClickRect.bottomLeft(), mathClickRect.bottomRight());
            QList<QLineF> lines;
            lines << ll << rr << tt << bb;
            QPointF pt;
            QLineF::IntersectType itype;
            bool isIntersects = false;
            foreach ( QLineF line, lines ) {
                itype = qp.intersect(line,&pt);
                if ( itype == QLineF::BoundedIntersection ) {
                    isIntersects = true;

                    // Calc distance from mouse click point to curve line seg
                    double d;
                    QLineF pq(p,q);
                    QLineF qc(q,R.center());
                    QLineF pc(p,R.center());
                    double alpha = qp.angleTo(qc);
                    double beta  = pq.angleTo(pc) ;
                    double a = qp.length();
                    double b = qc.length();
                    if ( alpha >= 90.0 ) {
                        d = b;
                    } else if ( beta >= 90.0 ) {
                        d = a;
                    } else {
                        d = b*sin(alpha*M_PI/180.0);
                    }
                    if ( d < dMin ) {
                        // Curve with min distance will be first
                        dMin = d;
                        curveIdxs.prepend(curveIdx);
                    } else {
                        curveIdxs << curveIdx;
                    }

                    break;
                }
            }
            if ( isIntersects || (isXTime && t > t1) ) {
                break;
            }
            q = p;
            ++it;
        }

        curveModel->unmap();
    }

    return curveIdxs;
}

void CurvesView::mouseMoveEvent(QMouseEvent *mouseMove)
{
    QRectF W = viewport()->rect();
    if ( W.width() < 1 || W.height() < 1 ) return;

    double Ww = W.width();  // greater > 0, by top line
    double Wh = W.height();

    QRectF M = _mathRect();
    double Mw = M.width();
    double Mh = M.height();

    if ( mouseMove->buttons() == Qt::NoButton && currentIndex().isValid() ) {

        QString tag = model()->data(currentIndex()).toString();
        QString presentation = _bookModel()->getDataString(rootIndex(),
                                                   "PlotPresentation","Plot");
        if ( tag == "Curve" &&
             (presentation == "coplot" || presentation.isEmpty()) ) {

            double a = M.width()/W.width();
            double b = M.height()/W.height();
            double c = M.x() - a*W.x();
            double d = M.y() - b*W.y();
            QTransform T(a, 0,
                         0, b, /*+*/ c, d);
            QPointF wPt = mouseMove->pos();
            QPointF mPt = T.map(wPt);

            TrickCurveModel* curveModel = _bookModel()->
                                             getTrickCurveModel(currentIndex());
            if ( curveModel ) {

                curveModel->map();
                TrickModelIterator it = curveModel->begin();
                double xs = _xScale(curveModel,currentIndex());
                double ys = _yScale(curveModel,currentIndex());
                int rc = curveModel->rowCount() ;
                QModelIndex liveTimeIdx = _bookModel()->getDataIndex(
                                                               QModelIndex(),
                                                               "LiveCoordTime");

                // TODO: do not use hard-coded time name
                if ( curveModel->x()->name() == "sys.exec.out.time" ) {

                    QPointF liveCoord(DBL_MAX,DBL_MAX);

                    if ( rc == 0 ) {

                        // "null" out liveCoord
                        liveCoord = QPointF(DBL_MAX,DBL_MAX);

                    } else if ( rc == 1 ) {

                        liveCoord = QPointF(it.x()*xs,it.y()*ys);

                    } else if ( rc == 2 ) {
                        // TODO: Test curve with 2 points
                        QPointF p0(it[0].x()*xs,it[0].y()*ys);
                        QPointF p1(it[1].x()*xs,it[1].y()*ys);
                        QLineF l0(p0,mPt);
                        QLineF l1(p1,mPt);
                        if ( l0.length() < l1.length() ) {
                            liveCoord = p0;
                        } else {
                            liveCoord = p1;
                        }

                    } else if ( rc >= 3 ) {

                        int i = curveModel->indexAtTime(mPt.x());
                        QPointF p(it[i].x()*xs,it[i].y()*ys);

                        //
                        // Find dt between p and prev or next point
                        //
                        double dt = 0.0;
                        if ( i == 0 ) {
                            // Start point
                            dt = it[1].x()*xs - p.x();
                        } else if ( i > 0 ) {
                            dt = p.x() - it[i-1].x()*xs;
                        } else {
                            qDebug() << "snap [bad scoobs]:1: "
                                        "CurvesView::mouseMoveEvent() ";
                            exit(-1);
                        }
                        if ( dt < 1.0e-9 ) {
                            qDebug() << "snap [bad scoobs]:2: "
                                        "CurvesView::mouseMoveEvent() " << i;
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
                            QPointF pt(it[m].x()*xs,it[m].y()*ys);
                            if ( m > 0 && m < k ) {
                                double ya = it[m-1].y()*ys;
                                double y  = it[m].y()*ys;
                                double yb = it[m+1].y()*ys;
                                if ( y > ya && y > yb ) {
                                    if ( localMaxs.isEmpty() ) {
                                        localMaxs << pt;
                                    } else {
                                        if ( pt.y() > localMaxs.first().y() ) {
                                            localMaxs.prepend(pt);
                                        } else {
                                            localMaxs << pt;
                                        }
                                    }
                                } else if ( y < ya && y < yb ) {
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
                            liveCoord = QPointF(it[0].x()*xs,it[0].y()*ys);
                        } else if ( k == rc-1 ) {
                            // Mouse near end of curve, set to last pt
                            liveCoord = QPointF(it[k].x()*xs,it[k].y()*ys);
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
                                qDebug() << "snap [bad scoobs]:3: CurvesView::"
                                            "mouseMoveEvent()";
                                exit(-1);
                            }
                        }
                    }

                    // Set live coord in model
                    model()->setData(liveTimeIdx,liveCoord.x());

                } else {  // Curve x is not time e.g. ball xy-position

                    double liveTime = DBL_MAX;
                    double dMin = DBL_MAX;
                    TrickModelIterator e = curveModel->end();
                    while ( it != e ) { // find closest point on curve to mouse
                        QPointF p(xs*it.x(),ys*it.y());
                        double d = QLineF(mPt,p).length();
                        if ( d < dMin ) {
                            dMin = d;
                            liveTime = it.t();
                        }
                        ++it;
                    }
                    // Set live coord in model
                    model()->setData(liveTimeIdx,liveTime);
                }

                curveModel->unmap();
                viewport()->update();
            }
        }

        } else if ( mouseMove->buttons() == Qt::LeftButton ) {

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
            QRectF bbox = _currBBox;
            if ( rightRect.contains(_mousePressPos) ) {
                //
                // f(0) = w0
                // f(x) = w0*e^(ax) // scrunch on an exponential
                // a = (1/W)*log(w1/w0), because:
                //
                //        f(W) = w0*e^(aW) = w1
                //           -> w0*e^(aW) = w1
                //           -> e^(aW) = w1/w0
                //           -> a = (1/W)*log(w1/w0)
                //
                QRectF Mstart = _mousePressMathRect;
                QRectF Mend(Mstart);
                Mend.setRight(bbox.right());
                double w0 = Mstart.width();
                double w1 = Mend.width();
                double a = (1.0/Ww)*log(w0/w1);
                double w = w0*exp(a*wPt.x());
                if ( w > w1 ) w = w1;
                M.setWidth(w);
            } else if ( leftRect.contains(_mousePressPos) ) {
                QRectF Mstart = _mousePressMathRect;
                QRectF Mend(Mstart);
                Mend.setLeft(bbox.left());
                double w0 = Mstart.width();
                double w1 = Mend.width();
                double a = (1.0/Ww)*log(w0/w1);
                double w = w0*exp(a*(-1.0)*wPt.x());
                double l = Mstart.right()-w;
                if ( l < bbox.left() ) l = bbox.left();
                M.setLeft(l);
            } else if ( topRect.contains(_mousePressPos) ) {
                QRectF bboxMath = QRectF(bbox.bottomLeft(),bbox.topRight());
                QRectF Mstart = _mousePressMathRect;
                QRectF Mend(Mstart);
                Mend.setTop(bboxMath.top());
                double h0 = qAbs(Mstart.height());
                double h1 = qAbs(Mend.height());
                double a = (1.0/Wh)*log(h0/h1);
                double h = h0*exp(a*(-1.0)*wPt.y());
                double top = Mstart.bottom()+h;
                if ( top > Mend.top() ) {
                    top = Mend.top();
                }
                M.setTop(top);
            } else if ( botRect.contains(_mousePressPos) ) {
                QRectF bboxMath = QRectF(bbox.bottomLeft(),bbox.topRight());
                QRectF Mstart = _mousePressMathRect;
                QRectF Mend(Mstart);
                Mend.setBottom(bboxMath.bottom());
                double h0 = qAbs(Mstart.height());
                double h1 = qAbs(Mend.height());
                double a = (1.0/Wh)*log(h0/h1);
                double h = h0*exp(a*wPt.y());
                double bot = Mstart.top()-h;
                if ( bot < Mend.bottom() ) {
                    bot = Mend.bottom();
                }
                M.setBottom(bot);
            } else {
                // Pan on scoobie case
                M.moveTo(mPt);
            }
        }
        _setPlotMathRect(M);
        viewport()->update();
    } else {
        mouseMove->ignore();
    }
}

QRectF CurvesView::_calcBBox() const
{
    QRectF bbox;
    QModelIndex curvesIdx = _bookModel()->getIndex(rootIndex(),"Curves","Plot");
    int rc = model()->rowCount(curvesIdx);
    for (int i = 0; i < rc; ++i) {
        QModelIndex curveIdx = model()->index(i,0,curvesIdx);
        QModelIndex curveDataIdx = _bookModel()->getDataIndex(curveIdx,
                                                              "CurveData",
                                                              "Curve");
        QVariant v = model()->data(curveDataIdx);
        TrickCurveModel* curveModel =QVariantToPtr<TrickCurveModel>::convert(v);

        if ( curveModel ) {
            QRectF curveBBox = _curveBBox(curveModel,curveIdx);
            bbox = bbox.united(curveBBox);
        }
    }
    return bbox;
}

QRectF CurvesView::_curveBBox(TrickCurveModel* curveModel,
                              const QModelIndex& curveIdx) const
{
    QPainterPath* path = _curve2path.value(curveModel);
    double xScale = _xScale(curveModel,curveIdx);
    double yScale = _yScale(curveModel,curveIdx);
    QRectF pathBox = path->boundingRect();
    double w = pathBox.width();
    double h = pathBox.height();
    QPointF topLeft(xScale*pathBox.topLeft().x(),
                    (yScale*pathBox.topLeft().y()));
    QRectF scaledPathBox(topLeft,QSizeF(xScale*w,yScale*h));

    return scaledPathBox;
}

void CurvesView::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_Space: _keyPressSpace();break;
    case Qt::Key_Up: _keyPressUp();break;
    case Qt::Key_Down: _keyPressDown();break;
    default: ; // do nothing
    }
}

void CurvesView::currentChanged(const QModelIndex &current,
                                const QModelIndex &previous)
{
    viewport()->update();
}

// For two curves hitting the spacebar will toggle between viewing
// the two curves in error, coplot and error+coplot views
void CurvesView::_keyPressSpace()
{
    QModelIndex curvesIdx = _bookModel()->getIndex(rootIndex(),"Curves","Plot");
    int rc = model()->rowCount(curvesIdx);
    if ( rc != 2 ) return;

    QString plotPresentation = _bookModel()->getDataString(rootIndex(),
                                                   "PlotPresentation","Plot");

    if ( plotPresentation == "error" || plotPresentation.isEmpty() ) {
        plotPresentation = "coplot";
    } else if ( plotPresentation == "coplot" ) {
        plotPresentation = "error+coplot";
    } else if ( plotPresentation == "error+coplot" ) {
        plotPresentation = "error";
    } else {
        qDebug() << "snap [bad scoobs]: _keyPressSpace() : "
                 << "plotPresentation=" << plotPresentation;
        exit(-1);
    }

    QModelIndex plotPresentationIdx = _bookModel()->getDataIndex(rootIndex(),
                                                   "PlotPresentation","Plot");
    model()->setData(plotPresentationIdx,plotPresentation);

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
