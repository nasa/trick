#include "bookview.h"

BookView::BookView(QWidget *parent) :
    BookIdxView(parent)
{
    _mainLayout = new QVBoxLayout;

    _nb = new QTabWidget(this);
    _nb->setTabsClosable(true);
    _nb->setFocusPolicy(Qt::StrongFocus);
    _nb->setMovable(true);

    connect(_nb,SIGNAL(tabCloseRequested(int)),
            this,SLOT(_nbCloseRequested(int)));

    _mainLayout->addWidget(_nb);

    setLayout(_mainLayout);
}


void BookView::currentChanged(const QModelIndex &current,
                              const QModelIndex &previous)
{
    Q_UNUSED(previous);
    if ( _bookModel()->isIndex(current,"Page") ) {
        QString pageName = _bookModel()->getDataString(current,
                                                       "PageName","Page");
        pageName = pageName.split(":").at(0);
        QFileInfo fi(pageName);
        pageName = fi.fileName();
        for (int i = 0; i < _nb->count(); ++i) {
            if ( _nb->tabText(i) == pageName ) {
                _nb->setCurrentIndex(i);
                break;
            }
        }
    }
}

void BookView::selectionChanged(const QItemSelection &selected,
                                const QItemSelection &deselected)
{
    Q_UNUSED(selected);
    Q_UNUSED(deselected);
}

int BookView::_modelIdxToTabId(const QModelIndex &idx)
{
    int tabId = -1;

    QString name;
    QString tag = model()->data(idx).toString();
    if ( tag == "Page" ) {
        name = _bookModel()->getDataString(idx,"PageName","Page");
    } else if ( tag == "Table" ) {
        name = _bookModel()->getDataString(idx,"TableName","Table");
    } else {
        fprintf(stderr,"koviz [bad scoobs]:1: BookView::_modelIdxToTabId\n");
        exit(-1);
    }

    int nTabs = _nb->count();
    for ( int i = 0; i < nTabs; ++i ) {

        QString tabToolTip = _nb->tabToolTip(i);
        if ( tabToolTip == name ) {
            tabId = i;
            break;
        }
    }

    if ( tabId == -1 ) {
        fprintf(stderr,"koviz [bad scoobs]:2: BookView::_modelIdxToTabId()\n");
        exit(-1);
    }

    return tabId;
}

QModelIndex BookView::_tabIdToModelIdx(int tabId)
{
    QModelIndex idx;

    QString tabToolTip = _nb->tabToolTip(tabId);
    QString wt = _nb->tabWhatsThis(tabId);

    QModelIndex pidx;
    if ( wt == "Page" ) {
        pidx = _bookModel()->getIndex(QModelIndex(), "Pages");
    } else if ( wt == "Table") {
        pidx = _bookModel()->getIndex(QModelIndex(), "Tables");
    }

    int rc = model()->rowCount(pidx);
    for ( int i = 0; i < rc ; ++i ) {
        QString name;
        if ( wt == "Page") {
            QModelIndex pageIdx = model()->index(i,0,pidx);
            name = _bookModel()->getDataString(pageIdx,"PageName","Page");
        } else if ( wt == "Table" ) {
            QModelIndex tableIdx = model()->index(i,0,pidx);
            name = _bookModel()->getDataString(tableIdx,"TableName","Table");
        }
        if ( name == tabToolTip ) {
            idx = model()->index(i,0,pidx);
            break;
        }
    }

    if ( !idx.isValid() ) {
        fprintf(stderr,"koviz [bad scoobs]:1: BookView::_tabIdToPageIdx() "
                       "Could not find page/table using tabId=%d\n", tabId);
        exit(-1);
    }

    return idx;
}

//////////////////////////////////////////////////////////////////////
//
// This is straight from the QTransform help page  -
// QTransform transforms a point in the plane to another point
// using the following formulas:
//
//        x' = m11*x + m21*y + dx
//        y' = m22*y + m12*x + dy
//        if (is not affine) {
//            w' = m13*x + m23*y + m33
//            x' /= w'
//            y' /= w'
//        }
//
// Keith's note:
//
//  Goal is to find T, the 3x3 (column major) matrix (with no projection):
//
//   +- -+   +-          -++- -+   +-                -+   +-                -+
//   | x'|   |m11  m21  dx|| x |   |m11*x + m21*y + dx|   |m11*x + m21*y + dx|
//   | y'| = |m12  m22  dy|| y | = |m12*x + m22*y + dy| = |m22*y + m12*x + dy|
//   | 1 |   | 0    0    1|| 1 |   |      1           |   |         1        |
//   +- -+    +-         -++- -+   +-                -+   +-                -+
//
//  and no shearing i.e. m12=m21=0:
//
//   +- -+   +-          -+
//   | x'|   | m11*x + dx |
//   | y'| = | m22*y + dy | === T
//   | 1 |   |     1      |
//   +- -+   +-          -+
//
//   We must find m11, m22, dx and dy
//
//////////////////////////////////////////////////////////////////////
// D is the "drawing coordinate system"
// M is the "math coordinate system"
//
// DC===curvesRect in drawing coord sys
// Dp===Abitrary point p in D
// Dv===vector in from DC.topLeft to Dp
//
// MC===curvesRect in math coord sys as found in model.PlotMathRect
// Mp===Abitrary point p(x,y) in M
// Mv===vector in from MC.topLeft to Mp
//
// 1. Dp = DC.topLeft() + Dv                      (See pic below)
// 2. Mp = MC.topLeft() + Mv                      (See pic below)
// 3. Dv = k*Mv                                   (k has independent x/y scale)
// 4. Dp = DC.topLeft() + k*Mv                    (Use 3, sub k*Mv for Dv in 1)
// 5. Mv = Mp-MC.topLeft()                        (See pic below)
// 6. Dp = DC.topLeft()+k*(Mp-MC.topLeft())       (substitution using 4,5)
// 7. y'===Dp.y = DC.y() + k.y()*(Mp.y()-MC.y())  (use 6. and y-component)
// 8. b===k.y= DC.height()/MC.height()            (linearity,I think,but unsure)
// 9.  y' = DC.y() + b*(Mp.y()-MC.y())            (sub)
// 10. y' = b*Mp.y() + DC.y() - b*MC.y()          (distribution and rearrange)
// 11. y' = b*y      + DC.y() - b*MC.y()          (Mp.y() is arbitrary math y)
// 12. m22 = b                                    (10,11 and y'=m22*y+dy)
// 13. dy  = DC.y() - b*MC.y()                    (10,11 and y'=m22*y+dy)
// 14. m11 = a                                    (similar to y)
// 15. dx  = DC.x() - a*MC.x()                    (similar to y)
//////////////////////////////////////////////////////////////////////
//   M               * Mp                       D                        
//      ^           /                           +--------------------> 
//      |          /                            |        * Dp          
//      |         / Mv                   T()    |       /              
//      |        /                       -->    |      / Dv            
//      |       +--------------------*          |     +-----------*
//      |       |                    |          |     |           |    
//      |       |                    |          |     |    DC     |
//      |       |         MC         |          |     |           |
//      |       |                    |          |     *-----------*
//      |       *--------------------*          v
//      +----------------------------------->                         
//////////////////////////////////////////////////////////////////////
//
// For a little speed and eloquence, this method relies on the Model to
// keep the math rectangle a size that will not break the division by
// the M.width() and the M.height()
//
//////////////////////////////////////////////////////////////////////
//
// C===curvesRect
//
//////////////////////////////////////////////////////////////////////
QTransform BookView::_coordToDotTransform(const QRectF& C,
                                          const QModelIndex& plotIdx)
{
    if ( !model() ) return QTransform(0,0,0,0,0,0);

    QRectF M = _plotMathRect(plotIdx);
    double a = C.width()/M.width();
    double b = C.height()/M.height();
    double c = C.x() - a*M.x();
    double d = C.y() - b*M.y();

    QTransform T( a,    0,
                  0,    b, /*+*/ c,    d);

    return T;
}

void BookView::savePdf(const QString &fname)
{
    //
    // Setup printer
    //
    QPrinter printer(QPrinter::HighResolution);
    printer.setCreator("Koviz");
    printer.setDocName(fname);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setPageSize(QPrinter::Letter);
    printer.setOutputFileName(fname);
    QString orient = _bookModel()->getDataString(QModelIndex(),"Orientation");
    if ( orient == "landscape" ) {
        printer.setOrientation(QPrinter::Landscape);
    } else if ( orient == "portrait" ) {
        printer.setOrientation(QPrinter::Portrait);
    } else {
        fprintf(stderr, "koviz [bad scoobs]: savePdf! Aborting!\n");
        exit(-1);
    }

    //
    // Begin Printing
    //
    QPainter painter;
    if (! painter.begin(&printer)) {
        return;
    }
    painter.save();

    //
    // Set pen
    //
    QPen pen((QColor(Qt::black)));
    double pointSize = printer.logicalDpiX()/72.0;
    pen.setWidthF(pointSize);
    painter.setPen(pen);

    //
    // Print pages
    //
    bool isFirst = true;
    int nTabs = _nb->count();
    for ( int i = 0; i < nTabs; ++i) {

        QModelIndex idx = _tabIdToModelIdx(i);
        QString tag = model()->data(idx).toString();
        if ( tag != "Page") {
            continue; // only print pages (i.e. not tables)
        }
        if ( isFirst ) {
            isFirst = false;
        } else {
            printer.newPage();
        }
        _printPage(&painter,idx);
    }

    //
    // End printing
    //
    painter.restore();
    painter.end();
}

void BookView::_printPage(QPainter *painter, const QModelIndex& pageIdx)
{
    QPaintDevice* paintDevice = painter->device();
    if ( !paintDevice ) return;

    painter->save();

    // Foreground
    QPen origPen = painter->pen();
    QColor fg = _bookModel()->pageForegroundColor(pageIdx);
    QPen pagePen = painter->pen();
    pagePen.setColor(fg);
    painter->setPen(pagePen);

    // Background
    QColor bg = _bookModel()->pageBackgroundColor(pageIdx);
    painter->fillRect(QRect(0,0,paintDevice->width(),paintDevice->height()),bg);

    const int margin = 96;

    QRect pageTitleRect = _paintPageTitle(pageIdx,*painter);

    QRect R(pageTitleRect.bottomLeft().x(),
            pageTitleRect.bottomLeft().y(),
            paintDevice->width(),
            paintDevice->height()-pageTitleRect.height());
    R.setTop(R.top()+margin); // 96 dot margin between page title box and plots

    QModelIndex plotsIdx = _bookModel()->getIndex(pageIdx,"Plots", "Page");
    int nPlots = model()->rowCount(plotsIdx);

    if ( nPlots == 0 ) {
        return;
    } else if ( nPlots == 1 ) {

        int q = 4;
        int titleFontSize = 12;
        int axisLabelFontSize = 10;
        int ticLabelFontSize = 8;
        int ticWidth = 4;
        int ticHeight = 96;
        int axisLinePtSize = 16;

        _printPlot(R,painter,model()->index(0,0,plotsIdx),
                   q,0,48,48,48,48,0,  // margins
                   titleFontSize,
                   axisLabelFontSize, ticLabelFontSize,
                   axisLinePtSize, ticWidth, ticHeight);

    } else if ( nPlots == 2 ) {

        int q = 4;
        int titleFontSize = 12;
        int axisLabelFontSize = 10;
        int ticLabelFontSize = 8;
        int ticWidth = 4;
        int ticHeight = 96;
        int axisLinePtSize = 12;

        QRect r(R);
        r.setHeight(R.height()/2);

        _printPlot(r,painter,model()->index(0,0,plotsIdx),
                   q,0,48,48,48,24,96,
                   titleFontSize,
                   axisLabelFontSize, ticLabelFontSize,
                   axisLinePtSize, ticWidth, ticHeight);

        r.moveTo(r.bottomLeft());

        _printPlot(r,painter,model()->index(1,0,plotsIdx),
                   q,0,48,48,48,48,0,
                   titleFontSize,
                   axisLabelFontSize, ticLabelFontSize,
                   axisLinePtSize, ticWidth, ticHeight);

    } else if ( nPlots == 3 ) {

        int q = 4;
        int titleFontSize = 12;
        int axisLabelFontSize = 10;
        int ticLabelFontSize = 8;
        int ticWidth = 4;
        int ticHeight = 96;
        int axisLinePtSize = 12;

        QRect r(R);
        r.setHeight(R.height()/3);
        _printPlot(r,painter,model()->index(0,0,plotsIdx),
                   q,0,48,48,48,48,96,
                   titleFontSize,
                   axisLabelFontSize, ticLabelFontSize,
                   axisLinePtSize, ticWidth, ticHeight);

        r.moveTo(r.bottomLeft());
        _printPlot(r,painter,model()->index(1,0,plotsIdx),
                   q,0,48,48,48,48,96,
                   titleFontSize,
                   axisLabelFontSize, ticLabelFontSize,
                   axisLinePtSize, ticWidth, ticHeight);

        r.moveTo(r.bottomLeft());
        _printPlot(r,painter,model()->index(2,0,plotsIdx),
                   q,0,48,48,48,48,0,
                   titleFontSize,
                   axisLabelFontSize, ticLabelFontSize,
                   axisLinePtSize, ticWidth, ticHeight);

    } else if ( nPlots == 4 ) {

        int q = 4;
        int titleFontSize = 12;
        int axisLabelFontSize = 10;
        int ticLabelFontSize = 8;
        int ticWidth = 4;
        int ticHeight = 96;
        int axisLinePtSize = 12;

        QRect r(R);
        r.setWidth(R.width()/2);
        r.setHeight(R.height()/2);
        _printPlot(r,painter,model()->index(0,0,plotsIdx),
                   q,0,48,48,48,48,96,
                   titleFontSize,
                   axisLabelFontSize, ticLabelFontSize,
                   axisLinePtSize, ticWidth, ticHeight);

        r.moveTo(QPoint(R.width()/2,R.top()));
        _printPlot(r,painter,model()->index(1,0,plotsIdx),
                   q,240,48,48,48,48,96,
                   titleFontSize,
                   axisLabelFontSize, ticLabelFontSize,
                   axisLinePtSize, ticWidth, ticHeight);

        r.moveTo(QPoint(R.left()/2,r.bottom()));
        _printPlot(r,painter,model()->index(2,0,plotsIdx),
                   q,0,48,48,48,48,96,
                   titleFontSize,
                   axisLabelFontSize, ticLabelFontSize,
                   axisLinePtSize, ticWidth, ticHeight);

        r.moveTo(r.topRight());
        _printPlot(r,painter,model()->index(3,0,plotsIdx),
                   q,240,48,48,48,48,0,
                   titleFontSize,
                   axisLabelFontSize, ticLabelFontSize,
                   axisLinePtSize, ticWidth, ticHeight);

    } else if ( nPlots == 5 ) {

        int q = 4;
        int titleFontSize = 12;
        int axisLabelFontSize = 10;
        int ticLabelFontSize = 8;
        int ticWidth = 4;
        int ticHeight = 96;
        int axisLinePtSize = 12;

        QRect r(R);

        r.setWidth(R.width()/2);
        r.setHeight(R.height()/3);
        _printPlot(r,painter,model()->index(0,0,plotsIdx),
                   q,0,48,48,48,48,96,
                   titleFontSize,
                   axisLabelFontSize, ticLabelFontSize,
                   axisLinePtSize, ticWidth, ticHeight);

        r.moveTo(QPoint(R.width()/2,R.top()));
        _printPlot(r,painter,model()->index(1,0,plotsIdx),
                   q,240,48,48,48,48,96,
                   titleFontSize,
                   axisLabelFontSize, ticLabelFontSize,
                   axisLinePtSize, ticWidth, ticHeight);

        r.moveTo(QPoint(R.left(),r.bottom()));
        _printPlot(r,painter,model()->index(2,0,plotsIdx),
                   q,0,48,48,48,48,96,
                   titleFontSize,
                   axisLabelFontSize, ticLabelFontSize,
                   axisLinePtSize, ticWidth, ticHeight);

        r.moveTo(r.topRight());
        _printPlot(r,painter,model()->index(3,0,plotsIdx),
                   q,240,48,48,48,48,96,
                   titleFontSize,
                   axisLabelFontSize, ticLabelFontSize,
                   axisLinePtSize, ticWidth, ticHeight);

        r.moveTo(QPoint(R.left(),r.bottom()));
        r.setWidth(R.width());
        _printPlot(r,painter,model()->index(4,0,plotsIdx),
                   q,0,48,48,48,48,96,
                   titleFontSize,
                   axisLabelFontSize, ticLabelFontSize,
                   axisLinePtSize, ticWidth, ticHeight);

    } else if ( nPlots == 6 ) {

        QRect r(R);
        r.setWidth(R.width()/2);
        r.setHeight(R.height()/3);
        QRect rr(r);

        int q = 2;
        int titleFontSize = 10;
        int axisLabelFontSize = 8;
        int ticLabelFontSize = 6;
        int ticWidth = 2;
        int ticHeight = 72;
        int axisLinePtSize = 8;

        rr.setRight(r.right()-margin);
                          //int q, int wm0, int wm1, int wm2, int hm3, int hm4,
        _printPlot(rr,painter,model()->index(0,0,plotsIdx),
                   q,0,0,24,24,24,48,  // margins
                   titleFontSize,
                   axisLabelFontSize, ticLabelFontSize,
                   axisLinePtSize, ticWidth, ticHeight);
        QRect tlr(rr);

        r.moveTo(r.bottomLeft());
        rr = r; rr.setRight(r.right()-margin);
        _printPlot(rr,painter,model()->index(1,0,plotsIdx),
                   q,0,0,24,24,24,48,
                   titleFontSize,
                   axisLabelFontSize, ticLabelFontSize,
                   axisLinePtSize, ticWidth, ticHeight);

        r.moveTo(r.bottomLeft());
        rr = r; rr.setRight(r.right()-margin);
        _printPlot(rr,painter,model()->index(2,0,plotsIdx),
                   q,0,0,24,24,24,48,
                   titleFontSize,
                   axisLabelFontSize, ticLabelFontSize,
                   axisLinePtSize, ticWidth, ticHeight);

        r.moveTo(tlr.topRight());
        rr = r; rr.moveLeft(r.left()+margin);
        _printPlot(rr,painter,model()->index(3,0,plotsIdx),
                   q,0,0,24,24,24,48,
                   titleFontSize,
                   axisLabelFontSize, ticLabelFontSize,
                   axisLinePtSize, ticWidth, ticHeight);

        r.moveTo(r.bottomLeft());
        rr = r; rr.moveLeft(r.left()+margin);
        _printPlot(rr,painter,model()->index(4,0,plotsIdx),
                   q,0,0,24,24,24,48,
                   titleFontSize,
                   axisLabelFontSize, ticLabelFontSize,
                   axisLinePtSize, ticWidth, ticHeight);

        r.moveTo(r.bottomLeft());
        rr = r; rr.moveLeft(r.left()+margin);
        _printPlot(rr,painter,model()->index(5,0,plotsIdx),
                   q,0,0,24,24,24,48,
                   titleFontSize,
                   axisLabelFontSize, ticLabelFontSize,
                   axisLinePtSize, ticWidth, ticHeight);

    } else if ( nPlots == 7 ) {

        int q = 2;
        int titleFontSize = 10;
        int axisLabelFontSize = 8;
        int ticLabelFontSize = 6;
        int ticWidth = 2;
        int ticHeight = 72;
        int axisLinePtSize = 8;

        QRect r(R);
        r.setWidth(R.width()/2);
        r.setHeight(R.height()/4);

        _printPlot(r,painter,model()->index(0,0,plotsIdx),
                   q,0,48,48,48,48,96,
                   titleFontSize,
                   axisLabelFontSize, ticLabelFontSize,
                   axisLinePtSize, ticWidth, ticHeight);

        r.moveTo(r.bottomLeft());
        _printPlot(r,painter,model()->index(1,0,plotsIdx),
                   q,0,48,48,48,48,96,
                   titleFontSize,
                   axisLabelFontSize, ticLabelFontSize,
                   axisLinePtSize, ticWidth, ticHeight);

        r.moveTo(r.bottomLeft());
        _printPlot(r,painter,model()->index(2,0,plotsIdx),
                   q,0,48,48,48,48,96,
                   titleFontSize,
                   axisLabelFontSize, ticLabelFontSize,
                   axisLinePtSize, ticWidth, ticHeight);

        r.moveTo(R.topLeft());
        r.moveTo(r.topRight());
        _printPlot(r,painter,model()->index(3,0,plotsIdx),
                   q,240,48,48,48,48,96,
                   titleFontSize,
                   axisLabelFontSize, ticLabelFontSize,
                   axisLinePtSize, ticWidth, ticHeight);

        r.moveTo(r.bottomLeft());
        _printPlot(r,painter,model()->index(4,0,plotsIdx),
                   q,240,48,48,48,48,96,
                   titleFontSize,
                   axisLabelFontSize, ticLabelFontSize,
                   axisLinePtSize, ticWidth, ticHeight);

        r.moveTo(r.bottomLeft());
        _printPlot(r,painter,model()->index(5,0,plotsIdx),
                   q,240,48,48,48,48,96,
                   titleFontSize,
                   axisLabelFontSize, ticLabelFontSize,
                   axisLinePtSize, ticWidth, ticHeight);

        r.moveTo(QPoint(R.left(),r.bottom()));
        r.setWidth(R.width());
        _printPlot(r,painter,model()->index(6,0,plotsIdx),
                   q,0,48,48,48,48,96,
                   titleFontSize,
                   axisLabelFontSize, ticLabelFontSize,
                   axisLinePtSize, ticWidth, ticHeight);

    } else {
        fprintf(stderr,"koviz [error]: _printPage() : cannot print more than "
                       "7 plots on a page.\n");
        exit(-1);
    }

    painter->setPen(origPen);
    painter->restore();
}

//   R.width = m0+a+m1+b+m2+c+d+c
//           = a+b+c+2d+m0+m1+m2
//
//   b is most variable since values have different widths
//   e.g. length("1.0") != length("1.0001")
//
//   m0 takes most slack up since m1 and m2 are fairly fixed
//
//    m0     a   m1     b      m2  c           d                 c
//  +-----++---++-++---------++-++---++-----------------------++---+
//  |     ||   || ||         || || C ||        Tics           || C | c
//  |     ||   || ||         || |+---++-----------------------++---+
//  |     ||   || ||         || |+---++-----------------------++---+
//  |     ||   || ||         || ||   ||       Title           ||   |
//  |     || Y || ||    Y    || ||   |+-----------------------+|   |
//  |     ||   || ||         || ||   |+-----------------------+|   |
//  |     || A || ||    T    || ||   ||          q1           ||   |
//  |     || x || ||    i    || ||   |+-----------------------+|   |
//  |  m  || i ||m||    c    ||m||   |+-++-----------------++-+|   |
//  |  0  || s ||1||         ||2|| T || ||                 || || T |
//  |     ||   || ||    L    || || i ||q||                 ||q|| i |
//  |     || L || ||    a    || || c ||4||    Curves       ||2|| c |
//  |     || a || ||    b    || || s || ||                 || || s |
//  |     || b || ||    e    || ||   || ||                 || ||   |
//  |     || e || ||    l    || ||   |+-++-----------------++-+|   |
//  |     || l || ||    s    || ||   |+-----------------------+|   |
//  |     ||   || ||         || ||   ||         q3            ||   |
//  |     ||   || ||         || |+---++-----------------------++---+
//  |     ||   || ||         || |+---++-----------------------++---+
//  |     ||   || ||         || || C ||       Tics            || C | c
//  +-----++---++-++---------++-++---++-----------------------++---+
//  +--------------------------------------------------------------+
//  |                                          m3                  | m3
//  +--------------------------------------------------------------+
//  +--------------------------------------------------------------+
//  |                                     X Tic Labels             | e
//  +--------------------------------------------------------------+
//  +--------------------------------------------------------------+
//  |                                          m4                  | m4
//  +--------------------------------------------------------------+
//  +--------------------------------------------------------------+
//  |                                     X Axis Label             | f
//  +--------------------------------------------------------------+
//  +--------------------------------------------------------------+
//  |                                          m5                  | m5
//  +--------------------------------------------------------------+
//
void BookView::_printPlot(const QRect &R,
                          QPainter *painter, const QModelIndex &plotIdx,
                          int q,
                          int wm0, int wm1, int wm2, int hm3, int hm4, int hm5,
                          int titleFontSize,
                          int axisLabelFontSize, int ticLabelFontSize,
                          int axisLineDotSize, int ticWidth, int ticHeight)
{
    painter->save();

    QPen origPen = painter->pen();
    QPen penq(origPen); penq.setWidth(q);
    QPen pena(origPen); pena.setWidth(axisLineDotSize);
    QPen penw(origPen); penw.setWidth(ticWidth);
    QPen penh(origPen); penh.setWidth(ticHeight);

    QFont origFont = painter->font();
    QFont fontTitle(origFont);    fontTitle.setPointSize(titleFontSize);
    QFont fontTics(origFont);     fontTics.setPointSize(ticLabelFontSize);
    QFont fontAxisLabel(origFont);fontAxisLabel.setPointSize(axisLabelFontSize);

    QFontMetrics fmTitle(fontTitle);
    QFontMetrics fmTics(fontTics);
    QFontMetrics fmAxisLabel(fontAxisLabel);

    QString title = _bookModel()->getDataString(plotIdx,
                                                "PlotTitle","Plot");
    QString xAxisLabel = _bookModel()->getDataString(plotIdx,
                                                "PlotXAxisLabel","Plot");
    QString yAxisLabel = _bookModel()->getDataString(plotIdx,
                                                "PlotYAxisLabel","Plot");

    int c = q+ticHeight; // corner width and height
    int titleh = fmTitle.boundingRect(title).height();
    int xalh = fmAxisLabel.boundingRect(xAxisLabel).height();
    int yalh = fmAxisLabel.boundingRect(yAxisLabel).height();
    int tlh = fmTics.xHeight();

    // Calculate y-tic-label width (~200)
    // All plots on the page should be vertically aligned.
    // The easy way to do this is to make the y-tic-label width
    // the max y-tic-label width of all formatted tic labels on the entire page
    int ytlw = 0;
    QModelIndex pageIdx = plotIdx.parent().parent();
    QModelIndexList plotIdxs = _bookModel()->plotIdxs(pageIdx);
    foreach ( QModelIndex plotIdx, plotIdxs ) {
        QList<double> modelTics = _majorYTics(plotIdx);
        foreach ( double tic, modelTics ) {
            QString strVal = _format(tic);
            QRectF bb = fmTics.boundingRect(strVal);
            if ( bb.width() > ytlw ) {
                ytlw = bb.width();
            }
        }
    }


    //
    // Calculate all rectangles (see ascii drawing above)
    //
    QRect m0(R.topLeft(),QSize(wm0,R.height()-hm3-xalh-tlh-hm4-hm5));
    QRect yal(m0.topRight(),QSize(yalh,m0.height()));
    QRect m1(yal.topRight(),QSize(wm1,m0.height()));
    QRect ytl(m1.topRight(),QSize(ytlw,m0.height()));
    QRect m2(ytl.topRight(),QSize(wm2,m0.height()));
    QRect tlc(m2.topRight(),QSize(c,c));

    QRect ttics(tlc.topRight(),
                QSize(R.width()-wm0-yal.width()-wm1-ytl.width()-wm2-2*c,c));
    QRect trc(ttics.topRight(),QSize(c,c));
    QRect rtics(trc.bottomLeft(),QSize(c,m0.height()-2*c));
    QRect brc(rtics.bottomLeft(),QSize(c,c));

    QRect ltics(tlc.bottomLeft(),rtics.size());
    QRect blc(ltics.bottomLeft(),QSize(c,c));
    QRect btics(blc.topRight(),ttics.size());

    QRect titleRect(tlc.bottomRight(),QSize(ttics.width(),titleh));
    QRect q1(titleRect.bottomLeft(),QSize(titleRect.width(),q/2));
    QRect q4(q1.bottomLeft(),QSize(q/2,m0.height()-2*ttics.height()-titleh-q));

    QRect q3(q4.bottomLeft(),QSize(q1.width(),q/2)); Q_UNUSED(q3);
    QRect curvesRect(q4.topRight(),QSize(q1.width()-q,rtics.height()-titleh-q));
    QRect q2(curvesRect.topRight(),QSize(q/2,q4.height())); Q_UNUSED(q2);

    QRect m3(m0.bottomLeft(),QSize(R.width(),hm3));
    QRect xtl(m3.bottomLeft(),QSize(R.width(),tlh));
    QRect m4(xtl.bottomLeft(),QSize(R.width(),hm4));
    QRect xal(m4.bottomLeft(),QSize(R.width(),xalh));
    QRect m5(xal.bottomLeft(),QSize(R.width(),hm5)); Q_UNUSED(m5);

    //
    // Paint!
    //
    _printPlotTitle(titleRect,painter,plotIdx);

    // Curves
    painter->save();
    QRect clipRect(titleRect.topLeft().x(),
                   titleRect.topLeft().y(),
                   titleRect.width(),
                   ytl.height());
    painter->setClipRect(clipRect);
    painter->setPen(penq);
    _printCurves(curvesRect,painter,plotIdx);
    painter->setPen(origPen);
    painter->restore();

    // Paint axis lines, corners and tics
    painter->setPen(pena);
    _printTopLeftCorner(tlc,painter,plotIdx);
    _printTopRightCorner(trc,painter,plotIdx);
    _printBottomRightCorner(brc,painter,plotIdx);
    _printBottomLeftCorner(blc,painter,plotIdx);
    _printXTicsTop(ttics,painter,plotIdx,curvesRect);
    _printXTicsBottom(btics,painter,plotIdx,curvesRect);
    _printYTicsLeft(ltics,painter,plotIdx,curvesRect);
    _printYTicsRight(rtics,painter,plotIdx,curvesRect);
    painter->setPen(origPen);

    // Paint axis tic labels
    painter->setFont(fontTics);
    _printXTicLabels(xtl,painter,plotIdx,curvesRect);
    _printYTicLabels(ytl,painter,plotIdx,curvesRect);
    painter->setPen(origPen);

    // Paint axis labels
    painter->setFont(fontAxisLabel);
    _printXAxisLabel(xal,painter,plotIdx);
    _printYAxisLabel(yal,painter,plotIdx);
    painter->setPen(origPen);

    painter->setFont(origFont);
    painter->setPen(origPen);
    painter->restore();
}

void BookView::_printPlotTitle(const QRect &R,
                               QPainter *painter, const QModelIndex &plotIdx)
{
    QString s = _bookModel()->getDataString(plotIdx,"PlotTitle","Plot");
    QFontMetrics fm = painter->fontMetrics();
    QRect bbox = fm.boundingRect(s);
    int x = R.x() + R.width()/2 - bbox.width()/2;
    int q = (R.height()-bbox.height())/2;
    int y = R.y() + q + fm.ascent() - 1;
    painter->drawText(x,y,s);
}

void BookView::_printCurves(const QRect& R,
                            QPainter *painter, const QModelIndex &plotIdx)
{
    if ( !model() ) return;

    painter->save();
    painter->setClipRect(R);

    QModelIndex curvesIdx = _bookModel()->getIndex(plotIdx,"Curves","Plot");
    int nCurves = model()->rowCount(curvesIdx);

    // Print!
    if ( nCurves == 2 ) {
        QString plotPresentation = _bookModel()->getDataString(plotIdx,
                                                     "PlotPresentation","Plot");
        if ( plotPresentation == "compare" ) {
            _printCoplot(R,painter,plotIdx);
        } else if (plotPresentation == "error" || plotPresentation.isEmpty()) {
            _printErrorplot(R,painter,plotIdx);
        } else if ( plotPresentation == "error+compare" ) {
            _printErrorplot(R,painter,plotIdx);
            _printCoplot(R,painter,plotIdx);
        } else {
            fprintf(stderr,"koviz [bad scoobs]: printCurves() : pres=\"%s\" "
                           "not recognized.\n",
                           plotPresentation.toLatin1().constData());
            exit(-1);
        }
    } else {

        int nElements = 0;
        for ( int i = 0; i < nCurves; ++i ) {
            QModelIndex curveIdx = _bookModel()->index(i,0,curvesIdx);
            QPainterPath* path = _bookModel()->getCurvePainterPath(curveIdx);
            nElements += path->elementCount();
        }

        if ( nElements > 100000 || nCurves > 64 ) {

            // Use pixmaps to reduce file size
            double rw = R.width()/painter->device()->logicalDpiX();
            double rh = R.height()/painter->device()->logicalDpiY();
            QPixmap nullPixmap(1,1); // used for dpi
            QPainter nullPainter(&nullPixmap);
            int w = 2*qRound(rw)*nullPainter.device()->logicalDpiX();
            int h = 2*qRound(rh)*nullPainter.device()->logicalDpiY();
            QPixmap pixmap(w,h);
            QModelIndex pageIdx = plotIdx.parent().parent();
            pixmap.fill(_bookModel()->pageBackgroundColor(pageIdx));
            QPainter pixmapPainter(&pixmap);
            QPen pen;
            pen.setWidth(0);
            pixmapPainter.setRenderHint(QPainter::Antialiasing);

            QRectF M = _plotMathRect(plotIdx);
            double a = w/M.width();
            double b = h/M.height();
            double c = -a*M.x();
            double d = -b*M.y();
            QTransform T( a,    0,
                          0,    b,
                          c,    d);

            for ( int i = 0; i < nCurves; ++i ) {
                QModelIndex curveIdx = model()->index(i,0,curvesIdx);
                QPainterPath* path =_bookModel()->getCurvePainterPath(curveIdx);
                if ( path ) {
                    // Line color
                    QColor color(_bookModel()->getDataString(curveIdx,
                                                         "CurveColor","Curve"));
                    pen.setColor(color);
                    pixmapPainter.setPen(pen);

                    // Scale transform (e.g. for unit axis scaling)
                    double xs = _bookModel()->xScale(curveIdx);
                    double ys = _bookModel()->yScale(curveIdx);
                    double xb = _bookModel()->xBias(curveIdx);
                    double yb = _bookModel()->yBias(curveIdx);
                    QTransform Tscaled(T);
                    Tscaled = Tscaled.scale(xs,ys);
                    Tscaled = Tscaled.translate(xb/xs,yb/ys);
                    pixmapPainter.setTransform(Tscaled);

                    // Draw curve!
                    pixmapPainter.drawPath(*path);
                }
            }
            QRectF S(pixmap.rect());
            painter->drawPixmap(R,pixmap,S);
        } else {
            _printCoplot(R,painter,plotIdx);
        }
    }

    // Restore the painter state off the painter stack
    painter->restore();

    // Draw legend
    // If all plots on the page have the same legend, PageTitle will show legend
    if (!_bookModel()->isPlotLegendsSame(curvesIdx.parent().parent().parent())){
        _paintCurvesLegend(R,curvesIdx,*painter);
    }
}

void BookView::__paintSymbol(const QPointF &p,
                             const QString &symbol, QPainter& painter)
{
    QPen origPen = painter.pen();
    QPen pen = painter.pen();

    if ( symbol == "circle" ) {
        painter.drawEllipse(p,36,36);
    } else if ( symbol == "thick_circle" ) {
        pen.setWidth(18.0);
        painter.setPen(pen);
        painter.drawEllipse(p,32,32);
    } else if ( symbol == "solid_circle" ) {
        pen.setWidthF(18.0);
        painter.setPen(pen);
        painter.drawEllipse(p,24,24);
        painter.drawEllipse(p,12,12);
    } else if ( symbol == "square" ) {
        double x = p.x()-30.0;
        double y = p.y()-30.0;
        painter.drawRect(QRectF(x,y,60,60));
    } else if ( symbol == "thick_square") {
        pen.setWidthF(16.0);
        painter.setPen(pen);
        double x = p.x()-30.0;
        double y = p.y()-30.0;
        painter.drawRect(QRectF(x,y,60,60));
    } else if ( symbol == "solid_square" ) {
        pen.setWidthF(16.0);
        painter.setPen(pen);
        double x = p.x()-30.0;
        double y = p.y()-30.0;
        painter.drawRect(QRectF(x,y,60,60));
        pen.setWidthF(24.0);
        painter.setPen(pen);
        x = p.x()-12.0;
        y = p.y()-12.0;
        painter.drawRect(QRectF(x,y,24,24));
    } else if ( symbol == "star" ) { // *
        pen.setWidthF(12.0);
        painter.setPen(pen);
        double r = 36.0;
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
        pen.setWidthF(12.0);
        painter.setPen(pen);
        QPointF a(p.x()+24.0,p.y()+24.0);
        QPointF b(p.x()-24.0,p.y()+24.0);
        QPointF c(p.x()-24.0,p.y()-24.0);
        QPointF d(p.x()+24.0,p.y()-24.0);
        painter.drawLine(p,a);
        painter.drawLine(p,b);
        painter.drawLine(p,c);
        painter.drawLine(p,d);
    } else if ( symbol == "triangle" ) {
        double r = 48.0;
        pen.setJoinStyle(Qt::MiterJoin);
        painter.setPen(pen);
        QPointF a(p.x(),p.y()-r);
        QPointF b(p.x()-r*cos(30.0*M_PI/180.0),
                  p.y()+r*sin(30.0*M_PI/180.0));
        QPointF c(p.x()+r*cos(30.0*M_PI/180.0),
                  p.y()+r*sin(30.0*M_PI/180.0));
        QPolygonF triangle;
        triangle << a << b << c;
        painter.drawConvexPolygon(triangle);
    } else if ( symbol == "thick_triangle" ) {
        pen.setWidthF(24.0);
        pen.setJoinStyle(Qt::MiterJoin);
        painter.setPen(pen);
        double r = 48.0;
        QPointF a(p.x(),p.y()-r);
        QPointF b(p.x()-r*cos(30.0*M_PI/180.0),
                  p.y()+r*sin(30.0*M_PI/180.0));
        QPointF c(p.x()+r*cos(30.0*M_PI/180.0),
                  p.y()+r*sin(30.0*M_PI/180.0));
        QPolygonF triangle;
        triangle << a << b << c;
        painter.drawConvexPolygon(triangle);
    } else if ( symbol == "solid_triangle" ) {
        pen.setWidthF(36.0);
        pen.setJoinStyle(Qt::MiterJoin);
        painter.setPen(pen);
        double r = 36.0;
        QPointF a(p.x(),p.y()-r);
        QPointF b(p.x()-r*cos(30.0*M_PI/180.0),
                  p.y()+r*sin(30.0*M_PI/180.0));
        QPointF c(p.x()+r*cos(30.0*M_PI/180.0),
                  p.y()+r*sin(30.0*M_PI/180.0));
        QPolygonF triangle;
        triangle << a << b << c;
        painter.drawConvexPolygon(triangle);
    } else if ( symbol.startsWith("number_") && symbol.size() == 8 ) {
        QFont origFont = painter.font();
        QBrush origBrush = painter.brush();

        // Calculate bbox to draw text in
        QString number = symbol.right(1); // last char is '0'-'9'
        QFont font = painter.font();
        font.setPointSize(6);
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

        painter.setPen(origPen);
    }
}

void BookView::_printCoplot(const QRect& R,
                            QPainter *painter, const QModelIndex &plotIdx)
{
    QTransform T = _coordToDotTransform(R,plotIdx);

    double start = _bookModel()->getDataDouble(QModelIndex(),"StartTime");
    double stop = _bookModel()->getDataDouble(QModelIndex(),"StopTime");

    QList<QPainterPath*> paths;
    QModelIndex curvesIdx = _bookModel()->getIndex(plotIdx,"Curves","Plot");
    int rc = model()->rowCount(curvesIdx);
    for ( int i = 0; i < rc; ++i ) {

        QModelIndex curveIdx = model()->index(i,0,curvesIdx);
        QModelIndex curveDataIdx = _bookModel()->getDataIndex(curveIdx,
                                                          "CurveData","Curve");
        QVariant v = model()->data(curveDataIdx);
        CurveModel* curveModel =QVariantToPtr<CurveModel>::convert(v);

        if ( curveModel ) {

            double xs = _bookModel()->xScale(curveIdx);
            double ys = _bookModel()->yScale(curveIdx);
            double xb = _bookModel()->xBias(curveIdx);
            double yb = _bookModel()->yBias(curveIdx);

            QPainterPath* path = new QPainterPath;
            paths << path;

            curveModel->map();
            ModelIterator* it = curveModel->begin();

            bool isFirst = true;
            while ( !it->isDone() ) {

                if ( it->t() < start || it->t() > stop ) {
                    it->next();
                    continue;
                }

                QPointF p(it->x()*xs+xb,it->y()*ys+yb);
                p = T.map(p);

                if ( isFirst ) {
                    path->moveTo(p.x(),p.y());
                    isFirst = false;
                } else {
                    path->lineTo(p);
                }

                it->next();
            }
            delete it;

            // If curve is flat (constant), label with "Flatline=#"
            QRectF curveBBox = path->boundingRect();
            if ( curveBBox.height() == 0.0 ) {
                it = curveModel->begin();
                double y = it->y()*ys+yb;  // y is constant, so use first point
                delete it;
                QString s;
                s = s.sprintf("%.9g",y);
                QVariant v(s);
                double y2 = v.toDouble();
                double e = qAbs(y-y2);
                if ( e > 1.0e-9 ) {
                    // If %.9g loses too much accuracy, use %lf
                    s = s.sprintf("%.9lf",y);
                }
                s = QString("Flatline=%1").arg(s);
                int h = fontMetrics().height();
                QColor color( _bookModel()->getDataString(curveIdx,
                                                  "CurveColor","Curve"));
                QPen pen = painter->pen();
                pen.setColor(color);
                painter->setPen(pen);
                painter->drawText(curveBBox.topLeft()-QPointF(0,h+10),s);
            }

            curveModel->unmap();
        }
    }

    painter->save();
    painter->setRenderHint(QPainter::Antialiasing);

    QPen pen = painter->pen();

    if ( paths.size() > 20 ) {
        pen.setWidthF(8.0);  // smaller point size if many curves
    } else if ( paths.size() > 5 && paths.size() < 20 ) {
        pen.setWidthF(12.0);
    } else if ( paths.size() < 5 ) {
        pen.setWidthF(16.0);
    }

    int i = 0;
    foreach ( QPainterPath* path, paths ) {
        QModelIndex curveIdx = model()->index(i,0,curvesIdx);
        QColor color( _bookModel()->getDataString(curveIdx,
                                                  "CurveColor","Curve"));
        pen.setColor(color);
        pen.setDashPattern(_bookModel()->getLineStylePattern(curveIdx));
        painter->setPen(pen);
        painter->drawPath(*path);

        // Draw symbols
        QString symbolStyle = _bookModel()->getDataString(curveIdx,
                                               "CurveSymbolStyle", "Curve");
        symbolStyle = symbolStyle.toLower();
        if ( !symbolStyle.isEmpty() && symbolStyle != "none" ) {
            QVector<qreal> pattern;
            pen.setDashPattern(pattern); // plain lines for drawing symbols
            double w = pen.widthF();
            pen.setWidthF(8.0);
            painter->setPen(pen);
            QPointF pLast;
            for ( int i = 0; i < path->elementCount(); ++i ) {
                QPainterPath::Element el = path->elementAt(i);
                QPointF p(el.x,el.y);
                if ( i > 0 ) {
                    double r = 288.0;
                    double x = pLast.x()-r/2.0;
                    double y = pLast.y()-r/2.0;
                    QRectF R(x,y,r,r);
                    if ( R.contains(p) ) {
                        continue;
                    }
                }
                __paintSymbol(p,symbolStyle,*painter);

                pLast = p;
            }
            pen.setWidthF(w);
            painter->setPen(pen);
        }
        delete path;
        ++i;
    }
    painter->restore();
}

void BookView::_printErrorplot(const QRect& R,
                               QPainter *painter, const QModelIndex &plotIdx)
{
    QModelIndex curvesIdx = _bookModel()->getIndex(plotIdx,"Curves","Plot");

    QModelIndex curveIdx0 = model()->index(0,0,curvesIdx);
    QModelIndex curveIdx1 = model()->index(1,0,curvesIdx);

    QVariant v0 = model()->data(_bookModel()->
                                getDataIndex(curveIdx0,"CurveData","Curve"));
    QVariant v1 = model()->data(_bookModel()->
                                getDataIndex(curveIdx1,"CurveData","Curve"));
    CurveModel* c0 = QVariantToPtr<CurveModel>::convert(v0);
    CurveModel* c1 = QVariantToPtr<CurveModel>::convert(v1);

    if ( c0 == 0 || c1 == 0 ) {
        fprintf(stderr,"koviz [bad scoobs]:1: BookView::_printErrorplot()\n");
        exit(-1);
    }

    // Make list of scaled (e.g. unit) data points
    double start = _bookModel()->getDataDouble(QModelIndex(),"StartTime");
    double stop = _bookModel()->getDataDouble(QModelIndex(),"StopTime");
    QList<QPointF> pts;
    double k0 = _bookModel()->getDataDouble(curveIdx0,"CurveYScale","Curve");
    double k1 = _bookModel()->getDataDouble(curveIdx1,"CurveYScale","Curve");
    double ys0 = _bookModel()->yScale(curveIdx0);
    double ys1 = (k1/k0)*_bookModel()->yScale(curveIdx1);
    c0->map();
    c1->map();
    ModelIterator* i0 = c0->begin();
    ModelIterator* i1 = c1->begin();
    while ( !i0->isDone() && !i1->isDone() ) {
        double t0 = i0->t();
        double t1 = i1->t();
        if ( qAbs(t1-t0) < 0.000001 ) {
            if ( t0 >= start && t0 <= stop ) {
                double d = ys0*i0->y() - ys1*i1->y();
                pts << QPointF(t0,d);
            }
            i0->next();
            i1->next();
        } else {
            if ( t0 < t1 ) {
                i0->next();
            } else if ( t1 < t0 ) {
                i1->next();
            } else {
                fprintf(stderr,"koviz [bad scoobs]:2: _printErrorplot()\n");
                exit(-1);
            }
        }
    }
    delete i0;
    delete i1;
    c0->unmap();
    c1->unmap();


    // Create path from points
    QPainterPath path;
    if ( !pts.isEmpty() ) {
        QTransform T = _coordToDotTransform(R,plotIdx);
        bool isFirst = true;
        foreach ( QPointF p, pts ) {
            p = T.map(p);
            if ( isFirst ) {
                isFirst = false;
                path.moveTo(p);
            } else {
                path.lineTo(p);
            }
        }
    }

    painter->save();
    painter->setRenderHint(QPainter::Antialiasing);

    // Set pen color
    QPen origPen = painter->pen();
    QPen ePen(painter->pen());
    ePen.setWidthF(16.0);
    QRectF curveBBox = path.boundingRect();
    if (curveBBox.height() == 0.0 && !pts.isEmpty() && pts.first().y() == 0.0) {
        // Color green if error plot is flatline zero
        ePen.setColor(_bookModel()->createCurveColors(4).at(3)); // green
    } else {
        ePen.setColor(_bookModel()->errorLineColor());
    }
    painter->setPen(ePen);

    if ( curveBBox.height() == 0.0 && !pts.isEmpty() ) {
        // If curve is flat (constant), label with "Flatline=#"
        QString yval;
        if ( pts.at(0).y() == 0.0 ) {
            yval = yval.sprintf("Flatline=0.0");
        } else {
            yval = yval.sprintf("Flatline=%g",pts.at(0).y());
        }
        int h = fontMetrics().height();
        painter->drawText(curveBBox.topLeft()-QPointF(0,h+10),yval);
    }

    painter->drawPath(path);  // print!

    painter->setPen(origPen);
    painter->restore();
}


void BookView::_printXAxisLabel(const QRect& R,
                                QPainter *painter,
                                const QModelIndex &plotIdx)
{
    QString s = _bookModel()->getDataString(plotIdx,"PlotXAxisLabel","Plot");
    QModelIndex curvesIdx = _bookModel()->getIndex(plotIdx,"Curves", "Plot");
    QString u = _bookModel()->getCurvesXUnit(curvesIdx);
    s = s + " {" + u + "}";

    QFontMetrics fm = painter->fontMetrics();
    QRect bbox = fm.boundingRect(s);
    int x = R.x() + R.width()/2 - bbox.width()/2;
    int q = (R.height()-bbox.height())/2;
    int y = R.y() + q + fm.ascent() - 1;

    painter->drawText(x,y,s);
}

void BookView::_printXTicLabels(const QRect &R, QPainter *painter,
                                const QModelIndex &plotIdx,
                                const QRectF& curvesRect)
{
    QList<double> modelTics = _majorXTics(plotIdx);

    QTransform T = _coordToDotTransform(curvesRect,plotIdx);

    QList<LabelBox> boxes;
    foreach ( double tic, modelTics ) {
        QPointF center;
        center.setX(tic);
        center = T.map(center);
        center.setY(R.center().y());

        QString strVal = _format(tic);
        QRectF bb = painter->fontMetrics().boundingRect(strVal);
        bb.moveCenter(center);

        LabelBox box;
        box.center = center;
        box.bb = bb;
        box.strVal = strVal;
        boxes << box;
    }

    // Draw!
    painter->save();
    foreach ( LabelBox box, boxes ) {
        painter->drawText(box.bb,Qt::AlignHCenter|Qt::AlignVCenter,box.strVal);
    }
    painter->restore();
}

void BookView::_printYTicLabels(const QRect &R, QPainter *painter,
                                const QModelIndex &plotIdx,
                                const QRectF& curvesRect)
{
    QList<double> modelTics = _majorYTics(plotIdx);

    QTransform T = _coordToDotTransform(curvesRect,plotIdx);

    QList<LabelBox> boxes;
    foreach ( double tic, modelTics ) {
        QPointF center;
        center.setY(tic);
        center = T.map(center);
        center.setX(R.center().x());

        QString strVal = _format(tic);
        QRectF bb = painter->fontMetrics().boundingRect(strVal);
        bb.moveCenter(center);

        LabelBox box;
        box.center = center;
        box.bb = bb;
        box.strVal = strVal;
        boxes << box;
    }

    // Draw!
    painter->save();
    foreach ( LabelBox box, boxes ) {
        painter->drawText(box.bb,Qt::AlignHCenter|Qt::AlignVCenter,box.strVal);
    }
    painter->restore();
}

QString BookView::_format(double tic) const
{
    QString s;
    if ( qAbs(tic) < 1.0e-16 ) {
        tic = 0.0;
    }
    s = s.sprintf("%g", tic);
    return s;
}

void BookView::_printXTicsBottom(const QRect &R,
                                 QPainter *painter,
                                 const QModelIndex &plotIdx,
                                 const QRect& curvesRect)
{
    int q = painter->pen().width();

    // Axis line
    QPoint p1 = R.bottomLeft();
    int y = p1.y()-q/2;
    p1.setX(p1.x()+q/2);
    p1.setY(y);
    QPoint p2 = R.bottomRight();
    p2.setX(p2.x()-q/2);
    p2.setY(y);
    painter->drawLine(p1,p2);

    QPen origPen = painter->pen();
    QPen pen = painter->pen();
    pen.setWidth(q/2);
    painter->setPen(pen);
    QTransform T = _coordToDotTransform(curvesRect,plotIdx);
    QList<double> xtics = _majorXTics(plotIdx);
    foreach ( double x, xtics ) {
        QPointF a = T.map(QPointF(x,0));
        a.setY(R.top()+q/2);
        QPointF b(a);
        b.setY(R.bottom()-q/2);
        painter->drawLine(a,b);
    }
    painter->setPen(origPen);

}

void BookView::_printXTicsTop(const QRect &R, QPainter *painter,
                              const QModelIndex &plotIdx,
                              const QRect &curvesRect)
{
    int q = painter->pen().width();

    // Axis line
    QPoint p1 = R.topLeft();
    int y = p1.y()+q/2;
    p1.setX(p1.x()+q/2);
    p1.setY(y);
    QPoint p2 = R.topRight();
    p2.setX(p2.x()-q/2);
    p2.setY(y);
    painter->drawLine(p1,p2);

    QPen origPen = painter->pen();
    QPen pen = painter->pen();
    pen.setWidth(q/2);
    painter->setPen(pen);
    QTransform T = _coordToDotTransform(curvesRect,plotIdx);
    QList<double> xtics = _majorXTics(plotIdx);
    foreach ( double x, xtics ) {
        QPointF a = T.map(QPointF(x,0));
        QPointF b(a);
        a.setY(R.bottom());
        b.setY(y);
        painter->drawLine(a,b);
    }
    painter->setPen(origPen);
}

void BookView::_printYTicsLeft(const QRect &R,
                               QPainter *painter, const QModelIndex &plotIdx,
                               const QRect &curvesRect)
{
    int q = painter->pen().width();

    // Axis line
    QPoint p1 = R.topLeft();
    p1.setX(p1.x()+q/2);
    p1.setY(p1.y()+q/2);
    QPoint p2 = R.bottomLeft();
    p2.setX(p2.x()+q/2);
    p2.setY(p2.y()-q/2);
    painter->drawLine(p1,p2);

    // Y-tics
    QPen origPen = painter->pen();
    QPen pen = painter->pen();
    pen.setWidth(q/2);
    painter->setPen(pen);
    QTransform T = _coordToDotTransform(curvesRect,plotIdx);
    QList<double> ytics = _majorYTics(plotIdx);
    foreach ( double y, ytics ) {
        QPointF pt1 = T.map(QPointF(0,y));
        QPointF pt2(pt1);
        pt1.setX(R.left()+q/2);
        pt2.setX(R.right()-q/2);
        painter->drawLine(pt1,pt2);
    }
    painter->setPen(origPen);
}

void BookView::_printYTicsRight(const QRect &R,
                                QPainter *painter, const QModelIndex &plotIdx,
                                const QRect &curvesRect)
{
    int q = painter->pen().width();

    // Axis line
    QPoint p1 = R.topRight();
    p1.setX(p1.x()-q/2);
    p1.setY(p1.y()+q/2);
    QPoint p2 = R.bottomRight();
    p2.setX(p2.x()-q/2);
    p2.setY(p2.y()-q/2);
    painter->drawLine(p1,p2);

    QPen origPen = painter->pen();
    QPen pen = painter->pen();
    pen.setWidth(q/2);
    painter->setPen(pen);
    QTransform T = _coordToDotTransform(curvesRect,plotIdx);
    QList<double> ytics = _majorYTics(plotIdx);
    foreach ( double y, ytics ) {
        QPointF a = T.map(QPointF(0,y));
        a.setX(R.right()-q/2);
        QPointF b(a);
        b.setX(R.left()+q/2);
        painter->drawLine(a,b);
    }
    painter->setPen(origPen);
}

void BookView::_printTopLeftCorner(const QRect &R,
                                   QPainter *painter,const QModelIndex &plotIdx)
{
    Q_UNUSED(plotIdx);

    int q = painter->pen().width();

    // Vertical axis line
    QPoint p1 = R.topLeft();
    p1.setX(p1.x()+q/2);
    p1.setY(p1.y()+q/2);
    QPoint p2 = R.bottomLeft();
    p2.setX(p2.x()+q/2);
    p2.setY(p2.y()-q/2);
    painter->drawLine(p1,p2);

    // Horizontal axis line
    p1 = R.topLeft();
    p1.setX(p1.x()+q/2);
    p1.setY(p1.y()+q/2);
    p2 = R.topRight();
    p2.setX(p2.x()-q/2);
    p2.setY(p2.y()+q/2);
    painter->drawLine(p1,p2);
}

void BookView::_printTopRightCorner(const QRect &R,
                                    QPainter *painter,
                                    const QModelIndex &plotIdx)
{
    Q_UNUSED(plotIdx);

    int q = painter->pen().width();

    // Vertical axis line
    QPoint p1 = R.topRight();
    p1.setX(p1.x()-q/2);
    p1.setY(p1.y()+q/2);
    QPoint p2 = R.bottomRight();
    p2.setX(p2.x()-q/2);
    p2.setY(p2.y()-q/2);
    painter->drawLine(p1,p2);

    // Horizontal axis line
    p1 = R.topLeft();
    p1.setX(p1.x()+q/2);
    p1.setY(p1.y()+q/2);
    p2 = R.topRight();
    p2.setX(p2.x()-q/2);
    p2.setY(p2.y()+q/2);
    painter->drawLine(p1,p2);
}

void BookView::_printBottomRightCorner(const QRect &R,
                                       QPainter *painter,
                                       const QModelIndex &plotIdx)
{
    Q_UNUSED(plotIdx);

    int q = painter->pen().width();

    // Vertical axis line
    QPoint p1 = R.topRight();
    p1.setX(p1.x()-q/2);
    p1.setY(p1.y()+q/2);
    QPoint p2 = R.bottomRight();
    p2.setX(p2.x()-q/2);
    p2.setY(p2.y()-q/2);
    painter->drawLine(p1,p2);

    // Horizontal axis line
    p1 = R.bottomLeft();
    p1.setX(p1.x()+q/2);
    p1.setY(p1.y()-q/2);
    p2 = R.bottomRight();
    p2.setX(p2.x()-q/2);
    p2.setY(p2.y()-q/2);
    painter->drawLine(p1,p2);
}

void BookView::_printBottomLeftCorner(const QRect &R,
                                      QPainter *painter,
                                      const QModelIndex &plotIdx)
{
    Q_UNUSED(plotIdx);

    int q = painter->pen().width();

    // Vertical axis line
    QPoint p1 = R.topLeft();
    p1.setX(p1.x()+q/2);
    p1.setY(p1.y()+q/2);
    QPoint p2 = R.bottomLeft();
    p2.setX(p2.x()+q/2);
    p2.setY(p2.y()-q/2);
    painter->drawLine(p1,p2);

    // Horizontal axis line
    p1 = R.bottomLeft();
    p1.setX(p1.x()+q/2);
    p1.setY(p1.y()-q/2);
    p2 = R.bottomRight();
    p2.setX(p2.x()-q/2);
    p2.setY(p2.y()-q/2);
    painter->drawLine(p1,p2);
}

void BookView::_printYAxisLabel(const QRect& R,
                                 QPainter *painter,
                                 const QModelIndex &plotIdx)
{
    QString s = _bookModel()->getDataString(plotIdx,"PlotYAxisLabel","Plot");
    QModelIndex curvesIdx = _bookModel()->getIndex(plotIdx,"Curves", "Plot");
    QString u = _bookModel()->getCurvesYUnit(curvesIdx);
    s = s + " {" + u + "}";

    QFontMetrics fm = painter->fontMetrics();
    s = fm.elidedText(s, Qt::ElideLeft, R.height());
    QRect bbox = fm.boundingRect(s);

    // Draw!
    painter->save();
    painter->translate(R.x()+ R.width()-fm.descent()-fm.leading(),
                       R.y()+R.height()/2+bbox.width()/2);
    painter->rotate(270);
    painter->drawText(0,0,s);
    painter->restore();
}

void BookView::_nbCloseRequested(int tabId)
{
    if ( model() == 0 ) return;

    QModelIndex pageIdx = _tabIdToModelIdx(tabId);
    model()->removeRow(pageIdx.row(),pageIdx.parent()); // rowsAboutToBeRemoved deletes page
}

void BookView::_pageViewCurrentChanged(const QModelIndex &currIdx,
                                       const QModelIndex &prevIdx)
{
    Q_UNUSED(prevIdx);
    selectionModel()->setCurrentIndex(currIdx,QItemSelectionModel::NoUpdate);
}

void BookView::dataChanged(const QModelIndex &topLeft,
                           const QModelIndex &bottomRight)
{
    Q_UNUSED(topLeft);
    Q_UNUSED(bottomRight);
}

void BookView::rowsInserted(const QModelIndex &pidx, int start, int end)
{
    if (  model()->data(pidx).toString() != "Page" &&
          model()->data(pidx).toString() != "Table" ) {
        return;
    }

    for ( int i = start; i <= end; ++i ) {
        QModelIndex idx = model()->index(i,0,pidx);
        QString cText = model()->data(idx).toString();

        if ( cText == "PageName" ) {
            PageView* pageView = new PageView;
            _childViews << pageView;
            pageView->setModel(model());
            pageView->setRootIndex(idx.parent());
            connect(pageView->selectionModel(),
                    SIGNAL(currentChanged(QModelIndex,QModelIndex)),
                    this,
                    SLOT(_pageViewCurrentChanged(QModelIndex,QModelIndex)));
            int tabId = _nb->addTab(pageView,"Page");
            QString pageName = _bookModel()->getDataString(pidx,
                                                           "PageName","Page");
            QString shortName = pageName.split(":").at(0);
            QFileInfo fi(shortName);
            shortName = fi.fileName();
            _nb->setTabToolTip(tabId,pageName);
            _nb->setTabText(tabId,shortName);
            _nb->setTabWhatsThis(tabId, "Page");
        } else if ( cText == "TableName") {
            TablePageView* tablePageView = new TablePageView;
            _childViews << tablePageView;
            tablePageView->setModel(model());
            tablePageView->setRootIndex(idx.parent());
            int tabId = _nb->addTab(tablePageView,"Table");
            QString tableName = _bookModel()->getDataString(pidx,
                                                           "TableName","Table");
            QString shortName = tableName.split(":").at(0) + ".table";
            QFileInfo fi(shortName);
            shortName = fi.fileName();
            _nb->setTabToolTip(tabId,tableName);
            _nb->setTabText(tabId,shortName);
            _nb->setTabWhatsThis(tabId, "Table");
        }
    }
}

void BookView::rowsAboutToBeRemoved(const QModelIndex &pidx, int start, int end)
{
    if ( start != end ) {
        fprintf(stderr,"koviz [bad scoobs]:1:BookView::rowsAboutToBeRemoved(): "
                       "TODO: support deleting multiple rows at once.\n");
        exit(-1);
    }
    if ( start < 0 || start >= _childViews.size() ) {
        fprintf(stderr,"koviz [bad scoobs]:2:BookView::rowsAboutToBeRemoved(): "
                       "childViews list not in sync with model.\n");
        exit(-1);
    }

    QModelIndex idx = model()->index(start,0,pidx);

    int tabId = _modelIdxToTabId(idx);
    QWidget* widget = _nb->widget(tabId);
    _nb->removeTab(tabId);

    foreach ( QAbstractItemView* view, _childViews ) {
        if ( view == widget ) {
            disconnect(view,0,0,0);
            _childViews.removeOne(view);
            delete view;
            break;
        }
    }
}
