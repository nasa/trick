#include "layoutitem_plottitle.h"

PlotTitleLayoutItem::PlotTitleLayoutItem(const QFontMetrics &fontMetrics,
                                         PlotBookModel* bookModel,
                                         const QModelIndex& plotIdx) :
    PaintableLayoutItem(),
    _fontMetrics(fontMetrics),
    _bookModel(bookModel),
    _plotIdx(plotIdx)
{
}

PlotTitleLayoutItem::~PlotTitleLayoutItem()
{
}

Qt::Orientations PlotTitleLayoutItem::expandingDirections() const
{
    return Qt::Horizontal;
}

QRect PlotTitleLayoutItem::geometry() const
{
    return _rect;
}

bool PlotTitleLayoutItem::isEmpty() const
{
    return true;
}

QSize PlotTitleLayoutItem::maximumSize() const
{
    return QSize(0,0);
}

QSize PlotTitleLayoutItem::minimumSize() const
{
    return QSize(0,0);
}

void PlotTitleLayoutItem::setGeometry(const QRect &r)
{
    _rect = r;
}

QSize PlotTitleLayoutItem::sizeHint() const
{
    QSize size(0,0);
    QRect bb = _fontMetrics.boundingRect("Thequickbrownfoxjumpsoverthelazydog");
    size.setHeight(bb.height());
    return size;
}

void PlotTitleLayoutItem::paint(QPainter *painter,
                                const QRect &R, const QRect &RG,
                                const QRect &C, const QRectF &M)
{
    painter->save();
    _paintGrid(painter,R,RG,C,M);
    painter->setRenderHint(QPainter::Antialiasing);
    QString s = _bookModel->getDataString(_plotIdx,"PlotTitle","Plot");
    QFontMetrics fm = painter->fontMetrics();
    QRect bbox = fm.boundingRect(s);
    int x = R.x() + R.width()/2 - bbox.width()/2;
    int y = R.y() + fm.ascent();
    painter->drawText(x,y,s);
    painter->restore();
}

void PlotTitleLayoutItem::_paintGrid(QPainter* painter,
                                     const QRect &R,const QRect &RG,
                                     const QRect &C, const QRectF &M)
{
    // If Grid is off, do not paint grid
    QModelIndex isGridIdx = _bookModel->getDataIndex(_plotIdx,
                                                     "PlotGrid","Plot");
    bool isGrid = _bookModel->data(isGridIdx).toBool();
    if ( !isGrid ) {
        return;
    }

    QString plotXScale = _bookModel->getDataString(_plotIdx,
                                                   "PlotXScale","Plot");
    QString plotYScale = _bookModel->getDataString(_plotIdx,
                                                   "PlotYScale","Plot");
    bool isXLogScale = ( plotXScale == "log" ) ? true : false;
    bool isYLogScale = ( plotYScale == "log" ) ? true : false;

    QList<double> xtics = _bookModel->majorXTics(_plotIdx);
    if ( isXLogScale ) {
        xtics.append(_bookModel->minorXTics(_plotIdx));
    }
    QList<double> ytics = _bookModel->majorYTics(_plotIdx);
    if ( isYLogScale ) {
        ytics.append(_bookModel->minorYTics(_plotIdx));
    }

    QVector<QPointF> vLines;
    QVector<QPointF> hLines;

    const QRectF RM = mathRect(RG,C,M);
    QTransform T = coordToDotTransform(R,RM);

    foreach ( double x, xtics ) {
        vLines << T.map(QPointF(x,RM.top())) << T.map(QPointF(x,RM.bottom()));
    }
    foreach ( double y, ytics ) {
        hLines << T.map(QPointF(RM.left(),y)) << T.map(QPointF(RM.right(),y));
    }

    bool isAntiAliasing = (QPainter::Antialiasing & painter->renderHints()) ;

    // Grid Color
    QModelIndex pageIdx = _bookModel->getIndex(_plotIdx,"Page","Plot");
    QColor color = _bookModel->pageForegroundColor(pageIdx);
    color.setAlpha(40);

    // Pen
    QVector<qreal> dashes;
    qreal space = 4;
    if ( isXLogScale || isYLogScale ) {
        dashes << 1 << 1 ;
    } else {
        dashes << 4 << space ;
    }

    //
    // Draw!
    //
    painter->save();
    painter->setClipRect(R);
    painter->setRenderHint(QPainter::Antialiasing,false);
    QPen origPen = painter->pen();
    QPen pen = painter->pen();
    pen.setColor(color);
    int penWidth = painter->fontMetrics().xHeight()/7;  // 7 arbitrary
    pen.setWidth(penWidth);
    pen.setDashPattern(dashes);
    painter->setPen(pen);
    painter->drawLines(hLines);
    painter->drawLines(vLines);
    painter->setPen(origPen);
    if ( isAntiAliasing ) {
        painter->setRenderHint(QPainter::Antialiasing);
    }
    painter->restore();
}
