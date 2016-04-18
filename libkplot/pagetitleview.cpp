#include "pagetitleview.h"

PageTitleView::PageTitleView(const QModelIndex &pageIdx, QWidget *parent) :
    QAbstractItemView(parent),_pageIdx(pageIdx)
{
    _mainLayout = new QVBoxLayout(this);
    _titleFrame = new QFrame(this);
    _titleFrame->setFrameStyle(QFrame::NoFrame);
    //_titleFrame->setFrameStyle(QFrame::Box);
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
}

void PageTitleView::setModel(QAbstractItemModel *model)
{
    QStandardItemModel* smodel = dynamic_cast<QStandardItemModel*>(model);
    if ( smodel ) {
        QList<QStandardItem*> items = smodel->findItems("DefaultPageTitles",
                                                        Qt::MatchStartsWith);
        if ( items.isEmpty() ) {
            qDebug() << "PageTitleView::setModel() can't "
                        "find DefaultPageTitles";
            exit(-1);
        }
        QStandardItem* pItem = items.at(0);
        QStandardItem* titleItem;

        titleItem = pItem->child(0,1);
        _title1->setText(titleItem->text());
        titleItem = pItem->child(1,1);
        _title2->setText(titleItem->text());
        titleItem = pItem->child(2,1);
        _title3->setText(titleItem->text());
        titleItem = pItem->child(3,1);
        _title4->setText(titleItem->text());


    } else {
        qDebug() << "PageTitleView::setModel() needs a PlotBookModel";
        exit(-1);
    }

    QAbstractItemView::setModel(model);
}

QModelIndex PageTitleView::indexAt(const QPoint &point) const
{
    QModelIndex idx;

    return idx;
}

QRect PageTitleView::visualRect(const QModelIndex &index) const
{
}

void PageTitleView::scrollTo(const QModelIndex &index,
                             QAbstractItemView::ScrollHint hint)
{
}

QModelIndex PageTitleView::moveCursor(
        QAbstractItemView::CursorAction cursorAction,
        Qt::KeyboardModifiers modifiers)
{
}

int PageTitleView::horizontalOffset() const
{
    return 0;
}

int PageTitleView::verticalOffset() const
{
    return 0;
}

bool PageTitleView::isIndexHidden(const QModelIndex &index) const
{
}

void PageTitleView::setSelection(const QRect &rect,
                                 QItemSelectionModel::SelectionFlags command)
{
}

QRegion PageTitleView::visualRegionForSelection(
        const QItemSelection &selection) const
{
}
