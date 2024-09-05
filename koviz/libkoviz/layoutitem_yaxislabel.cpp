#include "layoutitem_yaxislabel.h"


YAxisLabelLayoutItem::YAxisLabelLayoutItem(const QFontMetrics &fontMetrics,
                                           PlotBookModel* bookModel,
                                           const QModelIndex& plotIdx) :
    PaintableLayoutItem(),
    _fontMetrics(fontMetrics),
    _bookModel(bookModel),
    _plotIdx(plotIdx)
{
}

YAxisLabelLayoutItem::~YAxisLabelLayoutItem()
{
}

Qt::Orientations YAxisLabelLayoutItem::expandingDirections() const
{
    return Qt::Vertical;
}

QRect YAxisLabelLayoutItem::geometry() const
{
    return _rect;
}

bool YAxisLabelLayoutItem::isEmpty() const
{
    return true;
}

QSize YAxisLabelLayoutItem::maximumSize() const
{
    return QSize(0,0);
}

QSize YAxisLabelLayoutItem::minimumSize() const
{
    return QSize(0,0);
}

void YAxisLabelLayoutItem::setGeometry(const QRect &r)
{
    _rect = r;
}

QSize YAxisLabelLayoutItem::sizeHint() const
{
    QSize size(0,0);
    QRect bb = _fontMetrics.boundingRect("Thequickbrownfoxjumpsoverthelazydog");
    size.setWidth(bb.height());
    return size;
}

void YAxisLabelLayoutItem::paint(QPainter *painter,
                                 const QRect &R, const QRect &RG,
                                 const QRect &C, const QRectF &M)
{
    Q_UNUSED(RG);
    Q_UNUSED(C);
    Q_UNUSED(M);

    int rx = R.x();
    int ry = R.y();
    int rh = R.height();
    QFontMetrics fm = painter->fontMetrics();
    QString txt = fm.elidedText(_yAxisLabelText(), Qt::ElideLeft, R.height());
    QRect bb = fm.tightBoundingRect(txt);
    int bw = bb.width();

    // Draw!
    painter->save();
    painter->translate(rx,ry);
    painter->translate(fm.ascent(),(rh+bw)/2);
    painter->rotate(270);
    painter->drawText(0,0,txt);
    painter->restore();
}

QString YAxisLabelLayoutItem::_yAxisLabelText() const
{
    QString label;

    if ( !_bookModel->isChildIndex(_plotIdx,"Plot","PlotYAxisLabel")) {
        return label;
    }

    // Try commandline option for yaxislabel
    label = _bookModel->getDataString(QModelIndex(),"YAxisLabel");

    if ( label.isEmpty() ) {
        // Normal case when yaxislabel not overridden with command line option
        label = _bookModel->getDataString(_plotIdx,"PlotYAxisLabel");

        QModelIndex curvesIdx = _bookModel->getIndex(_plotIdx,"Curves","Plot");
        QString unit = _bookModel->getCurvesYUnit(curvesIdx);

        label = label.trimmed();
        if ( !label.isEmpty() ) {
            label = label + " {" + unit + "}";
        }
    }

    return label;
}
