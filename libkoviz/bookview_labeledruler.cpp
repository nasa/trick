#include "bookview_labeledruler.h"

LabeledRulerView::LabeledRulerView(Qt::Alignment alignment,
                                   QWidget *parent) :
    BookIdxView(parent),
    _alignment(alignment),
    _margin5(5)
{
    setFrameShape(QFrame::NoFrame);
}

void LabeledRulerView::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    if ( !model() ) return;


    QRect W = viewport()->rect();
    if ( W.width() == 0.0 || W.height() == 0.0 ) {
        return;
    }

    QRectF M = _mathRect();

    QModelIndex plotIdx = _bookModel()->getIndex(rootIndex(),"Plot");

    QList<double> modelTics;
    if ( _alignment == Qt::AlignBottom ) {
        modelTics = _majorXTics(plotIdx);
    } else if ( _alignment == Qt::AlignLeft ) {
        modelTics = _majorYTics(plotIdx);
    } else {
        // No support (yet) for right and top axes
        return;
    }

    QTransform T = _coordToPixelTransform();

    QList<LabelBox> boxes;
    foreach ( double tic, modelTics ) {
        QPointF center;
        if ( _alignment == Qt::AlignBottom ) {
            center.setX(tic);
            center.setY(M.center().y());
        } else if ( _alignment == Qt::AlignLeft ) {
            center.setX(M.center().x());
            center.setY(tic);
        }
        center = T.map(center);
        if ( _alignment == Qt::AlignBottom ) {
            center.setY(W.center().y());
        } else if ( _alignment == Qt::AlignLeft ) {
            center.setX(W.center().x());
        }

        QString strVal = _format(tic);
        QFontMetrics fm = viewport()->fontMetrics();
        QRectF bb = fm.boundingRect(strVal);
        bb.moveCenter(center);
        if ( bb.right() > W.right() ) {
            bb.translate(W.right()-bb.right(),0.0);  // move bb inside W
        }
        if ( _alignment == Qt::AlignLeft ) {
            bb.setLeft(W.left());
            bb.setRight(W.right()-_margin5);
        }

        LabelBox box;
        box.center = center;
        box.bb = bb;
        box.strVal = strVal;
        boxes << box;
    }

    // Draw!
    QPainter painter(viewport());
    painter.save();
    foreach ( LabelBox box, boxes ) {
        if ( _alignment == Qt::AlignLeft ) {
            painter.drawText(box.bb,Qt::AlignRight|Qt::AlignVCenter,box.strVal);
        } else {
            painter.drawText(box.bb,Qt::AlignCenter,box.strVal);
        }
    }
    painter.restore();
    painter.end();

}

QSize LabeledRulerView::minimumSizeHint() const
{
    QSize s = sizeHint();
    return s;
}

QSize LabeledRulerView::sizeHint() const
{
    QSize s(0,0);

    if ( _alignment == Qt::AlignBottom ) {
        s = _sizeHintBottom();
    } else if ( _alignment == Qt::AlignLeft ) {
        s = _sizeHintLeft();
    } else {
        fprintf(stderr, "koviz [bad scoobs]: LabeledRulerView::sizeHint() "
                        "Bad alignment.  Aborting!!!\n");
        exit(-1);
    }

    return s;
}

// Note: In order to align y axes across plots on a page, the size hint for
//       the left tic numbers takes all the plots on a page into account.
//       The sizehint is the same forall plots on the page.
QSize LabeledRulerView::_sizeHintLeft() const
{
    QSize s(0,0);

    QModelIndex pageIdx = _bookModel()->getIndex(rootIndex(),"Plot").
                          parent().parent();
    QModelIndexList plotIdxs = _bookModel()->plotIdxs(pageIdx);
    QFontMetrics fm = viewport()->fontMetrics();
    double w = 0;
    double h = 0;
    foreach ( QModelIndex plotIdx, plotIdxs ) {
        QList<double> modelTics = _majorYTics(plotIdx);
        foreach ( double tic, modelTics ) {
            QString strVal = _format(tic);
            QRectF bb = fm.boundingRect(strVal);
            if ( bb.width() > w )  w = bb.width();
            if ( bb.height() > h ) h = bb.height();
        }
    }

    s.setWidth(w+3*_margin5);  // 3 is arbitrary
    s.setHeight(h);

    return s;
}

QSize LabeledRulerView::_sizeHintBottom() const
{
    QSize s(0,0);

    QModelIndex plotIdx = _bookModel()->getIndex(rootIndex(),"Plot");
    QList<double> modelTics = _majorXTics(plotIdx);

    double w = 0;
    double h = 0;
    QFontMetrics fm = viewport()->fontMetrics();
    foreach ( double tic, modelTics ) {
        QString strVal = _format(tic);
        QRectF bb = fm.boundingRect(strVal);
        if ( bb.width() > w )  w = bb.width();
        if ( bb.height() > h ) h = bb.height();
    }

    s.setWidth(w*1.61);   // arbitrary margin with scale
    s.setHeight(h*1.61);

    return s;
}

void LabeledRulerView::dataChanged(const QModelIndex &topLeft,
                                   const QModelIndex &bottomRight)
{
    if ( topLeft.parent() != rootIndex() ) return;
    if ( topLeft.column() != 1 ) return;
    if ( topLeft != bottomRight ) return;

    if ( topLeft == _plotMathRectIdx(rootIndex()) ) {
        viewport()->update();
        updateGeometry();
    }
}

void LabeledRulerView::wheelEvent(QWheelEvent *e)
{
    QString from ;
    QModelIndex scaleIdx;
    if ( _alignment == Qt::AlignBottom ) {
        from = _bookModel()->getDataString(rootIndex(),"PlotXScale","Plot");
        scaleIdx = _bookModel()->getDataIndex(rootIndex(),"PlotXScale","Plot");
    } else if ( _alignment == Qt::AlignLeft ) {
        from = _bookModel()->getDataString(rootIndex(),"PlotYScale","Plot");
        scaleIdx = _bookModel()->getDataIndex(rootIndex(),"PlotYScale","Plot");
    }

    QString to;
    if ( from == "linear" ) {
        to = "log";
    } else if ( from == "log" ) {
        to = "linear";
    }

    _bookModel()->setData(scaleIdx,to);
}

QString LabeledRulerView::_format(double tic) const
{
    QString s;
    if ( qAbs(tic) < 1.0e-16 ) {
        tic = 0.0;
    }
    s = s.sprintf("%g", tic);
    return s;
}
