#include "bookview_corner.h"

CornerView::CornerView(Qt::Corner corner, QWidget *parent) :
    BookIdxView(parent), _corner(corner)
{
    setFrameShape(QFrame::NoFrame);
    this->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}

void CornerView::setModel(QAbstractItemModel *model)
{
    QAbstractItemView::setModel(model);
    if ( model ) {
        disconnect(model,SIGNAL(rowsInserted(QModelIndex,int,int)),
                   this,SLOT(rowsInserted(QModelIndex,int,int)));
    }
}

void CornerView::paintEvent(QPaintEvent *event)
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
    PlotCornerLayoutItem layoutItem(fontMetrics(),_corner);
    layoutItem.paint(&painter,R,RG,C,M);
}

QSize CornerView::minimumSizeHint() const
{
    return sizeHint();
}

QSize CornerView::sizeHint() const
{
    QSize s;
    PlotCornerLayoutItem layoutItem(fontMetrics(),_corner);
    s = layoutItem.sizeHint();
    return s;
}
