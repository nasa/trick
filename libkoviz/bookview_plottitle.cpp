#include "bookview_plottitle.h"

PlotTitleView::PlotTitleView(QWidget *parent) :
    BookIdxView(parent)
{
    setFrameShape(QFrame::NoFrame);
}

void PlotTitleView::_update()
{
}


// TODO: For now and only handle single item changes
void PlotTitleView::dataChanged(const QModelIndex &topLeft,
                                const QModelIndex &bottomRight)
{
    if ( topLeft.parent() != rootIndex() ) return;

    // Value is in column 1
    if ( topLeft.column() != 1 ) return;

    // TODO: For now and only handle single item changes
    if ( topLeft != bottomRight ) return;

    _update();
}

// TODO: only single append works
void PlotTitleView::rowsInserted(const QModelIndex &pidx, int start, int end)
{
    return;
}


QSize PlotTitleView::minimumSizeHint() const
{
    return sizeHint();
}

QSize PlotTitleView::sizeHint() const
{
    QSize sz;

    if ( _bookModel()->isChildIndex(rootIndex(),"Plot","PlotTitle") ) {
        QFontMetrics fm = fontMetrics();
        QString s = _bookModel()->getDataString(rootIndex(),"PlotTitle","Plot");
        QRect bb = fm.boundingRect(s);
        sz = bb.size();
    }

    return sz;
}

void PlotTitleView::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    if ( !model() ) return;

    QPainter painter(viewport());

    painter.save();
    painter.setRenderHint(QPainter::Antialiasing);

    QFont origFont = painter.font();
    QPaintDevice* paintDevice = painter.device();
    QRect R(0,0,paintDevice->width(),paintDevice->height());

    QString s = _bookModel()->getDataString(rootIndex(),"PlotTitle","Plot");
    QFontMetrics fm = painter.fontMetrics();
    QRect bbox = fm.boundingRect(s);
    int x = R.x() + R.width()/2 - bbox.width()/2;
    int q = (R.height()-bbox.height())/2;
    int y = R.y() + q + fm.ascent() - 1;
    painter.drawText(x,y,s);

    painter.setFont(origFont);
    painter.restore();
}
