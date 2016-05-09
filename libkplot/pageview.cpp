#include "pageview.h"

PageView::PageView(QWidget *parent) :
    BookIdxView(parent),
    _gridLayout(0),
    _titleView(0),
    _plots(QList<PlotView*>())
{
    // Create/configure gridlayout for page
    _gridLayout = new QGridLayout;
    _gridLayout->setContentsMargins(0, 0, 0, 0);
    _gridLayout->setSpacing(0);

    // Create and add titleView
    _titleView = new PageTitleView(this);
    _gridLayout->addWidget(_titleView,0,0,1,100);
    _gridLayout->setRowStretch(0,1);

    setLayout(_gridLayout);
}

void PageView::dataChanged(const QModelIndex &topLeft,
                                const QModelIndex &bottomRight)
{
    QModelIndex pidx = topLeft.parent();
    if ( pidx != bottomRight.parent() ) return;

    // TODO: For now and only handle single item changes
    if ( topLeft != bottomRight ) return;

    // Value is in column 1
    if ( topLeft.column() != 1 ) return;

#if 0
    int row = topLeft.row();
    QModelIndex idx0 = model()->index(row,0,pidx);
    if ( model()->data(idx0).toString() == "PageTitle" ) {
        QString title = model()->data(topLeft).toString();
        _title1->setText(title);
    }
#endif
}

// TODO: this will die if not appending plots one by one
void PageView::rowsInserted(const QModelIndex &pidx, int start, int end)
{
    if ( model()->data(pidx).toString() != "Plots" ) return;

    int nPlotsToAdd = end-start+1;
    if ( nPlotsToAdd != 1 ) {
        qDebug() << "snap [bad scoobs]: PageView::rowsInserted(). "
                    "Attempting to insert more than a single row.";
        exit(-1);
    }

    int nPlots = model()->rowCount(pidx);

    if ( start != nPlots-1 || end != nPlots-1 ) {
        qDebug() << "snap [bad scoobs]: PageView::rowsInserted(). "
                    "Attempting to insert a row instead of appending it.";
        exit(-1);
    }

    QModelIndex plotIdx = model()->index(start,0,pidx);
    PlotView* plot = new PlotView(this);
    _plots << plot;
    plot->setModel(model());
    plot->setRootIndex(plotIdx);

    switch ( nPlots ) {
    case 1: {
        _gridLayout->addWidget(plot,1,0);
        _gridLayout->setRowStretch(1,100);
        break;
    }
    default: {
        break;
    }
    }
}

void PageView::_update()
{
    if ( _titleView ) {
        _titleView->setModel(model());
        _titleView->setRootIndex(_myIdx);
    }
    foreach ( PlotView* plot, _plots ) {
        plot->setModel(model());
        plot->setRootIndex(_myIdx);
    }
}
