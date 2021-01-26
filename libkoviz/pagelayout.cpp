#include "pagelayout.h"

PageLayout::PageLayout() : QLayout()
{
}

PageLayout::~PageLayout()
{
}

void PageLayout::addItem(QLayoutItem *item)
{
    _items << item;
}

void PageLayout::addWidget(QWidget *widget)
{
    _items << new QWidgetItem(widget);
}

int PageLayout::count() const
{
    return _items.size();
}

QLayoutItem *PageLayout::itemAt(int index) const
{
    return _items.value(index,0);
}

void PageLayout::setGeometry(const QRect &rect)
{
    int w = rect.width();
    int h = rect.height();
    int m = rect.width()*0.01;

    // Get PageTitleView height
    int h0 = _items.at(0)->sizeHint().height(); // PageTitleView is item 0
    if ( _items.at(0)->widget() ) {
        // If item is a widget, use it's sizehint since
        // the item's sizehint is not the same as the item's widget sizehint
        h0 = _items.at(0)->widget()->sizeHint().height();
    }
    _items.at(0)->setGeometry(QRect(0,0,w-m,h0));

    QList<QLayoutItem*> items;
    int nItems = _items.count();
    for ( int i = 1; i < nItems; ++i ) {
        QLayoutItem* item =  _items.at(i);
        if ( item->widget() ) {
            if ( !item->widget()->isHidden() ) {
                items << item;
            }
        } else {
            items << item;
        }
    }

    int nPlots = items.size();
    switch ( nPlots ) {
        case 1: {
            items.at(0)->setGeometry(QRect(m,h0,w-2*m,h-h0));
            break;
        }
        case 2: {
            int h1 = (h-h0-2*m)/2;
            int h2 = h-h0-h1-2*m;
            items.at(0)->setGeometry(QRect(m,h0,w-2*m,h1));
            items.at(1)->setGeometry(QRect(m,h0+h1+2*m,w-2*m,h2));
            break;
        }
        case 3: {
            int h1 = (h-h0-4*m)/3;
            int h2 = (h-h0-h1-2*m)/2;
            int h3 = h-h0-h1-h2-4*m;
            items.at(0)->setGeometry(QRect(m,h0,w-2*m,h1));
            items.at(1)->setGeometry(QRect(m,h0+h1+2*m,w-2*m,h2));
            items.at(2)->setGeometry(QRect(m,h0+h1+h2+4*m,w-2*m,h3));
            break;
        }
        case 4: {
            int h1 = (h-h0-2*m)/2;
            int h2 = h-h0-h1-2*m;
            int w1 = (w-3*m)/2;
            int w2 = w-3*m-w1;
            items.at(0)->setGeometry(QRect(m,h0,w1,h1));
            items.at(1)->setGeometry(QRect(m+w1+m,h0,w2,h1));
            items.at(2)->setGeometry(QRect(m,h0+h1+2*m,w1,h2));
            items.at(3)->setGeometry(QRect(m+w1+m,h0+h1+2*m,w2,h2));
            break;
        }
        case 5: {
            int h1 = (h-h0-4*m)/3;
            int h2 = (h-h0-h1-2*m)/2;
            int h3 = h-h0-h1-h2-4*m;
            int w1 = (w-3*m)/2;
            int w2 = w-3*m-w1;
            items.at(0)->setGeometry(QRect(m,h0,w1,h1));
            items.at(1)->setGeometry(QRect(m+w1+m,h0,w2,h1));
            items.at(2)->setGeometry(QRect(m,h0+h1+2*m,w1,h2));
            items.at(3)->setGeometry(QRect(m+w1+m,h0+h1+2*m,w2,h2));
            items.at(4)->setGeometry(QRect(m,h0+h1+h2+4*m,w-2*m,h3));
            break;
        }
        case 6: {
            int h1 = (h-h0-4*m)/3;
            int h2 = (h-h0-h1-2*m)/2;
            int h3 = h-h0-h1-h2-4*m;
            int w1 = (w-3*m)/2;
            int w2 = w-3*m-w1;
            items.at(0)->setGeometry(QRect(m,h0,w1,h1));
            items.at(1)->setGeometry(QRect(m,h0+h1+2*m,w1,h2));
            items.at(2)->setGeometry(QRect(m,h0+h1+h2+4*m,w1,h3));
            items.at(3)->setGeometry(QRect(m+w1+m,h0,w2,h1));
            items.at(4)->setGeometry(QRect(m+w1+m,h0+h1+2*m,w2,h2));
            items.at(5)->setGeometry(QRect(m+w1+m,h0+h1+h2+4*m,w2,h3));
            break;
        }
        case 7: {
            int h1 = (h-h0-3*m)/4;
            int h2 = (h-h0-h1-3*m)/3;
            int h3 = (h-h0-h1-h2-2*m)/2;
            int h4 = h-h0-h1-h2-h3-3*m;
            int w1 = (w-3*m)/2;
            int w2 = w-3*m-w1;
            items.at(0)->setGeometry(QRect(m,h0,w1,h1));
            items.at(1)->setGeometry(QRect(m,h0+h1+m,w1,h2));
            items.at(2)->setGeometry(QRect(m,h0+h1+m+h2+m,w1,h3));
            items.at(3)->setGeometry(QRect(m+w1+m,h0,w2,h1));
            items.at(4)->setGeometry(QRect(m+w1+m,h0+h1+m,w2,h2));
            items.at(5)->setGeometry(QRect(m+w1+m,h0+h1+m+h2+m,w2,h3));
            items.at(6)->setGeometry(QRect(m,h0+h1+h2+h3+3*m,w-m,h4));
            break;
        }
    }
}

QSize PageLayout::sizeHint() const
{
    return QSize();
}

QLayoutItem *PageLayout::takeAt(int index)
{
    Q_UNUSED(index);
    fprintf(stderr, "koviz [bad scoobs]: PageLayout::takeAt() "
                    "not implemented!\n");
    exit(-1);
    return 0;
}
