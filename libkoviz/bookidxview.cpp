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

QList<double> BookIdxView::_majorXTics(const QModelIndex& plotIdx) const
{
    QList<double> X;
    QRectF r = _plotMathRect(plotIdx);
    double a = r.left();
    double b = r.right();
    X = _calcTicSet(a,b,1.0,10.0);
    return X;
}

QList<double> BookIdxView::_minorXTics(const QModelIndex &plotIdx) const
{
    QList<double> X;
    QRectF r = _plotMathRect(plotIdx);
    double a = r.left();
    double b = r.right();
    QString plotScale = _bookModel()->getDataString(plotIdx,
                                                    "PlotXScale","Plot");
    X = _calcMinorTicSet(a,b,plotScale);
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

QList<double> BookIdxView::_minorYTics(const QModelIndex &plotIdx) const
{
    QList<double> Y;
    QRectF r = _plotMathRect(plotIdx);
    double a = r.bottom();
    double b = r.top();
    QString plotScale = _bookModel()->getDataString(plotIdx,
                                                    "PlotYScale","Plot");
    Y = _calcMinorTicSet(a,b,plotScale);
    return Y;
}

QList<double> BookIdxView::_calcMinorTicSet(double a, double b,
                                            const QString &plotScale) const
{
    QList<double> Minors;

    QList<double> Majors = _calcTicSet(a,b,1.0,10.0);

    if ( Majors.size() >= 2 ) {
        if ( plotScale == "linear" ) {
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
        } else if ( plotScale == "log" ) {
            double d = Majors.at(1)-Majors.at(0);
            QList<double> majors;
            majors << Majors.at(0)-d;
            majors.append(Majors);
            for ( int i = 0; i < majors.size(); ++i ) {
                double m = majors.at(i);
                for (int j = 2; j <= 9; ++j) {
                    double tic = m+log10(j)*d;
                    if ( tic >= a && tic <= b ) {
                        Minors << tic;
                    }
                }
            }
        } else {
            fprintf(stderr, "koviz [bad scoobs]: _calcMinorTicSet()\n");
            exit(-1);
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
    if ( !std::isfinite(aIn) || !std::isfinite(bIn) ||
         !std::isfinite(n)   || !std::isfinite(u) ) {
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
                if ( std::fpclassify(x) == FP_INFINITE ) {
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
                if ( std::fpclassify(x) == FP_INFINITE ) {
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
                if ( std::fpclassify(x) == FP_INFINITE ) break; else continue;
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
                if ( std::fpclassify(x) == FP_INFINITE ) break; else continue;
            } else if ( _isEqual(x,b) ) { // x in [a,b]... and no more
                X << x;
                break;
            } else if ( x < b ) { // x in [a,b]... try to find another
                X << x;
                x += d*u;
                if ( std::fpclassify(x) == FP_INFINITE ) break; else continue;
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
        QString s;
        s = s.sprintf("%g",x);
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
            fprintf(stderr, "koviz [bad scoobs] in _isEqual(a,b)\n");
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
            fprintf(stderr, "koviz [bad scoobs] in _isEqual(a,b)\n");
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

// Get title strings from model
QStringList BookIdxView::_pageTitles(const QModelIndex &titleIdx) const
{
    QStringList titles;

    QModelIndex title1Idx;
    if ( _bookModel()->isIndex(titleIdx,"Page")) {
        if ( _bookModel()->isChildIndex(titleIdx,"Page","PageTitle")) {
            title1Idx = _bookModel()->getDataIndex(titleIdx,"PageTitle","Page");
        } else {
            titles << " " << " " << " " << " ";
            return titles;
        }
    } else {
        if ( _bookModel()->isChildIndex(titleIdx,"Table","TableTitle")) {
            title1Idx = _bookModel()->getDataIndex(titleIdx,
                                                   "TableTitle","Table");
        } else {
            titles << " " << " " << " " << " ";
            return titles;
        }
    }
    QModelIndex defTitlesIdx = _bookModel()->getIndex(QModelIndex(),
                                                      "DefaultPageTitles");
    QModelIndex title2Idx = _bookModel()->getDataIndex(defTitlesIdx,
                                                       "Title2",
                                                       "DefaultPageTitles");
    QModelIndex title3Idx = _bookModel()->getDataIndex(defTitlesIdx,
                                                       "Title3",
                                                       "DefaultPageTitles");
    QModelIndex title4Idx = _bookModel()->getDataIndex(defTitlesIdx,
                                                       "Title4",
                                                       "DefaultPageTitles");
    QString title1 = model()->data(title1Idx).toString();
    QString t1 = _bookModel()->getDataString(defTitlesIdx,
                                             "Title1","DefaultPageTitles");
    if ( title1.isEmpty() && t1.startsWith("koviz") ) {
        // Since subtitle has RUNs, don't use t1 (it has RUNs too)
        title1 = "Koviz Plots";
    } else if ( !t1.startsWith("koviz") ) {
        // DP page title overwritten by -t1 optional title
        title1 = t1;
    }
    QString title2 = model()->data(title2Idx).toString();
    QString title3 = model()->data(title3Idx).toString();
    QString title4 = model()->data(title4Idx).toString();

    titles << title1 << title2 << title3 << title4;

    return titles;
}

QRect BookIdxView::_paintPageTitle(const QModelIndex& pageIdx,
                                   QPainter& painter)
{
    // Save state
    painter.save();
    painter.setRenderHint(QPainter::Antialiasing);
    QFont origFont = painter.font();

    // Local vars used in geometry calcs
    int x = 0;
    int y = 0;
    int w = 0;
    QFont font(origFont);

    // Get canvas rectangle (R)
    QPaintDevice* paintDevice = painter.device();
    QRect R(0,0,paintDevice->width(),paintDevice->height());

    // Get titles from model
    QString title1 = _pageTitles(pageIdx).at(0);
    QString title2 = _pageTitles(pageIdx).at(1);
    QString title3 = _pageTitles(pageIdx).at(2);
    QString title4 = _pageTitles(pageIdx).at(3);

    //
    // Draw main title
    //
    font.setPointSize(14);
    painter.setFont(font);
    QFontMetrics fm1(font,painter.device());
    w = fm1.boundingRect(title1).width();
    x = (R.width()-w)/2;
    y = fm1.lineSpacing();
    if ( !title1.isEmpty() && !title1.trimmed().isEmpty() ) {
        painter.drawText(x,y,title1);
    }
    int yTitle1 = y;

    //
    // Draw subtitle with RUNs
    //
    font.setPointSize(11);
    painter.setFont(font);
    QFontMetrics fm2(font,paintDevice);
    if ( !title2.isEmpty() && !title2.trimmed().isEmpty() ) {
        QStringList lines = title2.split('\n', QString::SkipEmptyParts);
        y += fm1.descent() + fm1.leading() + fm2.ascent();
        if ( lines.size() == 1 ) {
            // single RUN
            w = fm2.boundingRect(title2).width();
            x = (R.width()-w)/2;
            painter.drawText(x,y,title2);
        } else if ( lines.size() > 1 ) {
            // multiple RUNs (show two RUNs and elide rest with elipsis)
            QStringList runs;
            runs << lines.at(0) << lines.at(1);
            runs = Runs::abbreviateRunNames(runs);
            QString s1 = runs.at(0);
            QString s2 = runs.at(1);
            if ( lines.size() > 2 ) {
                if ( !s2.endsWith(',') ) {
                    s2 += ",";
                }
                s2 += "...)";
            }
            QString s;
            if ( s1.endsWith(',') ) {
                s = s1 + s2;
            } else {
                s = s1 + "," + s2 ;
            }
            w = fm2.boundingRect(s).width();
            x = (R.width()-w)/2;
            painter.drawText(x,y,s);
        }
    }
    int yTitle2 = y;

    //
    // Draw title3 and title4 (align on colon if possible)
    //
    int leftTitle34 = 0;
    font.setPointSize(11);
    painter.setFont(font);
    QFontMetrics fm3(font,paintDevice);
    if ( !title3.isEmpty() && title3.contains(':') &&
         !title4.isEmpty() && title4.contains(':') ) {

        // Normal case, most likely user,date with colons
        // e.g. user: vetter
        //      date: July 8, 2016
        int i = title3.indexOf(':');
        QString s31 = title3.left(i);
        QString s32 = title3.mid(i+1);
        i = title4.indexOf(':');
        QString s41 = title4.left(i);
        QString s42 = title4.mid(i+1);
        int w3;
        if ( s32.size() > s42.size() ) {
            w3 = fm3.boundingRect(s32).width();
        } else {
            w3 = fm3.boundingRect(s42).width();
        }
        x = R.width()-w3;
        if ( _bookModel()->isPageLegend(pageIdx) ) {
            y = yTitle2 + fm2.descent() + fm2.leading() + fm3.ascent();
        } else {
            y = yTitle1;
        }
        painter.drawText(x,y,s32);
        y += fm3.lineSpacing();
        painter.drawText(x,y,s42);
        x -= fm3.boundingRect(" : ").width();
        y -= fm3.lineSpacing();
        painter.drawText(x,y," : ");
        y += fm3.lineSpacing();
        painter.drawText(x,y," : ");
        x -= fm3.boundingRect(s31).width();
        y -= fm3.lineSpacing();
        painter.drawText(x,y,s31);
        leftTitle34 = x;
        x += fm3.boundingRect(s31).width();
        x -= fm3.boundingRect(s41).width();
        y += fm3.lineSpacing();
        painter.drawText(x,y,s41);
        if ( x < leftTitle34 ) {
            leftTitle34 = x;
        }

    } else {
        // title3,title4 are custom (i.e. not colon delimited user/date)
        x = R.width() - fm3.boundingRect(title3).width();
        if ( _bookModel()->isPageLegend(pageIdx)) {
            y = yTitle2 + fm2.descent() + fm2.leading() + fm3.ascent();
        } else {
            y = yTitle1;
        }
        painter.drawText(x,y,title3);
        leftTitle34 = x;
        x = R.width() - fm3.boundingRect(title4).width();
        y += fm3.lineSpacing();
        painter.drawText(x,y,title4);
        if ( x < leftTitle34 ) {
            leftTitle34 = x;
        }
    }

    // Legend
    int yBottom = y + fm3.descent() + fm3.leading();
    if ( _bookModel()->isPageLegend(pageIdx) ) {
        // Print legend in box that is about 3 entries in height
        QModelIndexList plotIdxs = _bookModel()->plotIdxs(pageIdx);
        QModelIndex curvesIdx = _bookModel()->getIndex(plotIdxs.at(0),
                                                       "Curves","Plot");
        QFont legendFont = painter.font();
        legendFont.setPointSize(8);
        QFontMetrics fml(legendFont,painter.device());
        int c = fml.averageCharWidth();
        int h3  = 3*fml.lineSpacing();
        y = yTitle2 + fm2.descent() + fm2.leading() + fml.ascent();
        QRect S(R);
        S.setTopLeft(QPoint(R.x()+10*c,y));
        S.setHeight(h3);
        _paintPageLegend(S,curvesIdx,painter);
        yBottom = S.bottom();
    }

    // Restore painter
    painter.setFont(origFont);
    painter.restore();

    // Estimate bounding box of page title
    QRect B(R);
    B.setBottom(yBottom);
    return B;
}

void BookIdxView::_paintCurvesLegend(const QRect& R,
                                     const QModelIndex &curvesIdx,
                                     QPainter &painter)
{
    const int maxEntries = 7;

    int nCurves = model()->rowCount(curvesIdx);
    if ( nCurves > maxEntries || nCurves <= 1 ) {
        return;
    }

    // If all plots on the page have the same legend, PageTitle will show legend
    if (_bookModel()->isPlotLegendsSame(curvesIdx.parent().parent().parent())) {
        return;
    }

    QString pres = _bookModel()->getDataString(curvesIdx.parent(),
                                               "PlotPresentation","Plot");
    if ( pres == "error" ) {
        return;
    }

    QModelIndex plotIdx = curvesIdx.parent();
    QList<QPen*> pens = _bookModel()->legendPens(plotIdx,
                                                 painter.paintEngine()->type());
    QStringList symbols = _bookModel()->legendSymbols(plotIdx);
    QStringList labels = _bookModel()->legendLabels(plotIdx);

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
    const int top = fh/4;   // top margin
    const int right = fw/2; // right margin
    QPoint legendTopLeft(R.right()-w-right,R.top()+top);

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

QRect BookIdxView::_paintPageLegend(const QRect &R,
                                 const QModelIndex &curvesIdx,
                                 QPainter &painter)
{
    const int maxEntries = 7;

    int nCurves = model()->rowCount(curvesIdx);
    if ( nCurves > maxEntries || nCurves <= 1 ) {
        return QRect();
    }

    QModelIndex plotIdx = curvesIdx.parent();
    QString pres = _bookModel()->getDataString(plotIdx,
                                               "PlotPresentation","Plot");
    if ( pres == "error" ) {
        return QRect();
    }

    QList<QPen*> pens = _bookModel()->legendPens(plotIdx,
                                                 painter.paintEngine()->type());
    QStringList symbols = _bookModel()->legendSymbols(plotIdx);
    QStringList labels = _bookModel()->legendLabels(plotIdx);

    if ( pres == "error+compare" ) {
        QPen* magentaPen = new QPen(_bookModel()->errorLineColor());
        pens << magentaPen;
        symbols << "none";
        labels << "error";
    }

    QRect B = __paintPageLegend(R,pens,symbols,labels,painter);

    // Clean up
    foreach ( QPen* pen, pens ) {
        delete pen;
    }

    return B;
}

// pens,symbols and labels are ordered/collated foreach legend curve/label
// Returns bounding box of painted area (B)
QRect BookIdxView::__paintPageLegend(const QRect &R,
                                  const QList<QPen *> &pens,
                                  const QStringList &symbols,
                                  const QStringList &labels,
                                  QPainter &painter)
{
    QFont origFont = painter.font();
    QPen origPen = painter.pen();

    QFont font(origFont);
    font.setPointSize(8);
    painter.setFont(font);
    QFontMetrics fm = painter.fontMetrics();

    const int c = fm.averageCharWidth();
    const int v = fm.leading();   // vertical space between legend entries
    const int s = c;   // spaceBetweenLineAndLabel
    const int l = 5*c; // line width
    const int q = 5*c; // space between legend columns

    QRect lastBB;
    QRect B; // current overall bounding box (union of bbs)
    for ( int i = 0; i < pens.size(); ++i ) {

        QPoint topLeft;
        if ( i == 0 ) {
            topLeft.setX(R.x());
            topLeft.setY(R.y());
        } else {
            topLeft.setX(lastBB.bottomLeft().x());
            topLeft.setY(lastBB.bottomLeft().y()+v);
        }

        // Calc bbox for legend entry
        int labelWidth  = fm.boundingRect(labels.at(i)).width();
        int labelHeight = fm.boundingRect(labels.at(i)).height();
        QRect bb(topLeft,QSize(l+s+labelWidth,labelHeight));

        if ( bb.bottom() > R.bottom() ) {
            bb.moveTopLeft(QPoint(B.right()+q,R.top()));
        }

        _paintLegendEntry(bb,
                          l,s,
                          *pens.at(i),
                          symbols.at(i),
                          labels.at(i),
                          painter);

        B = B.united(bb);
        lastBB = bb;
    }

    painter.setFont(origFont);
    painter.setPen(origPen);

    return B;
}

// Note: symbols can be drawn outside of R
void BookIdxView::_paintLegendEntry(const QRect &R,
                                      int l,  // line width
                                      int s,  // spaceBetweenLineAndLabel
                                      const QPen &pen,
                                      const QString &symbol,
                                      const QString &label,
                                      QPainter &painter)
{
    QPen origPen = painter.pen();

    // Draw line segment
    painter.setPen(pen);
    QPoint p1(R.left(),R.center().y());
    QPoint p2(R.left()+l,R.center().y());
    painter.drawLine(p1,p2);

    // Draw symbols on line segment endpoints
    __paintSymbol(p1,symbol,painter);
    __paintSymbol(p2,symbol,painter);

    // Draw label
    QRect labelRect(R);
    QPoint p(R.topLeft().x()+l+s, R.topLeft().y());
    labelRect.moveTopLeft(p);
    painter.drawText(labelRect, Qt::AlignLeft, label);

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
    // If Grid is off, do not paint grid
    QModelIndex isGridIdx = _bookModel()->getDataIndex(plotIdx,
                                                       "PlotGrid","Plot");
    bool isGrid = _bookModel()->data(isGridIdx).toBool();
    if ( !isGrid ) {
        return;
    }

    QString plotXScale = _bookModel()->getDataString(plotIdx,
                                                     "PlotXScale","Plot");
    QString plotYScale = _bookModel()->getDataString(plotIdx,
                                                     "PlotYScale","Plot");
    bool isXLogScale = ( plotXScale == "log" ) ? true : false;
    bool isYLogScale = ( plotYScale == "log" ) ? true : false;

    QList<double> xtics = _majorXTics(plotIdx);
    if ( isXLogScale ) {
        xtics.append(_minorXTics(plotIdx));
    }
    QList<double> ytics = _majorYTics(plotIdx);
    if ( isYLogScale ) {
        ytics.append(_minorYTics(plotIdx));
    }

    QVector<QPointF> vLines;
    QVector<QPointF> hLines;

    const QRectF M = _mathRect();

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
