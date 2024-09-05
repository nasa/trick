#include "layoutitem_xaxislabel.h"

XAxisLabelLayoutItem::XAxisLabelLayoutItem(const QFontMetrics &fontMetrics,
                                           PlotBookModel* bookModel,
                                           const QModelIndex& plotIdx) :
    PaintableLayoutItem(),
    _fontMetrics(fontMetrics),
    _bookModel(bookModel),
    _plotIdx(plotIdx)
{
}

XAxisLabelLayoutItem::~XAxisLabelLayoutItem()
{
}

Qt::Orientations XAxisLabelLayoutItem::expandingDirections() const
{
    return Qt::Horizontal;
}

QRect XAxisLabelLayoutItem::geometry() const
{
    return _rect;
}

bool XAxisLabelLayoutItem::isEmpty() const
{
    return true;
}

QSize XAxisLabelLayoutItem::maximumSize() const
{
    return QSize(0,0);
}

QSize XAxisLabelLayoutItem::minimumSize() const
{
    return QSize(0,0);
}

void XAxisLabelLayoutItem::setGeometry(const QRect &r)
{
    _rect = r;
}

QSize XAxisLabelLayoutItem::sizeHint() const
{
    QSize size(0,0);
    QRect bb = _fontMetrics.boundingRect("Thequickbrownfoxjumpsoverthelazydog");
    size.setHeight(bb.height());
    return size;
}

void XAxisLabelLayoutItem::paint(QPainter *painter,
                                 const QRect &R, const QRect &RG,
                                 const QRect &C, const QRectF &M)
{
    Q_UNUSED(RG);
    Q_UNUSED(C);
    Q_UNUSED(M);

    int rx = R.x();
    int ry = R.y();
    int rw = R.width();
    QFontMetrics fm = painter->fontMetrics();
    QString txt = fm.elidedText(_xAxisLabelText(), Qt::ElideLeft, R.width());
    QRect bb = fm.tightBoundingRect(txt);
    int bw = bb.width();

    // Draw!
    painter->save();
    painter->translate(rx,ry);
    painter->translate((rw-bw)/2, fm.ascent());
    painter->drawText(0,0,txt);
    painter->restore();
}

QString XAxisLabelLayoutItem::_xAxisLabelText() const
{
    QString label;

    if ( !_bookModel->isChildIndex(_plotIdx,"Plot","PlotXAxisLabel")) {
        return label;
    }

    // Try commandline option for xaxislabel
    label = _bookModel->getDataString(QModelIndex(),"XAxisLabel");

    if ( label.isEmpty() ) {
        // Normal case when xaxislabel not overridden with command line option
        label = _bookModel->getDataString(_plotIdx,"PlotXAxisLabel");

        QModelIndex curvesIdx = _bookModel->getIndex(_plotIdx,"Curves","Plot");
        QString unit = _bookModel->getCurvesXUnit(curvesIdx);

        label = label.trimmed();
        if ( !label.isEmpty() ) {
            label = label + " {" + unit + "}";
        }
    }

    return label;
}
