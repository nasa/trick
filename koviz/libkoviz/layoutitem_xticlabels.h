#ifndef LAYOUTITEM_XTICLABELS_H
#define LAYOUTITEM_XTICLABELS_H

#include <QLayoutItem>
#include <QPainter>

class XTicLabelsLayoutItem : public QLayoutItem
{
public:
    XTicLabelsLayoutItem(const QPainter* painter);
    ~XTicLabelsLayoutItem();
    virtual Qt::Orientations expandingDirections() const;
    virtual QRect  geometry() const;
    virtual bool  isEmpty() const;
    virtual QSize  maximumSize() const;
    virtual QSize  minimumSize() const;
    virtual void  setGeometry(const QRect &r);
    virtual QSize  sizeHint() const;

private:
    const QPainter* _painter;
    QRect _rect;
};

#endif // LAYOUTITEM_XTICLABELS_H
