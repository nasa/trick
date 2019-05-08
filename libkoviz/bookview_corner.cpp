#include "bookview_corner.h"

CornerView::CornerView(Qt::Corner corner, QWidget *parent) :
    BookIdxView(parent), _corner(corner)
{
    setFrameShape(QFrame::NoFrame);
    this->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}

void CornerView::setModel(QAbstractItemModel *model)
{
    QAbstractItemView::setModel(model);
    if ( model ) {
        disconnect(model,SIGNAL(rowsInserted(QModelIndex,int,int)),
                   this,SLOT(rowsInserted(QModelIndex,int,int)));
    }
}

void CornerView::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    if ( !model() ) return;

    QPainter painter(viewport());

    double ptSizeHLine = 0.0;
    double ptSizeVLine = 0.0;

    QPen hPen = painter.pen();
    hPen.setWidthF(ptSizeHLine);
    QPen vPen = painter.pen();
    vPen.setWidthF(ptSizeVLine);

    const QRect V = viewport()->rect();
    double x0 = V.left();
    double y0 = V.top();
    double x1 = V.right();
    double y1 = V.bottom();

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
    painter.setPen(hPen);
    painter.drawLines(hlines);
    painter.setPen(vPen);
    painter.drawLines(vlines);
    painter.restore();
}

QSize CornerView::minimumSizeHint() const
{
    return QSize(12,12);
}

QSize CornerView::sizeHint() const
{
    return QSize(12,12);
}
