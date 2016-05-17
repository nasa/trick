#include "yaxislabelview.h"

YAxisLabelView::YAxisLabelView(QWidget *parent) :
    BookIdxView(parent),
    _yAxisLabelText("")
{
    setFrameShape(QFrame::NoFrame);
}

void YAxisLabelView::_update()
{
}

void YAxisLabelView::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    if ( !model() ) return;

    int vw = viewport()->width();
    int vh = viewport()->height();
    QFontMetrics fm = viewport()->fontMetrics();
    QString txt = fm.elidedText(_yAxisLabelText, Qt::ElideLeft, vh);
    QRect bb = fm.boundingRect(txt);
    int bw = bb.width();
    int bh = bb.height();

    // Draw!
    QPainter painter(viewport());
    painter.save();
    painter.translate( (vw+bh)/2, (vh+bw)/2);
    painter.rotate(270);
    painter.drawText(0,0, txt);
    painter.restore();
}

QSize YAxisLabelView::minimumSizeHint() const
{
    return sizeHint();
}

// Note: this accounts for 270 degree rotation
QSize YAxisLabelView::sizeHint() const
{
    QSize s;
    QFontMetrics fm = viewport()->fontMetrics();
    QRect bb = fm.boundingRect(_yAxisLabelText);
    s.setWidth(2*bb.height());  // 2 is arbitrary
    s.setHeight(bb.width());
    return s;
}

void YAxisLabelView::dataChanged(const QModelIndex &topLeft,
                                 const QModelIndex &bottomRight)
{
    if ( !model()) return;
    if ( topLeft.column() != 1 ) return;
    if ( topLeft.parent() != _myIdx ) return;
    if ( topLeft != bottomRight ) return; // TODO: support multiple changes
    QModelIndex tagIdx = model()->index(topLeft.row(),0,topLeft.parent());
    QString tag = model()->data(tagIdx).toString();
    if ( tag != "PlotYAxisLabel" ) return;

    _yAxisLabelText = model()->data(topLeft).toString();

    viewport()->update(); // important to use viewport()->update(), not update()
                          // since refresh will not be immediate with update()
}

void YAxisLabelView::rowsInserted(const QModelIndex &pidx, int start, int end)
{
    if ( _myIdx != pidx ) return;
    if ( !pidx.isValid() ) return;
    if ( !model()) return;

    for ( int i = start; i <= end; ++i ) {
        QModelIndex idx = model()->index(i,0,pidx);
        if ( model()->data(idx).toString() == "PlotYAxisLabel" ) {
            idx = model()->sibling(i,1,idx);
            _yAxisLabelText = model()->data(idx).toString();
        }
    }
}
