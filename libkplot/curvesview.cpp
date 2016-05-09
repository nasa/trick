#include "curvesview.h"

CurvesView::CurvesView(QWidget *parent) :
    BookIdxView(parent)
{
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
    QColor curveBlue(32,64,172);
    QPen pen(curveBlue);
    pen.setWidthF(ptSizeCurve);

    // Draw!
    painter.setTransform(T);
    painter.setPen(pen);
    foreach ( QPainterPath* path, _curve2path.values() ) {
        painter.drawPath(*path);
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
    if ( start != end ) return;

    PlotBookModel* bookModel = _bookModel();

    QModelIndex plotIdx = bookModel->getIndex(pidx,"Plot","Plot");

    if ( plotIdx != _myIdx ) return;  // not my plot!

    for ( int i = start; i <= end; ++i ) {
        QModelIndex idx = bookModel->index(i,0,pidx);
        QString s = bookModel->data(idx).toString();
        if ( s == "CurveData" ) {
            QVariant v = bookModel->data(idx,Qt::UserRole);
            TrickCurveModel* curveModel =
                    QVariantToPtr<TrickCurveModel>::convert(v);
            QPainterPath* path = _createPainterPath(curveModel);
            _curve2path.insert(curveModel,path);
            QRectF M = _bbox();
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
    double dt = 0.01;
    while (1) {
        t += dt;
        if ( t > 2*M_PI-0.01 ) {
            t = 2*M_PI;
        }
        path.lineTo(t,sin(t));
        if ( t > 2*M_PI-0.01 ) break;
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
