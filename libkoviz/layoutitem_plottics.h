#ifndef LAYOUTITEM_PLOTTICS_H
#define LAYOUTITEM_PLOTTICS_H

#include <QFontMetrics>
#include "layoutitem_paintable.h"
#include "layoutitem_plotcorner.h"
#include "layoutitem_plottics.h"
#include "bookmodel.h"

class PlotTicsLayoutItem : public PaintableLayoutItem
{
public:
    PlotTicsLayoutItem(const QFontMetrics& fontMetrics,
                       PlotBookModel* bookModel,
                       const QModelIndex& plotIdx);
    ~PlotTicsLayoutItem();
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
    PlotBookModel* _bookModel;
    QModelIndex _plotIdx;
    PlotCornerLayoutItem _cornerLayoutItem;
    QRect _rect;
};

#endif // LAYOUTITEM_PLOTTICS_H
