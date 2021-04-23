#include "bookview_linedruler.h"

LinedRulerView::LinedRulerView(Qt::Alignment alignment, QWidget *parent) :
    BookIdxView(parent)
{
    _alignment = alignment;

    setFrameShape(QFrame::NoFrame);
    if ( _alignment == Qt::AlignTop || _alignment == Qt::AlignBottom ) {
        this->setSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::Fixed);
    } else {
        this->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::MinimumExpanding);
    }
}

void LinedRulerView::setModel(QAbstractItemModel* model)
{
    BookIdxView::setModel(model);
    if ( model ) {
        disconnect(model,SIGNAL(rowsInserted(QModelIndex,int,int)),
                   this,SLOT(rowsInserted(QModelIndex,int,int)));
    }
}

void LinedRulerView::paintEvent(QPaintEvent *event)
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
    PlotTicsLayoutItem layoutItem(painter.fontMetrics(),
                                  _bookModel(),rootIndex());
    layoutItem.setAlignment(_alignment);
    layoutItem.paint(&painter,R,RG,C,M);
}

QSize LinedRulerView::minimumSizeHint() const
{
    return sizeHint();
}

QSize LinedRulerView::sizeHint() const
{

    QSize s;
    PlotTicsLayoutItem layoutItem(fontMetrics(),_bookModel(),rootIndex());
    layoutItem.setAlignment(_alignment);
    s = layoutItem.sizeHint();
    return s;
}

void LinedRulerView::dataChanged(const QModelIndex &topLeft,
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

void LinedRulerView::rowsInserted(const QModelIndex &parent, int start, int end)
{
    Q_UNUSED(parent);
    Q_UNUSED(start);
    Q_UNUSED(end);
}
