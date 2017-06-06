#include "bookview_pagetitle.h"

PageTitleView::PageTitleView(QWidget *parent) :
    BookIdxView(parent)
{
    setFrameShape(QFrame::NoFrame);

    _grid = new QGridLayout;
    _grid->setContentsMargins(0, 0, 8, 8);
    _grid->setSpacing(0);

    QFont font1;
    int pointSize = font().pointSize();
    font1.setPointSize(pointSize+4);

    _title1 = new QLabel(this);
    _title1->setAlignment(Qt::AlignHCenter| Qt::AlignVCenter);
    _title1->setFont(font1);

    _title2 = new QLabel(this);
    _title2->setAlignment(Qt::AlignHCenter| Qt::AlignTop);

    // Default to username
    _title3 = new QLabel(this);
    _title3->setAlignment(Qt::AlignRight| Qt::AlignBottom);

    // Default to date
    _title4 = new QLabel(this);
    _title4->setAlignment(Qt::AlignRight| Qt::AlignTop);

    _grid->addWidget(_title1,0,0,1,1);
    _grid->addWidget(_title2,1,0,1,1);
    _grid->addWidget(_title3,0,1,1,1);
    _grid->addWidget(_title4,1,1,1,1);

    _grid->setRowStretch(0,1);
    _grid->setRowStretch(1,1);

    _grid->setColumnStretch(0,10);
    _grid->setColumnStretch(1,1);

    setLayout(_grid);
}

void PageTitleView::setModel(QAbstractItemModel *model)
{

    QAbstractItemView::setModel(model);

    PlotBookModel* bookModel = _bookModel();

    QList<QStandardItem*> items = bookModel->findItems("DefaultPageTitles",
                                                       Qt::MatchStartsWith);
    if ( items.isEmpty() ) {
        fprintf(stderr,"koviz [bad scoobs]: PageTitleView::setModel() can't "
                       "find DefaultPageTitles.\n");
        exit(-1);
    }
    QStandardItem* titleItem;
    QStandardItem* pItem = items.at(0);

    QString pageTitle ;
    if ( bookModel->isChildIndex(rootIndex(),"Page","PageTitle") ) {
        QModelIndex pageTitleIdx  = bookModel->getIndex(rootIndex(),
                                                        "PageTitle","Page");
        pageTitleIdx = bookModel->index(pageTitleIdx.row(),1,rootIndex());
        pageTitle = bookModel->data(pageTitleIdx).toString();
    } else {
        pageTitle = pItem->child(0,1)->text();
    }

    QModelIndex rootIdx = QModelIndex();
    QModelIndex dptIdx = _bookModel()->getIndex(rootIdx,
                                                "DefaultPageTitles");
    QString t1 = _bookModel()->getDataString(dptIdx,
                                             "Title1","DefaultPageTitles");
    if ( t1 != "Snap Plots!" ) {
        // Default title overwritten by -t1 optional title
        pageTitle = t1;
    }

    _title1->setText(pageTitle);

    titleItem = pItem->child(1,1);
    _title2->setText(_elideRunsTitle(titleItem->text()));
    titleItem = pItem->child(2,1);
    _title3->setText(titleItem->text());
    titleItem = pItem->child(3,1);
    _title4->setText(titleItem->text());
}

// TODO: Handle changes to default title and titles 2,3 and 4
void PageTitleView::dataChanged(const QModelIndex &topLeft,
                                const QModelIndex &bottomRight)
{
    QModelIndex pidx = topLeft.parent();
    if ( pidx != bottomRight.parent() ) return;

    // TODO: For now and only handle single item changes
    if ( topLeft != bottomRight ) return;

    // TODO: if default title changes, this fails
    if ( pidx != rootIndex() ) return;

    // Value is in column 1
    if ( topLeft.column() != 1 ) return;

    int row = topLeft.row();
    QModelIndex idx0 = model()->index(row,0,pidx);
    if ( model()->data(idx0).toString() == "PageTitle" ) {
        QString title = model()->data(topLeft).toString();
        _title1->setText(title);
    }
}

void PageTitleView::rowsInserted(const QModelIndex &pidx, int start, int end)
{
    if ( pidx != rootIndex() ) return;

    if ( !_bookModel()->isChildIndex(rootIndex(),"Page","PageTitle") ) {
        return;
    }

    for ( int i = start; i <= end; ++i ) {
        QModelIndex idx = model()->index(i,0,pidx);
        QString cText = model()->data(idx).toString();
        if ( cText == "PageTitle" ) {
            PlotBookModel* bookModel = _bookModel();

            QList<QStandardItem*> items = bookModel->
                                            findItems("DefaultPageTitles",
                                                      Qt::MatchStartsWith);
            if ( items.isEmpty() ) {
                fprintf(stderr,"koviz [bad scoobs]: PageTitleView::setModel() "
                               "can't find DefaultPageTitles\n");
                exit(-1);
            }
            QStandardItem* titleItem;
            QStandardItem* pItem = items.at(0);

            QString pageTitle ;
            if ( bookModel->isChildIndex(rootIndex(),"Page","PageTitle") ) {
                QModelIndex pageTitleIdx  = bookModel->getIndex(rootIndex(),
                                                                "PageTitle",
                                                                "Page");
                pageTitleIdx = bookModel->index(pageTitleIdx.row(),1,
                                                rootIndex());
                pageTitle = bookModel->data(pageTitleIdx).toString();
            } else {
                pageTitle = pItem->child(0,1)->text();
            }

            QModelIndex rootIdx = QModelIndex();
            QModelIndex dptIdx = _bookModel()->getIndex(rootIdx,
                                                        "DefaultPageTitles");
            QString t1 = _bookModel()->getDataString(dptIdx,
                                                     "Title1",
                                                     "DefaultPageTitles");

            if ( pageTitle.isEmpty() && t1.startsWith("koviz") ) {
                // Since subtitle has RUNs, don't use t1 (it has RUNs too)
                pageTitle = "Koviz Plots";
            } else if ( !t1.startsWith("koviz") ) {
                // DP page title overwritten by -t1 optional title
                pageTitle = t1;
            }

            _title1->setText(pageTitle);

            titleItem = pItem->child(1,1);
            _title2->setText(_elideRunsTitle(titleItem->text()));
            titleItem = pItem->child(2,1);
            _title3->setText(titleItem->text());
            titleItem = pItem->child(3,1);
            _title4->setText(titleItem->text());
        }
    }
}

QString PageTitleView::_elideRunsTitle(QString title)
{
    QString s = title;


    if ( title.count('\n') > 3 ) {
        int j = 0;
        int k = 0;
        for ( int i = 0 ; i < 2; ++i ) {
            j = title.indexOf('\n',k);
            k = j+1;
        }
        s = title.left(j);
        s += "...)";
    }

    // Take out newline chars and see if title fits
    QString fs = s.replace('\n',' ');
    QFontMetrics fm = fontMetrics();
    int fw = fm.width(fs);
    if ( fw < width() ) {
        s = fs;
    }

    return s;
}

