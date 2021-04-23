#include "bookview_plottitle.h"

PlotTitleView::PlotTitleView(QWidget *parent) :
    BookIdxView(parent)
{
    _alignment = Qt::AlignTop;

    setFrameShape(QFrame::NoFrame);
    this->setSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::Fixed);
    setMouseTracking(true);
}

// TODO: For now and only handle single item changes
void PlotTitleView::dataChanged(const QModelIndex &topLeft,
                                const QModelIndex &bottomRight)
{
    if ( topLeft.parent() != rootIndex() ) return;
    if ( topLeft.column() != 1 ) return;
    if ( topLeft != bottomRight ) return;
    if ( !rootIndex().isValid() ) return;

    if ( topLeft == _plotMathRectIdx(rootIndex()) ) {
        viewport()->update();
    }
}

// TODO: only single append works
void PlotTitleView::rowsInserted(const QModelIndex &pidx, int start, int end)
{
    Q_UNUSED(pidx);
    Q_UNUSED(start);
    Q_UNUSED(end);
    return;
}


QSize PlotTitleView::minimumSizeHint() const
{
    return sizeHint();
}

QSize PlotTitleView::sizeHint() const
{
    QSize s;
    PlotTitleLayoutItem layoutItem(fontMetrics(),_bookModel(),rootIndex());
    s = layoutItem.sizeHint();
    return s;
}

void PlotTitleView::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    if ( !model() ) return;

    QRect R = viewport()->rect();
    QRect RG =  R;
    RG.moveTo(viewport()->mapToGlobal(RG.topLeft()));
    QRect  C = _curvesView->viewport()->rect();
    C.moveTo(_curvesView->viewport()->mapToGlobal(C.topLeft()));
    QRectF M = _bookModel()->getPlotMathRect(rootIndex());
    QPainter painter(viewport());
    PlotTitleLayoutItem layoutItem(fontMetrics(),_bookModel(),rootIndex());
    layoutItem.paint(&painter,R,RG,C,M);
}
