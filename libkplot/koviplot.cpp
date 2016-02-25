#include "koviplot.h"

QLabel *createLabel(QWidget* parent, const QString& txt, const QColor &color);
VerticalLabel *createVLabel(QWidget* parent,
                            KPlotModel* plotModel,
                            const QModelIndex& plotIdx,
                            const QString& txt, const QColor &color);
LinedRuler *createRuler(QWidget* parent, KPlotModel* plotModel,
                         Qt::Alignment align, const QColor &color);
LabeledRuler *createLabeledRuler(QWidget* parent, KPlotModel* plotModel,
                                 Qt::Alignment align, const QColor &color);
PlotCorner *createPlotCorner(QWidget* parent, KPlotModel* plotModel,
                             Qt::Corner corner, const QColor &color);
KPlot* createPlot(QWidget* parent, KPlotModel* plotModel, const QColor& color);
bool isEqual(double a, double b, ulong maxD=10, bool isNeighborMethod=true);
QList<double> calcTicSet(double aIn, double bIn, double u=1.0, double n=10.0);

KoviPlot::KoviPlot(PlotBookModel *bookModel,
                   const QModelIndex &plotIdx, QWidget *parent) :
    _bookModel(bookModel),
    _plotIdx(plotIdx),
    QFrame(parent)
{
    setFrameShape(QFrame::Box);
    setContentsMargins(5,5,5,5);

    _layout = new QGridLayout(this);

    KPlotModel* plotModel = new KPlotModel(bookModel,plotIdx,this);

    // The composite plot widget is composed of 15 widgets
    KPlot* a00 = createPlot(this, plotModel, Qt::white);
    plotModel->setPlotWidget(a00);
    PlotCorner* a01 = createPlotCorner(this, plotModel, Qt::TopLeftCorner, Qt::white);
    LinedRuler* a02 = createRuler(this, plotModel, Qt::AlignTop, Qt::white);
    PlotCorner* a03 = createPlotCorner(this, plotModel, Qt::TopRightCorner, Qt::white);
    PlotCorner* a05 = createPlotCorner(this, plotModel, Qt::BottomRightCorner, Qt::white);
    LinedRuler* a06 = createRuler(this,  plotModel, Qt::AlignBottom, Qt::white);
    PlotCorner* a07 = createPlotCorner(this, plotModel, Qt::BottomLeftCorner, Qt::white);
    LinedRuler* a08 = createRuler(this, plotModel, Qt::AlignLeft, Qt::white);
    LinedRuler* a09 = createRuler(this, plotModel, Qt::AlignRight, Qt::white);
    LabeledRuler* a10 = createLabeledRuler(this, plotModel, Qt::AlignLeft, Qt::white);
    QLabel* a11 = createLabel(this, "X-Axis Label", Qt::white);
    VerticalLabel* a12 = createVLabel(this, plotModel, plotIdx,"Y-Axis Label", Qt::white);
    QLabel* a13 = createLabel(this, "Title", Qt::white);
    LabeledRuler* a14 = createLabeledRuler(this, plotModel, Qt::AlignBottom, Qt::white);


    // Add widgets to grid (need picture)
    _layout->addWidget(a12,0,0,6,1);
    _layout->addWidget(a13,0,1,1,4);
    _layout->addWidget(a10,1,1,3,1);
    _layout->addWidget(a01,1,2,1,1);
    _layout->addWidget(a02,1,3,1,1);
    _layout->addWidget(a03,1,4,1,1);
    _layout->addWidget(a08,2,2,1,1);
    _layout->addWidget(a00,2,3,1,1);
    _layout->addWidget(a09,2,4,1,1);
    _layout->addWidget(a07,3,2,1,1);
    _layout->addWidget(a06,3,3,1,1);
    _layout->addWidget(a05,3,4,1,1);
    _layout->addWidget(a14,4,1,1,4);
    _layout->addWidget(a11,5,1,1,4);

    // Stretch factors
    _layout->setColumnStretch(0,1);
    _layout->setColumnStretch(1,1);
    _layout->setColumnStretch(2,1);
    _layout->setColumnStretch(3,100);
    _layout->setColumnStretch(4,1);
    _layout->setRowStretch(0,1);
    _layout->setRowStretch(1,1);
    _layout->setRowStretch(2,100);
    _layout->setRowStretch(3,1);
    _layout->setRowStretch(4,1);
    _layout->setRowStretch(5,1);

    // Squish all widgets together
    _layout->setVerticalSpacing(0);
    _layout->setHorizontalSpacing(0);
}

QLabel* createLabel(QWidget* parent, const QString& txt, const QColor &color)
{
    QLabel* w = new QLabel(parent);

    w->setText(txt);

    QPalette pal(parent->palette());
    pal.setColor(QPalette::Window, color);
    w->setAutoFillBackground(true);
    w->setPalette(pal);

    return w;
}

VerticalLabel* createVLabel(QWidget* parent,
                            KPlotModel* plotModel,
                            const QModelIndex& plotIdx,
                            const QString& txt, const QColor &color)
{
    VerticalLabel* w = new VerticalLabel(plotModel,plotIdx,parent);

    w->setText(txt);

    QPalette pal(parent->palette());
    pal.setColor(QPalette::Window, color);
    w->setAutoFillBackground(true);
    w->setPalette(pal);

    return w;
}

LinedRuler *createRuler(QWidget* parent,
                        KPlotModel* plotModel,
                        Qt::Alignment align,
                        const QColor &color)
{
    LinedRuler* w = new LinedRuler(plotModel,align,parent);

    QPalette pal(parent->palette());
    pal.setColor(QPalette::Window, color);
    w->setAutoFillBackground(true);
    w->setPalette(pal);

    if ( align == Qt::AlignTop || align == Qt::AlignBottom ) {
        w->setFixedHeight(8);
    } else {
        w->setFixedWidth(8);
    }

    return w;
}

PlotCorner *createPlotCorner(QWidget* parent, KPlotModel* plotModel,
                             Qt::Corner corner, const QColor &color)
{
    PlotCorner* w = new PlotCorner(plotModel,corner,parent);

    QPalette pal(parent->palette());
    pal.setColor(QPalette::Window, color);
    w->setAutoFillBackground(true);
    w->setPalette(pal);

    w->setFixedHeight(8);
    w->setFixedWidth(8);

    return w;
}

LabeledRuler *createLabeledRuler(QWidget* parent,
                        KPlotModel* plotModel,
                        Qt::Alignment align,
                        const QColor &color)
{
    LabeledRuler* w = new LabeledRuler(plotModel,align,parent);

    QPalette pal(parent->palette());
    pal.setColor(QPalette::Window, color);
    w->setAutoFillBackground(true);
    w->setPalette(pal);

    return w;
}

KPlot *createPlot(QWidget* parent, KPlotModel *plotModel, const QColor &color)
{
    //Plot* w = new Plot(QRectF(0,1.0,2*M_PI,2.0),parent);
    KPlot* w = new KPlot(plotModel,parent);

    QPalette pal(parent->palette());
    pal.setColor(QPalette::Window, color);
    w->setAutoFillBackground(true);
    w->setPalette(pal);

    return w;
}

//
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
bool isEqual(double a, double b, ulong maxD, bool isNeighborMethod)
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
QList<double> calcTicSet(double aIn, double bIn, double u, double n)
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

//
// TODO: Make this into unit tests
//
#if 0
    QList<QPair<double,double> > bounds;
    bounds << qMakePair( 1.12345678912345e-163,
                         1.12345678912346e-163);
    bounds << qMakePair( 1.12345678912345e+163,
                         1.12345678912346e+163);
    bounds << qMakePair(1.797693134862315e304,
                       (1.0+(1.0-pow(2.0,-52)))*pow(2.0,1023));
    bounds << qMakePair(-1.2351234561e-300,
                        -1.2351234575e-300);
    bounds << qMakePair(-1.0e-245, 200.01);
    bounds << qMakePair(1.0e-245, 200.01);
    bounds << qMakePair(1.0e-245, 199.9999999999);
    bounds << qMakePair( 2.225073858506e-308,
                         2.225073858507e-308);
    bounds << qMakePair( 1.01e308, 1.99e308);
    bounds << qMakePair( -DBL_MAX,-DBL_MAX );
    bounds << qMakePair( -DBL_MIN,-DBL_MIN );
    bounds << qMakePair( -DBL_MAX,DBL_MAX );  // Largest interval possible
    bounds << qMakePair( -DBL_MAX,-DBL_MIN );
    bounds << qMakePair( -DBL_MAX,DBL_MIN );
    bounds << qMakePair( -DBL_MIN,DBL_MAX );
    bounds << qMakePair( -DBL_MIN,DBL_MIN );
    bounds << qMakePair(  DBL_MIN,DBL_MAX );
    bounds << qMakePair(  DBL_MIN,DBL_MAX );
    bounds << qMakePair(  0.0, DBL_MIN );
    bounds << qMakePair(  -DBL_MIN, 0.0 );
    bounds << qMakePair(11.0,29.0);
    bounds << qMakePair(-29.0,-11.0);
    bounds << qMakePair( 1983.0, 1989.0 );
    bounds << qMakePair( 1983.0, 1989.0 );
    bounds << qMakePair( 1983.25, 1989.99 );
    bounds << qMakePair(  -9983.0, -9989.0 );
    bounds << qMakePair(  9983.0, 9989.0 );

    for ( int i = 0 ; i < bounds.size(); ++i ) {
        QPair<double,double> bound = bounds.at(i);
        double a = bound.first;
        double b = bound.second;

        fprintf(stderr, "----------------------------\n");
        fprintf(stderr, "PAIR: ");
        fprintf(stderr, "[%0.15g,%0.15g]\n",a,b);

        timer.start();
        QList<double> K = calcTicSet(a, b);
        QList<double> Minors = calcTicSet(a,b,0.5);
        timer.snap("Time=");

        fprintf(stderr, "|%d|{ ",K.size());
        foreach ( double k, K ) {
            fprintf(stderr, "%0.15g ",k);
        }
        fprintf(stderr, "}\n");

        fprintf(stderr, "|%d|{ ",Minors.size());
        foreach ( double m, Minors ) {
            fprintf(stderr, "%0.15g ",m);
        }
        fprintf(stderr, "}\n");
    }
    exit(-1);
#endif


