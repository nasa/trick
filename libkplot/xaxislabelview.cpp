#include "xaxislabelview.h"

XAxisLabelView::XAxisLabelView(QWidget *parent) :
    BookIdxView(parent),
    _xAxisLabelText("")
{
    setFrameShape(QFrame::NoFrame);
}

void XAxisLabelView::_update()
{
}

void XAxisLabelView::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    if ( !model() ) return;

    int vw = viewport()->width();
    QFontMetrics fm = viewport()->fontMetrics();
    QString txt = fm.elidedText(_xAxisLabelText, Qt::ElideLeft, vw);
    QRect bb = fm.boundingRect(txt);
    int bw = bb.width();
    int bh = bb.height();

    // Draw!
    QPainter painter(viewport());
    painter.save();
    painter.translate((vw-bw)/2, bh);
    painter.drawText(0,0, txt);
    painter.restore();
}

QSize XAxisLabelView::minimumSizeHint() const
{
    return sizeHint();
}

QSize XAxisLabelView::sizeHint() const
{
    QSize s;
    QFontMetrics fm = viewport()->fontMetrics();
    QRect bb = fm.boundingRect(_xAxisLabelText);
    s.setWidth(bb.width());
    s.setHeight(2*bb.height()); // 2 is arbitrary
    return s;
}

void XAxisLabelView::dataChanged(const QModelIndex &topLeft,
                                 const QModelIndex &bottomRight)
{
    if ( !model()) return;
    if ( topLeft.column() != 1 ) return;
    if ( topLeft.parent() != _myIdx ) return;
    if ( topLeft != bottomRight ) return; // TODO: support multiple changes
    QModelIndex tagIdx = model()->index(topLeft.row(),0,topLeft.parent());
    QString tag = model()->data(tagIdx).toString();
    if ( tag != "PlotXAxisLabel" ) return;

    _xAxisLabelText = model()->data(topLeft).toString();

    viewport()->update(); // important to use viewport()->update(), not update()
                          // since refresh will not be immediate with update()
}

void XAxisLabelView::rowsInserted(const QModelIndex &pidx, int start, int end)
{
    if ( _myIdx != pidx ) return;
    if ( !pidx.isValid() ) return;
    if ( !model()) return;

    for ( int i = start; i <= end; ++i ) {
        QModelIndex idx = model()->index(i,0,pidx);
        if ( model()->data(idx).toString() == "PlotXAxisLabel" ) {
            idx = model()->sibling(i,1,idx);
            _xAxisLabelText = model()->data(idx).toString();
        }
    }
}
