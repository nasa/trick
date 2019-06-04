#include "pagelayout.h"

PageLayout::PageLayout() : QLayout()
{
}

PageLayout::~PageLayout()
{
}

void PageLayout::addItem(QLayoutItem *item)
{
    Q_UNUSED(item);
    fprintf(stderr, "koviz [bad scoobs]: PageLayout::addItem() "
                    "not implemented!\n");
    exit(-1);
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
    int m = 10;

    int h0 = _items.at(0)->sizeHint().height(); // TitleView is 0
    _items.at(0)->setGeometry(QRect(0,0,w-m,h0));

    QList<QWidgetItem*> items;
    int nItems = _items.count();
    for ( int i = 1; i < nItems; ++i ) {
        QWidgetItem* item =  _items.at(i);
        if ( !item->widget()->isHidden() ) {
            items << item;
        }
    }

    int nPlots = items.size();
    switch ( nPlots ) {
        case 1: {
            items.at(0)->setGeometry(QRect(0,h0,w-m,h-h0));
            break;
        }
        case 2: {
            int h1 = (h-h0)/2;
            int h2 = h-(h0+h1);
            items.at(0)->setGeometry(QRect(0,h0,w-m,h1));
            items.at(1)->setGeometry(QRect(0,h0+h1,w-m,h2));
            break;
        }
        case 3: {
            int h1 = (h-h0)/3;
            int h2 = (h-(h0+h1))/2;
            int h3 = h-(h0+h1+h2);
            items.at(0)->setGeometry(QRect(0,h0,w-m,h1));
            items.at(1)->setGeometry(QRect(0,h0+h1,w-m,h2));
            items.at(2)->setGeometry(QRect(0,h0+h1+h2,w-m,h3));
            break;
        }
        case 4: {
            int h1 = (h-h0)/2;
            int h2 = h-(h0+h1);
            int w1 = (w-m)/2;
            int w2 = w-m-w1;
            items.at(0)->setGeometry(QRect(0,h0,w1,h1));
            items.at(1)->setGeometry(QRect(w1,h0,w2,h1));
            items.at(2)->setGeometry(QRect(0,h0+h1,w1,h2));
            items.at(3)->setGeometry(QRect(w1,h0+h1,w2,h2));
            break;
        }
        case 5: {
            int h1 = (h-h0)/3;
            int h2 = (h-(h0+h1))/2;
            int h3 = h-(h0+h1+h2);
            int w1 = (w-m)/2;
            int w2 = w-m-w1;
            items.at(0)->setGeometry(QRect(0,h0,w1,h1));
            items.at(1)->setGeometry(QRect(w1,h0,w2,h1));
            items.at(2)->setGeometry(QRect(0,h0+h1,w1,h2));
            items.at(3)->setGeometry(QRect(w1,h0+h1,w2,h2));
            items.at(4)->setGeometry(QRect(0,h0+h1+h2,w-m,h3));
            break;
        }
        case 6: {
            int h1 = (h-h0)/3;
            int h2 = (h-(h0+h1))/2;
            int h3 = h-(h0+h1+h2);
            int w1 = (w-m)/2;
            int w2 = w-m-w1;
            items.at(0)->setGeometry(QRect(0,h0,w1,h1));
            items.at(1)->setGeometry(QRect(0,h0+h1,w1,h2));
            items.at(2)->setGeometry(QRect(0,h0+h1+h2,w1,h3));
            items.at(3)->setGeometry(QRect(w1,h0,w2,h1));
            items.at(4)->setGeometry(QRect(w1,h0+h1,w2,h2));
            items.at(5)->setGeometry(QRect(w1,h0+h1+h2,w2,h3));
            break;
        }
        case 7: {
            int h1 = (h-h0)/4;
            int h2 = (h-(h0+h1))/3;
            int h3 = (h-(h0+h1+h2))/2;
            int h4 = h-(h0+h1+h2+h3);
            int w1 = (w-m)/2;
            int w2 = w-m-w1;
            items.at(0)->setGeometry(QRect(0,h0,w1,h1));
            items.at(1)->setGeometry(QRect(0,h0+h1,w1,h2));
            items.at(2)->setGeometry(QRect(0,h0+h1+h2,w1,h3));
            items.at(3)->setGeometry(QRect(w1,h0,w2,h1));
            items.at(4)->setGeometry(QRect(w1,h0+h1,w2,h2));
            items.at(5)->setGeometry(QRect(w1,h0+h1+h2,w2,h3));
            items.at(6)->setGeometry(QRect(0,h0+h1+h2+h3,w-m,h4));
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
