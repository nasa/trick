#include "plotcorner.h"

PlotCorner::PlotCorner(KPlotModel *plotModel,
                       Qt::Corner corner, QWidget *parent) :
    QWidget(parent),
    _plotModel(plotModel),
    _corner(corner)
{
}

// TODO: We could get fancy and, optionally, round the corners
void PlotCorner::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    if ( !_plotModel ) return;

    QPainter painter(this);

    QTransform T = _plotModel->coordToPixelTransform(this);

    double ptSizeHLine = _plotModel->hLinePointSize(this);
    double ptSizeVLine = _plotModel->vLinePointSize(this);

    QPen hPen(Qt::black);
    hPen.setWidthF(ptSizeHLine);
    QPen vPen(Qt::black);
    vPen.setWidthF(ptSizeVLine);

    const QRectF rect = _plotModel->mathRect(this);
    double x0 = rect.left();
    double y0 = rect.top();
    double x1 = rect.right();
    double y1 = rect.bottom();

    QVector<QPointF> hlines;
    QVector<QPointF> vlines;

    // (x0,y0) *--------------* (x1,y0)
    //         |              |
    //         |              |
    //         |              |
    // (x0,y1) *--------------* (x1,y1)
    if ( _corner == Qt::TopLeftCorner ) {
        hlines << QPointF(x0,y0) << QPointF(x1,y0);
        vlines << QPointF(x0,y0) << QPointF(x0,y1);
    } else if ( _corner == Qt::TopRightCorner ) {
        hlines << QPointF(x1,y0) << QPointF(x0,y0);
        vlines << QPointF(x1,y0) << QPointF(x1,y1);
    } else if ( _corner == Qt::BottomRightCorner ) {
        hlines << QPointF(x1,y1) << QPointF(x0,y1);
        vlines << QPointF(x1,y1) << QPointF(x1,y0);
    } else if ( _corner == Qt::BottomLeftCorner ) {
        hlines << QPointF(x0,y1) << QPointF(x1,y1);
        vlines << QPointF(x0,y1) << QPointF(x0,y0);
    } else {
        // Bad scoobs
    }

    //
    // Draw!
    //
    painter.save();
    painter.setTransform(T);
    painter.setPen(hPen);
    painter.drawLines(hlines);
    painter.setPen(vPen);
    painter.drawLines(vlines);
    painter.restore();
}

QSize PlotCorner::minimumSizeHint() const
{
    return QSize(8,8);
}

QSize PlotCorner::sizeHint() const
{
    return QSize(8,8);
}
