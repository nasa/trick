#ifndef LAYOUTITEM_PAINTABLE_H
#define LAYOUTITEM_PAINTABLE_H

#include <QLayoutItem>
#include <QPainter>
#include <QRect>
#include <QRectF>
#include <QTransform>

class PaintableLayoutItem : public QLayoutItem
{
public:
    PaintableLayoutItem();

    // R is the child rectangle to paint on
    // RG is the child rectangle in global coordinates
    // C is the curves rect in global coordinates
    // M is the curve's math rectangle
    virtual void paint(QPainter* painter,
                       const QRect& R, const QRect& RG,
                       const QRect& C, const QRectF& M) = 0;

    static QRectF mathRect(const QRect& R, const QRect& C, const QRectF& M);

    static QTransform coordToDotTransform(const QRect& R, const QRectF& RM);
};

#endif // LAYOUTITEM_PAINTABLE_H
