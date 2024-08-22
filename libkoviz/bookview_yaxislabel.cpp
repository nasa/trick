#include "bookview_yaxislabel.h"

YAxisLabelView::YAxisLabelView(QWidget *parent) :
    BookIdxView(parent)
{
    setFrameShape(QFrame::NoFrame);
    this->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::MinimumExpanding);
}

void YAxisLabelView::paintEvent(QPaintEvent *event)
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
    YAxisLabelLayoutItem layoutItem(fontMetrics(),_bookModel(),rootIndex());
    layoutItem.paint(&painter,R,RG,C,M);
}

QSize YAxisLabelView::minimumSizeHint() const
{
    return sizeHint();
}

QSize YAxisLabelView::sizeHint() const
{
    QSize s;
    YAxisLabelLayoutItem layoutItem(fontMetrics(),_bookModel(),rootIndex());
    s = layoutItem.sizeHint();
    return s;
}

void YAxisLabelView::dataChanged(const QModelIndex &topLeft,
                                 const QModelIndex &bottomRight)
{
    if ( !model()) return;
    if ( topLeft.column() != 1 ) return;
    if ( topLeft != bottomRight ) return; // TODO: support multiple changes
    QModelIndex tagIdx = model()->index(topLeft.row(),0,topLeft.parent());
    QString tag = model()->data(tagIdx).toString();
    if ( tag == "PlotYAxisLabel" || tag == "CurveYUnit" || tag == "CurveData") {

        QString label;
        if ( _bookModel()->isChildIndex(rootIndex(),"Plot","PlotYAxisLabel")) {
            label = _bookModel()->getDataString(rootIndex(),
                                                "PlotYAxisLabel", "Plot");
        }

        QString unit;
        QModelIndex curvesIdx;
        if ( _bookModel()->isChildIndex(rootIndex(),"Plot","Curves") ) {
            curvesIdx = _bookModel()->getIndex(rootIndex(),"Curves","Plot");
            if ( curvesIdx.isValid() ) {
                unit = _bookModel()->getCurvesYUnit(curvesIdx);
            }
        }

        viewport()->update();
    }
}

void YAxisLabelView::rowsInserted(const QModelIndex &pidx, int start, int end)
{
    if ( rootIndex() != pidx ) return;
    if ( !pidx.isValid() ) return;
    if ( !model()) return;

    for ( int i = start; i <= end; ++i ) {
        QModelIndex idx = model()->index(i,0,pidx);
        if ( model()->data(idx).toString() == "PlotYAxisLabel" ) {
            viewport()->update();
            break;
        }
    }
}

// Change curve units based on current unit
void YAxisLabelView::wheelEvent(QWheelEvent *e)
{
    QModelIndex curvesIdx;
    if ( _bookModel()->isChildIndex(rootIndex(),"Plot","Curves") ) {
        curvesIdx = _bookModel()->getIndex(rootIndex(),"Curves","Plot");
    }

    if ( curvesIdx.isValid() ) {

        QString fromUnit = _bookModel()->getCurvesYUnit(curvesIdx);
        QString toUnit;
        if ( e->angleDelta().y() > 0 ) {
            toUnit = Unit::next(fromUnit);
        } else if ( e->angleDelta().y() < 0 ) {
            toUnit = Unit::prev(fromUnit);
        }

        QModelIndexList curveIdxs = _bookModel()->getIndexList(curvesIdx,
                                                              "Curve","Curves");
        // Set all curves to next/prev unit
        bool block = model()->blockSignals(true);
        foreach (QModelIndex curveIdx, curveIdxs ) {
            QModelIndex yUnitIdx = _bookModel()->getDataIndex(curveIdx,
                                                         "CurveYUnit", "Curve");
            model()->setData(yUnitIdx,toUnit);
        }
        model()->blockSignals(block);

        // Get plot scale (log or linear)
        QString plotXScale = _bookModel()->getDataString(rootIndex(),
                                                         "PlotXScale",
                                                         "Plot");
        QString plotYScale = _bookModel()->getDataString(rootIndex(),
                                                         "PlotYScale",
                                                         "Plot");

        // Recalculate and update bounding box (since unit change)
        if ( plotXScale == "linear" && plotYScale == "linear" ) {
            double scale = Unit::scale(fromUnit,toUnit);
            double bias = Unit::bias(fromUnit,toUnit);
            QModelIndex plotMathRectIdx = _bookModel()->getDataIndex(rootIndex(),
                                                       "PlotMathRect","Plot");
            QRectF R = model()->data(plotMathRectIdx).toRectF();
            R.moveTop(R.y()*scale+bias);  // bias for temperature
            R.setHeight(R.height()*scale);
            _bookModel()->setPlotMathRect(R,rootIndex());
        } else {
            QRectF bbox = _bookModel()->calcCurvesBBox(curvesIdx);
            _bookModel()->setPlotMathRect(bbox,rootIndex());
        }

        viewport()->update();
    }
}
