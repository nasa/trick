#include "layoutitem_xticlabels.h"

XTicLabelsLayoutItem::XTicLabelsLayoutItem(const QPainter *painter) :
    _painter(painter)
{
}

XTicLabelsLayoutItem::~XTicLabelsLayoutItem()
{
}

Qt::Orientations XTicLabelsLayoutItem::expandingDirections() const
{
    return Qt::Horizontal;
}

QRect XTicLabelsLayoutItem::geometry() const
{
    return _rect;
}

bool XTicLabelsLayoutItem::isEmpty() const
{
    return true;
}

QSize XTicLabelsLayoutItem::maximumSize() const
{
    return QSize(0,0);
}

QSize XTicLabelsLayoutItem::minimumSize() const
{
    return QSize(0,0);
}

void XTicLabelsLayoutItem::setGeometry(const QRect &r)
{
    _rect = r;
}

QSize XTicLabelsLayoutItem::sizeHint() const
{
    QSize size(0,0);
    QFontMetrics fm = _painter->fontMetrics();
    QRect bb = fm.boundingRect("0123456789");
    size.setHeight(bb.height());
    return size;
}
