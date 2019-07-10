#ifndef LAYOUTITEM_YAXISLABEL_H
#define LAYOUTITEM_YAXISLABEL_H

#include <QFontMetrics>
#include "layoutitem_paintable.h"
#include "bookmodel.h"

class YAxisLabelLayoutItem : public PaintableLayoutItem
{
public:
    YAxisLabelLayoutItem(const QFontMetrics& fontMetrics,
                         PlotBookModel* bookModel,
                         const QModelIndex& plotIdx);

    ~YAxisLabelLayoutItem();
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
    QRect _rect;
    QString _yAxisLabelText() const;
};

#endif // LAYOUTITEM_YAXISLABEL_H
