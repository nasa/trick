#include "kplot.h"
#include <math.h>

KPlot::KPlot(KPlotModel* kPlotModel,
             const QModelIndex& plotIdx,
             QWidget *parent) :
    QWidget(parent),
    _kPlotModel(kPlotModel),
    _plotIdx(plotIdx)
{
    PlotBookModel* bookModel = kPlotModel->bookModel();
    connect(bookModel,SIGNAL(rowsInserted(QModelIndex,int,int)),
            this, SLOT(rowsInserted(QModelIndex,int,int)));
}

KPlot::~KPlot()
{
    // TODO: make virtual???
    foreach ( QPainterPath* path, _curve2path.values() ) {
        delete path;
    }
}

void KPlot::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);
    //_paintSinTest();
    //_paintSinTestPattern();
    _paintTrickCurves();
}

void KPlot::_paintTrickCurves()
{
    if ( !_kPlotModel ) return;

    QPainter painter(this);

    //if ( _kPlotModel->curvePointSize(this) > 0 ) {
        //painter.setRenderHint(QPainter::Antialiasing);
    //}
    painter.setRenderHint(QPainter::Antialiasing);

    QTransform T = _kPlotModel->coordToPixelTransform(this);
    double ptSizeCurve = _kPlotModel->curvePointSize(this);
    QColor curveBlue(32,64,172);
    QPen pen(curveBlue);
    pen.setWidthF(ptSizeCurve);

    // Draw!
    painter.save();
    painter.setTransform(T);
    painter.setPen(pen);
    foreach ( QPainterPath* path, _curve2path.values() ) {
        painter.drawPath(*path);
    }
    painter.restore();

    // TODO
#if 0
    // Save the painter state on the painter stack
    painter->save();

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

    // Restore the painter state off the painter stack
    painter->restore();
#endif
}

QSize KPlot::minimumSizeHint() const
{
    QSize s(50,50);
    return s;
}

QSize KPlot::sizeHint() const
{
    QSize s(200,200);
    return s;
}


QPainterPath KPlot::_sinPath()
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

void KPlot::rowsInserted(const QModelIndex &pidx,
                         int start, int end)
{
    const QAbstractItemModel* model = pidx.model();

    if ( !model ) return;
    if ( model != (QAbstractItemModel*)(_kPlotModel->bookModel()) ) return;


    PlotBookModel* bookModel = _kPlotModel->bookModel();
    QModelIndex plotIdx = bookModel->getIndex(pidx,"Plot","Plot");

    if ( plotIdx != _plotIdx ) return;  // not my plot!

    for ( int i = start; i <= end; ++i ) {
        QModelIndex idx = bookModel->index(i,0,pidx);
        QStandardItem* item = bookModel->itemFromIndex(idx);
        if ( item->text() == "CurveData" ) {
            QVariant v = bookModel->data(idx,Qt::UserRole);
            TrickCurveModel* curveModel =
                    QVariantToPtr<TrickCurveModel>::convert(v);
            _createPainterPath(curveModel);
            break;
        }
    }
}

void KPlot::_createPainterPath(TrickCurveModel *curveModel)
{
    //_isPainterPathCreated = true;
    //_painterPath = QPainterPath();
    QPainterPath* path = new QPainterPath;
    _curve2path.insert(curveModel,path);

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
}

void KPlot::_paintSinTest()
{
    if ( !_kPlotModel ) return;

    QPainter painter(this);

    if ( _kPlotModel->curvePointSize(this) > 0 ) {
        painter.setRenderHint(QPainter::Antialiasing);
    }

    QTransform T = _kPlotModel->coordToPixelTransform(this);
    double ptSizeCurve = _kPlotModel->curvePointSize(this);
    QColor curveBlue(32,112,172);
    QPen pen(curveBlue);
    pen.setWidthF(ptSizeCurve);
    QPainterPath path = _sinPath();

    // Draw!
    painter.save();
    painter.setTransform(T);
    painter.setPen(pen);
    painter.drawPath(path);
    painter.restore();
}

void KPlot::_paintSinTestPattern()
{
    if ( !_kPlotModel ) return;

    QPainter painter(this);

    QTransform T = _kPlotModel->coordToPixelTransform(this);

    double ptSizeHLine = _kPlotModel->hLinePointSize(this);
    double ptSizeVLine = _kPlotModel->vLinePointSize(this);
    double ptSizeCurve = _kPlotModel->curvePointSize(this);

    //
    // Turn on antialiasing (works better with thin lines)
    //
    //painter.setRenderHint(QPainter::Antialiasing);

    //
    // Create diff pens for horiz line, vert line and curves
    //
    QPen hPen(Qt::black);
    hPen.setWidthF(ptSizeHLine);
    QPen vPen(Qt::black);
    vPen.setWidthF(ptSizeVLine);
    QPen cPen(Qt::red);
    cPen.setWidthF(ptSizeCurve);
    QPen piPen(Qt::red);
    piPen.setWidthF(ptSizeVLine);
    QPen dPen(Qt::cyan);
    dPen.setWidthF(ptSizeCurve);

    //
    // Create outter box, diagonals and cross
    //
    QVector<QPointF> a,b,c,d,e,f, xaxis, ypi, hlines, vlines, dlines;
    a << QPointF(0,1)       << QPointF(2*M_PI,1);
    b << QPointF(2*M_PI,1)  << QPointF(2*M_PI,-1);
    c << QPointF(2*M_PI,-1) << QPointF(0,-1);
    d << QPointF(0,-1)      << QPointF(0,1);
    e << QPointF(0,-1)      << QPointF(2*M_PI,1);
    f << QPointF(0,1)       << QPointF(2*M_PI,-1);
    xaxis << QPointF(0,0)   << QPointF(2*M_PI,0);
    ypi << QPointF(M_PI,-1) << QPointF(M_PI,1);
    hlines << a << c << xaxis ;
    vlines << b << d << ypi;
    dlines << e << f ;

    // Make x = pi axis
    QPainterPath piPath;
    piPath.moveTo(M_PI,-1);
    piPath.lineTo(M_PI,1);

    // Sin path
    QPainterPath path = _sinPath();

    //
    // Draw!
    //
    painter.save();
    painter.setTransform(T);
    painter.setPen(hPen);
    painter.drawLines(hlines);
    painter.setPen(vPen);
    painter.drawLines(vlines);
    painter.setPen(dPen);
    painter.drawLines(dlines);
    painter.setPen(cPen);
    painter.drawPath(path);
    painter.setPen(piPen);
    painter.drawPath(piPath);
    painter.restore();
}

