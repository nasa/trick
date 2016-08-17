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

    if ( _errorPath ) {
        delete _errorPath;
        _errorPath = 0;
    }
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
            _paintErrorplot(painter,pen,_errorPath,_myIdx);
        } else if ( plotPresentation == "error+coplot" ) {
            _paintErrorplot(painter,pen,_errorPath,_myIdx);
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
        qDebug() << "snap [todo]: Handle case when name/units different "
                    "for x or y";
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

    double ys0 = _yScale(c0,curveIdx0);
    double ys1 = _yScale(c1,curveIdx1);

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

