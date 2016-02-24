#include "kplotmodel.h"
#include <stdio.h>
#include <QDebug>


KPlotModel::KPlotModel(PlotBookModel *plotBookModel,
                       const QModelIndex &plotIdx, QObject *parent) :
    _plotBookModel(plotBookModel),
    _plotIndex(plotIdx),
    QStandardItemModel(parent),
    _plotWidget(0)
{
    _initModel();
}

QRectF KPlotModel::xyRect() const
{
    QRectF rect = item(1,0)->data().toRectF();
    return rect;
}

QRectF KPlotModel::mathRect(const QWidget *widget) const
{
    QRectF mRect(0,0,0,0);
    if ( !_plotWidget ) return mRect;

    mRect = this->xyRect();

    if ( widget != _plotWidget ) {

        double ptSize = this->_pointSize();
        double q = ptSize/2.0;
        QRectF R1 = _plotWidget->rect();
        R1.moveTo(_plotWidget->mapToParent(R1.topLeft().toPoint()));
        QRectF R2(R1);
        R2.adjust(q,q,-q,-q);
        QRectF R3 = widget->rect();
        R3.moveTo(widget->mapToParent(R3.topLeft().toPoint()));
        R3.adjust(q,q,-q,-q);

        double pixelWidth =  mRect.width()/R2.width();
        double pixelHeight = mRect.height()/R2.height();
        QPointF windowPixelOffset = R2.topLeft()-R3.topLeft();
        double xMathOffset = pixelWidth*windowPixelOffset.x();
        double yMathOffset = pixelHeight*windowPixelOffset.y();
        QPointF r3MathTopLeft(mRect.x()-xMathOffset,
                              mRect.y()-yMathOffset);
        double r3MathWidth  = pixelWidth*R3.width();
        double r3MathHeight = pixelHeight*R3.height();
        QRectF r3MathRect(r3MathTopLeft.x(), r3MathTopLeft.y(),
                          r3MathWidth, r3MathHeight);

        mRect = r3MathRect;
    }

    return mRect;
}

//
// (Unscaled) Point size of curve drawing pen
// (0 is "cosmetic" i.e. 1pt and unaffected by scaling)
//
double KPlotModel::_pointSize() const
{
    double ptSize = item(3,0)->data().toDouble();
    return ptSize;
}


double KPlotModel::curvePointSize(const QWidget* widget) const
{
    double curvePtSize = 0.0;

    if ( _pointSize() > 0.0 ) {
        QPointF scaledPointSize = _scaledPointSize(widget);
        double v = qAbs(scaledPointSize.x());
        double h = qAbs(scaledPointSize.y());
        double k = 0.7;  // by experiment (and not perfect)
        curvePtSize = k*sqrt(v*v + h*h);
    }

    return curvePtSize;
}

double KPlotModel::hLinePointSize(const QWidget* widget) const
{
    double ptSize = 0.0;

    if ( _pointSize() > 0.0 ) {
        QPointF scaledPointSize = _scaledPointSize(widget);
        ptSize = qAbs(scaledPointSize.y());
    }

    return ptSize;
}

double KPlotModel::vLinePointSize(const QWidget* widget) const
{
    double ptSize = 0.0;

    if ( _pointSize() > 0.0 ) {
        QPointF scaledPointSize = _scaledPointSize(widget);
        ptSize = qAbs(scaledPointSize.x());
    }

    return ptSize;
}

QList<double> KPlotModel::majorXTics()
{
    QList<double> X;
    QRectF r = xyRect();
    double a = r.left();
    double b = r.right();
    X = calcTicSet(a,b, M_PI/2.0);
    //X = calcTicSet(a,b,1.0);
    return X;
}

QList<double> KPlotModel::minorXTics()
{
    QList<double> X;
    QRectF r = xyRect();
    double a = r.left();
    double b = r.right();
    X = calcTicSet(a,b,M_PI/8.0);
    //X = calcTicSet(a,b,1.0/4.0);
    return X;
}

QList<double> KPlotModel::majorYTics()
{
    QList<double> Y;
    QRectF r = xyRect();
    double a = r.bottom();
    double b = r.top();
    Y = calcTicSet(a,b,1.0);
    return Y;
}

QList<double> KPlotModel::minorYTics()
{
    QList<double> Y;
    QRectF r = xyRect();
    double a = r.bottom();
    double b = r.top();
    Y = calcTicSet(a,b,1.0/4.0);
    return Y;
}

QTransform KPlotModel::coordToPixelTransform(const QWidget* widget) const
{
    QTransform I;
    if ( !_plotWidget ) return I;

    QRectF mathRect = this->mathRect(widget);
    double ptSize = this->_pointSize();
    double q = ptSize/2.0;

    // Window coords (topleft,bottomright)
    double u0 = q;
    double v0 = q;
    double u1 = widget->rect().bottomRight().x()-q;
    double v1 = widget->rect().bottomRight().y()-q;

    // Math coords
    double x0 = mathRect.topLeft().x();
    double y0 = mathRect.topLeft().y();
    double x1 = mathRect.bottomRight().x();
    double y1 = mathRect.bottomRight().y();

    // Transform (TODO: check that x0-x1 > 0 etc)
    // TODO: if x0=-DBL_MAX and x1=+DBL_MAX etc
    double du = u0-u1;
    double dv = v0-v1;
    double dx = x0-x1;
    double dy = y0-y1;
    double a = du/dx;
    double b = dv/dy;
    double c = u0-(du/dx)*x0;
    double d = v0-(dv/dy)*y0;

    QTransform T( a,    0,
                  0,    b,
                  c,    d);

    return T;
}

QPointF KPlotModel::_scaledPointSize(const QWidget* widget) const
{
    QPointF ptSizeScaled(0,0); // cosmetic

    double ptSize = _pointSize();

    if ( ptSize > 0.0 ) {

        QTransform T = coordToPixelTransform(widget);

        QTransform P(
                    T.m11(),   T.m12(),
                    T.m21(),   T.m22(),
                    0, 0);

        ptSizeScaled = P.inverted().map(QPointF(ptSize,ptSize));
    }

    return ptSizeScaled;
}

void KPlotModel::_initModel()
{
    QStandardItem *rootItem = invisibleRootItem();

    // 0,0
    QStandardItem *titleItem = new QStandardItem("PlotTitle");
    titleItem->setData("Plot Title");
    rootItem->appendRow(titleItem);

    // 1,0
    QStandardItem *xyRectItem = new QStandardItem("PlotXYRect");
    QPointF topLeft(0.0,1.0);
    QPointF botRight(2.0*M_PI,-1.0);
    QRectF xyRect(topLeft,botRight);
    xyRectItem->setData(xyRect);
    rootItem->appendRow(xyRectItem);

    // 2,0
    QStandardItem *winRectItem = new QStandardItem("PlotWindowRect");
    QPointF zero(0,0);
    QRectF winRect(zero,zero);
    winRectItem->setData(winRect);
    rootItem->appendRow(winRectItem);

    // 3,0
    QStandardItem *curvePtSizeItem = new QStandardItem("PointSize");
    curvePtSizeItem->setData(0.0);
    rootItem->appendRow(curvePtSizeItem);

    // 4,0
    QStandardItem *xAxisLabelItem = new QStandardItem("XAxisLabel");
    xAxisLabelItem->setData("XAxisLabel");
    rootItem->appendRow(xAxisLabelItem);

    // 5,0
    QStandardItem *yAxisLabelItem = new QStandardItem("YAxisLabel");
    yAxisLabelItem->setData("YAxisLabel");
    rootItem->appendRow(yAxisLabelItem);

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
bool KPlotModel::isEqual(double a, double b, ulong maxD, bool isNeighborMethod)
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
            fprintf(stderr, "bad scoobs in isEqual(a,b)\n");
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
            fprintf(stderr, "bad scoobies in isEqual(a,b)\n");
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

//
// Return set X which is smallest tic set between [a,b] for n,u.
//
// Given [a,b],n,u:
// Return X = {x: x=i*n^k*u in [a,b]
//                i is some ordered list of ints
//                and k minimizes set}
//
//
// TODO: test n==2, n==e.  At this point, n==10 is the only tested value
// TODO: Need to bail if n not 2,10 or e
// TODO: Haven't tested u > 1 or u < 0
QList<double> KPlotModel::calcTicSet(double aIn, double bIn, double u, double n)
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
    if ( isEqual(a,0) ) {
        // if (a == 0) then x0 = 0, because 0*d == 0 <= a == 0
    } else if ( x < a ) {
        while ( 1 ) {
            if ( isEqual(x,a) ) {
                break;
            } else if ( x < a ) {
                // keep going
                x0 = x;
                x += d;
                if ( isinf(x) ) {
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
            if ( isEqual(x,a) ) {
                break;
            } else if ( x < a ) {
                break;
            } else if ( x > a ) {
                // keep going
                x -= d;
                if ( isinf(x) ) {
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
    while ( X.isEmpty() ) {  // |X| > 0 (unless a problem occurs)

        double x = x0;  // Start search x <= a (keeps X ordered)
        d = pow(n,k);

        while ( 1 ) {
            if ( isEqual(x,a) ) {
                X << x;
                x += d*u;
                if ( isinf(x) ) break; else continue;
            } else if ( x < a ) { // x not in [a,b]... continue search
                x0 = x;
                double xu = x+d*u;
                if ( isEqual(xu,a) ) {
                    x += d*u;
                } else if ( xu > a && xu < b ) {
                    double xd = x+d;
                    if ( isEqual(xd,a) ) {
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
                if ( isinf(x) ) break; else continue;
            } else if ( isEqual(x,b) ) { // x in [a,b]... and no more
                X << x;
                break;
            } else if ( x < b ) { // x in [a,b]... try to find another
                X << x;
                x += d*u;
                if ( isinf(x) ) break; else continue;
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
    k += 1.0;

    // Clean up the set by removing any points not strictly within [a,b]
    for ( int i = X.size()-1; i >= 0; --i ) {
        double x = X.at(i);
        if ( x < a || x > b ) {
            X.removeAt(i);
        }
    }
    if ( X.isEmpty() ) {
        X << a;
    }

    return X;
}


#if 0
QTransform KPlotModel::coordToPixelTransform(const QRectF &windowRect) const
{e
    QRectF mathRect = this->xyRect();
    double ptSize = this->_pointSize();

    // Reset below
    double du = windowRect.width();
    double dv = windowRect.height();
    double dx = mathRect.width();
    double dy = qAbs(mathRect.height());
    // TODO: if dx or dy is 0

    //
    // Create transform from coord to pixel for painter
    //
    double p = 1;
    if ( ptSize > 0 ) {
        p = ptSize;
    }
    du -= p;  // important for keeping "math" axis at half a pixel
    dv -= p;
    double u0 = windowRect.x();
    double v0 = windowRect.y();
    double x0 = mathRect.x();
    double y0 = mathRect.y();
    if ( ptSize == 0 ) {
        // Cosmetic lines should not translate p/2
        p = 0;
    }
    QTransform T(
                du/dx,        0,
                    0,   -dv/dy,
                    (du/dx)*(-x0) + u0 + p/2.0,
                    (dv/dy)*(y0)  + v0 + p/2.0);

    return T;
}

QTransform KPlotModel::coordToPixelTransform(const QWidget* widget) const
{
    QTransform I;
    if ( !_plotWidget ) return I;

    QRectF mathRect = this->xyRect();
    double ptSize = this->_pointSize();

    if ( widget != _plotWidget ) {
        double q = ptSize/2.0;
        QRectF R1 = _plotWidget->rect();
        R1.moveTo(_plotWidget->mapToParent(R1.topLeft().toPoint()));
        QRectF R2(R1);
        R2.adjust(q,q,-q,-q);
        QRectF R3 = widget->rect();
        R3.moveTo(widget->mapToParent(R3.topLeft().toPoint()));
        R3.adjust(q,q,-q,-q);
        QPointF dp = R3.topLeft()-R2.topLeft();
        double sx = mathRect.width()/R2.width();
        double sy = mathRect.height()/R2.height();
        double mx = sx*dp.x();
        double my = sy*dp.y();
        QPointF mm(mx,my);
        double nx = sx*R3.width();
        double ny = sy*R3.height();
        QRectF mRect(mathRect);
        mRect.moveTopLeft(mm);
        mRect.setWidth(nx);
        mRect.setHeight(ny);
        mathRect = mRect;
    }

    // Reset below
    double du = widget->rect().width();
    double dv = widget->rect().height();
    double dx = mathRect.width();
    double dy = qAbs(mathRect.height());
    // TODO: if dx or dy is 0

    //
    // Create transform from coord to pixel for painter
    //
    double p = 1;
    if ( ptSize > 0 ) {
        p = ptSize;
    }
    du -= p;  // important for keeping "math" axis at half a pixel
    dv -= p;
    double u0 = widget->rect().x();
    double v0 = widget->rect().y();
    double x0 = mathRect.x();
    double y0 = mathRect.y();
    if ( ptSize == 0 ) {
        // Cosmetic lines should not translate p/2
        p = 0;
    }
    QTransform T(
                du/dx,        0,
                    0,   -dv/dy,
                    (du/dx)*(-x0) + u0 + p/2.0,
                    (dv/dy)*(y0)  + v0 + p/2.0);
    return T;
}
#endif

QString KPlotModel::title() const
{
    QString s = item(0,0)->data().toString();
    return s;
}

void KPlotModel::setTitle(const QString &title)
{
    QModelIndex idx = indexFromItem(item(0,0));
    setData(idx,title);
}

void KPlotModel::setXAxisLabel(const QString &label)
{
    QModelIndex idx = indexFromItem(item(4,0));
    setData(idx,label);
}

void KPlotModel::setYAxisLabel(const QString &label)
{
    QModelIndex idx = indexFromItem(item(5,0));
    setData(idx,label);
}
