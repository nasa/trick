#ifndef LAYOUTITEM_PLOTTITLE_H
#define LAYOUTITEM_PLOTTITLE_H

#include <QFontMetrics>
#include "layoutitem_paintable.h"
#include "bookmodel.h"

class PlotTitleLayoutItem : public PaintableLayoutItem
{
public:
    PlotTitleLayoutItem(const QFontMetrics& fontMetrics,
                        PlotBookModel* bookModel,
                        const QModelIndex& plotIdx);
    ~PlotTitleLayoutItem();
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
    const QFontMetrics _fontMetrics;
    PlotBookModel* _bookModel;
    QModelIndex _plotIdx;
    QRect _rect;
    void _paintGrid(QPainter* painter,
                    const QRect& R, const QRect &RG,
                    const QRect&C, const QRectF& M);
};

#endif // LAYOUTITEM_PLOTTITLE_H
