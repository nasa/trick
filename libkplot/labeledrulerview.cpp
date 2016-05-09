#include "labeledrulerview.h"

LabeledRulerView::LabeledRulerView(Qt::Alignment alignment,
                                   QWidget *parent) :
    BookIdxView(parent),
    _alignment(alignment)
{
}

void LabeledRulerView::_update()
{
}

void LabeledRulerView::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    if ( !model() ) return;

    QRect W = viewport()->rect();

    QRectF M = _mathRect();

    QRect V = _curvesView->viewport()->rect();
    if ( W.width() == 0.0 || W.height() == 0.0 ) {
        return;
    }


    QList<double> modelTics;
    if ( _alignment == Qt::AlignBottom ) {
        modelTics = _majorXTics();
    } else if ( _alignment == Qt::AlignLeft ) {
        modelTics = _majorYTics();
    } else {
        // No support (yet) for right and top axes
        return;
    }

    double minGap;
    if ( _alignment == Qt::AlignBottom ) {
        minGap = fontMetrics().boundingRect("xx").width();
    } else if ( _alignment == Qt::AlignLeft ) {
        minGap = fontMetrics().boundingRect("X").height();
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

        LabelBox box;
        box.center = center;
        box.bb = bb;
        box.strVal = strVal;
        boxes << box;
    }

#if 0
    while (1) {

        if ( _isFits(boxes,W,minGap,_alignment) ) {
            break;
        }

        for ( int i = 0; i < boxes.size(); ++i ) {
            QList<LabelBox> movedBox = _moveCurrBoxToFit(boxes,i,i-1,W,minGap);
            if ( !movedBox.isEmpty() ) {
                boxes.replace(i,movedBox.at(0));
            }
        }

        for ( int i = boxes.size()-1; i >= 0; --i ) {
            QList<LabelBox> movedBox = _moveCurrBoxToFit(boxes,i,i+1,W,minGap);
            if ( !movedBox.isEmpty() ) {
                boxes.replace(i,movedBox.at(0));
            }
        }

        if ( !_isFits(boxes,W,minGap,_alignment) ) {

            boxes = _halfBoxSet(boxes,_alignment);

            // Move all boxes back to center
            for ( int i = 0; i < boxes.size(); ++i ) {
                LabelBox box = boxes.at(i) ;
                QRectF bb = box.bb;
                bb.moveCenter(box.center);
                box.bb = bb;
                boxes.replace(i,box);
            }

        } else {
            break;
        }
    }
#endif

    // Draw!
    QPainter painter(viewport());
    painter.save();
    foreach ( LabelBox box, boxes ) {
        painter.drawText(box.bb, Qt::AlignHCenter|Qt::AlignVCenter, box.strVal);
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

    QList<double> modelTics;
    if ( _alignment == Qt::AlignBottom ) {
        modelTics = _majorXTics();
    } else if ( _alignment == Qt::AlignLeft ) {
        modelTics = _majorYTics();
    } else {
        // No support (yet) for right and top axes
        return s;
    }

    double w = 0;
    double h = 0;
    QFontMetrics fm = viewport()->fontMetrics();
    foreach ( double tic, modelTics ) {
        QString strVal = _format(tic);
        QRectF bb ;
        if ( _alignment == Qt::AlignBottom ) {
            bb = fm.boundingRect(strVal);
        } else if ( _alignment == Qt::AlignLeft ) {
            bb = fm.boundingRect(strVal);
        }
        if ( bb.width() > w ) {
            w = bb.width();
        }
        if ( bb.height() > h ) {
            h = bb.height();
        }
    }

    s.setWidth(w);
    s.setHeight(h);

    return s;
}

// Do boxes fit in W with minGap between each box in boxes?
bool LabeledRulerView::_isFits(const QList<LabelBox> &boxes,
                           const QRectF &W, double minGap,
                           Qt::Alignment alignment )
{
    bool isFits = true;

    for ( int i = 0; i < boxes.size(); ++i ) {

        QRectF bb = boxes.at(i).bb;

        if ( !W.contains(bb) ) {
            isFits = false;
            break;
        }
        if ( i == 0 ) {
            continue;
        }
        QRectF lastBB = boxes.at(i-1).bb;
        if ( bb.intersects(lastBB) ) {
            isFits = false;
            break;
        }
        if ( alignment == Qt::AlignBottom ) {
            if ( (bb.left() - lastBB.right()) < minGap ) {
                isFits = false;
                break;
            }
        } else if ( alignment == Qt::AlignLeft ) {
            if ( (lastBB.bottom() - bb.top()) < minGap ) {
                isFits = false;
                break;
            }
        }
    }


    return isFits;
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

//
// Return singleton with a moved box if curr box moved to fit
// otherwise return empty set
//
QList<LabelBox> LabeledRulerView::_moveCurrBoxToFit(
                                       const QList<LabelBox> &boxes,
                                       int currBoxIdx,
                                       int lastBoxIdx,
                                       const QRectF &W, double minGap)
{
    QList<LabelBox> bxs;

    if ( boxes.isEmpty() ) return bxs;
    if ( currBoxIdx == lastBoxIdx ) return bxs;
    if ( currBoxIdx < 0 || currBoxIdx >= boxes.size() ) return bxs;

    QRectF bb = boxes.at(currBoxIdx).bb;
    QPointF center = boxes.at(currBoxIdx).center;

    QRectF lastBB;
    if ( (currBoxIdx > lastBoxIdx && lastBoxIdx >= 0) ||
         (currBoxIdx < lastBoxIdx && lastBoxIdx < boxes.size()) ) {
        lastBB = boxes.at(lastBoxIdx).bb;
    }

    if ( !W.contains(bb) ) {
        // Bounding box does not fit in window rect
        if ( _alignment == Qt::AlignBottom ) {
            if ( bb.left() < W.left() ) {
                double dx = W.left() - bb.left();
                bb.translate(dx,0);
            } else if ( bb.right() > W.right() ) {
                double dx = bb.right() - W.right();
                bb.translate(-dx,0);
            }
        } else if ( _alignment == Qt::AlignLeft ) {
        }
    }

    if ( bb.intersects(lastBB) ) {
        // Last bounding box overlaps current bounding box (numbers overlap)
        QRectF intersection = bb.intersected(lastBB);
        if ( _alignment == Qt::AlignBottom ) {
            double dx = intersection.width() + minGap;
            dx = ( currBoxIdx < lastBoxIdx ) ? -dx : dx;
            bb.translate(dx,0);
        } else if ( _alignment == Qt::AlignLeft ) {
        }
    }

    // Check that last bounding box and current spread apart by minGap
    if ( currBoxIdx > 0 && lastBoxIdx >= 0 && lastBoxIdx < boxes.size() )  {
        if ( _alignment == Qt::AlignBottom ) {
            if ( currBoxIdx > lastBoxIdx ) {
                double dx = bb.left() - lastBB.right();
                if ( dx < minGap ) {
                    bb.translate(minGap-dx,0);
                }
            } else {
                double dx = lastBB.left() - bb.right();
                if ( dx < minGap ) {
                    bb.translate(dx-minGap,0);
                }
            }
        } else if ( _alignment == Qt::AlignLeft ) {
        }
    }

    // Finally, after possibly adjusting bounding box,
    // make sure label is aligned with tic mark
    if ( bb.contains(center) ) {
        LabelBox bx;
        bx.center = boxes.at(currBoxIdx).center;
        bx.strVal = boxes.at(currBoxIdx).strVal;
        bx.bb = bb;
        bxs << bx;
    }

    return bxs;
}

// Return "even subset" of input set
//
// Eg. In:      {0,1,2,3,4,5,6}
//     Return:  {0,2,4,6}
//
// Eg. In:      {0.375, 0.500, 0.625}
//     Return:  {0.500}
//
// Eg. In:      {0.2, 0.4, 0.6, 0.8}
//     Return:  {0.4,0.8}
//
QList<LabelBox> LabeledRulerView::_halfBoxSet(const QList<LabelBox> &boxes,
                                              Qt::Alignment alignment)
{
    QList<LabelBox> bxs;

    if ( !model() )   return bxs;
    if ( boxes.isEmpty() )   return bxs;
    if ( boxes.size() == 1 ) return bxs;

    double c0 = boxes.at(0).center.x();
    double c1 = boxes.at(1).center.x();
    double cN = boxes.at(boxes.size()-1).center.x();
    if ( alignment == Qt::AlignLeft ) {
        c0 = boxes.at(0).center.y();
        c1 = boxes.at(1).center.y();
        cN = boxes.at(boxes.size()-1).center.y();
    }

    int startIdx = 0;
    if ( _isEqual(c0,0.0) ) {
        // First box is 0.0, grab first and every other
        // {0,2,4,6,8} -> {0,4,8}
        startIdx = 0;
    } else if  ( c0 < 0 && cN > 0 ) {
        // If boxes pass from negative to positive
        // {-6,-4,-2,0,2,4,6,8,10} -> {-4,0,4,8}
        // {-1,1} -> {1}
        // {-3,-1,1,3,5} -> {-3,1,5}
        startIdx = 1;
        int i = 0 ;
        foreach ( LabelBox box, boxes ) {
            double c;
            if ( alignment == Qt::AlignBottom ) {
                c = box.center.x();
            } else {
                c = box.center.y();
            }
            if ( c > 0 || _isEqual(c,0.0) ) {
                startIdx = i;
                break;
            }
            ++i;
        }
        i = 0;
        while ( startIdx > 0 ) {
            startIdx -= 2*i;
            ++i;
        }
        startIdx += 2;
    } else if ( (c0 > 0 && cN > 0) || (c0 < 0 && cN < 0) ) {
        double d = c1 - c0;
        double a = round(c0/d);
        if ( fmod(a,2) == 0 ) {
            startIdx = 0;
        } else {
            startIdx = 1;
        }
    } else {
        // bad scoobs
        return bxs;
    }


    int i = startIdx;
    while ( 1 ) {
        LabelBox box = boxes.at(i);
        bxs << box;
        i += 2;
        if ( i >= boxes.size() ) {
            break;
        }
    }

    return bxs;
}
