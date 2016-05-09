#include "bookview.h"

BookView::BookView(QWidget *parent) :
    QAbstractItemView(parent)
{
    _mainLayout = new QVBoxLayout;

    _nb = new QTabWidget(this);
    _nb->setTabsClosable(true);
    _nb->setFocusPolicy(Qt::StrongFocus);
    _nb->setMovable(true);

    _mainLayout->addWidget(_nb);

    setLayout(_mainLayout);
}


void BookView::setModel(QAbstractItemModel *model)
{
    // Allow setting model to null
    if ( !model ) {
        QAbstractItemView::setModel(model);
        return;
    }

    PlotBookModel* bookModel = dynamic_cast<PlotBookModel*>(model);
    if ( !bookModel ) {
        qDebug() << "snap [bad scoobs]: BookView::setModel() "
                    "could not cast model at address"
                 << (void*)model
                 << "to a PlotBookModel";
        exit(-1);
    }

    QAbstractItemView::setModel(model);
}

void BookView::dataChanged(const QModelIndex &topLeft,
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

void BookView::rowsInserted(const QModelIndex &pidx, int start, int end)
{
    if ( model()->data(pidx).toString() != "Pages" ) return;

    for ( int i = start; i <= end; ++i ) {
        QModelIndex idx = model()->index(i,0,pidx);
        QString cText = model()->data(idx).toString();
        if ( cText == "Page" ) {
            PageView* pw = new PageView;
            pw->setModel(model());
            pw->setRootIndex(idx);
            _nb->addTab(pw,"Hey");
        }
    }
}

QModelIndex BookView::indexAt(const QPoint &point) const
{
    QModelIndex idx;
    return idx;
}

QRect BookView::visualRect(const QModelIndex &index) const
{
}

void BookView::scrollTo(const QModelIndex &index,
                             QAbstractItemView::ScrollHint hint)
{
}

QModelIndex BookView::moveCursor(
        QAbstractItemView::CursorAction cursorAction,
        Qt::KeyboardModifiers modifiers)
{
}

int BookView::horizontalOffset() const
{
    return 0;
}

int BookView::verticalOffset() const
{
    return 0;
}

// TODO: the index can be hidden
bool BookView::isIndexHidden(const QModelIndex &index) const
{
    return false;
}

void BookView::setSelection(const QRect &rect,
                                 QItemSelectionModel::SelectionFlags command)
{
}

QRegion BookView::visualRegionForSelection(
        const QItemSelection &selection) const
{
}

