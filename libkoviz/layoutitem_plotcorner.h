#ifndef LAYOUTITEM_PLOTCORNER_H
#define LAYOUTITEM_PLOTCORNER_H

#include <QFontMetrics>
#include "layoutitem_paintable.h"


class PlotCornerLayoutItem : public PaintableLayoutItem
{
public:
    PlotCornerLayoutItem(const QFontMetrics& fontMetrics,
                         Qt::Corner corner);
    ~PlotCornerLayoutItem();
    virtual Qt::Orientations expandingDirections() const;
    virtual QRect  geometry() const;
    virtual bool  isEmpty() const;
    virtual QSize  maximumSize() const;
    virtual QSize  minimumSize() const;
    virtual void  setGeometry(const QRect &r);
    virtual QSize  sizeHint() const;
    virtual void paint(QPainter* painter,
                       const QRect& R, const QRect& RG,
                       const QRect& C, const QRectF& M);

private:
    QFontMetrics _fontMetrics;
    Qt::Corner _corner;
    QRect _rect;
};

#endif // LAYOUTITEM_PLOTCORNER_H
