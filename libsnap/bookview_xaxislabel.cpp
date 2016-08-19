#include "bookview_xaxislabel.h"

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
    QRect bb = fm.boundingRect(_xAxisLabelText);
    s.setWidth(bb.width());
    s.setHeight(2*bb.height()); // 2 is arbitrary
    return s;
}

// Changes unit for some common conversions
//
// TODO: There is code duplication in YAxisLabelView
void XAxisLabelView::wheelEvent(QWheelEvent *e)
{
    // Extract what is inside curly brackets in Plot[XY]AxisLabel
    // e.g. if PlotXAxisLabel == "Time {s}" return "s"
    QString unit;
    int openCurly = _xAxisLabelText.indexOf('{');
    if ( openCurly >= 0 ) {
        int closeCurly = _xAxisLabelText.indexOf('}');
        if ( closeCurly > openCurly+1 ) {
            // e.g. for "Time {s}" ->  openCurly=5 closeCurly=7
            unit = _xAxisLabelText.mid(openCurly+1, closeCurly-openCurly-1);
        }
    }
    if ( unit.isEmpty() || unit == "--" ) {
        return;
    }

    QList<QString> times;
    times << "s" << "ms" << "us" << "min" << "hr" << "day" ;
    QList<QString> lengths;
    lengths <<  "m" << "ft" << "in" << "mm" << "cm" << "km" ;
    QList<QString> angles;
    angles << "r" << "d" ;
    QList<QString> masses;
    masses << "kg" << "sl" << "lbm", "g" ;
    QList<QString> forces;
    forces <<   "N" << "kN" << "oz" << "lbf";
    QList<QString> speeds;
    speeds << "m/s" << "cm/s" << "ft/s" ;

    QList<QList<QString> > units;
    units << times << lengths << angles << masses << forces << speeds;

    QString nextUnit = unit;
    int len = units.length();
    for (int i = 0; i < len; ++i) {
        QList<QString> unitFamily = units.at(i);
        if ( unitFamily.contains(unit) ) {
            int j = unitFamily.indexOf(unit);
            int k = 0;
            if ( e->delta() > 0 ) {
                k = j+1;
                if ( k >= unitFamily.length() ) {
                    k = 0;
                }
                nextUnit = unitFamily.at(k);
            } else {
                k = j-1;
                if ( k < 0 ) {
                    k = unitFamily.length()-1;
                }
            }
            nextUnit = unitFamily.at(k);
        }
    }

    QModelIndex curvesIdx = _bookModel()->getIndex(_myIdx,"Curves","Plot");
    QModelIndexList curveIdxs = _bookModel()->getIndexList(curvesIdx,
                                                           "Curve","Curves");
    foreach (QModelIndex curveIdx, curveIdxs ) {
        QModelIndex xUnitIdx = _bookModel()->getDataIndex(curveIdx,
                                                         "CurveXUnit", "Curve");
        model()->setData(xUnitIdx,nextUnit);
    }
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
