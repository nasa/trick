#include "layoutitem_ticlabels.h"

TicLabelsLayoutItem::TicLabelsLayoutItem(const QFontMetrics &fontMetrics,
                                         const QFontMetrics &fm8,
                                         PlotBookModel *bookModel,
                                         const QModelIndex &plotIdx) :
    _fontMetrics(fontMetrics),
    _fm8(fm8),
    _bookModel(bookModel),
    _plotIdx(plotIdx)
{
    _m1 = fontMetrics.averageCharWidth()/2;
    _m2 = fontMetrics.averageCharWidth()/2;
    _m3 = fontMetrics.xHeight()/2;
}

TicLabelsLayoutItem::~TicLabelsLayoutItem()
{
}

Qt::Orientations TicLabelsLayoutItem::expandingDirections() const
{
    return Qt::Vertical;
}

QRect TicLabelsLayoutItem::geometry() const
{
    return _rect;
}

bool TicLabelsLayoutItem::isEmpty() const
{
    return true;
}

QSize TicLabelsLayoutItem::maximumSize() const
{
    return QSize(0,0);
}

QSize TicLabelsLayoutItem::minimumSize() const
{
    return QSize(0,0);
}

void TicLabelsLayoutItem::setGeometry(const QRect &r)
{
    _rect = r;
}

// Note: In order to align y axes across plots on a page, the size hint for
//       the y tic numbers takes all the plots on a page into account.
//       The sizehint is the same forall plots on the page.
QSize TicLabelsLayoutItem::sizeHint() const
{
    QSize s(0,0);

    if ( alignment() == Qt::AlignBottom ) {
        s = _sizeHintBottom();
    } else if ( alignment() == Qt::AlignLeft ) {
        s = _sizeHintLeft();
    } else {
        fprintf(stderr, "koviz [bad scoobs]: LabeledRulerView::sizeHint() "
                        "Bad alignment.  Aborting!!!\n");
        exit(-1);
    }

    return s;
}

void TicLabelsLayoutItem::paint(QPainter *painter,
                                const QRect &R, const QRect &RG,
                                const QRect &C, const QRectF &M)
{
    QList<double> modelTics;
    if ( alignment() == Qt::AlignBottom ) {
        modelTics = _bookModel->majorXTics(_plotIdx);
    } else if ( alignment() == Qt::AlignLeft ) {
        modelTics = _bookModel->majorYTics(_plotIdx);
    } else {
        // No support (yet) for right and top axes
        return;
    }

    const QRectF RM = mathRect(RG,C,M);
    QTransform T = coordToDotTransform(R,RM);

    QList<RulerLabelBox> boxes;
    foreach ( double tic, modelTics ) {
        QPointF center;
        if ( alignment() == Qt::AlignBottom ) {
            center.setX(tic);
            center.setY(M.center().y());
        } else if ( alignment() == Qt::AlignLeft ) {
            center.setX(M.center().x());
            center.setY(tic);
        }
        center = T.map(center);
        if ( alignment() == Qt::AlignBottom ) {
            center.setY(R.center().y());
        } else if ( alignment() == Qt::AlignLeft ) {
            center.setX(R.center().x());
        }

        QString strVal = _format(tic);
        QRectF bb = _boundingRect(strVal);
        bb.moveCenter(center);
        if ( bb.right() > R.right() ) {
            bb.translate(R.right()-bb.right(),0.0);  // move bb inside W
        }
        if ( alignment() == Qt::AlignLeft ) {
            bb.setLeft(R.left()-_m1);
            bb.setRight(R.right()-_m2);
        }

        RulerLabelBox box;
        box.center = center;
        box.bb = bb;
        box.strVal = strVal;
        boxes << box;
    }

    // Get plot scale
    QString plotXScale = _bookModel->getDataString(_plotIdx,
                                                   "PlotXScale","Plot");
    QString plotYScale = _bookModel->getDataString(_plotIdx,
                                                   "PlotYScale","Plot");

    // Draw!
    QFont fontOrig = painter->font();
    if ( fontOrig.pointSizeF() == 11.0 ) {
        // Special tweak for a pointsize of 11.0
        QFont font105 = fontOrig;
        font105.setPointSizeF(10.5);
        painter->setFont(font105);
    }
    foreach ( RulerLabelBox box, boxes ) {
        if ( alignment() == Qt::AlignLeft ) {
            if ( plotYScale == "linear" ) {
                painter->drawText(box.bb,Qt::AlignRight|Qt::AlignVCenter,
                                 box.strVal);
            } else if ( plotYScale == "log" ) {
                _paint10Exponent(painter,box);
            }
        } else if ( alignment() == Qt::AlignBottom ) {
            if ( plotXScale == "linear" ) {
                painter->drawText(box.bb,Qt::AlignCenter,box.strVal);
            } else if ( plotXScale == "log" ) {
                _paint10Exponent(painter,box);
            }
        }
    }
    painter->setFont(fontOrig);
}

QRect TicLabelsLayoutItem::_boundingRect(const QString &strVal) const
{
    QRect bb;
    if ( !_bookModel->isChildIndex(_plotIdx,"Plot","PlotXScale") ||
         !_bookModel->isChildIndex(_plotIdx,"Plot","PlotYScale") ) {
        return bb;
    }

    QString plotXScale = _bookModel->getDataString(_plotIdx,
                                                   "PlotXScale","Plot");
    QString plotYScale = _bookModel->getDataString(_plotIdx,
                                                   "PlotYScale","Plot");

    if ( (plotXScale == "log" && alignment() == Qt::AlignBottom) ||
         (plotYScale == "log" && alignment() == Qt::AlignLeft) ) {
        QRect box10 = _fontMetrics.boundingRect("10");
        QRect boxExponent = _fm8.boundingRect(strVal);
        QPoint p;
        p.setX(box10.right()+_fm8.averageCharWidth());
        p.setY(box10.top()+_fm8.height()/2.0);
        boxExponent.moveTo(p);
        bb = box10.united(boxExponent);
    } else {
        bb = _fontMetrics.boundingRect(strVal);
    }

    return bb;
}

QString TicLabelsLayoutItem::_format(double tic) const
{
    QString s;
    s = s.asprintf("%g", tic);
    return s;
}

void TicLabelsLayoutItem::_paint10Exponent(QPainter *painter,
                                           const RulerLabelBox& box) const
{
    QFont fontOrig = painter->font();
    QFontMetrics fm(painter->font());
    QFont font8 = painter->font();
    font8.setPointSizeF(8);
    QFontMetrics fm8(font8);
    QRectF B = _boundingRect(box.strVal); // strVal is exponent
    B.moveCenter(box.bb.center());
    B.moveRight(box.bb.right());

    painter->setFont(fontOrig);
    QRectF box10 = fm.boundingRect("10 ");
    box10.moveBottomLeft(B.bottomLeft());
    painter->drawText(B,Qt::AlignLeft|Qt::AlignBottom,"10");

    painter->setFont(font8);
    QRectF boxExponent = fm8.boundingRect(box.strVal);
    boxExponent.moveTopRight(B.topRight());
    boxExponent.translate(-fm8.averageCharWidth()/2.0,0);
    boxExponent.setRight(B.right());
    painter->drawText(boxExponent,box.strVal);

    painter->setFont(fontOrig);
}

QSize TicLabelsLayoutItem::_sizeHintLeft() const
{
    QSize s(0,0);

    QModelIndex pageIdx = _bookModel->getIndex(_plotIdx,"Plot").
                                      parent().parent();
    QModelIndexList plotIdxs = _bookModel->plotIdxs(pageIdx);
    double w = 0;
    double h = 0;
    foreach ( QModelIndex plotIdx, plotIdxs ) {
        QList<double> modelTics = _bookModel->majorYTics(plotIdx);
        foreach ( double tic, modelTics ) {
            QString strVal = _format(tic);
            QRectF bb = _boundingRect(strVal);
            if ( bb.width() > w )  w = bb.width();
            if ( bb.height() > h ) h = bb.height();
        }
    }

    s.setWidth(_m1+w+_m2);
    s.setHeight(h);

    return s;
}

QSize TicLabelsLayoutItem::_sizeHintBottom() const
{
    QSize s(0,0);

    QList<double> modelTics = _bookModel->majorXTics(_plotIdx);

    double w = 0;
    double h = 0;
    foreach ( double tic, modelTics ) {
        QString strVal = _format(tic);
        QRectF bb = _boundingRect(strVal);
        if ( bb.width() > w )  w = bb.width();
        if ( bb.height() > h ) h = bb.height();
    }

    s.setWidth(w);
    s.setHeight(h+_m3);

    return s;
}
