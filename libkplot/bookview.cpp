#include "bookview.h"

BookView::BookView(QWidget *parent) :
    BookIdxView(parent)
{
    _mainLayout = new QVBoxLayout;

    _nb = new QTabWidget(this);
    _nb->setTabsClosable(true);
    _nb->setFocusPolicy(Qt::StrongFocus);
    _nb->setMovable(true);

    connect(_nb,SIGNAL(tabCloseRequested(int)),
            this,SLOT(_nbCloseRequested(int)));
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

void BookView::_nbCloseRequested(int idx)
{
    if ( model() == 0 ) return;

    QWidget* tabWidget = _nb->widget(idx);
    QString tabToolTip = _nb->tabToolTip(idx);
    QString wt = _nb->tabWhatsThis(idx);
    if ( wt == "Page" ) {
        QModelIndex pagesIdx = _bookModel()->getIndex(QModelIndex(), "Pages");
        int row = -1;
        int rc = model()->rowCount(pagesIdx);
        for ( int i = 0; i < rc ; ++i ) {
            QModelIndex pageIdx = model()->index(i,0,pagesIdx);
            QModelIndex pageNameIdx = _bookModel()->getIndex(pageIdx,
                                                             "PageName","Page");
            pageNameIdx = model()->index(pageNameIdx.row(),1,pageIdx);
            QString pageName = model()->data(pageNameIdx).toString();
            if ( pageName == tabToolTip ) {
                row = i;
                break;
            }
        }
        if ( row < 0 ) {
            qDebug() << "snap [bad scoobs]: BookView::_nbCloseRequested() "
                        "Could not find page to remove!  Aborting!";
            exit(-1);
        }
        model()->removeRow(row,pagesIdx);
        _nb->removeTab(idx);
    } else if ( wt == "Table" ) {
        // TODO: Table
    } else {
        qDebug() << "snap [bad scoobs]: BookView::_nbCloseRequested() "
                    "tabWhatsThis should have been set.";
        exit(-1);
    }

    delete tabWidget;
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
    Q_UNUSED(topLeft);
    Q_UNUSED(bottomRight);
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
            int tabId = _nb->addTab(pw,"Page");
            _nb->setTabWhatsThis(tabId, "Page");
        } else if ( cText == "PageName" ) {
            QModelIndex pageNameIdx = _bookModel()->index(i,1,pidx);
            QString pageName = model()->data(pageNameIdx).toString();
            QFileInfo fi(pageName);
            QString fName = fi.fileName();
            int row = pidx.row();
            _nb->setTabToolTip(row,pageName);
            _nb->setTabText(row,fName);
        }
    }
}
