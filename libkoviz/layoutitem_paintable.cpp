#include "layoutitem_paintable.h"

PaintableLayoutItem::PaintableLayoutItem() : QLayoutItem()
{
}

// Return R's math rect given curves rect (C) and curves math rect (M)
QRectF PaintableLayoutItem::mathRect(const QRect &R,
                                     const QRect &C, const QRectF &M)
{

    QRectF RM = M;

    if ( R != C ) {

        double dotWidth  = ( C.width() > 0 )  ? M.width()/C.width()   : 0 ;
        double dotHeight = ( C.height() > 0 ) ? M.height()/C.height() : 0 ;
        QPointF v = C.topLeft()-R.topLeft();
        double ox = dotWidth*v.x();
        double oy = dotHeight*v.y();
        double rmw = dotWidth*R.width();
        double rmh = dotHeight*R.height();
        QPointF pt(M.x()-ox, M.y()-oy);
        RM = QRectF(pt.x(),pt.y(), rmw, rmh);
    }

    return RM;
}

// Given window rect (R) and the mathrect associated with R (RM)
// return a math coord to window coord transformation
QTransform PaintableLayoutItem::coordToDotTransform(const QRect &R,
                                                    const QRectF &RM)
{
    double a = R.width()/RM.width();
    double b = R.height()/RM.height();
    double c = R.x() - a*RM.x();
    double d = R.y() - b*RM.y();

    QTransform T( a,    0,
                  0,    b, /*+*/ c,    d);

    return T;
}
