#include "bookview_tablepage.h"

TablePageView::TablePageView(QWidget *parent) :
    BookIdxView(parent),
    _grid(0)
{
    setFrameShape(QFrame::NoFrame);

    // Create/configure gridlayout for page
    _grid = new QGridLayout;
    _grid->setContentsMargins(0, 0, 5, 0);
    _grid->setSpacing(0);

    // Create and add titleView
    _titleView = new PageTitleView(this);
    _childViews << _titleView;
    _grid->addWidget(_titleView,0,0,1,100);
    _grid->setRowStretch(0,1);

    // Create and add tableView
    _tableView = new BookTableView(this);
    _childViews << _tableView;
    _grid->addWidget(_tableView,1,0,1,100);
    _grid->setRowStretch(1,100);

    setLayout(_grid);
}

TablePageView::~TablePageView()
{
}

void TablePageView::dataChanged(const QModelIndex &topLeft,
                                const QModelIndex &bottomRight)
{
    QModelIndex pidx = topLeft.parent();
    if ( pidx != bottomRight.parent() ) return;

    // TODO: For now and only handle single item changes
    if ( topLeft != bottomRight ) return;

    // Value is in column 1
    if ( topLeft.column() != 1 ) return;
}

void TablePageView::setModel(QAbstractItemModel *model)
{
    _titleView->setModel(model);
    _tableView->setModel(model);
    QAbstractItemView::setModel(model);
}

void TablePageView::setRootIndex(const QModelIndex &index)
{
    if ( !index.isValid() ) {
        return;
    }

    _titleView->setRootIndex(index);
    _tableView->setRootIndex(index);
    QAbstractItemView::setRootIndex(index);
}
