#include "bookidxview.h"

BookIdxView::BookIdxView(QWidget *parent) :
    QAbstractItemView(parent),
    _curvesView(0)
{
}

QTransform BookIdxView::_coordToPixelTransform()
{
    QTransform I;
    if ( !_curvesView ) return I;

    QRectF M = _mathRect();

    // Window coords (topleft,bottomright)
    double u0 = 0.0;
    double v0 = 0.0;
    double u1 = viewport()->rect().bottomRight().x();
    double v1 = viewport()->rect().bottomRight().y();

    // Math coords
    double x0 = M.topLeft().x();
    double y0 = M.topLeft().y();
    double x1 = M.bottomRight().x();
    double y1 = M.bottomRight().y();

    // Transform (TODO: check that x0-x1 > 0 etc)
    // TODO: if x0=-DBL_MAX and x1=+DBL_MAX etc
    double du = u0-u1;
    double dv = v0-v1;
    double dx = x0-x1;
    double dy = y0-y1;
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
        fprintf(stderr, "snap [bad scoobs]: BookIdxView::_mathRect() "
                        "called without _curvesView set.\n");
        exit(-1);
    }

    QRectF M = _plotMathRect(rootIndex());
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

// The viewport math rect is in the BookModel under Plot.PlotMathRect
QRectF BookIdxView::_plotMathRect(const QModelIndex& plotIdx) const
{
    QRectF M;
    if ( !model() ) return M;

    QModelIndex plotMathRectIdx = _plotMathRectIdx(plotIdx);
    plotMathRectIdx = plotMathRectIdx.sibling(plotMathRectIdx.row(),1);
    M = model()->data(plotMathRectIdx).toRectF();

    // Flip if y-axis not directed "up" (this happens with bboxes)
    if ( M.topLeft().y() < M.bottomLeft().y() ) {
        M = QRectF(M.bottomLeft(),M.topRight());
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

// The viewport math rect is in the BookModel under Plot.PlotViewport
void BookIdxView::_setPlotMathRect(const QRectF& mathRect)
{
    if ( !model() ) return;

    QModelIndex plotIdx = _bookModel()->getIndex(rootIndex(),"Plot");
    QModelIndex plotMathRectIdx = _bookModel()->getDataIndex(plotIdx,
                                                             "PlotMathRect",
                                                             "Plot");

    // Flip if y-axis not directed "up" (this happens with bboxes)
    QRectF M = mathRect;
    if ( M.topLeft().y() < M.bottomLeft().y() ) {
        M = QRectF(M.bottomLeft(),M.topRight());
    }

    model()->setData(plotMathRectIdx, M);
}

QList<double> BookIdxView::_majorXTics(const QModelIndex& plotIdx) const
{
    QList<double> X;
    QRectF r = _plotMathRect(plotIdx);
    double a = r.left();
    double b = r.right();
    X = _calcTicSet(a,b,1.0,10.0);
    return X;
}

QList<double> BookIdxView::_minorXTics() const
{
    QList<double> X;
    QRectF r = _plotMathRect(rootIndex());
    double a = r.left();
    double b = r.right();
    X = _calcMinorTicSet(a,b);
    return X;
}

QList<double> BookIdxView::_majorYTics(const QModelIndex &plotIdx) const
{
    QList<double> Y;
    QRectF r = _plotMathRect(plotIdx);
    double a = r.bottom();
    double b = r.top();
    Y = _calcTicSet(a,b,1.0,10.0);
    return Y;
}

QList<double> BookIdxView::_minorYTics() const
{
    QList<double> Y;
    QRectF r = _plotMathRect(rootIndex());
    double a = r.bottom();
    double b = r.top();
    Y = _calcMinorTicSet(a,b);
    return Y;
}

QList<double> BookIdxView::_calcMinorTicSet(double a, double b) const
{
    QList<double> Minors;

    QList<double> Majors = _calcTicSet(a,b,1.0,10.0);

    if ( Majors.size() >= 2 ) {
        double d = (Majors.at(1)-Majors.at(0))/4.0;
        double x = Majors.at(0);
        if ( x-d-d-d >= a ) Minors << x-d-d-d;
        if ( x-d-d >= a )   Minors << x-d-d;
        if ( x-d >= a )     Minors << x-d;
        foreach (double x, Majors) {
            if ( x+d <= b )     Minors << x+d;
            if ( x+d+d <= b )   Minors << x+d+d;
            if ( x+d+d+d <= b ) Minors << x+d+d+d;
        }
    }

    return Minors;
}

//
// Return set X which is smallest tic set between [a,b] for n,u.
//
// Given [a,b],n,u:
// Return X = {x: x=i*n^k*u in [a,b]
//                i is some ordered list of ints
//                and k minimizes size of X}
//
//
// TODO: test n==2, n==e.  At this point, n==10 is the only tested value
// TODO: Need to bail if n not 2,10 or e
// TODO: Haven't tested u > 1 or u < 0
QList<double> BookIdxView::_calcTicSet(double aIn, double bIn,
                                       double u, double n) const
{
    QList<double> X;

    // Make sure inputs are finite
    if ( !isfinite(aIn) || !isfinite(bIn) || !isfinite(n) || !isfinite(u) ) {
        return X;
    }

    // If [a,b] is a singleton, return {aIn}
    if ( aIn == bIn ) {
        X << aIn;
        return X;
    }

    // Ensure a <= b
    double a = aIn;
    double b = bIn;
    if ( bIn < aIn ) {
        a = bIn;
        b = aIn;
    }

    // k in i*n^k*u is power of n for making set X
    double k;
    if ( n == 10.0 ) {
        k = qMax(  floor(log10(qAbs(a))), floor(log10(qAbs(b))) );
    } else if ( n == 2.0 ) {
        k = qMax(  floor(log2(qAbs(a))), floor(log2(qAbs(b))) );
    } else if ( qAbs(n-exp(1)) < 1.0e-9 ) {
        k = qMax(  floor(log(qAbs(a))), floor(log(qAbs(b))) );
    } else {
        // bad scoobs
        return X;
    }

    double d = pow(n,k);

    //
    // Given d = n^k, find x0 such that x0*d <= a
    //
    double x0 = 0.0;
    double x = x0;
    if ( _isEqual(a,0) ) {
        // if (a == 0) then x0 = 0, because 0*d == 0 <= a == 0
    } else if ( x < a ) {
        while ( 1 ) {
            if ( _isEqual(x,a) ) {
                break;
            } else if ( x < a ) {
                // keep going
                x0 = x;
                x += d;
                if ( fpclassify(x) == FP_INFINITE ) {
                    break;
                }
            } else if ( x > a ) {
                break;
            } else {
                X.clear(); return X; // bad scoobs
            }
        }
    } else if ( x > a ) {
        while ( 1 ) {
            if ( _isEqual(x,a) ) {
                break;
            } else if ( x < a ) {
                break;
            } else if ( x > a ) {
                // keep going
                x -= d;
                if ( fpclassify(x) == FP_INFINITE ) {
                    break;
                }
                x0 = x;
            } else {
                X.clear(); return X; // bad scoobs
            }
        }
    } else {
        X.clear(); return X; // bad scoobs
    }

    // From last block, x0 <= a
    //
    // Find set X = {x: x in [a,b] and x = q*n^k for q in Z (ints)}
    // |X| > 0, and smallest of all such sets
    //
    // This algorithm (possibly returning from the *rabbithole):
    //
    //       bunny starts at x0 (x0 stays <= a)
    //       Hops the distance d (or d*u) until it either
    //
    //       Lands in [a,b]
    //       OR
    //       Hops OVER [a,b]
    //
    //       If hop inside  [a,b]:
    //              Add hop spot to a list
    //              Keep hopping and appending hop spots to the list
    //              until I hop past b or hit infinity
    //
    //       If hops is OVER [a,b]
    //              Hop back to x0!
    //              Refine hop distance scale from n^k to (n^(k-1))
    //              Try again (go back to x0 (rabbithole))
    //
    while ( X.size() < 2 ) {  // |X| >= 2 (unless a problem occurs)

        X.clear();
        double x = x0;  // Start search x <= a (keeps X ordered)
        d = pow(n,k);

        while ( 1 ) {
            if ( _isEqual(x,a) ) {
                X << x;
                x += d*u;
                if ( fpclassify(x) == FP_INFINITE ) break; else continue;
            } else if ( x < a ) { // x not in [a,b]... continue search
                x0 = x;
                double xu = x+d*u;
                if ( _isEqual(xu,a) ) {
                    x += d*u;
                } else if ( xu > a && xu < b ) {
                    double xd = x+d;
                    if ( _isEqual(xd,a) ) {
                        x += d;
                    } else if ( xd > a && xd < b ) {
                        if ( xd < xu ) {
                            x += d;
                        } else {
                            x += d*u;
                        }
                    } else {
                        x += d;
                    }
                } else {
                    x += d;
                }
                if ( fpclassify(x) == FP_INFINITE ) break; else continue;
            } else if ( _isEqual(x,b) ) { // x in [a,b]... and no more
                X << x;
                break;
            } else if ( x < b ) { // x in [a,b]... try to find another
                X << x;
                x += d*u;
                if ( fpclassify(x) == FP_INFINITE ) break; else continue;
                continue;
            } else if ( x > b ) { // x > [a,b]
                break;
            } else {
                // this should not happen
                X.clear(); return X; // bad scoobs
            }
        }

        k -= 1.0;   // smaller d = n^(k), n^(k-1), n^(k-2)...
    }

    // Clean up the set by removing any points not strictly within [a,b]
    for ( int i = X.size()-1; i >= 0; --i ) {
        double x = X.at(i);
        if ( x < a || x > b ) {
            X.removeAt(i);
        }
    }

    // Shrink tic set until |X| <=7
    // Tics are removed in favor of keeping tics "even" in order
    // to keep them the same while panning
    while ( X.size() > 7 ) {
        double x = X.at(0);
        QString s = QString::asprintf("%g",x);
        if ( s.contains('e') ) {
            int n = s.indexOf('e');
            s = s.left(n);
        }
        bool isEven = false;
        if ( s.endsWith('0') || s.endsWith('2') || s.endsWith('4') ||
             s.endsWith('6') || s.endsWith('8') ) {
            isEven = true;
        }

        for ( int i = X.size()-1; i >= 0; --i ) {
            if ( isEven ) {
                if ( i%2 != 0 ) {
                    X.removeAt(i);
                }
            } else {
                if ( i%2 == 0 ) {
                    X.removeAt(i);
                }
            }
        }
    }

    if ( X.isEmpty() ) {
        X << a;
    }

    return X;
}

// I read "if two floating-point numbers in the same format are ordered
// , then they are ordered the same way when their bits are reinterpreted
// as Sign-Magnitude integers."
// This means, I think,
// for double x, y;
//
// if x > 0, y > 0 and (x < y)
// then *(ulong)&x < *(ulong)&y
//
// if we let i=*(ulong)&x and j=*(ulong)&y
// let d = qAbs(j-i)
// d is, I think,the number of "neighbors" in (i,j] for i
// A neighbor doesn't include itself, so if i==j, the number is 0
//
// Equality is then defined as:
//      if ( numNeighborsIn (a,b] < maxD ) return true; else return false;
bool BookIdxView::_isEqual(double a, double b, ulong maxD,
                           bool isNeighborMethod) const
{
    bool ok = false;

    if ( a == b ) return true;

    if ( isNeighborMethod && sizeof(double) == sizeof(ulong)) {
        ulong n = maxD;
        if ( a == 0 ) {
            if ( b > 0 ) {
                n = (*(ulong*)&b);
            } else if ( b < 0 ) {
                double bb = qAbs(b);
                n = *(ulong*)&bb;
            }
        } else if ( a > 0 ) {
            if ( b == 0 ) {
                n = (*(ulong*)&a);
            } else if ( b > 0 ) {
                if ( b > a ) {
                    n = (*(ulong*)&b - *(ulong*)&a);
                } else {
                    n = (*(ulong*)&a - *(ulong*)&b);
                }
            } else if ( b < 0 ) {
                double bb = qAbs(b);
                n = (*(ulong*)&a + *(ulong*)&bb);
            }
        } else if ( a < 0 ) {
            if ( b == 0 ) {
                n = (*(ulong*)&a);
            } else if ( b > 0 ) {
                double aa = qAbs(a);
                n = (*(ulong*)&aa + *(ulong*)&b);
            } else if ( b < 0 ) {
                double aa = qAbs(a);
                double bb = qAbs(b);
                if ( bb > aa ) {
                    n = (*(ulong*)&bb - *(ulong*)&aa);
                } else {
                    n = (*(ulong*)&aa - *(ulong*)&bb);
                }
            }
        } else {
            // bad scoobies
            fprintf(stderr, "bad scoobs in _isEqual(a,b)\n");
            return false;
        }

        if ( n < maxD ) {
            ok = true;
        }
    } else {

        // Assuming a != b from above

        double k;
        if ( a != 0.0 && b != 0.0 ) {
            k = qMax(log2(qAbs(a)),log2(qAbs(b)));
        } else if ( a == 0 ) {
            k = log2(qAbs(b));
        } else if ( b == 0 ) {
            k = log2(qAbs(a));
        } else {
            // bad scoobs
            fprintf(stderr, "bad scoobies in _isEqual(a,b)\n");
            return false;
        }
        k = floor(k);

        double e;
        if ( k >= -1074 + 32 ) {
            e = pow(2.0,k-2*32);
        } else {
            e = pow(2.0,-1074);
        }

        ok = (qAbs(a-b) < e) ? true : false  ;
    }

    return ok;
}

void BookIdxView::setModel(QAbstractItemModel *model)
{
    foreach ( QAbstractItemView* view, _childViews ) {
        view->setModel(model);
    }
    QAbstractItemView::setModel(model);
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
    if ( event->button() == Qt::MidButton ){
        event->ignore();
    } else {
        QAbstractItemView::mousePressEvent(event);
    }
}

void BookIdxView::mouseMoveEvent(QMouseEvent *event)
{
    if ( event->buttons() == Qt::MidButton ){
        event->ignore();
    } else {
        QAbstractItemView::mouseMoveEvent(event);
    }
}

void BookIdxView::mouseReleaseEvent(QMouseEvent *event)
{
    if ( event->button() == Qt::MidButton ){
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
        fprintf(stderr,"snap [bad scoobs]: BookIdxView::_bookModel() "
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
        fprintf(stderr,"snap [bad scoobs]: BookIdxView::_curvesUnit "
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
