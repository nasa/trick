#include "bookview.h"

BookView::BookView(QWidget *parent) :
    BookIdxView(parent)
{
    _mainLayout = new QVBoxLayout;

    _nb = new QTabWidget(this);
    _nb->setTabsClosable(true);
    _nb->setFocusPolicy(Qt::StrongFocus);
    _nb->setMovable(true);

    connect(_nb, SIGNAL(currentChanged(int)),
            this,SLOT(_nbCurrentChanged(int)));

    _mainLayout->addWidget(_nb);

    setLayout(_mainLayout);
}

void BookView::_update()
{
}

void BookView::currentChanged(const QModelIndex &current,
                              const QModelIndex &previous)
{
    Q_UNUSED(previous);
    _nb->setCurrentIndex(current.row());
}

void BookView::selectionChanged(const QItemSelection &selected,
                                const QItemSelection &deselected)
{
    Q_UNUSED(selected);
    Q_UNUSED(deselected);
}

void BookView::_nbCurrentChanged(int idx)
{
    if ( selectionModel() ) {
        QModelIndex pagesIdx = _bookModel()->getIndex(QModelIndex(),"Pages");
        QModelIndex pageIdx = model()->index(idx, 0,pagesIdx);
        selectionModel()->setCurrentIndex(pageIdx,QItemSelectionModel::Current);
    }
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
    if ( model()->data(pidx).toString() != "Pages" &&
         model()->data(pidx).toString() != "Page" ) return;

    for ( int i = start; i <= end; ++i ) {
        QModelIndex idx = model()->index(i,0,pidx);
        QString cText = model()->data(idx).toString();
        if ( cText == "Page" ) {
            PageView* pw = new PageView;
            pw->setModel(model());
            pw->setRootIndex(idx);
            _nb->addTab(pw,"Page");
        } else if ( cText == "PageName" ) {
            QModelIndex pageNameIdx = _bookModel()->index(i,1,pidx);
            QString pageName = model()->data(pageNameIdx).toString();
            QFileInfo fi(pageName);
            pageName = fi.fileName();
            int row = pidx.row();
            _nb->setTabText(row,pageName);
        }
    }
}
