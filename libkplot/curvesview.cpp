#include "curvesview.h"

CurvesView::CurvesView(QWidget *parent) :
    BookIdxView(parent)
{
    setFrameShape(QFrame::NoFrame);
    _colorBandsNormal = _createColorBands(9,false);
    _colorBandsRainbow = _createColorBands(10,true);
}

CurvesView::~CurvesView()
{
    foreach ( QPainterPath* path, _curve2path.values() ) {
        delete path;
    }
    _curve2path.clear();
}

void CurvesView::_update()
{
    if ( !_myIdx.isValid() ) return;
    // TODO??? do I have to do anything???
}


void CurvesView::paintEvent(QPaintEvent *event)
{
    if ( !model() ) return;

    QPainter painter(viewport());
    painter.save();

    //if ( _kPlotModel->curvePointSize(this) > 0 ) {
        //painter.setRenderHint(QPainter::Antialiasing);
    //}
    painter.setRenderHint(QPainter::Antialiasing);

    QTransform T = _coordToPixelTransform();
    double ptSizeCurve = _curvePointSize();
    QPen pen;
    pen.setWidthF(ptSizeCurve);

    // Draw!
    painter.setTransform(T);
    int nCurves = _curve2path.size();
    int nBands = _colorBandsRainbow.size();
    int nCurvesPerBand = div(nCurves,nBands).quot;
    QModelIndex curvesIdx = _bookModel()->getIndex(_myIdx,"Curves","Plot");
    int rc = model()->rowCount(curvesIdx);
    for ( int i = 0; i < rc; ++i ) {

        QModelIndex curveIdx = model()->index(i,0,curvesIdx);
        QModelIndex curveDataIdx = _bookModel()->getDataIndex(curveIdx,
                                                          "CurveData","Curve");
        QVariant v = model()->data(curveDataIdx);
        TrickCurveModel* curveModel =QVariantToPtr<TrickCurveModel>::convert(v);

        if ( curveModel ) {

            // Color curves
            if ( nCurves < 10 ) {
                pen.setColor(_colorBandsNormal.at(i));
            } else  {
                div_t q = div(i,nCurvesPerBand);
                int j = qMin(q.quot,nBands-1);
                pen.setColor(_colorBandsRainbow.at(j));
            }
            painter.setPen(pen);

            // Get painter path
            QPainterPath* path = _curve2path.value(curveModel);

            // Scale transform (e.g. for unit axis scaling)
            double xs = _xScale(curveModel,curveIdx);
            double ys = _yScale(curveModel,curveIdx);
            QTransform Tscaled = T.scale(xs,ys);
            painter.setTransform(Tscaled);

            // Draw curve!
            painter.drawPath(*path);
        }
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
    if ( topLeft.column() != 1 ) return;
    if ( topLeft.parent().parent().parent() != _myIdx ) return;

    QModelIndex columnZeroIdx = topLeft.sibling(topLeft.row(),0);
    QString name = model()->data(columnZeroIdx).toString();

    if ( name == "CurveData" ) {
        QVariant v = model()->data(topLeft);
        TrickCurveModel* curveModel =QVariantToPtr<TrickCurveModel>::convert(v);
        if ( curveModel ) {

            // Update x/y units in axis labels in curlies
            QModelIndex curveIdx = columnZeroIdx.parent();
            _updateUnits(curveIdx,'x');
            _updateUnits(curveIdx,'y');

            // Create and insert path and reset curves bounding box
            QPainterPath* path = _createPainterPath(curveModel);
            _insertPath(path, curveModel, curveIdx);
        }
    }
}

void CurvesView::rowsInserted(const QModelIndex &pidx, int start, int end)
{
    if ( pidx.parent().parent() != _myIdx ) return; // not my plot

    for ( int i = start; i <= end; ++i ) {
        QModelIndex curveIdx = model()->index(i,0,pidx);
        QModelIndex curveDataIdx = model()->index(i,1,pidx);
        QString name = model()->data(curveIdx).toString();
        if ( name == "CurveData" ) {
            QVariant v = model()->data(curveDataIdx);
            TrickCurveModel* curveModel =
                    QVariantToPtr<TrickCurveModel>::convert(v);
            if ( curveModel ) {
                _updateUnits(curveIdx,'x');
                _updateUnits(curveIdx,'y');
                QPainterPath* path = _createPainterPath(curveModel);
                _insertPath(path,curveModel,curveIdx);
            }
            break;
        }
    }
}

void CurvesView::_insertPath(QPainterPath* path,
                             TrickCurveModel* curveModel,
                             const QModelIndex& curveIdx)
{
    _curve2path.insert(curveModel,path);

    // Make a scaled box around path just created
    double xScale = _xScale(curveModel,curveIdx);
    double yScale = _yScale(curveModel,curveIdx);
    QRectF pathBox = path->boundingRect();
    double w = pathBox.width();
    double h = pathBox.height();
    QPointF topLeft(xScale*pathBox.topLeft().x(),
                    (yScale*pathBox.topLeft().y()));
    QRectF scaledPathBox(topLeft,QSizeF(xScale*w,yScale*h));

    // Update current bounding box
    _currBBox = _currBBox.united(scaledPathBox);

    // TODO: even if zoomed in, this will reset the view
    _setPlotMathRect(_currBBox);
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

void CurvesView::_updateUnits(const QModelIndex &curveIdx, QChar axis) const
{
    QModelIndex axisLabelIdx;
    QModelIndex curveUnitIdx;
    if ( axis == 'x' ) {
        axisLabelIdx = _bookModel()->getDataIndex(_myIdx,
                                                  "PlotXAxisLabel", "Plot");
        curveUnitIdx = _bookModel()->getDataIndex(curveIdx,
                                                  "CurveXUnit","Curve");
    } else if ( axis == 'y' ) {
        axisLabelIdx = _bookModel()->getDataIndex(_myIdx,
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
            axisLabel.replace(openCurly+1,n,_curvesUnit(_myIdx,axis));
        } else {
            axisLabel += " {" + _curvesUnit(_myIdx,axis) + "}";
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
    } else if (  event->button() == Qt::MidButton ) {
        event->ignore();
    } else if ( event->button() == Qt::RightButton ) {
        _setPlotMathRect(_currBBox);
        viewport()->update();
    }
}

void CurvesView::mouseMoveEvent(QMouseEvent *mouseMove)
{
    if ( mouseMove->buttons() == Qt::LeftButton ) {
        int Ww = viewport()->width();
        int Wh = viewport()->height();
        if ( Ww != 0 && Wh != 0 ) {
            QRectF M = _mathRect();
            double Mw = M.width();
            double Mh = M.height();
            QTransform T(Mw/Ww, 0.0,
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
        }
    } else {
        mouseMove->ignore();
    }
}

double CurvesView::_xScale(TrickCurveModel* curveModel,
                           const QModelIndex& curveIdx) const
{
    double xs;

    QModelIndex curveXUnitIdx = _bookModel()->getDataIndex(curveIdx,
                                                          "CurveXUnit","Curve");
    QString bookXUnit = model()->data(curveXUnitIdx).toString();
    QString loggedXUnit = curveModel->x()->unit();
    xs = Unit::convert(1.0,
                       loggedXUnit.toAscii().constData(),
                       bookXUnit.toAscii().constData());

    return xs;
}

double CurvesView::_yScale(TrickCurveModel* curveModel,
                           const QModelIndex& curveIdx) const
{
    double ys;

    QModelIndex curveYUnitIdx = _bookModel()->getDataIndex(curveIdx,
                                                          "CurveYUnit","Curve");
    QString bookYUnit = model()->data(curveYUnitIdx).toString();
    QString loggedYUnit = curveModel->y()->unit();
    ys = Unit::convert(1.0,
                       loggedYUnit.toAscii().constData(),
                       bookYUnit.toAscii().constData());

    return ys;
}
