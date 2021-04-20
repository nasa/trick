#include "bookview_plottitle.h"

PlotTitleView::PlotTitleView(QWidget *parent) :
    BookIdxView(parent),
    _buttonSelectAndPan(Qt::LeftButton),
    _buttonRubberBandZoom(Qt::MidButton),
    _buttonResetView(Qt::RightButton)
{
    setFrameShape(QFrame::NoFrame);
    this->setSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::Fixed);
    setMouseTracking(true);
}

void PlotTitleView::setModel(QAbstractItemModel *model)
{
    BookIdxView::setModel(model);

    QHash<QString,Qt::MouseButton> button2mouse;
    button2mouse.insert("left",Qt::LeftButton);
    button2mouse.insert("right",Qt::RightButton);
    button2mouse.insert("middle",Qt::MiddleButton);

    QString buttonSelect = _bookModel()->getDataString(
                                         QModelIndex(),"ButtonSelectAndPan","");
    QString buttonZoom = _bookModel()->getDataString(
                                         QModelIndex(),"ButtonZoom","");
    QString buttonReset = _bookModel()->getDataString(
                                         QModelIndex(),"ButtonReset","");

    _buttonSelectAndPan = button2mouse.value(buttonSelect);
    _buttonRubberBandZoom = button2mouse.value(buttonZoom);
    _buttonResetView = button2mouse.value(buttonReset);
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

void PlotTitleView::mousePressEvent(QMouseEvent *event)
{
    if (  event->button() == _buttonSelectAndPan ) {
        _mousePressPos = event->pos();
    } else if (  event->button() == _buttonRubberBandZoom ) {
        event->ignore();
    }
}

void PlotTitleView::mouseReleaseEvent(QMouseEvent *event)
{
    if (  event->button() == _buttonSelectAndPan ) {
        double x0 = _mousePressPos.x();
        double y0 = _mousePressPos.y();
        double x1 = event->pos().x();
        double y1 = event->pos().y();
        double d = qSqrt((x1-x0)*(x1-x0)+(y1-y0)*(y1-y0));
        if ( d < 10 ) {
            // Toggle between single/multiview if not dragging mouse
            event->ignore();
        } else {
            QAbstractItemView::mouseReleaseEvent(event);
        }
    } else if ( event->button() == _buttonRubberBandZoom ) {
        event->ignore();
    } else {
        QAbstractItemView::mouseReleaseEvent(event);
    }
}

void PlotTitleView::mouseMoveEvent(QMouseEvent *event)
{
    if ( event->buttons() == _buttonRubberBandZoom ) {
        event->ignore();
    } else {
        QAbstractItemView::mouseMoveEvent(event);
    }
}
