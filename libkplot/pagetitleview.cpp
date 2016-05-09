#include "pagetitleview.h"

PageTitleView::PageTitleView(QWidget *parent) :
    BookIdxView(parent)
{
    _mainLayout = new QVBoxLayout;
    _titleFrame = new QFrame(this);
    _titleFrame->setFrameStyle(QFrame::NoFrame);
    QPalette Pal(palette());
    Pal.setColor(QPalette::Background, Qt::white);
    _titleFrame->setAutoFillBackground(true);
    _titleFrame->setPalette(Pal);

    _titleGrid = new QGridLayout(_titleFrame);
    _titleGrid->setColumnStretch(0,1);
    _titleGrid->setColumnStretch(1,8);
    _titleGrid->setColumnStretch(2,1);

    QFont font1;
    int pointSize = _titleFrame->font().pointSize();
    font1.setPointSize(pointSize+1);

    _title1 = new QLabel(_titleFrame);
    _title1->setTextFormat(Qt::RichText);
    _title1->setText("No Model");
    _title1->setAlignment(Qt::AlignHCenter| Qt::AlignVCenter);
    _title1->setFont(font1);
    _titleGrid->addWidget(_title1,0,1);

    _title2 = new QLabel(_titleFrame);
    _title2->setTextFormat(Qt::PlainText);
    _title2->setText("");
    _title2->setAlignment(Qt::AlignHCenter| Qt::AlignVCenter);
    _titleGrid->addWidget(_title2,1,1);

    // Default to username
    _title3 = new QLabel(_titleFrame);
    _title3->setTextFormat(Qt::PlainText);
    _title3->setText("");
    _title3->setAlignment(Qt::AlignRight| Qt::AlignVCenter);
    _titleGrid->addWidget(_title3,0,2);

    // Default to date
    _title4 = new QLabel(_titleFrame);
    _title4->setTextFormat(Qt::PlainText);
    _title4->setText("");
    _title4->setAlignment(Qt::AlignRight| Qt::AlignVCenter);
    _titleGrid->addWidget(_title4,1,2);

    _mainLayout->addWidget(_titleFrame);

    setLayout(_mainLayout);
}


void PageTitleView::dataChanged(const QModelIndex &topLeft,
                                const QModelIndex &bottomRight)
{
    QModelIndex pidx = topLeft.parent();
    if ( pidx != bottomRight.parent() ) return;

    // TODO: For now and only handle single item changes
    if ( topLeft != bottomRight ) return;

    // TODO: if default title changes, this fails
    if ( pidx != _myIdx ) return;

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
    if ( pidx != _myIdx ) return;

    for ( int i = start; i <= end; ++i ) {
        QModelIndex idx = model()->index(i,0,pidx);
        QString cText = model()->data(idx).toString();
        if ( cText == "PageTitle" ) {
            QModelIndex idx1 = model()->index(i,1,pidx);
            QString title = model()->data(idx1).toString();
            _title1->setText(title);
        }
    }
}

void PageTitleView::_update()
{
    PlotBookModel* bookModel = _bookModel();
    QModelIndex pageIdx = _myIdx;

    QList<QStandardItem*> items = bookModel->findItems("DefaultPageTitles",
                                                       Qt::MatchStartsWith);
    if ( items.isEmpty() ) {
        qDebug() << "snap [bad scoobs]: PageTitleView::setModel() can't "
                    "find DefaultPageTitles";
        exit(-1);
    }
    QStandardItem* pItem = items.at(0);
    QStandardItem* titleItem;

    QString pageTitle ;
    if ( bookModel->isChildIndex(pageIdx,"Page","PageTitle") ) {
        QModelIndex pageTitleIdx  = bookModel->getIndex(pageIdx,
                                                        "PageTitle","Page");
        pageTitle = bookModel->data(pageTitleIdx).toString();
    } else {
        pageTitle = pItem->child(0,1)->text();
    }

    titleItem = pItem->child(0,1);
    _title1->setText(pageTitle);
    titleItem = pItem->child(1,1);
    _title2->setText(titleItem->text());
    titleItem = pItem->child(2,1);
    _title3->setText(titleItem->text());
    titleItem = pItem->child(3,1);
    _title4->setText(titleItem->text());
}

