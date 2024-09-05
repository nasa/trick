#ifndef LAYOUTITEM_YTICLABELS_H
#define LAYOUTITEM_YTICLABELS_H

#include <QFontMetrics>
#include <QList>
#include <cmath>
#include "layoutitem_paintable.h"
#include "bookmodel.h"

struct RulerLabelBox
{
    QPointF center; // preferred center
    QString strVal;
    QRectF  bb;     // actual center is center of bb
};


class YTicLabelsLayoutItem : public PaintableLayoutItem
{
public:
    YTicLabelsLayoutItem(const QFontMetrics& fontMetrics,
                         PlotBookModel* bookModel,
                         const QModelIndex& plotIdx);
    ~YTicLabelsLayoutItem();
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

    QList<double> _majorYTics(const QModelIndex &plotIdx) const;
    QRectF _plotMathRect(const QModelIndex& plotIdx) const;
    QModelIndex _plotMathRectIdx(const QModelIndex &plotIdx) const;
    QString _format(double tic) const;
    QRect _boundingRect(const QString &strVal) const;
    void _paint10Exponent(QPainter* painter,
                          const RulerLabelBox& box) const;
};

#endif // LAYOUTITEM_YTICLABELS_H
