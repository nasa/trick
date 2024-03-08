#include "bookidxview.h"

BookIdxView::BookIdxView(QWidget *parent) :
    QAbstractItemView(parent),
    _curvesView(0),
    _buttonSelectAndPan(Qt::LeftButton),
    _buttonRubberBandZoom(Qt::MidButton),
    _buttonResetView(Qt::RightButton)
{
}

QTransform BookIdxView::_coordToPixelTransform()
{
    QTransform I;
    if ( !_curvesView ) return I;

    QRectF M = _mathRect();

    // Window coords (topleft)
    double u0 = 0.0;
    double v0 = 0.0;

    // Math coords (topleft)
    double x0 = M.topLeft().x();
    double y0 = M.topLeft().y();

    // TODO: if x0=-DBL_MAX and x1=+DBL_MAX etc
    double du = viewport()->rect().width();
    double dv = viewport()->rect().height();
    double dx = M.width();
    double dy = M.height();
    double a = du/dx;
    double b = dv/dy;
    double c = u0-a*x0;
    double d = v0-b*y0;

    QTransform T( a,    0,
                  0,    b,
                  c,    d);

    return T;
}

void BookIdxView::setCurvesView(QAbstractItemView *view)
{
    _curvesView = view;
}

// This window's math rect based on this windows rect(), and the
// plot's window and math rect
QRectF BookIdxView::_mathRect()
{
    if ( !_curvesView ) {
        fprintf(stderr, "koviz [bad scoobs]: BookIdxView::_mathRect() "
                        "called without _curvesView set.\n");
        exit(-1);
    }

    QRectF M = _bookModel()->getPlotMathRect(rootIndex());
    QRect  W = viewport()->rect();
    QRect  V = _curvesView->viewport()->rect();

    if ( W != V ) {

        V.moveTo(_curvesView->viewport()->mapToGlobal(V.topLeft()));
        W.moveTo(viewport()->mapToGlobal(W.topLeft()));
        double pixelWidth  = ( V.width() > 0 )  ? M.width()/V.width()   : 0 ;
        double pixelHeight = ( V.height() > 0 ) ? M.height()/V.height() : 0 ;
        QPointF vw = V.topLeft()-W.topLeft();
        double ox = pixelWidth*vw.x();
        double oy = pixelHeight*vw.y();
        double mw = pixelWidth*W.width();
        double mh = pixelHeight*W.height();
        QPointF pt(M.x()-ox, M.y()-oy);
        M = QRectF(pt.x(),pt.y(), mw, mh);
    }

    return M;
}

QModelIndex BookIdxView::_plotMathRectIdx(const QModelIndex &plotIdx) const
{
    QModelIndex idx;
    if ( !model() ) return idx;
    idx = _bookModel()->getDataIndex(plotIdx, "PlotMathRect", "Plot");
    return idx;
}

void BookIdxView::setModel(QAbstractItemModel *model)
{
    foreach ( QAbstractItemView* view, _childViews ) {
        view->setModel(model);
    }
    QAbstractItemView::setModel(model);

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

void BookIdxView::setRootIndex(const QModelIndex &index)
{
    foreach (QAbstractItemView* view, _childViews ) {
        view->setRootIndex(index);
    }
    QAbstractItemView::setRootIndex(index);
}

void BookIdxView::setCurrentCurveRunID(int runID)
{
    foreach (QAbstractItemView* view, _childViews ) {
        BookIdxView* bookIdxView = dynamic_cast<BookIdxView*>(view);
        if ( bookIdxView ) {
            bookIdxView->setCurrentCurveRunID(runID);
        }
    }
}

// Root index of a page view will be a Page Index of a Book Model
// Noop "template" for a child class
void BookIdxView::dataChanged(const QModelIndex &topLeft,
                              const QModelIndex &bottomRight)
{
    if ( topLeft != rootIndex() || topLeft.parent() != rootIndex() ) return;
    if ( topLeft.column() != 1 ) return;
    if ( topLeft != bottomRight ) return;

    // Code
}

// Noop "template" for a child class
void BookIdxView::rowsInserted(const QModelIndex &pidx, int start, int end)
{
    Q_UNUSED(start);
    Q_UNUSED(end);
    if ( pidx != rootIndex() ) return;

    // Code
}

void BookIdxView::slotDropEvent(QDropEvent* dropEvent, const QModelIndex &idx)
{
    emit signalDropEvent(dropEvent,idx);
}

QModelIndex BookIdxView::indexAt(const QPoint &point) const
{
    Q_UNUSED(point);
    QModelIndex idx;
    return idx;
}

QRect BookIdxView::visualRect(const QModelIndex &index) const
{
    Q_UNUSED(index);

    QRect rect;
    return rect;
}

void BookIdxView::scrollTo(const QModelIndex &index,
                             QAbstractItemView::ScrollHint hint)
{
    Q_UNUSED(index);
    Q_UNUSED(hint);
}

void BookIdxView::mousePressEvent(QMouseEvent *event)
{
    if (  event->button() == _buttonSelectAndPan ) {
        _mousePressPos = event->pos();
        _mousePressMathRect = _bookModel()->getPlotMathRect(rootIndex());
    } else if (  event->button() == _buttonRubberBandZoom ) {
        event->ignore();
    }
}

void BookIdxView::mouseMoveEvent(QMouseEvent *event)
{
    if ( ! _curvesView ) {
        QAbstractItemView::mouseMoveEvent(event);
        return;
    }

    if ( event->buttons() == _buttonRubberBandZoom ) {
        event->ignore(); // pass event to plot view event filter
    } else if ( event->buttons() == _buttonSelectAndPan ) {

        QRectF M = _bookModel()->getPlotMathRect(rootIndex());

        QPointF dW = event->pos()-_mousePressPos;

        QModelIndex curvesIdx = _bookModel()->getIndex(rootIndex(),
                                                       "Curves","Plot");
        QRectF B = _bookModel()->calcCurvesBBox(curvesIdx);
        if ( B.topLeft().y() < B.bottomLeft().y() ) {
            // Flip if y-axis not directed "up" (this happens with bboxes)
            B = QRectF(B.bottomLeft(),B.topRight());
        }

        M = _mousePressMathRect;

        QEasingCurve ez(QEasingCurve::InOutQuad);

        double tx = dW.x()/(_curvesView->viewport()->width());
        double ty = dW.y()/(_curvesView->viewport()->height());
        double kx = ez.valueForProgress(qAbs(tx));
        double ky = ez.valueForProgress(qAbs(ty));

        if ( _alignment == Qt::AlignTop || _alignment == Qt::AlignBottom ) {

            Qt::Alignment sideStartMouseDrag;
            if ( _mousePressPos.x() > viewport()->width()/2 ) {
                sideStartMouseDrag = Qt::AlignRight;
            } else {
                sideStartMouseDrag = Qt::AlignLeft;
            }

            if ( sideStartMouseDrag == Qt::AlignRight ) {
                if ( tx > 0 ) {
                    M.setRight(M.right()-kx*(B.right()-M.right()));
                } else if ( tx < 0 ) {
                    M.setRight(M.right()+kx*(B.right()-M.right()));
                }
            } else if ( sideStartMouseDrag == Qt::AlignLeft ) {
                if ( tx > 0 ) {
                    M.setLeft(M.left()-kx*(M.left()-B.left()));
                } else if ( tx < 0 ) {
                    M.setLeft(M.left()+kx*(M.left()-B.left()));
                }
            }

            if ( _alignment == Qt::AlignTop ) {
                if ( ty > 0 ) {
                    M.setTop(M.top()+ky*(B.top()-M.top()));
                } else if ( ty < 0 ) {
                    M.setTop(M.top()-ky*(B.top()-M.top()));
                }
            } else if ( _alignment == Qt::AlignBottom ) {
                if ( ty > 0 ) {
                    M.setBottom(M.bottom()-ky*(B.bottom()-M.bottom()));
                } else if ( ty < 0 ) {
                    M.setBottom(M.bottom()+ky*(B.bottom()-M.bottom()));
                }
            }
        } else if (_alignment == Qt::AlignLeft || _alignment == Qt::AlignRight){

            Qt::Alignment sideStartMouseDrag;
            if ( _mousePressPos.y() < viewport()->height()/2 ) {
                sideStartMouseDrag = Qt::AlignTop;
            } else {
                sideStartMouseDrag = Qt::AlignBottom;
            }

            if ( sideStartMouseDrag == Qt::AlignTop ) {
                if ( ty > 0 ) {
                    M.setTop(M.top()+ky*(B.top()-M.top()));
                } else if ( ty < 0 ) {
                    M.setTop(M.top()-ky*(B.top()-M.top()));
                }
            } else if ( sideStartMouseDrag == Qt::AlignBottom ) {
                if ( ty > 0 ) {
                    M.setBottom(M.bottom()-ky*(B.bottom()-M.bottom()));
                } else if ( ty < 0 ) {
                    M.setBottom(M.bottom()+ky*(B.bottom()-M.bottom()));
                }
            }

            if ( _alignment == Qt::AlignLeft ) {
                if ( tx > 0 ) {
                    M.setLeft(M.left()-kx*(M.left()-B.left()));
                } else if ( tx < 0 ) {
                    M.setLeft(M.left()+kx*(M.left()-B.left()));
                }
            } else if ( _alignment == Qt::AlignRight ) {
                if ( tx > 0 ) {
                    M.setRight(M.right()-kx*(B.right()-M.right()));
                } else if ( tx < 0 ) {
                    M.setRight(M.right()+kx*(B.right()-M.right()));
                }
            }
        } else {
            return;
        }

        _bookModel()->setPlotMathRect(M,rootIndex());
        viewport()->update();

    } else {
        QAbstractItemView::mouseMoveEvent(event);
    }
}

void BookIdxView::mouseReleaseEvent(QMouseEvent *event)
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

// Need this so that pageView can capture double click event
void BookIdxView::mouseDoubleClickEvent(QMouseEvent *event)
{
    if ( event->button() == Qt::LeftButton ){
        event->ignore();
    } else {
        QAbstractItemView::mouseDoubleClickEvent(event);
    }
}

QModelIndex BookIdxView::moveCursor(
        QAbstractItemView::CursorAction cursorAction,
        Qt::KeyboardModifiers modifiers)
{
    Q_UNUSED(cursorAction);
    Q_UNUSED(modifiers);

    QModelIndex idx;
    return idx;
}

int BookIdxView::horizontalOffset() const
{
    return 0;
}

int BookIdxView::verticalOffset() const
{
    return 0;
}

bool BookIdxView::isIndexHidden(const QModelIndex &index) const
{
    Q_UNUSED(index);
    return false;
}

void BookIdxView::setSelection(const QRect &rect,
                                 QItemSelectionModel::SelectionFlags command)
{
    Q_UNUSED(rect);
    Q_UNUSED(command);
}

QRegion BookIdxView::visualRegionForSelection(
        const QItemSelection &selection) const
{
    Q_UNUSED(selection);
    QRegion region;
    return region;
}

PlotBookModel* BookIdxView::_bookModel() const
{
    PlotBookModel* bookModel = dynamic_cast<PlotBookModel*>(model());

    if ( !bookModel ) {
        fprintf(stderr,"koviz [bad scoobs]: BookIdxView::_bookModel() "
                       "could not cast model() to a PlotBookModel*.\n");
        exit(-1);
    }

    return bookModel;
}

// Note: This can be slow.  It checks every curve.  If all curves have
//       same x unit, it returns x unit for all curves.
QString BookIdxView::_curvesXUnit(const QModelIndex& plotIdx) const
{
    QString curvesXUnit;
    QString dashDash("--");

    bool isCurvesIdx = _bookModel()->isChildIndex(plotIdx, "Plot", "Curves");
    if ( !isCurvesIdx ) return dashDash;
    QModelIndex curvesIdx = _bookModel()->getIndex(plotIdx, "Curves", "Plot");

    bool isCurveIdx = _bookModel()->isChildIndex(curvesIdx, "Curves", "Curve");
    if ( !isCurveIdx ) return dashDash;
    QModelIndexList curveIdxs = _bookModel()->getIndexList(curvesIdx,
                                                           "Curve","Curves");

    foreach ( QModelIndex curveIdx, curveIdxs ) {

        bool isCurveXUnit = _bookModel()->isChildIndex(curveIdx,
                                                       "Curve", "CurveXUnit");
        if ( !isCurveXUnit ) {
            // Since curve has no xunit, bail
            curvesXUnit.clear();
            break;
        }

        QModelIndex curveXUnitIdx = _bookModel()->getDataIndex(curveIdx,
                                                               "CurveXUnit",
                                                               "Curve");
        QString unit = model()->data(curveXUnitIdx).toString();

        if ( curvesXUnit.isEmpty() ) {
            curvesXUnit = unit;
        } else {
            if ( curvesXUnit != unit ) {
                curvesXUnit = dashDash;
                break;
            }
        }
    }

    return curvesXUnit;
}

QString BookIdxView::_curvesUnit(const QModelIndex &plotIdx, QChar axis) const
{
    if ( axis != 'x' && axis != 'y' ) {
        fprintf(stderr,"koviz [bad scoobs]: BookIdxView::_curvesUnit "
                       "called with bad axis=%c\n", axis.toLatin1());
        exit(-1);
    }

    QString curvesUnit;
    QString dashDash("--");

    bool isCurvesIdx = _bookModel()->isChildIndex(plotIdx, "Plot", "Curves");
    if ( !isCurvesIdx ) return dashDash;
    QModelIndex curvesIdx = _bookModel()->getIndex(plotIdx, "Curves", "Plot");

    bool isCurveIdx = _bookModel()->isChildIndex(curvesIdx, "Curves", "Curve");
    if ( !isCurveIdx ) return dashDash;
    QModelIndexList curveIdxs = _bookModel()->getIndexList(curvesIdx,
                                                           "Curve","Curves");

    foreach ( QModelIndex curveIdx, curveIdxs ) {

        bool isCurveUnit;
        if ( axis == 'x' ) {
            isCurveUnit = _bookModel()->isChildIndex(curveIdx,
                                                     "Curve", "CurveXUnit");
        } else {
            isCurveUnit = _bookModel()->isChildIndex(curveIdx,
                                                     "Curve", "CurveYUnit");
        }
        if ( !isCurveUnit ) {
            // Since curve has no xunit, bail
            curvesUnit.clear();
            break;
        }

        QModelIndex curveUnitIdx;
        if ( axis == 'x' ) {
            curveUnitIdx = _bookModel()->getDataIndex(curveIdx,
                                                      "CurveXUnit", "Curve");
        } else {
            curveUnitIdx = _bookModel()->getDataIndex(curveIdx,
                                                      "CurveYUnit", "Curve");
        }

        QString unit = model()->data(curveUnitIdx).toString();

        if ( curvesUnit.isEmpty() ) {
            curvesUnit = unit;
        } else {
            if ( curvesUnit != unit ) {
                curvesUnit = dashDash;
                break;
            }
        }
    }

    return curvesUnit;
}

void BookIdxView::_paintCurvesLegend(const QRect& R,
                                     const QModelIndex &curvesIdx,
                                     QPainter &painter)
{
    // If all plots on the page have the same legend, PageTitle will show legend
    // (unless explicitly set via -showPlotLegend option)
    QString isShowPlotLegend = _bookModel()->getDataString(QModelIndex(),
                                                         "IsShowPlotLegend","");
    if ( isShowPlotLegend == "no" ) {
        return;
    }
    bool isPlotLegendsSame = _bookModel()->isPlotLegendsSame(
                                         curvesIdx.parent().parent().parent());
    if ( isPlotLegendsSame && isShowPlotLegend != "yes" ) {
        return;
    }

    // If all labels on plot are identical, do not show plot legend
    // (unless explicitly set via -showPlotLegend option)
    QModelIndex plotIdx = curvesIdx.parent();
    QStringList labels = _bookModel()->legendLabels(plotIdx);
    if ( labels.size() > 1 ) {
        bool isLabelsSame = true;
        foreach ( QString label, labels ) {
            if ( label != labels.at(0) ) {
                isLabelsSame = false;
                break;
            }
        }
        if ( isLabelsSame && isShowPlotLegend != "yes" ) {
            return;
        }
    }

    QString pres = _bookModel()->getDataString(curvesIdx.parent(),
                                               "PlotPresentation","Plot");
    if ( pres == "error" ) {
        return;
    }

    QList<QPen*> pens = _bookModel()->legendPens(plotIdx,
                                                 painter.paintEngine()->type());
    QStringList symbols = _bookModel()->legendSymbols(plotIdx);

    if ( pres == "error+compare" ) {
        QPen* magentaPen = new QPen(_bookModel()->errorLineColor());
        pens << magentaPen;
        symbols << "none";
        labels << "error";
    }

    __paintCurvesLegend(R,curvesIdx,pens,symbols,labels,painter);

    // Clean up
    foreach ( QPen* pen, pens ) {
        delete pen;
    }
}

// pens,symbols and labels are ordered/collated foreach legend curve/label
void BookIdxView::__paintCurvesLegend(const QRect& R,
                                      const QModelIndex &curvesIdx,
                                      const QList<QPen *> &pens,
                                      const QStringList &symbols,
                                      const QStringList &labels,
                                      QPainter &painter)
{
    QPen origPen = painter.pen();

    int n = pens.size();

    // Width Of Legend Box
    const int fw = painter.fontMetrics().averageCharWidth();
    const int ml = fw; // marginLeft
    const int mr = fw; // marginRight
    const int s = fw;  // spaceBetweenLineAndLabel
    const int l = 4*fw;  // line width , e.g. ~5 for: *-----* Gravity
    int w = 0;
    foreach (QString label, labels ) {
        int labelWidth = painter.fontMetrics().boundingRect(label).width();
        int ww = ml + l + s + labelWidth + mr;
        if ( ww > w ) {
            w = ww;
        }
    }

    // Height Of Legend Box
    const int ls = painter.fontMetrics().lineSpacing();
    const int fh = painter.fontMetrics().height();
    const int v = ls/8;  // vertical space between legend entries (0 works)
    const int mt = fh/4; // marginTop
    const int mb = fh/4; // marginBot
    int sh = 0;
    foreach (QString label, labels ) {
        sh += painter.fontMetrics().boundingRect(label).height();
    }
    int h = (n-1)*v + mt + mb + sh;

    // Legend box top left point
    QString pos = _bookModel()->getDataString(QModelIndex(),
                                              "PlotLegendPosition","");
    const int tb = fh/4; // top/bottom margin
    const int rl = fw/2; // right/left margin
    QPoint legendTopLeft;
    if ( pos == "ne" ) { // The default
        legendTopLeft = QPoint(R.right()-w-rl,R.top()+tb);
    } else if ( pos == "e" ) {
        legendTopLeft = QPoint(R.right()-w-rl,R.height()/2-h/2);
    } else if ( pos == "se" ) {
        legendTopLeft = QPoint(R.right()-w-rl,R.height()-h-tb);
    } else if ( pos == "s" ) {
        legendTopLeft = QPoint(R.width()/2-w/2,R.height()-h-tb);
    } else if ( pos == "sw" ) {
        legendTopLeft = QPoint(R.left()+rl,R.height()-h-tb);
    } else if ( pos == "w" ) {
        legendTopLeft = QPoint(R.left()+rl,R.height()/2-h/2);
    } else if ( pos == "nw" ) {
        legendTopLeft = QPoint(R.left()+rl,R.top()+tb);
    } else if ( pos == "n" ) {
        legendTopLeft = QPoint(R.width()/2-w/2,R.top()+tb);
    } else {
        fprintf(stderr, "koviz [bad scoobs]: __paintCurvesLegend() has "
                        "bad legend position \"%s\"\n",
                pos.toLatin1().constData());
        exit(-1);
    }

    // Legend box
    QRect LegendBox(legendTopLeft,QSize(w,h));

    // Background color
    QModelIndex pageIdx = curvesIdx.parent().parent().parent();
    QColor bg = _bookModel()->pageBackgroundColor(pageIdx);
    bg.setAlpha(190);

    // Draw legend box with semi-transparent background
    painter.setBrush(bg);
    QPen penGray(QColor(120,120,120,255));
    painter.setPen(penGray);
    painter.drawRect(LegendBox);
    painter.setPen(origPen);

    QRect lastBB;
    for ( int i = 0; i < n; ++i ) {

        QString label = labels.at(i);

        // Calc bounding rect (bb) for line and label
        QPoint topLeft;
        if ( i == 0 ) {
            topLeft.setX(legendTopLeft.x()+ml);
            topLeft.setY(legendTopLeft.y()+mt);
        } else {
            topLeft.setX(lastBB.bottomLeft().x());
            topLeft.setY(lastBB.bottomLeft().y()+v);
        }
        QRect bb = painter.fontMetrics().boundingRect(label);
        bb.moveTopLeft(topLeft);
        bb.setWidth(l+s+bb.width());

        // Draw line segment
        QPen* pen = pens.at(i);
        painter.setPen(*pen);
        QPoint p1(bb.left(),bb.center().y());
        QPoint p2(bb.left()+l,bb.center().y());
        painter.drawLine(p1,p2);

        // Draw symbols on line segment endpoints
        QString symbol = symbols.at(i);
        __paintSymbol(p1,symbol,painter);
        __paintSymbol(p2,symbol,painter);

        // Draw label
        QRect labelRect(bb);
        QPoint p(bb.topLeft().x()+l+s, bb.topLeft().y());
        labelRect.moveTopLeft(p);
        painter.drawText(labelRect, Qt::AlignLeft|Qt::AlignVCenter, label);

        lastBB = bb;
    }

    painter.setPen(origPen);
}

void BookIdxView::__paintSymbol(const QPointF& p,
                               const QString &symbol, QPainter &painter)
{

    QPen origPen = painter.pen();
    QPen pen = painter.pen();
    pen.setStyle(Qt::SolidLine);
    painter.setPen(pen);

    if ( symbol == "circle" ) {
        painter.drawEllipse(p,2,2);
    } else if ( symbol == "thick_circle" ) {
        pen.setWidth(2.0);
        painter.setPen(pen);
        painter.drawEllipse(p,3,3);
    } else if ( symbol == "solid_circle" ) {
        pen.setWidthF(2.0);
        painter.setPen(pen);
        painter.drawEllipse(p,1,1);
    } else if ( symbol == "square" ) {
        double x = p.x()-2.0;
        double y = p.y()-2.0;
        painter.drawRect(QRectF(x,y,4,4));
    } else if ( symbol == "thick_square") {
        pen.setWidthF(2.0);
        painter.setPen(pen);
        double x = p.x()-3.0;
        double y = p.y()-3.0;
        painter.drawRect(QRectF(x,y,6,6));
    } else if ( symbol == "solid_square" ) {
        pen.setWidthF(4.0);
        painter.setPen(pen);
        painter.drawPoint(p); // happens to be a solid square
    } else if ( symbol == "star" ) { // *
        double r = 3.0;
        QPointF a(p.x()+r*cos(18.0*M_PI/180.0),
                  p.y()-r*sin(18.0*M_PI/180.0));
        QPointF b(p.x(),p.y()-r);
        QPointF c(p.x()-r*cos(18.0*M_PI/180.0),
                  p.y()-r*sin(18.0*M_PI/180.0));
        QPointF d(p.x()-r*cos(54.0*M_PI/180.0),
                  p.y()+r*sin(54.0*M_PI/180.0));
        QPointF e(p.x()+r*cos(54.0*M_PI/180.0),
                  p.y()+r*sin(54.0*M_PI/180.0));
        painter.drawLine(p,a);
        painter.drawLine(p,b);
        painter.drawLine(p,c);
        painter.drawLine(p,d);
        painter.drawLine(p,e);
    } else if ( symbol == "xx" ) {
        pen.setWidthF(2.0);
        painter.setPen(pen);
        QPointF a(p.x()+2.0,p.y()+2.0);
        QPointF b(p.x()-2.0,p.y()+2.0);
        QPointF c(p.x()-2.0,p.y()-2.0);
        QPointF d(p.x()+2.0,p.y()-2.0);
        painter.drawLine(p,a);
        painter.drawLine(p,b);
        painter.drawLine(p,c);
        painter.drawLine(p,d);
    } else if ( symbol == "triangle" ) {
        double r = 3.0;
        QPointF a(p.x(),p.y()-r);
        QPointF b(p.x()-r*cos(30.0*M_PI/180.0),
                  p.y()+r*sin(30.0*M_PI/180.0));
        QPointF c(p.x()+r*cos(30.0*M_PI/180.0),
                  p.y()+r*sin(30.0*M_PI/180.0));
        painter.drawLine(a,b);
        painter.drawLine(b,c);
        painter.drawLine(c,a);
    } else if ( symbol == "thick_triangle" ) {
        pen.setWidthF(2.0);
        painter.setPen(pen);
        double r = 4.0;
        QPointF a(p.x(),p.y()-r);
        QPointF b(p.x()-r*cos(30.0*M_PI/180.0),
                  p.y()+r*sin(30.0*M_PI/180.0));
        QPointF c(p.x()+r*cos(30.0*M_PI/180.0),
                  p.y()+r*sin(30.0*M_PI/180.0));
        painter.drawLine(a,b);
        painter.drawLine(b,c);
        painter.drawLine(c,a);
    } else if ( symbol == "solid_triangle" ) {
        pen.setWidthF(2.0);
        painter.setPen(pen);
        double r = 3.0;
        QPointF a(p.x(),p.y()-r);
        QPointF b(p.x()-r*cos(30.0*M_PI/180.0),
                  p.y()+r*sin(30.0*M_PI/180.0));
        QPointF c(p.x()+r*cos(30.0*M_PI/180.0),
                  p.y()+r*sin(30.0*M_PI/180.0));
        painter.drawLine(a,b);
        painter.drawLine(b,c);
        painter.drawLine(c,a);
    } else if ( symbol == "satellite" ) {
        painter.save();
        pen.setWidthF(1.0);
        painter.setPen(pen);
        painter.translate(p);
        painter.rotate(37.0);
        QBrush brush(painter.pen().color());
        painter.setBrush(brush);

        // Body
        double bodyWidth = 6.0;
        double bodyHeight = 9.0;
        QRectF bodyRect(-bodyWidth/2.0,-bodyHeight/2.0,bodyWidth,bodyHeight);
        painter.drawRect(bodyRect);

        // Solar Panels
        double panelBarLength = 3.0;
        double panelWidth = 11.0;
        double panelHeight = 5.0;
        QRectF panelRect1(-panelBarLength-bodyWidth/2.0-panelWidth,
                          -panelHeight/2.0,
                          panelWidth,panelHeight);
        QRectF panelRect2(panelBarLength+bodyWidth/2.0,
                          -panelHeight/2.0,
                          panelWidth,panelHeight);
        painter.drawRect(panelRect1);
        painter.drawRect(panelRect2);

        // Panel supports
        painter.drawLine(0,0, bodyWidth/2+panelBarLength,0);
        painter.drawLine(0,0,-bodyWidth/2-panelBarLength,0);

        // Dish support bar
        double dishBarLength = 3.0;
        painter.drawLine(0,bodyHeight/2+bodyWidth/2,
                         0,bodyHeight/2+bodyWidth/2+dishBarLength);

        // Dish
        QRectF dishRect(-bodyWidth/2.0, bodyHeight/2.0+dishBarLength,
                        bodyWidth, bodyWidth);
        painter.drawPie(dishRect,0,180*16);
        double focusHeight = 4.0;
        painter.drawLine(0,bodyHeight/2+bodyWidth/2+dishBarLength,
                         0,bodyHeight/2+bodyWidth/2+dishBarLength+focusHeight);

        painter.restore();
    } else if ( symbol.startsWith("number_",Qt::CaseInsensitive) &&
                symbol.size() == 8 ) {

        QFont origFont = painter.font();
        QBrush origBrush = painter.brush();

        // Calculate bbox to draw text in
        QString number = symbol.right(1); // last char is '0'-'9'
        QFont font = painter.font();
        font.setPointSize(7);
        painter.setFont(font);
        QFontMetrics fm = painter.fontMetrics();
        QRectF bbox(fm.tightBoundingRect(number));
        bbox.moveCenter(p);

        // Draw solid circle around number
        QRectF box(bbox);
        double l = 3.0*qMax(box.width(),box.height())/2.0;
        box.setWidth(l);
        box.setHeight(l);
        box.moveCenter(p);
        QBrush brush(pen.color());
        painter.setBrush(brush);
        painter.drawEllipse(box);

        // Draw number in white in middle of circle
        QPen whitePen("white");
        painter.setPen(whitePen);
        painter.drawText(bbox,Qt::AlignCenter,number);

        painter.setFont(origFont);
        painter.setBrush(origBrush);
    }

    painter.setPen(origPen);
}

void BookIdxView::_paintGrid(QPainter &painter, const QModelIndex& plotIdx)
{
    // If Grid DNE or off or math rect is zero, do not paint grid
    bool isGrid = _bookModel()->isChildIndex(plotIdx,"Plot","PlotGrid");
    if ( !isGrid ) {
        return;
    }
    QModelIndex isGridIdx = _bookModel()->getDataIndex(plotIdx,
                                                       "PlotGrid","Plot");
    isGrid = _bookModel()->data(isGridIdx).toBool();
    if ( !isGrid ) {
        return;
    }
    const QRectF M = _mathRect();
    if ( M.width() == 0.0 || M.height() == 0.0 ) {
        return;
    }

    QString plotXScale = _bookModel()->getDataString(plotIdx,
                                                     "PlotXScale","Plot");
    QString plotYScale = _bookModel()->getDataString(plotIdx,
                                                     "PlotYScale","Plot");
    bool isXLogScale = ( plotXScale == "log" ) ? true : false;
    bool isYLogScale = ( plotYScale == "log" ) ? true : false;

    QList<double> xtics = _bookModel()->majorXTics(plotIdx);
    if ( isXLogScale ) {
        xtics.append(_bookModel()->minorXTics(plotIdx));
    }
    QList<double> ytics = _bookModel()->majorYTics(plotIdx);
    if ( isYLogScale ) {
        ytics.append(_bookModel()->minorYTics(plotIdx));
    }

    QVector<QPointF> vLines;
    QVector<QPointF> hLines;

    foreach ( double x, xtics ) {
        vLines << QPointF(x,M.top()) << QPointF(x,M.bottom());
    }
    foreach ( double y, ytics ) {
        hLines << QPointF(M.left(),y) << QPointF(M.right(),y);
    }

    bool isAntiAliasing = (QPainter::Antialiasing & painter.renderHints()) ;

    // Grid Color
    QModelIndex pageIdx = _bookModel()->getIndex(plotIdx,"Page","Plot");
    QColor color = _bookModel()->pageForegroundColor(pageIdx);
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
    QPen origPen = painter.pen();
    QPen pen = painter.pen();
    pen.setColor(color);
    painter.save();
    painter.setRenderHint(QPainter::Antialiasing,false);
    pen.setWidthF(0.0);
    pen.setDashPattern(dashes);
    painter.setPen(pen);
    painter.setTransform(_coordToPixelTransform());
    painter.drawLines(hLines);
    painter.drawLines(vLines);
    painter.setPen(origPen);
    if ( isAntiAliasing ) {
        painter.setRenderHint(QPainter::Antialiasing);
    }
    painter.restore();
}
