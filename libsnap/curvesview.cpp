#include "curvesview.h"

CurvesView::CurvesView(QWidget *parent) :
    BookIdxView(parent),
    _errorPath(0)
{
    setFocusPolicy(Qt::StrongFocus);
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

    if ( _errorPath ) delete _errorPath;
}

void CurvesView::_update()
{
    if ( !_myIdx.isValid() ) return;
    // TODO??? do I have to do anything???
}


void CurvesView::paintEvent(QPaintEvent *event)
{
    if ( !model() ) return;

    QModelIndex curvesIdx = _bookModel()->getIndex(_myIdx,"Curves","Plot");
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
        QString plotPresentation = _bookModel()->getDataString(_myIdx,
                                                     "PlotPresentation","Plot");
        if ( plotPresentation == "coplot" ) {
            _paintCoplot(T,painter,pen);
        } else if (plotPresentation == "error" || plotPresentation.isEmpty()) {
            _paintErrorplot(T,painter,pen);
        } else if ( plotPresentation == "error+coplot" ) {
            _paintErrorplot(T,painter,pen);
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
            QColor color( _bookModel()->getDataString(curveIdx,
                                                      "CurveColor","Curve"));
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
        }
    }
}

void CurvesView::_paintErrorplot(const QTransform &T,
                                 QPainter &painter, QPen &pen)
{
    QModelIndex curvesIdx = _bookModel()->getIndex(_myIdx,"Curves","Plot");
    int rc = model()->rowCount(curvesIdx);
    if ( rc != 2 ) {
        qDebug() << "snap [bad scoobs]: _paintErrorplot()";
        exit(-1);
    }

    QModelIndex i0 = model()->index(0,0,curvesIdx);
    QModelIndex i1 = model()->index(1,0,curvesIdx);

    QString curve0XName = _bookModel()->getDataString(i0,"CurveXName","Curve");
    QString curve0XUnit = _bookModel()->getDataString(i0,"CurveXName","Curve");
    QString curve0YName = _bookModel()->getDataString(i0,"CurveYName","Curve");
    QString curve0YUnit = _bookModel()->getDataString(i0,"CurveYName","Curve");

    QString curve1XName = _bookModel()->getDataString(i1,"CurveXName","Curve");
    QString curve1XUnit = _bookModel()->getDataString(i1,"CurveXName","Curve");
    QString curve1YName = _bookModel()->getDataString(i1,"CurveYName","Curve");
    QString curve1YUnit = _bookModel()->getDataString(i1,"CurveYName","Curve");


    // TODO: indicate if error curve is flat,
    // TODO: need to put name of time in model
    //       so user has settable timename (not hardcoded "sys.exec.out.time")
    // TODO: error plot when x is not time e.g. x/y position
    //       d=sqrt((x0-x1)*(x0-x1)+(y0-y1)*(y0-y1))
    QString timeName("sys.exec.out.time");
    if ( curve0XName == timeName &&
         curve0XName == curve1XName && curve0XUnit == curve1XUnit &&
         curve0YName == curve1YName && curve0YUnit == curve1YUnit ) {
        //
        // Normal case: x and y params/units match and x is time
        //

        // Magenta for difference/error curve
        pen.setColor(_colorBandsNormal.at(2));
        painter.setPen(pen);

        // Draw error curve!
        painter.drawPath(*_errorPath);

    } else {
        // TODO: If name/units different for x or y, handle here
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

    if ( tag == "CurveData" && topLeft_ppp == _myIdx ) {
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
                    // normal case
                    otherCurveIdx = model()->index(0,0,curvesIdx);
                } else if ( curveIdx.row() == 0 ) {
                    otherCurveIdx = model()->index(1,0,curvesIdx);
                } else {
                    qDebug() << "snap [bad scoobs]: "
                                "CurvesView::dataChanged(), nCurves==2";
                    exit(-1);
                }
                _errorPath = _createErrorPath(otherCurveIdx,curveIdx);
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
    } else if ( tag == "CurveXUnit" && topLeft_ppp == _myIdx) {
        QModelIndex curveIdx = topLeft.parent();
        _updateAxisLabelUnits(curveIdx,'x');
        QRectF prevBBox = _currBBox;
        _currBBox = _calcBBox();
        if ( prevBBox.width() > 0 ) {
            double xs = _currBBox.width()/prevBBox.width();
            QRectF M = _plotMathRect(_myIdx);
            double w = M.width();
            double h = M.height();
            QPointF topLeft(xs*M.topLeft().x(), M.topLeft().y());
            QRectF scaledM(topLeft,QSizeF(xs*w,h));
            _setPlotMathRect(scaledM);
        }
    } else if ( tag == "CurveYUnit" && topLeft_ppp == _myIdx) {
        QModelIndex curveIdx = topLeft.parent();
        _updateAxisLabelUnits(curveIdx,'y');
        QRectF prevBBox = _currBBox;
        _currBBox = _calcBBox();
        if ( prevBBox.height() > 0 ) {
            double ys = _currBBox.height()/prevBBox.height();
            QRectF M = _plotMathRect(_myIdx);
            double w = M.width();
            double h = M.height();
            QPointF topLeft(M.topLeft().x(), ys*M.topLeft().y());
            QRectF scaledM(topLeft,QSizeF(w,ys*h));
            _setPlotMathRect(scaledM);
        }
    } else if ( tag == "PlotPresentation" && topLeft.parent() == _myIdx ) {
        if ( _bookModel()->isChildIndex(_myIdx,"Plot","Curves") ) {
            QModelIndex curvesIdx = _bookModel()->getIndex(_myIdx,
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
    if ( pidx.parent().parent() != _myIdx ) return; // not my plot

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
                    QModelIndex otherCurveIdx;
                    if ( curveIdx.row() == 1 ) {
                        // normal case
                        otherCurveIdx = model()->index(0,0,curvesIdx);
                    } else if ( curveIdx.row() == 0 ) {
                        otherCurveIdx = model()->index(1,0,curvesIdx);
                    } else {
                        qDebug() << "snap [bad scoobs]: "
                                    "CurvesView::rowsInserted, nCurves==2";
                        exit(-1);
                    }
                    _errorPath = _createErrorPath(otherCurveIdx,curveIdx);
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

QPainterPath *CurvesView::_createErrorPath(const QModelIndex &curve0Idx,
                                           const QModelIndex &curve1Idx)
{
    QPainterPath* path = new QPainterPath;

    QModelIndex curveData0Idx = _bookModel()->getDataIndex(curve0Idx,
                                                           "CurveData","Curve");
    QVariant v0 = model()->data(curveData0Idx);
    TrickCurveModel* model0 = QVariantToPtr<TrickCurveModel>::convert(v0);

    QModelIndex curveData1Idx = _bookModel()->getDataIndex(curve1Idx,
                                                           "CurveData","Curve");
    QVariant v1 = model()->data(curveData1Idx);
    TrickCurveModel* model1 = QVariantToPtr<TrickCurveModel>::convert(v1);

    double ys0 = _yScale(model0,curve0Idx);
    double ys1 = _yScale(model1,curve1Idx);

    if ( model0 != 0 && model1 != 0 ) {

        model0->map();
        model1->map();

        TrickModelIterator it0 = model0->begin();
        TrickModelIterator it1 = model1->begin();
        const TrickModelIterator e0 = model0->end();
        const TrickModelIterator e1 = model1->end();
        bool isFirst = true;
        while (it0 != e0 && it1 != e1) {
            double t0 = it0.t();
            double t1 = it1.t();
            if ( qAbs(t1-t0) < 0.000001 ) {
                double d = ys0*it0.y() - ys1*it1.y();
                if ( isFirst ) {
                    path->moveTo(t0,d);
                    isFirst = false;
                } else {
                    path->lineTo(t0,d);
                }
                ++it0;
                ++it1;
            } else {
                if ( t0 < t1 ) {
                    ++it0;
                } else if ( t1 < t0 ) {
                    ++it1;
                } else {
                    qDebug() << "snap [bad scoobs]: _createErrorPath()";
                    exit(-1);
                }
            }
        }

        model0->unmap();
        model1->unmap();

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
    double xs = 1.0;

    QModelIndex curveXUnitIdx = _bookModel()->getDataIndex(curveIdx,
                                                          "CurveXUnit","Curve");
    QString bookXUnit = model()->data(curveXUnitIdx).toString();
    if ( !bookXUnit.isEmpty() && bookXUnit != "--" ) {
        QString loggedXUnit = curveModel->x()->unit();
        xs = Unit::convert(1.0,
                           loggedXUnit.toAscii().constData(),
                           bookXUnit.toAscii().constData());
    }

    return xs;
}

double CurvesView::_yScale(TrickCurveModel* curveModel,
                           const QModelIndex& curveIdx) const
{
    double ys = 1.0;

    QModelIndex curveYUnitIdx = _bookModel()->getDataIndex(curveIdx,
                                                          "CurveYUnit","Curve");
    QString bookYUnit = model()->data(curveYUnitIdx).toString();
    if ( !bookYUnit.isEmpty() && bookYUnit != "--" ) {
        QString loggedYUnit = curveModel->y()->unit();
        ys = Unit::convert(1.0,
                           loggedYUnit.toAscii().constData(),
                           bookYUnit.toAscii().constData());
    }

    return ys;
}

QRectF CurvesView::_calcBBox() const
{
    QRectF bbox;
    QModelIndex curvesIdx = _bookModel()->getIndex(_myIdx,"Curves","Plot");
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
    default: ; // do nothing
    }
}

// For two curves hitting the spacebar will toggle between viewing
// the two curves in error, coplot and error+coplot views
void CurvesView::_keyPressSpace()
{
    QModelIndex curvesIdx = _bookModel()->getIndex(_myIdx,"Curves","Plot");
    int rc = model()->rowCount(curvesIdx);
    if ( rc != 2 ) return;

    QString plotPresentation = _bookModel()->getDataString(_myIdx,
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

    QModelIndex plotPresentationIdx = _bookModel()->getDataIndex(_myIdx,
                                                   "PlotPresentation","Plot");
    model()->setData(plotPresentationIdx,plotPresentation);

    viewport()->update();
}
