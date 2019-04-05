#include "bookview_yaxislabel.h"

YAxisLabelView::YAxisLabelView(QWidget *parent) :
    BookIdxView(parent)
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
    QString txt = fm.elidedText(_yAxisLabelText(), Qt::ElideLeft, vh);
    QRect bb = fm.tightBoundingRect(txt);
    int bw = bb.width();
    int bh = bb.height();

    // Draw!
    QPainter painter(viewport());
    painter.save();
    painter.translate(vw/2+bh-1,(vh+bw)/2);
    painter.rotate(270);
    painter.drawText(0,0,txt);
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
    QRect bb = fm.boundingRect(_yAxisLabelText());
    s.setWidth(2*bb.height());  // 2 is arbitrary
    s.setHeight(bb.width());
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

        //_yAxisLabelText = label + '{' + unit + '}';

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
            // TODOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO   if curve y unit changes, must change
            //idx = model()->sibling(i,1,idx);
            //_yAxisLabelText = model()->data(idx).toString();
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
        if ( e->delta() > 0 ) {
            toUnit = Unit::next(fromUnit);
        } else if ( e->delta() < 0 ) {
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

QString YAxisLabelView::_yAxisLabelText() const
{
    QString label;

    if ( !model() ) return label;
    QModelIndex plotIdx = rootIndex();
    if ( !plotIdx.isValid() ) return label;
    QString plotTag = model()->data(plotIdx).toString();
    if ( plotTag != "Plot" ) return label;

    label = _bookModel()->getDataString(plotIdx,"PlotYAxisLabel");

    QModelIndex curvesIdx = _bookModel()->getIndex(plotIdx,"Curves","Plot");
    QString unit = _bookModel()->getCurvesYUnit(curvesIdx);

    if ( !label.isEmpty() ) {
        label = label + " {" + unit + "}";
    }

    return label;
}
