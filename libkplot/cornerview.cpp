#include "cornerview.h"

CornerView::CornerView(Qt::Corner corner, QWidget *parent) :
    BookIdxView(parent), _corner(corner)
{
    setFrameShape(QFrame::NoFrame);
}

void CornerView::_update()
{
}

// TODO: We could get fancy and, optionally, round the corners
void CornerView::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    if ( !model() ) return;

    QPainter painter(viewport());

    QTransform T = _coordToPixelTransform();

    double ptSizeHLine = _hLinePointSize();
    double ptSizeVLine = _vLinePointSize();

    QPen hPen(Qt::black);
    hPen.setWidthF(ptSizeHLine);
    QPen vPen(Qt::black);
    vPen.setWidthF(ptSizeVLine);

    const QRectF M = _mathRect();
    double x0 = M.left();
    double y0 = M.top();
    double x1 = M.right();
    double y1 = M.bottom();

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

QSize CornerView::minimumSizeHint() const
{
    return QSize(8,8);
}

QSize CornerView::sizeHint() const
{
    return QSize(8,8);
}
