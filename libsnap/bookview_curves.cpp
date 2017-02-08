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
    painter.drawText(txtBox,Qt::TextDontClip|Qt::AlignCenter,txt);

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
    painter.setTransform(T);
    if ( nCurves == 2 ) {
        QString plotPresentation = _bookModel()->getDataString(rootIndex(),
                                                     "PlotPresentation","Plot");
        if ( plotPresentation.isEmpty() ) {
            plotPresentation = _bookModel()->getDataString(QModelIndex(),
                                                           "Presentation");
        }

        if ( plotPresentation == "compare" ) {
            _paintCoplot(T,painter,pen);
        } else if ( plotPresentation == "error" ) {
            _paintErrorplot(painter,pen,rootIndex());
        } else if ( plotPresentation == "error+compare" ) {
            _paintErrorplot(painter,pen,rootIndex());
            _paintCoplot(T,painter,pen);
        } else {
            qDebug() << "snap [bad scoobs]: paintEvent() : PlotPresentation="
                     << plotPresentation << "not recognized.";
            exit(-1);
        }
    } else {
        _paintCoplot(T,painter,pen);
    }

    // Restore the painter state off the painter stack
    painter.restore();

    // Draw legend (if needed)
    _paintLegend(curvesIdx,painter);
}


void CurvesView::_paintCoplot(const QTransform &T,QPainter &painter,QPen &pen)
{
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
        QColor bg(255,255,255,190);
        QBrush origBrush = painter.brush();
        painter.setBrush(bg);
        painter.fillRect(viewport()->rect(),bg);
        painter.setBrush(origBrush);

        // Paint curve (possibly with linestyle, symbols etc.)
        _paintCurve(currentIndex(),T,painter);

        // Draw coordinate arrow (arrow with (x,y) label) if needed
        QRectF M = _plotMathRect(rootIndex());
        if ( M.width() > 0 && qAbs(M.height()) > 0 ) {
            TrickCurveModel* curveModel = _bookModel()->
                                             getTrickCurveModel(currentIndex());
            _paintLiveCoordArrow(curveModel,currentIndex(),painter);
        }
    }

    painter.restore();
}

void CurvesView::_paintCurve(const QModelIndex& curveIdx,
                             const QTransform& T,
                             QPainter& painter)
{
    painter.save();
    QPen origPen = painter.pen();

    QModelIndex curveDataIdx = _bookModel()->getDataIndex(curveIdx,
                                                          "CurveData","Curve");
    QVariant v = model()->data(curveDataIdx);
    TrickCurveModel* curveModel =QVariantToPtr<TrickCurveModel>::convert(v);

    if ( curveModel ) {

        // Line color
        QPen pen;
        pen.setWidth(0);
        QColor color(_bookModel()->getDataString(curveIdx,
                                                 "CurveColor","Curve"));
        if ( currentIndex() == curveIdx ) {
            // If curve selected, draw with darker color
            color = color.darker(200);
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
        QTransform Tscaled(T);
        Tscaled = Tscaled.scale(xs,ys);
        painter.setTransform(Tscaled);

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
    double xs = _bookModel()->xScale(curveIdx);
    double ys = _bookModel()->yScale(curveIdx);
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

    // Prepend RunID to live text if more than 5 runs
    if ( model()->rowCount(curveIdx.parent()) > 5 ) {
        int id = _bookModel()->getDataInt(curveIdx,"CurveRunID","Curve");
        QString runID = QString("RUN_%1: ").arg(id);
        arrow.txt.prepend(runID);
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
        arrow.paintMe(painter,T);
    }

    curveModel->unmap();
    painter.restore();
}

void CurvesView::_paintErrorplot(QPainter &painter, const QPen &pen,
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
        QPainterPath* errorPath = _bookModel()->getCurvesErrorPath(curvesIdx);
        painter.drawPath(*errorPath);
        delete errorPath;

    } else {
        // TODO:!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        qDebug() << "snap [todo]: Handle error plot when name/units different "
                    "or x is not time.";
        exit(-1);
    }
}

void CurvesView::_paintLegend(const QModelIndex &curvesIdx, QPainter &painter)
{
    const int maxEntries = 7;

    int nCurves = model()->rowCount(curvesIdx);
    if ( nCurves > maxEntries ) {
        return;
    }

    QString pres = _bookModel()->getDataString(rootIndex(),
                                               "PlotPresentation","Plot");
    if ( pres == "error" ) {
        return;
    }

    QStringList runs;
    QStringList curveYNames;
    for ( int i = 0; i < nCurves; ++i ) {

        QModelIndex curveIdx = model()->index(i,0,curvesIdx);

        // Curve name
        QString curveYName = _bookModel()->getDataString(curveIdx,
                                                        "CurveYName", "Curve");
        if ( !curveYNames.contains(curveYName) ) {
            curveYNames.append(curveYName);
        }

        // Run
        TrickCurveModel* curveModel = _bookModel()->
                                      getTrickCurveModel(curvesIdx,i);
        QString trk = curveModel->trkFile();
        QFileInfo trkInfo(trk);
        QString run = trkInfo.absolutePath();
        if ( !runs.contains(run) ) {
            runs.append(run);
        }
    }

    int nRuns = runs.size();
    int nCurveYNames = curveYNames.size();

    QList<QPen*> pens;
    QStringList symbols;
    QStringList labels;

    for ( int i = 0; i < nCurves; ++i ) {

        QModelIndex curveIdx = model()->index(i,0,curvesIdx);

        // Label
        if ( nRuns == 1 && 2 <= nCurveYNames && nCurveYNames <= maxEntries ) {
            QString label =  _bookModel()->getDataString(curveIdx,
                                                        "CurveYLabel", "Curve");
            if ( label.isEmpty() ) {
                label = _bookModel()->getDataString(curveIdx,
                                                    "CurveYName", "Curve");
                int i = label.lastIndexOf('.');
                label = label.mid(i+1);
            }
            labels << label;
        } else if ( nCurveYNames == 1 && 2 <= nRuns && nRuns <= maxEntries ) {
            // Label (use RUN dir name)
            TrickCurveModel* curveModel = _bookModel()->
                    getTrickCurveModel(curveIdx);
            QString trk = curveModel->trkFile();
            QString runName = QFileInfo(trk).dir().dirName();
            labels << runName;
        } else {
            continue;  // if no labels, no legend
        }

        // Pen
        QString penColor = _bookModel()->getDataString(curveIdx,
                                                       "CurveColor", "Curve");
        QPen* pen = new QPen(penColor);
        QVector<qreal> pat = _bookModel()->getLineStylePattern(curveIdx);
        pen->setDashPattern(pat);
        pens << pen;

        // Symbol
        symbols << _bookModel()->getDataString(curveIdx,
                                               "CurveSymbolStyle", "Curve");

    }

    if ( pres == "error+compare" ) {
        QPen* magentaPen = new QPen(_bookModel()->createCurveColors(3).at(2));
        pens << magentaPen;
        symbols << "none";
        labels << "error";
    }

    __paintLegend(pens,symbols,labels,painter);

    // Clean up
    foreach ( QPen* pen, pens ) {
        delete pen;
    }
}

// pens,symbols and labels are ordered/collated foreach legend curve/label
void CurvesView::__paintLegend(const QList<QPen *> &pens,
                               const QStringList &symbols,
                               const QStringList &labels,
                               QPainter &painter)
{
    QPen origPen = painter.pen();

    int n = pens.size();

    // Width Of Legend Box
    const int ml = 10; // marginLeft
    const int mr = 10; // marginRight
    const int s = 10;  // spaceBetweenLineAndLabel
    const int l = 30;  // line width , e.g. ~5 for: *-----* Gravity
    int w = 0;
    foreach (QString label, labels ) {
        int labelWidth = painter.fontMetrics().boundingRect(label).width();
        int ww = ml + l + s + labelWidth + mr;
        if ( ww > w ) {
            w = ww;
        }
    }

    // Height Of Legend Box
    const int v = 3;  // vertical space between legend entries
    const int mt = 5; // marginTop
    const int mb = 5; // marginBot
    int sh = 0;
    foreach (QString label, labels ) {
        sh += painter.fontMetrics().boundingRect(label).height();
    }
    int h = (n-1)*v + mt + mb + sh;

    // Legend box top left point
    const int top = 5;   // top margin
    const int right = 10; // right margin
    QRect V = viewport()->rect();
    QPoint legendTopLeft(V.width()-w-right,top);

    // Legend box
    QRect LegendBox(legendTopLeft,QSize(w,h));

    // Draw legend box with semi-transparent background
    QColor bg(255,255,255,220);
    painter.setBrush(bg);
    QPen penGray(QColor(120,120,120,255));
    painter.setPen(penGray);
    painter.drawRect(LegendBox);
    painter.setPen(origPen);

    QRect lastBB;
    for ( int i = 0; i < n; ++i ) {

        QString label = labels.at(i);

        // Calc bounding rect (bb) for line and label
        QPoint topLeft;
        if ( i == 0 ) {
            topLeft.setX(legendTopLeft.x()+ml);
            topLeft.setY(legendTopLeft.y()+mt);
        } else {
            topLeft.setX(lastBB.bottomLeft().x());
            topLeft.setY(lastBB.bottomLeft().y()+v);
        }
        QRect bb = painter.fontMetrics().boundingRect(label);
        bb.moveTopLeft(topLeft);
        bb.setWidth(l+s+bb.width());

        // Draw line segment
        QPen* pen = pens.at(i);
        painter.setPen(*pen);
        QPoint p1(bb.left(),bb.center().y());
        QPoint p2(bb.left()+l,bb.center().y());
        painter.drawLine(p1,p2);

        // Draw symbols on line segment endpoints
        QString symbol = symbols.at(i);
        __paintSymbol(p1,symbol,painter);
        __paintSymbol(p2,symbol,painter);

        // Draw label
        QRect labelRect(bb);
        QPoint p(bb.topLeft().x()+l+s, bb.topLeft().y());
        labelRect.moveTopLeft(p);
        painter.drawText(labelRect, Qt::AlignLeft|Qt::AlignVCenter, label);

        lastBB = bb;
    }


    painter.setPen(origPen);
}

void CurvesView::__paintSymbol(const QPointF& p,
                               const QString &symbol, QPainter &painter)
{

    QPen origPen = painter.pen();
    QPen pen = painter.pen();

    if ( symbol == "circle" ) {
        painter.drawEllipse(p,2,2);
    } else if ( symbol == "thick_circle" ) {
        pen.setWidth(2.0);
        painter.setPen(pen);
        painter.drawEllipse(p,3,3);
    } else if ( symbol == "solid_circle" ) {
        pen.setWidthF(2.0);
        painter.setPen(pen);
        painter.drawEllipse(p,1,1);
    } else if ( symbol == "square" ) {
        double x = p.x()-2.0;
        double y = p.y()-2.0;
        painter.drawRect(QRectF(x,y,4,4));
    } else if ( symbol == "thick_square") {
        pen.setWidthF(2.0);
        painter.setPen(pen);
        double x = p.x()-3.0;
        double y = p.y()-3.0;
        painter.drawRect(QRectF(x,y,6,6));
    } else if ( symbol == "solid_square" ) {
        pen.setWidthF(4.0);
        painter.setPen(pen);
        painter.drawPoint(p); // happens to be a solid square
    } else if ( symbol == "star" ) { // *
        double r = 3.0;
        QPointF a(p.x()+r*cos(18.0*M_PI/180.0),
                  p.y()-r*sin(18.0*M_PI/180.0));
        QPointF b(p.x(),p.y()-r);
        QPointF c(p.x()-r*cos(18.0*M_PI/180.0),
                  p.y()-r*sin(18.0*M_PI/180.0));
        QPointF d(p.x()-r*cos(54.0*M_PI/180.0),
                  p.y()+r*sin(54.0*M_PI/180.0));
        QPointF e(p.x()+r*cos(54.0*M_PI/180.0),
                  p.y()+r*sin(54.0*M_PI/180.0));
        painter.drawLine(p,a);
        painter.drawLine(p,b);
        painter.drawLine(p,c);
        painter.drawLine(p,d);
        painter.drawLine(p,e);
    } else if ( symbol == "xx" ) {
        pen.setWidthF(2.0);
        painter.setPen(pen);
        QPointF a(p.x()+2.0,p.y()+2.0);
        QPointF b(p.x()-2.0,p.y()+2.0);
        QPointF c(p.x()-2.0,p.y()-2.0);
        QPointF d(p.x()+2.0,p.y()-2.0);
        painter.drawLine(p,a);
        painter.drawLine(p,b);
        painter.drawLine(p,c);
        painter.drawLine(p,d);
    } else if ( symbol == "triangle" ) {
        double r = 3.0;
        QPointF a(p.x(),p.y()-r);
        QPointF b(p.x()-r*cos(30.0*M_PI/180.0),
                  p.y()+r*sin(30.0*M_PI/180.0));
        QPointF c(p.x()+r*cos(30.0*M_PI/180.0),
                  p.y()+r*sin(30.0*M_PI/180.0));
        painter.drawLine(a,b);
        painter.drawLine(b,c);
        painter.drawLine(c,a);
    } else if ( symbol == "thick_triangle" ) {
        pen.setWidthF(2.0);
        painter.setPen(pen);
        double r = 4.0;
        QPointF a(p.x(),p.y()-r);
        QPointF b(p.x()-r*cos(30.0*M_PI/180.0),
                  p.y()+r*sin(30.0*M_PI/180.0));
        QPointF c(p.x()+r*cos(30.0*M_PI/180.0),
                  p.y()+r*sin(30.0*M_PI/180.0));
        painter.drawLine(a,b);
        painter.drawLine(b,c);
        painter.drawLine(c,a);
    } else if ( symbol == "solid_triangle" ) {
        pen.setWidthF(2.0);
        painter.setPen(pen);
        double r = 3.0;
        QPointF a(p.x(),p.y()-r);
        QPointF b(p.x()-r*cos(30.0*M_PI/180.0),
                  p.y()+r*sin(30.0*M_PI/180.0));
        QPointF c(p.x()+r*cos(30.0*M_PI/180.0),
                  p.y()+r*sin(30.0*M_PI/180.0));
        painter.drawLine(a,b);
        painter.drawLine(b,c);
        painter.drawLine(c,a);
    } else if ( symbol.startsWith("number_") && symbol.size() == 8 ) {

        QFont origFont = painter.font();
        QBrush origBrush = painter.brush();

        // Calculate bbox to draw text in
        QString number = symbol.right(1); // last char is '0'-'9'
        QFont font = painter.font();
        font.setPointSize(7);
        painter.setFont(font);
        QFontMetrics fm = painter.fontMetrics();
        QRectF bbox(fm.tightBoundingRect(number));
        bbox.moveCenter(p);

        // Draw solid circle around number
        QRectF box(bbox);
        double l = 3.0*qMax(box.width(),box.height())/2.0;
        box.setWidth(l);
        box.setHeight(l);
        box.moveCenter(p);
        QBrush brush(pen.color());
        painter.setBrush(brush);
        painter.drawEllipse(box);

        // Draw number in white in middle of circle
        QPen whitePen("white");
        painter.setPen(whitePen);
        painter.drawText(bbox,Qt::AlignCenter,number);

        painter.setFont(origFont);
        painter.setBrush(origBrush);
    }

    painter.setPen(origPen);
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
        if ( _isXTime(topLeft.parent()) && _isXTime(rootIndex()) ) {
            QRectF M = model()->data(topLeft).toRectF();
            QModelIndex plotRectIdx = _bookModel()->getDataIndex(rootIndex(),
                                                         "PlotMathRect","Plot");
            QRectF R = model()->data(plotRectIdx).toRectF();
            if ( M.left() != R.left() || M.right() != R.right() ) {
                R.setLeft(M.left());
                R.setRight(M.right());
                model()->setData(plotRectIdx,R);
            }
        }
    }

    viewport()->update();
}

QPixmap* CurvesView::_createLivePixmap()
{
    QPixmap* livePixmap = new QPixmap(viewport()->rect().size());
    livePixmap->fill(Qt::white);

    QPainter painter(livePixmap);
    painter.setRenderHint(QPainter::Antialiasing);

    QTransform T = _coordToPixelTransform();

    QModelIndex curvesIdx = _bookModel()->getIndex(rootIndex(),"Curves","Plot");
    int rc = model()->rowCount(curvesIdx);
    for ( int i = 0; i < rc; ++i ) {
        QModelIndex curveIdx = model()->index(i,0,curvesIdx);
        TrickCurveModel* curveModel =_bookModel()->getTrickCurveModel(curveIdx);
        if ( curveModel ) {
            _paintCurve(curveIdx,T,painter);
        }
    }

    return livePixmap;
}

// If any of the curves in plot has a curve with x being time, return true
bool CurvesView::_isXTime(const QModelIndex &plotIdx)
{
    bool isXTime = false;

    bool isExistsCurves = _bookModel()->isChildIndex(plotIdx, "Plot", "Curves");

    if ( isExistsCurves ) {
        QModelIndex curvesIdx = _bookModel()->getIndex(plotIdx,"Curves","Plot");
        int rc = model()->rowCount(curvesIdx);
        for ( int i = 0; i < rc ; ++i ) {
            QModelIndex curveIdx = model()->index(i,0,curvesIdx);
            QString xName = _bookModel()->getDataString(curveIdx,
                                                        "CurveXName","Curve");
            if ( xName == "sys.exec.out.time" ) { // TODO: TimeName
                isXTime = true;
                break;
            }
        }
    }

    return isXTime;
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
            TrickCurveModel* curveModel =
                                     QVariantToPtr<TrickCurveModel>::convert(v);
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
        _setPlotMathRect(bbox);
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
        QTransform Tscaled(T);
        Tscaled = Tscaled.scale(xs,ys);
        painter.setTransform(Tscaled);

        // Ignore paths whose bounding box does not intersect
        // math click rect. This speeds up a special case of
        // selecting a spike which falls outside most other curves
        if ( xs == 1.0 && ys == 1.0 ) {
            // TODO: Lazily checking for xs==ys==1.0 because of scaling issues
            if ( !path->intersects(M) ) {
                continue;
            }
        }

        // Draw curve onto monochrome image
        painter.drawPath(*path);

        // Check, pixel by pixel, to see if the curve
        // is in small rectangle around mouse click
        bool isFound = false;
        for ( int x = pt.x()-s/2; x < pt.x()+s/2; ++x ) {
            if ( x >= img.width() ) {
                continue;
            }
            for ( int y = pt.y()-s/2; y < pt.y()+s/2; ++y ) {
                if ( y >= img.height() ) {
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

        if ( !shiftPressed &&
             (tag == "Curve" &&
             (presentation == "compare" || presentation.isEmpty())) ) {

            QRectF M = _mathRect();

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
                double xs = _bookModel()->xScale(currentIndex());
                double ys = _bookModel()->yScale(currentIndex());
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

                curveModel->unmap();
                viewport()->update();
            }
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
            _setPlotMathRect(M);
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

            _setPlotMathRect(M);
            viewport()->update();
        }
    } else {
        mouseMove->ignore();
    }
}

QRectF CurvesView::_curveBBox(const QModelIndex& curveIdx) const
{
    QPainterPath* path = _bookModel()->getCurvePainterPath(curveIdx);
    double xScale = _bookModel()->xScale(curveIdx);
    double yScale = _bookModel()->yScale(curveIdx);
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
        qDebug() << "snap [bad scoobs]: _keyPressSpace() : "
                 << "plotPresentation=" << plotPresentation;
        exit(-1);
    }

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
    _setPlotMathRect(R);

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
