#include "bookview_pagetitle.h"

PageTitleView::PageTitleView(QWidget *parent) :
    BookIdxView(parent)
{
    setFrameShape(QFrame::NoFrame);

    setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
}

void PageTitleView::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    if ( !model() ) return;

    QRect R = viewport()->rect();
    QRect RG;
    QRect  C;
    QRectF M;
    QPainter painter(viewport());
    PageTitleLayoutItem layoutItem(_bookModel(),rootIndex(),font());
    layoutItem.paint(&painter,R,RG,C,M);
}

// Note: title3 and title4 are fit beside legend or title1/2
QSize PageTitleView::sizeHint() const
{
    QSize size;
    PageTitleLayoutItem layoutItem(_bookModel(),rootIndex(),font());
    size = layoutItem.sizeHint();
    return size;
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
    } else if ( tag == "CurveColor") {
        viewport()->update();
    } else if ( tag == "CurveData") {
        viewport()->update();
        updateGeometry();
    } else if ( tag == "CurveYName") {
        viewport()->update();
    } else if ( tag == "CurveYLabel" ) {
        viewport()->update();
    }
}
