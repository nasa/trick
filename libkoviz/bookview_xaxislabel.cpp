#include "bookview_xaxislabel.h"

XAxisLabelView::XAxisLabelView(QWidget *parent) :
    BookIdxView(parent)
{
    setFrameShape(QFrame::NoFrame);
    this->setSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::Fixed);
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
    QString txt = fm.elidedText(_xAxisLabelText(), Qt::ElideLeft, vw);
    QRect bb = fm.tightBoundingRect(txt);
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
    QRect bb = fm.boundingRect(_xAxisLabelText());
    s.setWidth(bb.width());
    s.setHeight(2*bb.height()); // 2 is arbitrary
    return s;
}

// TODO: Change curve units based on current unit
void XAxisLabelView::wheelEvent(QWheelEvent *e)
{
    Q_UNUSED(e);
    return;
}

QString XAxisLabelView::_xAxisLabelText() const
{
    QString label;

    if ( !model() ) return label;
    QModelIndex plotIdx = rootIndex();
    if ( !plotIdx.isValid() ) return label;
    QString plotTag = model()->data(plotIdx).toString();
    if ( plotTag != "Plot" ) return label;
    if ( !_bookModel()->isChildIndex(plotIdx,"Plot","PlotXAxisLabel")) {
        return label;
    }

    label = _bookModel()->getDataString(plotIdx,"PlotXAxisLabel");

    QModelIndex curvesIdx = _bookModel()->getIndex(plotIdx,"Curves","Plot");
    QString unit = _bookModel()->getCurvesXUnit(curvesIdx);

    label = label + " {" + unit + "}";

    return label;
}

void XAxisLabelView::dataChanged(const QModelIndex &topLeft,
                                 const QModelIndex &bottomRight)
{
    if ( !model()) return;
    if ( topLeft.column() != 1 ) return;
    if ( topLeft.parent() != rootIndex() ) return;
    if ( topLeft != bottomRight ) return; // TODO: support multiple changes
    QModelIndex tagIdx = model()->index(topLeft.row(),0,topLeft.parent());
    QString tag = model()->data(tagIdx).toString();
    if ( tag != "PlotXAxisLabel" ) return;

    viewport()->update(); // important to use viewport()->update(), not update()
                          // since refresh will not be immediate with update()
}

void XAxisLabelView::rowsInserted(const QModelIndex &pidx, int start, int end)
{
    if ( rootIndex() != pidx ) return;
    if ( !pidx.isValid() ) return;
    if ( !model()) return;

    for ( int i = start; i <= end; ++i ) {
        QModelIndex idx = model()->index(i,0,pidx);
        if ( model()->data(idx).toString() == "PlotXAxisLabel" ) {
            idx = model()->sibling(i,1,idx);
            viewport()->update();
            break;
        }
    }
}
