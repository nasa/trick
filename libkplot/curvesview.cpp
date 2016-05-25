#include "curvesview.h"

CurvesView::CurvesView(QWidget *parent) :
    BookIdxView(parent),
    _rubberBand(0)
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
    int i = 0;
    int nCurves = _curve2path.size();
    int nBands = _colorBandsRainbow.size();
    int nCurvesPerBand = div(nCurves,nBands).quot;
    foreach ( QPainterPath* path, _curve2path.values() ) {

        // Color curves
        if ( nCurves < 10 ) {
            pen.setColor(_colorBandsNormal.at(i));
        } else  {
            div_t q = div(i,nCurvesPerBand);
            int j = qMin(q.quot,nBands-1);
            pen.setColor(_colorBandsRainbow.at(j));
        }
        painter.setPen(pen);

        painter.drawPath(*path);

        ++i;
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

void CurvesView::dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight)
{
}

void CurvesView::rowsInserted(const QModelIndex &pidx, int start, int end)
{
    if ( pidx.parent().parent() != _myIdx ) return; // not my plot
    QModelIndex idx = model()->index(start,0,pidx);
    if ( model()->data(idx).toString() != "CurveData" ) return;

    for ( int i = start; i <= end; ++i ) {
        QModelIndex idx = model()->index(i,0,pidx);
        QString s = model()->data(idx).toString();
        if ( s == "CurveData" ) {
            QVariant v = model()->data(idx,Qt::UserRole);
            TrickCurveModel* curveModel =
                    QVariantToPtr<TrickCurveModel>::convert(v);
            QPainterPath* path = _createPainterPath(curveModel);
            _curve2path.insert(curveModel,path);
            QRectF M = _bbox();
            //M.adjust(-0.01,-0.01,0.01,0.01);
            _setPlotMathRect(M);
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

QRectF CurvesView::_bbox()
{
    QRectF bbox;
    foreach ( QPainterPath* path, _curve2path.values() ) {
        bbox = bbox.united(path->boundingRect());
    }
    return bbox;
}

void CurvesView::_setPlotMathRect(const QRectF &M)
{
    QModelIndex plotIdx = _bookModel()->getIndex(_myIdx,"Plot","Plot");
    QModelIndex plotMathRectIdx = _bookModel()->getIndex(plotIdx,
                                                         "PlotMathRect","Plot");
    plotMathRectIdx = plotMathRectIdx.sibling(plotMathRectIdx.row(),1);
    model()->setData(plotMathRectIdx,M);
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

void CurvesView::mousePressEvent(QMouseEvent *event)
{
    if ( event->button() == Qt::MidButton ){
        _rubberBandOrigin = event->pos();
        if ( !_rubberBand ) {
            _rubberBand = new QRubberBand(QRubberBand::Rectangle,
                                          this->viewport());
        }
        _rubberBand->setGeometry(QRect(_rubberBandOrigin, QSize()));
        _rubberBand->show();
    } else  if ( event->button() == Qt::RightButton ){
        //zoomToFit();
    } else {
    }

    QAbstractItemView::mousePressEvent(event);
}

void CurvesView::mouseMoveEvent(QMouseEvent *event)
{
    if ( event->buttons() == Qt::MidButton && _rubberBand ){
        _rubberBand->setGeometry(QRect(_rubberBandOrigin,
                                 event->pos()).normalized());
    }
}

//      ^
//     e3  = (Wox,Woy,1) = (0,0,1)
//    /
//  Wo-e1>-------Ww----------------------------->
//   |\                ^                       /|
//   e2\               |b                     / |
//   v<------a-------->v                     /  |
//   |   \             wo--ww--------->     /   |
//   |    \            /|             |    /    |
//   |     \          / wh            |   /     |
//   |      \        /  v-------------+  /      |
//   |       \      /                   /       |
//   |       Mo----/----Mw------------->        |
//   Wh       |  mo----mw--->          |        |
//   |        |   |mh       |          |        |
//   |       Mh   v---------+          |        |
//   |        |                        |        |
//   |        v------------------------+        |
//   |       /                          \       |
//   |      /                            \      |
//   |     /                              \     |
//   |    /                                \    |
//   |   /                                  \   |
//   |  /                                    \  |
//   | /                                      \ |
//   v-----------------------------------------+
//
//   In a nutshell:
//
//        This is obvious:
//          mw = (Mw/Ww)*ww       (since Ww:Mw as ww:mw)
//          mh = (Mh/Wh)*wh
//
//        The main question is what is T to take wo to mo:
//
//             T?
//          wo -> mo?
//
//   [wo] = +- -+    [mo]  = +- -+
//       W  | a |        M   | ? |
//          | b |            | ? |
//          | 1 |            | 1 |
//          +- -+            +- -+
//
//   [Wo] = +- -+    [Mo]  = +-   -+
//       W  | 0 |        M   | Mox |
//          | 0 |            | Moy |
//          | 1 |            |  1  |
//          +- -+            +-   -+
//
//  Ww, Wh, Wo, Mo, Mw, Mh, a & b are known. What is k11,k12,k21,k22, s & t?
//
//  +-                -+ +- -+   +-                 -+   +- -+
//  | k11?   k12?   s? | | a |   | k11*a + k12*b + s |   | ? |
//  | k21?   k22?   t? | | b | = | k21*a + k22*b + t | = | ? |
//  |   0     0     1  | | 1 |   |        1          |   | 1 |
//  +-                -+ +- -+   +-                 -+   +- -+
//
//  Solution:
//
//  T =  [ T(e1) | T(e2) | T(e3) ]
//
//       +-                 -+
//       | Mw/Ww  0      Mox |
//    =  |   0    Mh/Wh  Moy |
//       |   0    0       1  |
//       +-                 -+
//
// Note:
//       +-                 -+ +- -+   +-   -+
//       | Mw/Ww  0      Mox | | 0 |   | Mox |
//       |   0    Mh/Wh  Moy | | 0 | = | Moy |  works
//       |   0    0       1  | | 1 |   |  1  |
//       +-                 -+ +- -+   +-   -+
//
//       +-                 -+ +-  -+   +-        -+
//       | Mw/Ww  0      Mox | | Ww |   | Mw + Mox |
//       |   0    Mh/Wh  Moy | | Wh | = | Mh + Moy |  works
//       |   0    0       1  | |  1 |   |    1     |
//       +-                 -+ +-  -+   +-        -+
//
// In general:
//       +-                 -+ +- -+   +-               -+
//       | Mw/Ww  0      Mox | | a |   | (Mw/Ww)*a + Mox |
//       |   0    Mh/Wh  Moy | | b | = | (Mh/Wh)*b + Moy |
//       |   0    0       1  | | 1 |   |          1      |
//       +-                 -+ +- -+   +-               -+
//
// So, k12 == k21 == 0
//     k11 = Mw/Ww, k22 = Mh/Wh
//     s = Mox, t = Moy
//
void CurvesView::mouseReleaseEvent(QMouseEvent *event)
{
    if ( event->button() == Qt::MidButton && _rubberBand ) {
        QRect geom = _rubberBand->geometry();
        if ( geom.width() > 20 && geom.height() > 20 ) {
            // If rubberband too small, normally by accidental click, don't zoom
            int Ww = viewport()->width();
            int Wh = viewport()->height();
            if ( Ww == 0 || Wh == 0 ) return;
            QRectF M = _mathRect();
            double Mw = M.width();
            double Mh = M.height();
            double Mox = M.topLeft().x();
            double Moy = M.topLeft().y();
            QTransform T(Mw/Ww, 0.0,      // See giant comment above for details
                         0.0, Mh/Wh,
                         Mox, Moy);
            QPointF wo((double)geom.topLeft().x(),
                       (double)geom.topLeft().y());
            QPointF wbr((double)geom.bottomRight().x(),
                        (double)geom.bottomRight().y());
            QPointF mo  = T.map(wo);
            QPointF mbr = T.map(wbr);
            QRectF mrect(mo,mbr);
            _setPlotMathRect(mrect);
            viewport()->update();
        }
        _rubberBand->hide();
    }
}
