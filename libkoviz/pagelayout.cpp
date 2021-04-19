#include "pagelayout.h"

PageLayout::PageLayout() :
    QLayout(),
    _bookModel(0),
    _pageIdx(QModelIndex())
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

    bool isHiddenItems = false; // Hidden items implies toggled to single view
    QList<QLayoutItem*> items;
    int nItems = _items.count();
    for ( int i = 1; i < nItems; ++i ) {
        QLayoutItem* item =  _items.at(i);
        if ( item->widget() ) {
            if ( !item->widget()->isHidden() ) {
                items << item;
            } else {
                isHiddenItems = true;
            }
        } else {
            items << item;
        }
    }

    QList<QRectF> plotRects;
    if (_bookModel->isChildIndex(_pageIdx,"Page","Plots")) {
        QModelIndex plotsIdx = _bookModel->getIndex(_pageIdx, "Plots","Page");
        QModelIndexList plotIdxs = _bookModel->getIndexList(plotsIdx,
                                                              "Plot","Plots");
        foreach ( QModelIndex plotIdx, plotIdxs ) {
            if ( _bookModel->isChildIndex(plotIdx,"Plot", "PlotRect") ) {
                QRectF r = _bookModel->getDataRectF(plotIdx,"PlotRect","Plot");
                plotRects << r;
            }
        }
    }

    QList<QRect> rects;
    int nPlots = items.size();
    switch ( nPlots ) {
        case 0: {
            break;
        }
        case 1: {
            rects << QRect(m,h0,w-2*m,h-h0);
            break;
        }
        case 2: {
            int h1 = (h-h0-2*m)/2;
            int h2 = h-h0-h1-2*m;
            rects << QRect(m,h0,w-2*m,h1);
            rects << QRect(m,h0+h1+2*m,w-2*m,h2);
            break;
        }
        case 3: {
            int h1 = (h-h0-4*m)/3;
            int h2 = (h-h0-h1-2*m)/2;
            int h3 = h-h0-h1-h2-4*m;
            rects << QRect(m,h0,w-2*m,h1);
            rects << QRect(m,h0+h1+2*m,w-2*m,h2);
            rects << QRect(m,h0+h1+h2+4*m,w-2*m,h3);
            break;
        }
        case 4: {
            int h1 = (h-h0-2*m)/2;
            int h2 = h-h0-h1-2*m;
            int w1 = (w-3*m)/2;
            int w2 = w-3*m-w1;
            rects << QRect(m,h0,w1,h1);
            rects << QRect(m+w1+m,h0,w2,h1);
            rects << QRect(m,h0+h1+2*m,w1,h2);
            rects << QRect(m+w1+m,h0+h1+2*m,w2,h2);
            break;
        }
        case 5: {
            int h1 = (h-h0-4*m)/3;
            int h2 = (h-h0-h1-2*m)/2;
            int h3 = h-h0-h1-h2-4*m;
            int w1 = (w-3*m)/2;
            int w2 = w-3*m-w1;
            rects << QRect(m,h0,w1,h1);
            rects << QRect(m+w1+m,h0,w2,h1);
            rects << QRect(m,h0+h1+2*m,w1,h2);
            rects << QRect(m+w1+m,h0+h1+2*m,w2,h2);
            rects << QRect(m,h0+h1+h2+4*m,w-2*m,h3);
            break;
        }
        case 6: {
            int h1 = (h-h0-4*m)/3;
            int h2 = (h-h0-h1-2*m)/2;
            int h3 = h-h0-h1-h2-4*m;
            int w1 = (w-3*m)/2;
            int w2 = w-3*m-w1;
            rects << QRect(m,h0,w1,h1);
            rects << QRect(m,h0+h1+2*m,w1,h2);
            rects << QRect(m,h0+h1+h2+4*m,w1,h3);
            rects << QRect(m+w1+m,h0,w2,h1);
            rects << QRect(m+w1+m,h0+h1+2*m,w2,h2);
            rects << QRect(m+w1+m,h0+h1+h2+4*m,w2,h3);
            break;
        }
        case 7: {
            int h1 = (h-h0-3*m)/4;
            int h2 = (h-h0-h1-3*m)/3;
            int h3 = (h-h0-h1-h2-2*m)/2;
            int h4 = h-h0-h1-h2-h3-3*m;
            int w1 = (w-3*m)/2;
            int w2 = w-3*m-w1;
            rects << QRect(m,h0,w1,h1);
            rects << QRect(m,h0+h1+m,w1,h2);
            rects << QRect(m,h0+h1+m+h2+m,w1,h3);
            rects << QRect(m+w1+m,h0,w2,h1);
            rects << QRect(m+w1+m,h0+h1+m,w2,h2);
            rects << QRect(m+w1+m,h0+h1+m+h2+m,w2,h3);
            rects << QRect(m,h0+h1+h2+h3+3*m,w-m,h4);
            break;
        }
        default: {
            int nrows = ceil(sqrt(nPlots));
            div_t q = div(nPlots,nrows);
            int ncols = 0;
            if ( q.rem == 0 ) {
                ncols = q.quot;
            } else {
                ncols = q.quot+1;
            }
            int i = 0;
            int ww = w/ncols;
            int hh = (h-h0)/nrows;
            for ( int r = 0; r < nrows; ++r ) {
                for ( int c = 0; c < ncols; ++c ) {
                    if ( i == nPlots ) {
                        break;
                    }
                    int x = c*ww;
                    int y = h0+r*hh;
                    if ( i == nPlots-1 ) {
                        ww = (ncols-c)*ww; // Expand last plot
                    }
                    rects << QRect(x,y,ww,hh);
                    ++i;
                }
            }
        }
    }

    int i = 0;
    foreach ( QRect rect, rects ) {
        QRect r = rect;
        if ( !isHiddenItems &&
             plotRects.size() > i && !plotRects.at(i).isEmpty() ) {
            r = _calcRect(w,h,h0,plotRects.at(i));
        }
        items.at(i)->setGeometry(r);
        ++i;
    }
}

// Calc rect based on r which is a rect with percentages
QRect PageLayout::_calcRect(int pageWidth,
                            int pageHeight,
                            int titleHeight,
                            const QRectF &r)
{
    int x = qRound(r.x()*pageWidth);
    int y = qRound(r.y()*(pageHeight-titleHeight)+titleHeight);
    int w = floor(r.width()*pageWidth);
    int h = floor(r.height()*(pageHeight-titleHeight));
    return QRect(x,y,w,h);
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

void PageLayout::setModelIndex(PlotBookModel *bookModel,
                               const QModelIndex &pageIdx)
{
    _bookModel = bookModel;
    _pageIdx = pageIdx;
}
