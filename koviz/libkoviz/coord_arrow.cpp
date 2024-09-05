#include "coord_arrow.h"

CoordArrow::CoordArrow() :
    coord(QPointF(DBL_MAX,DBL_MAX)),
    r(2.0),
    h(16.0),
    a(48.0),
    b(18.0),
    m(4.0),
    angle(M_PI/4),
    tipAngle(M_PI/8)
{
}

CoordArrow::CoordArrow(const QPointF &coord,
                       double r, double h,
                       double a, double b, double m,
                       double angle, double tipAngle) :
    coord(coord),        // math coord of pt to draw arrow to
    r(r),                // radius of circle around point
    h(h),                // isoscelese triangle arrow head height
    a(a),                // arrow tail length part between arrow head 'b'
    b(b),                // arrow tail length part between 'a' and txt
    m(m),                // distance (margin) between tail and text
    angle(angle),
    tipAngle(tipAngle)
{
}

// T is coordToPix transform
QRectF CoordArrow::boundingBox(const QPainter& painter,
                               const QTransform& T) const
{
    QRectF bbox;

    // Map math coord to window pt
    QPointF pt = T.map(coord);

    // Text bbox width and height
    double tw = painter.fontMetrics().boundingRect(txt).width();
    double th = painter.fontMetrics().boundingRect(txt).height();

    double aw = (h+a)*qAbs(cos(angle)) + b + m + tw; //total arrow width (w/txt)
    double ah = (h+a)*qAbs(sin(angle)) + th/2.0;     //total arrow height
    QPointF atl; // top left point of arrow bbox
    if ( angle > 0.0 && angle < M_PI/2.0 ) {
        // Quadrant I
        atl.setX(pt.x()+r*cos(angle));
        atl.setY(pt.y()-(r+h+a)*sin(angle)-th/2.0); // neg since +y down
    } else if ( angle > M_PI/2.0 && angle < M_PI ) {
        // Quadrant II
        atl.setX(pt.x()+(r+h+a)*cos(angle)-m-b-tw);
        atl.setY(pt.y()-(r+h+a)*sin(angle)-th/2.0);
    } else if ( angle > M_PI && angle < 3*(M_PI/2.0) ) {
        // Quadrant III
        atl.setX(pt.x()+(r+h+a)*cos(angle)-m-b-tw);
        atl.setY(pt.y()-r*sin(angle));
    } else if ( angle > 3*(M_PI/2.0) && angle < 2*M_PI ) {
        // Quadrant IV
        atl.setX(pt.x()+r*cos(angle));
        atl.setY(pt.y()-r*sin(angle));
    } else {
        fprintf(stderr,"koviz [bad scoobs]: CoorArrow::boundingBox(): "
                       "arrow angle <=0,==90,==270 or >=360. "
                       "May want to support it.  Bailing!!!\n");
        exit(-1);
    }

    bbox.setTopLeft(atl);
    bbox.setSize(QSize(aw,ah));

    return bbox;
}

void CoordArrow::paintMe(QPainter &painter, const QTransform &T,
                         const QColor& fg, const QColor& bg) const
{
    // Save painter
    painter.save();
    QBrush origBrush = painter.brush();
    QPen origPen = painter.pen();

    // Map math coord to window pt
    QPointF pt = T.map(coord);

    double tw = painter.fontMetrics().boundingRect(txt).width();
    double th = painter.fontMetrics().boundingRect(txt).height();

    QRectF txtBox;
    QVector<QPointF> ptsArrowHead;
    QVector<QPointF> ptsArrowTail;
    if ( angle > 0.0 && angle < M_PI/2.0 ) {
        // Quadrant I
        QPointF tip(pt.x()+r*cos(angle),
                    pt.y()-r*sin(angle));  // note: minus since +y is down
        QPointF p(tip.x()+h*cos(angle+tipAngle/2.0),
                  tip.y()-h*sin(angle+tipAngle/2.0));
        QPointF q(tip.x()+h*cos(angle-tipAngle/2.0),
                  tip.y()-h*sin(angle-tipAngle/2.0));
        ptsArrowHead << tip << q << p;

        QPointF a0(tip.x()+h*cos(angle),
                   tip.y()-h*sin(angle));
        QPointF a1(a0.x()+a*cos(angle),
                   a0.y()-a*sin(angle));
        QPointF a2(a1.x()+b,a1.y());
        ptsArrowTail << a0 << a1 << a2;

        QPointF tl(a2.x()+m,
                   a2.y()-th/2.0);
        txtBox.setTopLeft(tl);
        txtBox.setSize(QSize(tw,th));

    } else if ( angle > M_PI/2.0 && angle < M_PI ) {
        // Quadrant II
        QPointF tip(pt.x()+r*cos(angle),
                    pt.y()-r*sin(angle));  // note: minus since +y is down
        QPointF p(tip.x()+h*cos(angle+tipAngle/2.0),
                  tip.y()-h*sin(angle+tipAngle/2.0));
        QPointF q(tip.x()+h*cos(angle-tipAngle/2.0),
                  tip.y()-h*sin(angle-tipAngle/2.0));
        ptsArrowHead << tip << q << p;

        QPointF a0(tip.x()+h*cos(angle),
                   tip.y()-h*sin(angle));
        QPointF a1(a0.x()+a*cos(angle),
                   a0.y()-a*sin(angle));
        QPointF a2(a1.x()-b,a1.y());
        ptsArrowTail << a0 << a1 << a2;

        QPointF tl(a2.x()-m-tw,
                   a2.y()-th/2.0);
        txtBox.setTopLeft(tl);
        txtBox.setSize(QSize(tw,th));

    } else if ( angle > M_PI && angle < 3*(M_PI/2.0) ) {
        // Quadrant III
        QPointF tip(pt.x()+r*cos(angle),
                    pt.y()-r*sin(angle));  // note: minus since +y is down
        QPointF p(tip.x()+h*cos(angle+tipAngle/2.0),
                  tip.y()-h*sin(angle+tipAngle/2.0));
        QPointF q(tip.x()+h*cos(angle-tipAngle/2.0),
                  tip.y()-h*sin(angle-tipAngle/2.0));
        ptsArrowHead << tip << q << p;

        QPointF a0(tip.x()+h*cos(angle),
                   tip.y()-h*sin(angle));
        QPointF a1(a0.x()+a*cos(angle),
                   a0.y()-a*sin(angle));
        QPointF a2(a1.x()-b,a1.y());
        ptsArrowTail << a0 << a1 << a2;

        QPointF tl(a2.x()-m-tw, a2.y()-th/2.0);
        txtBox.setTopLeft(tl);
        txtBox.setSize(QSize(tw,th));
    } else if ( angle > 3*(M_PI/2.0) && angle < 2*M_PI ) {
        // Quadrant IV
        QPointF tip(pt.x()+r*cos(angle),
                    pt.y()-r*sin(angle));  // note: minus since +y is down
        QPointF p(tip.x()+h*cos(angle+tipAngle/2.0),
                  tip.y()-h*sin(angle+tipAngle/2.0));
        QPointF q(tip.x()+h*cos(angle-tipAngle/2.0),
                  tip.y()-h*sin(angle-tipAngle/2.0));
        ptsArrowHead << tip << q << p;

        QPointF a0(tip.x()+h*cos(angle),
                   tip.y()-h*sin(angle));
        QPointF a1(a0.x()+a*cos(angle),
                   a0.y()-a*sin(angle));
        QPointF a2(a1.x()+b,a1.y());
        ptsArrowTail << a0 << a1 << a2;

        QPointF tl(a2.x()+m,a2.y()-th/2.0);
        txtBox.setTopLeft(tl);
        txtBox.setSize(QSize(tw,th));
    } else {
        fprintf(stderr,"koviz [bad scoobs]: CoorArrow::paintMe(): "
                       "arrow angle <=0,==90,==270 or >=360. "
                       "May want to support it.  Bailing!!!\n");
        exit(-1);
    }

    // Draw circle around point
    painter.setPen(fg);
    painter.setBrush(bg);
    painter.drawEllipse(pt,qRound(r),qRound(r));

    // Draw arrow head (tip on circle, not on point)
    QPolygonF arrowHead(ptsArrowHead);
    painter.setPen(fg);
    painter.setBrush(fg);
    painter.drawConvexPolygon(arrowHead);

    // Draw arrow tail (attached to triangle)
    QPolygonF polyLine(ptsArrowTail);
    painter.setPen(fg);
    painter.setBrush(fg);
    painter.drawPolyline(polyLine);

    // Draw background for text box
    painter.setPen(bg);
    painter.setBrush(bg);
    painter.drawRect(txtBox);

    // Draw coord text i.e. (x,y)
    painter.setPen(fg);
    painter.setBrush(bg);
    painter.drawText(txtBox,Qt::TextDontClip|Qt::AlignCenter,txt);

    // Restore painter
    painter.setPen(origPen);
    painter.setBrush(origBrush);
    painter.restore();
}

void CoordArrow::paintMeCenter(QPainter &painter,
                               const QTransform &T, const QRect &viewportRect,
                               const QColor &fg, const QColor &bg) const
{
    if (painter.fontMetrics().boundingRect(txt).width() > viewportRect.width()){
        return;  // don't paint if coord txt will not fit in window
    }

    // Map math coord to window pt
    QPointF pt = T.map(coord);

    if ( !viewportRect.contains(pt.toPoint()) ) {
        return; // don't paint if coord outside of window
    }

    // Save/set painter
    painter.save();
    QBrush origBrush = painter.brush();
    QPen origPen = painter.pen();

    // Init txtbox
    QRectF txtbox = painter.fontMetrics().boundingRect(txt);
    double tw = txtbox.width();
    double th = txtbox.height();

    // Set txtbox top left corner and arrow tail points
    QVector<QPointF> ptsArrowTail;
    QPointF a0;
    QPointF a1;
    QPointF a2;
    if ( pt.y() < viewportRect.height()/2 ) {
        // pt is in top half of viewport, so draw coord at bot of plot
        QPointF tl(viewportRect.width()/2.0-tw/2.0,
                   viewportRect.height()-th);
        txtbox.moveTopLeft(tl);

        a0.setX(txtbox.center().x());
        a0.setY(txtbox.top()-m);
        a1.setX(a0.x());
        a1.setY(a0.y()-b);
        a2.setX(pt.x());
        a2.setY(pt.y());

    } else {
        // pt is in bot half of viewport, so draw coord at top of plot
        QPointF tl(viewportRect.width()/2.0-tw/2.0,0);
        txtbox.moveTopLeft(tl);

        a0.setX(txtbox.center().x());
        a0.setY(txtbox.bottom()+m);
        a1.setX(a0.x());
        a1.setY(a0.y()+b);
        a2.setX(pt.x());
        a2.setY(pt.y());
    }
    ptsArrowTail << a0 << a1 << a2;

    // Draw background for text box
    painter.setPen(bg);
    painter.setBrush(bg);
    painter.drawRect(txtbox);

    // Draw coord text i.e. (x,y)
    painter.setPen(fg);
    painter.setBrush(bg);
    painter.drawText(txtbox,Qt::TextDontClip|Qt::AlignCenter,txt);

    // Draw arrow tail
    painter.setPen(fg);
    painter.setBrush(bg);
    painter.drawPolyline(ptsArrowTail);

    // Calculate arrow angle
    double dx = a2.x()-a1.x();
    double dy = -1*(a2.y()-a1.y());         // -1 since y+ is negative
    double arrowAngle = atan2(dy,dx)+M_PI;  // +pi to flip arrow head

    // Draw arrow head
    QVector<QPointF> ptsArrowHead;
    QPointF tip(pt.x()+r*cos(arrowAngle),
                pt.y()-r*sin(arrowAngle));  // note: minus since +y is down
    QPointF p(tip.x()+h*cos(arrowAngle+tipAngle/2.0),
              tip.y()-h*sin(arrowAngle+tipAngle/2.0));
    QPointF q(tip.x()+h*cos(arrowAngle-tipAngle/2.0),
              tip.y()-h*sin(arrowAngle-tipAngle/2.0));
    ptsArrowHead << tip << q << p;
    QPolygonF arrowHead(ptsArrowHead);
    painter.setPen(fg);
    painter.setBrush(fg);
    painter.drawConvexPolygon(arrowHead);

    // Draw circle around point
    painter.setPen(fg);
    painter.setBrush(bg);
    painter.drawEllipse(pt,qRound(r),qRound(r));

    // Restore painter
    painter.setPen(origPen);
    painter.setBrush(origBrush);
    painter.restore();
}
