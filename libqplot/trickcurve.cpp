#include "trickcurve.h"
#include "libsnapdata/timeit_linux.h"

TrickCurve::TrickCurve(QCPAxis *keyAxis, QCPAxis *valueAxis) :
    QCPAbstractPlottable(keyAxis, valueAxis) ,
    _isPainterPathCreated(false),
    _model(0),
    _timeVec(0),
    _valVec(0)
{
    setAntialiased(false);
    mPen.setColor(QColor(50, 100, 212));
    mPen.setStyle(Qt::SolidLine);
    mPen.setWidthF(0);
    mBrush.setColor(QColor(50, 100, 212));
    mBrush.setStyle(Qt::NoBrush);

    setSelectable(true);
    mSelectedPen = mPen;
    mSelectedPen.setWidthF(0);
    mSelectedPen.setColor(QColor(254, 50, 12));
    mSelectedBrush.setStyle(Qt::NoBrush);

    setLineStyle(lsLine);
}


TrickCurve::~TrickCurve()
{ }

void TrickCurve::setLineStyle(TrickCurve::LineStyle style)
{
    mLineStyle = style;
}

double TrickCurve::selectTest(const QPointF &pt,
                            bool onlySelectable, QVariant *details) const
{
    Q_UNUSED(details)

    if ((onlySelectable && !mSelectable)) return -1;

    double minD = 1.0e20; // this is returned if not within 50x50 rect about pt

    QTransform P = _coordToPixelTransform();
    QTransform C = P.inverted();

    // Make path of 50x50 rect centered at pt
    QPainterPath path;
    QRectF rect(pt.x()-25,
                pt.y()-25,
                50,50);
    rect = C.mapRect(rect);
    path.addRect(rect);

    if ( _painterPath.intersects(path) ) {
        int ec = _painterPath.elementCount()-1;
        for ( int ii = 0; ii < ec; ++ii) {
            QPointF p1(_painterPath.elementAt(ii).x,
                       _painterPath.elementAt(ii).y);
            p1 = P.map(p1);
            QPointF p2(_painterPath.elementAt(ii+1).x,
                       _painterPath.elementAt(ii+1).y);
            p2 = P.map(p2);
            qreal d = qSqrt(TrickCurve::_distSquaredLineSegmentToPoint(p1,p2,pt));
            if ( d < minD ) {
                minD = d;
            }
        }
    }

    return minD;
}

//
// Taken from RealTime Collision Detection by Christer Ericson
//
double TrickCurve::_distSquaredLineSegmentToPoint(const QPointF &l0,
                                               const QPointF &l1,
                                               const QPointF &pt)
{
    QVector2D a(l0);
    QVector2D b(l1);
    QVector2D c(pt);
    QVector2D ab = b - a ;
    QVector2D ac = c - a ;
    QVector2D bc = c - b ;

    double e = QVector2D::dotProduct(ac,ab);
    if ( e <= 0 ) return QVector2D::dotProduct(ac,ac);
    double f = QVector2D::dotProduct(ab,ab);
    if ( e >= f ) return QVector2D::dotProduct(bc,bc);

    return QVector2D::dotProduct(ac,ac)-e*e/f;
}


void TrickCurve::setData(TrickCurveModel *model)
{
   _model = model;
   QString yparam = model->headerData(2,Qt::Horizontal,Param::Name).toString();
   setName(yparam);
   _createPainterPath(model);
}

void TrickCurve::setData(const QVector<double> *t, const QVector<double> *v)
{
   _timeVec = t;
   _valVec = v;
   _createPainterPath(t,v);
}

void TrickCurve::draw(QCPPainter *painter)
{
    if ( (_model == 0 || _model->rowCount() == 0) &&
         (_timeVec == 0 || _valVec == 0 ) ) {
        return;
    }

    //
    // On first draw, create QPainterPath from curve data
    //
    if ( !_isPainterPathCreated) {
        // Hack on data type
        if ( _model ) {
            _createPainterPath(_model);
        } else {
            _createPainterPath(_timeVec,_valVec);
        }
    }

    //
    // Draw curve!
    //
    if (mLineStyle != lsNone &&
        mainPen().style() != Qt::NoPen && mainPen().color().alpha() != 0) {

        if ( mSelected ) {
            painter->setPen(selectedPen());
        } else {
            painter->setPen(mainPen());
        }

        if ( painter->modes().testFlag(QCPPainter::pmVectorized) ) {
            // Make PDF
            // hack - need inheritance and class business, but for now - lazy
            if ( _model ) {
                painter->drawPath(_scaledPainterPath(_model));
            } else {
                painter->drawPath(_scaledPainterPath(_timeVec,_valVec));
            }
        } else {
            painter->save();
            painter->setTransform(_coordToPixelTransform());
            painter->drawPath(_painterPath);
            painter->restore();
        }

        // If flatline curve, add y value label
        double dy = _painterPath.boundingRect().height();
        if ( dy < 1.0e-24 ) {
            _addFlatLineLabel(painter);
        }
    }
}

QPainterPath TrickCurve::_scaledPainterPath(TrickCurveModel* model)
{
    QPainterPath path;

    model->map();

    QTransform t = _coordToPixelTransform();

    TrickModelIterator it = model->begin();
    const TrickModelIterator e = model->end();
    QPointF pt0(it.x(),it.y());
    pt0 = t.map(pt0);
    path.moveTo(pt0.x(),pt0.y());
    while (it != e) {
        QPointF pt(it.x(),it.y());
        pt = t.map(pt);
        path.lineTo(pt.x(),pt.y());
        ++it;
    }

    model->unmap();

    return path;
}

QPainterPath TrickCurve::_scaledPainterPath(const QVector<double> *t,
                                           const QVector<double> *v)
{
    QPainterPath path;

    QTransform M = _coordToPixelTransform();

    QPointF pt0(t->at(0),v->at(0));
    pt0 = M.map(pt0);
    path.moveTo(pt0.x(),pt0.y());
    for ( int i = 0; i < t->size(); ++i) {
        QPointF pt(t->at(i),v->at(i));
        pt = M.map(pt);
        path.lineTo(pt.x(),pt.y());
    }

    return path;
}

// Adds a text label above curve of y-value (assumed flatlined)
void TrickCurve::_addFlatLineLabel(QCPPainter *painter)
{
    QRectF valueRect = _painterPath.boundingRect();
    double x = qMax(valueRect.left(),keyAxis()->range().lower);
    double y = valueRect.y();
    valueRect.moveTo(x,y);
    QRectF pixelRect = _coordToPixelTransform().mapRect(valueRect);
    QPointF pix = pixelRect.topLeft();
    pix.setX(pix.x()+15.0);
    pix.setY(pix.y()-5.0);
    QString yStr = QString().sprintf("%.8lf",y);
    painter->drawText(pix,yStr);
}

void TrickCurve::_createPainterPath(TrickCurveModel *model)
{
    _isPainterPathCreated = true;

    _model = model;

    _model->map();

    TrickModelIterator it = _model->begin();
    const TrickModelIterator e = _model->end();
    _painterPath.moveTo(it.x(),it.y());
    while (it != e) {
        _painterPath.lineTo(it.x(),it.y());
        ++it;
    }

    _model->unmap();
}

void TrickCurve::_createPainterPath(const QVector<double> *t,
                                   const QVector<double> *v)
{
    _isPainterPathCreated = true;

    _timeVec = t;
    _valVec = v;

    _painterPath.moveTo(t->at(0),v->at(0));
    for ( int i = 1; i < t->size(); ++i) {
        _painterPath.lineTo(t->at(i),v->at(i));
    }
}

/* inherits documentation from base class */
void TrickCurve::drawLegendIcon(QCPPainter *painter, const QRectF &rect) const
{
    // draw fill:
    if (mBrush.style() != Qt::NoBrush)
    {
        applyFillAntialiasingHint(painter);
        painter->fillRect(QRectF(rect.left(),
                                 rect.top()+rect.height()/2.0,
                                 rect.width(), rect.height()/3.0), mBrush);
    }
    // draw line vertically centered:
    if (mLineStyle != lsNone)
    {
        applyDefaultAntialiasingHint(painter);
        painter->setPen(mPen);
        // +5 on x2 else last segment is missing from dashed/dotted pens
        painter->drawLine(QLineF(rect.left(),
                                 rect.top()+rect.height()/2.0,
                                 rect.right()+5,
                                 rect.top()+rect.height()/2.0));
    }
}

/*
  called by TrickCurve::draw to generate a point vector (pixels) which represents
  the line of the curve. Line segments that aren't visible in the
  current axis rect are handled in an optimized
  way.
*/
void TrickCurve::getCurveData(QVector<QPointF> *lineData) const
{
    qDebug() << "snap [error]: getCurveData() should not be called";
    exit(-1);
    _model->map();
    /* Extended sides of axis rect R divide space into 9 regions:
     1__|_4_|__7
     2__|_R_|__8
     3  | 6 |  9
     General idea: If the two points of a line segment are in the same region
     (that is not R), the line segment corner is removed. Curves outside R
     become straight lines closely outside of R which greatly reduces drawing
     time, yet keeps the look of lines and fills inside R consistent. The region
     R has index 5.
  */
    QCPAxis *keyAxis = mKeyAxis.data();
    QCPAxis *valueAxis = mValueAxis.data();
    if (!keyAxis || !valueAxis) {
        qDebug() << Q_FUNC_INFO << "invalid key or value axis"; return;
    }

    QRect axisRect = mKeyAxis.data()->axisRect()->rect() &
                     mValueAxis.data()->axisRect()->rect();
    lineData->reserve(_model->rowCount());
    TrickModelIterator it;
    int lastRegion = 5;
    int currentRegion = 5;
    double RLeft = keyAxis->range().lower;
    double RRight = keyAxis->range().upper;
    double RBottom = valueAxis->range().lower;
    double RTop = valueAxis->range().upper;
    double x, y; // current key/value
    bool addedLastAlready = true;
    bool firstPoint = true; // first point must always be drawn,
                           // to make sure fill works correctly

    const TrickModelIterator e = _model->end();
    for (it = _model->begin(); it != e; ++it) {

        x = it.x();
        y = it.y();

        // determine current region:
        if (x < RLeft) // region 123
        {
            if (y > RTop)
                currentRegion = 1;
            else if (y < RBottom)
                currentRegion = 3;
            else
                currentRegion = 2;
        } else if (x > RRight) // region 789
        {
            if (y > RTop)
                currentRegion = 7;
            else if (y < RBottom)
                currentRegion = 9;
            else
                currentRegion = 8;
        } else // region 456
        {
            if (y > RTop)
                currentRegion = 4;
            else if (y < RBottom)
                currentRegion = 6;
            else
                currentRegion = 5;
        }

        // Watch out, the next part is very tricky. It modifies the curve such
        // that it seems like the whole thing is still drawn, but actually the
        // points outside the axisRect are simplified ("optimized") greatly.
        // There are some subtle special cases when line segments are large and
        // thereby each subsequent point may be in a different region or even
        // skip some.

        // determine whether to keep current point:
        if (currentRegion == 5 || (firstPoint && mBrush.style() != Qt::NoBrush)) {
           // current is in R, add current and last if it wasn't added already
            if (!addedLastAlready) {
                // in case curve just entered R,
                // make sure the last point outside R is also drawn correctly
                lineData->append(coordsToPixels((it-1).x(),(it-1).y()));

            } else if (lastRegion != 5) {
                // added last already. If that's the case,
                // we probably added it at optimized position. So go back and
                // make sure it's at original position (else the angle changes
                // under which this segment enters R)
                if (!firstPoint) {
                    // because on firstPoint, currentRegion is 5 and
                    // addedLastAlready is true, although there is no last point
                    lineData->replace(lineData->size()-1,
                                      coordsToPixels((it-1).x(),(it-1).y()));
                }
            }
            // add current point to vector
            lineData->append(coordsToPixels(it.x(), it.y()));
            addedLastAlready = true; // so in next iteration, we don't add
                                     // this point twice

        } else if (currentRegion != lastRegion) {

            // changed region, add current
            // and last if not added already

            // using outsideCoordsToPixels instead of coorsToPixels
            // for optimized point placement (places points just outside axisRect
            // instead of potentially far away)

            // if we're coming from R or we skip diagonally over
            // the corner regions (so line might still be visible in R),
            // we can't place points optimized
            if (lastRegion == 5 || // coming from R
                    ((lastRegion==2 && currentRegion==4) ||
                     (lastRegion==4 && currentRegion==2)) || // skip tleft diag
                    ((lastRegion==4 && currentRegion==8) ||
                     (lastRegion==8 && currentRegion==4)) || // skip tright diag
                    ((lastRegion==8 && currentRegion==6) ||
                     (lastRegion==6 && currentRegion==8)) || // skip bright diag
                    ((lastRegion==6 && currentRegion==2) ||
                     (lastRegion==2 && currentRegion==6))    // skip bleft diag
                    )
            {
                // always add last point if not added already, original:
                if (!addedLastAlready)
                    lineData->append(coordsToPixels((it-1).x(),(it-1).y()));
                // add current point, original:
                lineData->append(coordsToPixels(it.x(),it.y()));
            } else {
                // no special case that forbids optimized point placement,
                // so do it:

                // always add last point if not added already, optimized:
                if (!addedLastAlready) {
                    lineData->append(
                        outsideCoordsToPixels((it-1).x(), (it-1).y(),
                                              currentRegion, axisRect));
                }
                // add current point, optimized:
                lineData->append(
                        outsideCoordsToPixels(it.x(),it.y(),
                                              currentRegion, axisRect));
            }
            // so that if next point enters 5, or crosses another
            // region boundary, we don't add this point twice
            addedLastAlready = true;
        } else // neither in R, nor crossed a region boundary, skip current point
        {
            addedLastAlready = false;
        }
        lastRegion = currentRegion;
        firstPoint = false;
    }
    // If curve ends outside R, we want to add very last
    // point so the fill looks like it should when the curve started inside R:
    if (lastRegion != 5 && mBrush.style() != Qt::NoBrush &&
        _model->rowCount()>0) {

        lineData->append(coordsToPixels((_model->end()-1).x(),
                                       (_model->end()-1).y()));
    }

    _model->unmap();
}

// Calculates the (minimum) distance (in pixels) the curve's representation
// has from the given pixelPoint in pixels. This is used to determine whether
// the curve was clicked or not, e.g. in selectTest.
double TrickCurve::pointDistance(const QPointF &pixelPoint) const
{
    if ( !_model ) {
        qDebug() << "snap [bogasity]: this should not be called if _model = 0"
                 << "Hopefully it's not because you are using the bogus "
                 << "QVector<double>t,v stuff";
        exit(-1);
    }

    _model->map();
    if (_model->rowCount() == 0) {
        qDebug() << Q_FUNC_INFO
                 << "requested point distance on curve"
                 << mName << "without data";
        return 500;
    }
    if (_model->rowCount() == 1) {
        QPointF dataPoint = coordsToPixels(_model->begin().x(),
                                          _model->begin().y());
        return QVector2D(dataPoint-pixelPoint).length();
    }

    // calculate minimum distance to line segments:
    QVector<QPointF> *lineData = new QVector<QPointF>;
    getCurveData(lineData);
    double minDistSqr = std::numeric_limits<double>::max();
    for (int i=0; i<lineData->size()-1; ++i)
    {
        double currentDistSqr = distSqrToLine(lineData->at(i),
                                              lineData->at(i+1), pixelPoint);
        if (currentDistSqr < minDistSqr)
            minDistSqr = currentDistSqr;
    }
    delete lineData;
    _model->unmap();
    return sqrt(minDistSqr);
}



// This is a specialized coordsToPixels function for points that
// are outside the visible axisRect and just crossing a boundary (since
// getCurveData reduces non-visible curve segments to those line segments
// that cross region boundaries, see documentation there). It only uses the
// coordinate parallel to the region boundary of the axisRect. The other
// coordinate is picked just outside the axisRect (how far is determined
// by the scatter size and the line width). Together with the optimization
// in getCurveData this improves performance for large curves (or zoomed
// in ones) significantly while keeping the illusion the whole curve and its
// filling is still being drawn for the viewer.
QPointF TrickCurve::outsideCoordsToPixels(double key, double value, int region,
                                       QRect axisRect) const
{
    int margin = qCeil((double)mPen.widthF()) + 2;
    QPointF result = coordsToPixels(key, value);
    switch (region)
    {
    case 2: result.setX(axisRect.left()-margin); break; // left
    case 8: result.setX(axisRect.right()+margin); break; // right
    case 4: result.setY(axisRect.top()-margin); break; // top
    case 6: result.setY(axisRect.bottom()+margin); break; // bottom
    case 1: result.setX(axisRect.left()-margin);
        result.setY(axisRect.top()-margin); break; // top left
    case 7: result.setX(axisRect.right()+margin);
        result.setY(axisRect.top()-margin); break; // top right
    case 9: result.setX(axisRect.right()+margin);
        result.setY(axisRect.bottom()+margin); break; // bottom right
    case 3: result.setX(axisRect.left()-margin);
        result.setY(axisRect.bottom()+margin); break; // bottom left
    }
    return result;
}

//
// Since getKeyRange() is const, can't do the caching thing without a headache
//
QCPRange TrickCurve::xRange(bool &validRange, SignDomain inSignDomain)
{
    Q_UNUSED(validRange);
    Q_UNUSED(inSignDomain);
    return QCPRange(
                _painterPath.boundingRect().left(),
                _painterPath.boundingRect().right());
}

//
// Since getValueRange() is const, can't do the caching thing without a headache
//
QCPRange TrickCurve::yRange(bool &validRange, SignDomain inSignDomain)
{
    Q_UNUSED(validRange);
    Q_UNUSED(inSignDomain);

    return QCPRange(
                _painterPath.boundingRect().top(),
                _painterPath.boundingRect().bottom());
}

/* inherits documentation from base class */
QCPRange TrickCurve::getKeyRange(bool &validRange, SignDomain inSignDomain) const
{
    if ( !_model ) {
        qDebug() << "snap [bogasity]: this should not be called if _model = 0"
                 << "Hopefully it's not because you are using the bogus "
                 << "QVector<double>t,v stuff";
        exit(-1);
    }

    _model->map();
    QCPRange range;
    bool haveLower = false;
    bool haveUpper = false;

    double current;

    TrickModelIterator it = _model->begin();
    const TrickModelIterator e = _model->end();
    while (it != e) {
        current = it.x();
        if (inSignDomain == sdBoth ||
            (inSignDomain == sdNegative && current < 0) ||
            (inSignDomain == sdPositive && current > 0)) {

            if (current < range.lower || !haveLower) {
                range.lower = current;
                haveLower = true;
            }
            if (current > range.upper || !haveUpper) {
                range.upper = current;
                haveUpper = true;
            }
        }
        ++it;
    }

    validRange = haveLower && haveUpper;
    _model->unmap();
    return range;
}

/* inherits documentation from base class */
QCPRange TrickCurve::getValueRange(bool &validRange, SignDomain inSignDomain) const
{
    if ( !_model ) {
        qDebug() << "snap [bogasity]: this should not be called if _model = 0"
                 << "Hopefully it's not because you are using the bogus "
                 << "QVector<double>t,v stuff";
        exit(-1);
    }

    _model->map();
    QCPRange range;
    bool haveLower = false;
    bool haveUpper = false;

    double current;

    TrickModelIterator it = _model->begin();
    const TrickModelIterator e = _model->end();
    while (it != e) {
        current = it.y();
        if (inSignDomain == sdBoth ||
           (inSignDomain == sdNegative && current < 0) ||
                (inSignDomain == sdPositive && current > 0)) {

            if (current < range.lower || !haveLower) {
                range.lower = current;
                haveLower = true;
            } if (current > range.upper || !haveUpper)
            {
                range.upper = current;
                haveUpper = true;
            }
        }
        ++it;
    }

    validRange = haveLower && haveUpper;
    _model->unmap();
    return range;
}

void TrickCurve::setSelected(bool selected)
{
    if ( mSelected != selected ) {
        if ( selected == true ) {
            _deselectedLayerIdx = layer()->indexOfChild(this);
            layer()->raiseChild(this);
        } else {
            layer()->moveChild(this,_deselectedLayerIdx);
        }
        mSelected = selected;
        emit selectionChanged(this);
    }
}
