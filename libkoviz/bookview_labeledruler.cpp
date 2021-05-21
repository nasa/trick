#include "bookview_labeledruler.h"

LabeledRulerView::LabeledRulerView(Qt::Alignment alignment,
                                   QWidget *parent) :
    BookIdxView(parent)
{
    _alignment = alignment;

    setFrameShape(QFrame::NoFrame);
    if ( _alignment == Qt::AlignBottom ) {
        this->setSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::Fixed);
    } else if ( _alignment == Qt::AlignLeft ) {
        this->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::MinimumExpanding);
    } else {
        // No support (yet) for right and top axes
        return;
    }
}

void LabeledRulerView::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    if ( !model() ) return;

    QRect R = viewport()->rect();
    if ( R.width() == 0.0 || R.height() == 0.0 ) {
        return;
    }
    QRect RG =  R;
    RG.moveTo(viewport()->mapToGlobal(RG.topLeft()));
    QRect  C = _curvesView->viewport()->rect();
    C.moveTo(_curvesView->viewport()->mapToGlobal(C.topLeft()));
    QRectF M = _bookModel()->getPlotMathRect(rootIndex());
    QPainter painter(viewport());
    QFont font8 = painter.font();
    font8.setPointSizeF(8);
    QFontMetrics fm8(font8);
    TicLabelsLayoutItem layoutItem(painter.fontMetrics(), fm8,
                                   _bookModel(),rootIndex());
    layoutItem.setAlignment(_alignment);
    layoutItem.paint(&painter,R,RG,C,M);
}

QSize LabeledRulerView::minimumSizeHint() const
{
    QSize s = sizeHint();
    return s;
}

QSize LabeledRulerView::sizeHint() const
{
    QSize s;
    QFont fontRegular = font();
    QFontMetrics fm(fontRegular);
    QFont font8 = font();
    font8.setPointSizeF(8);
    QFontMetrics fm8(font8);
    TicLabelsLayoutItem layoutItem(fm,fm8,_bookModel(),rootIndex());
    layoutItem.setAlignment(_alignment);
    s = layoutItem.sizeHint();
    return s;
}

void LabeledRulerView::dataChanged(const QModelIndex &topLeft,
                                   const QModelIndex &bottomRight)
{
    if ( topLeft.parent() != rootIndex() ) return;
    if ( topLeft.column() != 1 ) return;
    if ( topLeft != bottomRight ) return;
    if ( !rootIndex().isValid() ) return;

    if ( topLeft == _plotMathRectIdx(rootIndex()) ) {
        viewport()->update();
        updateGeometry();
    }
}

void LabeledRulerView::wheelEvent(QWheelEvent *e)
{
    Q_UNUSED(e);

    QString from ;
    QModelIndex scaleIdx;
    if ( _alignment == Qt::AlignBottom ) {
        from = _bookModel()->getDataString(rootIndex(),"PlotXScale","Plot");
        scaleIdx = _bookModel()->getDataIndex(rootIndex(),"PlotXScale","Plot");
    } else if ( _alignment == Qt::AlignLeft ) {
        from = _bookModel()->getDataString(rootIndex(),"PlotYScale","Plot");
        scaleIdx = _bookModel()->getDataIndex(rootIndex(),"PlotYScale","Plot");
    }

    QString to;
    if ( from == "linear" ) {
        to = "log";
    } else if ( from == "log" ) {
        to = "linear";
    }

    _bookModel()->setData(scaleIdx,to);
}

