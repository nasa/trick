#ifndef PAGETITLELAYOUTITEM_H
#define PAGETITLELAYOUTITEM_H

#include <QStringList>
#include <QFont>
#include "layoutitem_paintable.h"
#include "bookmodel.h"

class PageTitleLayoutItem : public QLayoutItem
{
public:
    PageTitleLayoutItem(PlotBookModel* bookModel,
                        const QModelIndex& pageIdx,
                        const QFont& font);

    ~PageTitleLayoutItem();
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
    PlotBookModel* _bookModel;
    QModelIndex _pageIdx;
    QFont _font;
    QRect _rect;
    QSize _sizeLegend() const;
    QStringList _pageTitles(const QModelIndex &pageIdx) const;
    void _paintPageLegend(const QRect &R,
                          const QModelIndex &curvesIdx,
                          QPainter* painter);
    void __paintPageLegend(const QRect &R,
                           const QList<QPen *> &pens,
                           const QStringList &symbols,
                           const QStringList &labels,
                           QPainter* painter);
    void _paintLegendEntry(const QRect &R,
                           int l,  // line width
                           int s,  // spaceBetweenLineAndLabel
                           const QPen &pen,
                           const QString &symbol,
                           const QString &label,
                           QPainter* painter);
    void __paintSymbol(const QPointF& p,
                       const QString &symbol,
                       QPainter* painter);
};

#endif // PAGETITLELAYOUTITEM_H
