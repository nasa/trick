#include "bookview_pagetitle.h"

PageTitleView::PageTitleView(QWidget *parent) :
    BookIdxView(parent)
{
    setFrameShape(QFrame::NoFrame);
}

void PageTitleView::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    if ( !model() ) return;

    QPainter painter(viewport());
    _paintPageTitle(rootIndex(),painter);
}

void PageTitleView::dataChanged(const QModelIndex &topLeft,
                                const QModelIndex &bottomRight,
                                const QVector<int> &roles)
{
    Q_UNUSED(bottomRight);
    Q_UNUSED(roles);

    if ( topLeft.column() != 1 ) return;

    QString tag = model()->data(topLeft.sibling(topLeft.row(),0)).toString();

    if ( tag == "PlotPresentation" ) {
        viewport()->update();
    }
}
