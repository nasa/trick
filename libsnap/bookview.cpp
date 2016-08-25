#include "bookview.h"

#include "libsnap/timeit_linux.h"
static TimeItLinux timer;

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
    connect(_nb, SIGNAL(currentChanged(int)),
            this,SLOT(_nbCurrentChanged(int)));

    _mainLayout->addWidget(_nb);

    setLayout(_mainLayout);
}


void BookView::currentChanged(const QModelIndex &current,
                              const QModelIndex &previous)
{
    Q_UNUSED(previous);
    if ( _bookModel()->isIndex(current,"Page") ) {
         _nb->setCurrentIndex(current.row());
    }
}

void BookView::selectionChanged(const QItemSelection &selected,
                                const QItemSelection &deselected)
{
    Q_UNUSED(selected);
    Q_UNUSED(deselected);
}

int BookView::_pageIdxToTabId(const QModelIndex &pageIdx)
{
    int tabId = -1;

    QString pageName = _bookModel()->getDataString(pageIdx,"PageName","Page");

    int nTabs = _nb->count();
    for ( int i = 0; i < nTabs; ++i ) {

        QString tabToolTip = _nb->tabToolTip(i);
        QString wt = _nb->tabWhatsThis(i);
        if ( wt == "Page" ) {
            if ( pageName == tabToolTip ) {
                tabId = i;
                break;
            }
        } else {
            qDebug() << "snap [bad scoobs]: BookView::_pageIdxToTabId()";
            exit(-1);
        }
    }

    return tabId;
}

QModelIndex BookView::_tabIdToPageIdx(int tabId)
{
    QModelIndex idx;

    QString tabToolTip = _nb->tabToolTip(tabId);
    QString wt = _nb->tabWhatsThis(tabId);
    if ( wt == "Page" ) {
        QModelIndex pagesIdx = _bookModel()->getIndex(QModelIndex(), "Pages");
        int row = -1;
        int rc = model()->rowCount(pagesIdx);
        for ( int i = 0; i < rc ; ++i ) {
            QModelIndex pageIdx = model()->index(i,0,pagesIdx);
            QString pageName = _bookModel()->getDataString(pageIdx,
                                                           "PageName","Page");
            if ( pageName == tabToolTip ) {
                row = i;
                idx = model()->index(row,0,pagesIdx);
                break;
            }
        }
        if ( row < 0 ) {
            qDebug() << "snap [bad scoobs]:1: BookView::_tabIdToPageIdx() "
                        "Could not find page using tabId=" << tabId;
            exit(-1);
        }
    } else {
        qDebug() << "snap [bad scoobs]:2: BookView::_tabIdToPageIdx()";
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
    QModelIndex pagesIdx = _bookModel()->getIndex(QModelIndex(),"Pages");
    int pageCnt = model()->rowCount(pagesIdx);

    //
    // Setup printer
    //
    QPrinter printer(QPrinter::HighResolution);
    printer.setCreator("Snap!");
    printer.setDocName(fname);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setPageSize(QPrinter::Letter);
    printer.setOutputFileName(fname);
    printer.setOrientation(QPrinter::Landscape);

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
    QPen pen(QColor(Qt::black));
    double pointSize = printer.logicalDpiX()/72.0;
    pen.setWidthF(pointSize);
    painter.setPen(pen);

    //
    // Print pages
    //
    for ( int i = 0; i < pageCnt; ++i) {

        QModelIndex pageIdx = model()->index(i,0,pagesIdx);
        _printPage(&painter,pageIdx);

        // Insert new page in pdf booklet
        if ( i < pageCnt-1 ) {
            printer.newPage();
        }
    }

    //
    // End printing
    //
    painter.restore();
    painter.end();
}

void BookView::_printPage(QPainter *painter, const QModelIndex &pageIdx)
{
    QPaintDevice* paintDevice = painter->device();
    if ( !paintDevice ) return;

    painter->save();

    QString family("Liberation Serif");
    QFontDatabase fontdb;
    foreach ( QString s, fontdb.families() ) {
        if ( s == family ) {
            painter->setFont(family);
            break;
        }
    }

    int margin = 96;

    QRect pageTitleRect = _printPageTitle(painter,pageIdx);

    QModelIndex plotsIdx = _bookModel()->getIndex(pageIdx,"Plots", "Page");

    int nPlots = model()->rowCount(plotsIdx);

    QRect R(pageTitleRect.bottomLeft().x(),
            pageTitleRect.bottomLeft().y(),
            paintDevice->width(),
            paintDevice->height()-pageTitleRect.height());
    R.setTop(R.top()+margin); // 96 dot margin between page title box and plots

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
                   //q,0,0,0,0,0,  // margins
                   q,0,0,0,24,24,48,  // margins
                   titleFontSize,
                   axisLabelFontSize, ticLabelFontSize,
                   axisLinePtSize, ticWidth, ticHeight);
        QRect tlr(rr);

        r.moveTo(r.bottomLeft());
        rr = r; rr.setRight(r.right()-margin);
        _printPlot(rr,painter,model()->index(1,0,plotsIdx),
                   q,0,0,0,24,24,48,
                   titleFontSize,
                   axisLabelFontSize, ticLabelFontSize,
                   axisLinePtSize, ticWidth, ticHeight);

        r.moveTo(r.bottomLeft());
        rr = r; rr.setRight(r.right()-margin);
        _printPlot(rr,painter,model()->index(2,0,plotsIdx),
                   q,0,0,0,24,24,48,
                   titleFontSize,
                   axisLabelFontSize, ticLabelFontSize,
                   axisLinePtSize, ticWidth, ticHeight);

        r.moveTo(tlr.topRight());
        rr = r; rr.moveLeft(r.left()+margin);
        _printPlot(rr,painter,model()->index(3,0,plotsIdx),
                   q,0,0,0,24,24,48,
                   titleFontSize,
                   axisLabelFontSize, ticLabelFontSize,
                   axisLinePtSize, ticWidth, ticHeight);

        r.moveTo(r.bottomLeft());
        rr = r; rr.moveLeft(r.left()+margin);
        _printPlot(rr,painter,model()->index(4,0,plotsIdx),
                   q,0,0,0,24,24,48,
                   titleFontSize,
                   axisLabelFontSize, ticLabelFontSize,
                   axisLinePtSize, ticWidth, ticHeight);

        r.moveTo(r.bottomLeft());
        rr = r; rr.moveLeft(r.left()+margin);
        _printPlot(rr,painter,model()->index(5,0,plotsIdx),
                   q,0,0,0,24,24,48,
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
        qDebug() << "snap [error]: _printPage() : cannot print more than "
                    "7 plots on a page.";
        exit(-1);
    }

    painter->restore();
}

// Returns a bounding box of page title
QRect BookView::_printPageTitle(QPainter* painter, const QModelIndex &pageIdx)
{
    //
    // Init
    //
    QFont origFont = painter->font();
    QPaintDevice* paintDevice = painter->device();
    QRect R(0,0,paintDevice->width(),paintDevice->height());

    //
    // Get title strings from model
    //
    QModelIndex title1Idx = _bookModel()->getDataIndex(pageIdx,
                                                       "PageTitle", "Page");
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
    if ( t1 != "Snap Plots!" ) {
        // Default title overwritten by -t1 optional title
        title1 = t1;
    }
    QString title2 = model()->data(title2Idx).toString();
    QString title3 = model()->data(title3Idx).toString();
    QString title4 = model()->data(title4Idx).toString();


    // Draw main title
    //
    QFont font(origFont);
    font.setPointSize(18);
    painter->setFont(font);
    QFontMetrics fm1(font,paintDevice);
    int margin = paintDevice->logicalDpiX()/8.0; // 1/8th inch
    int w = fm1.boundingRect(title1).width();
    int x = (R.width()-w)/2;
    int y = margin + fm1.ascent();
    if ( !title1.isEmpty() ) {
        painter->drawText(x,y,title1);
        y += fm1.lineSpacing();
    }

    //
    // Draw subtitle with RUNs
    //
    if ( !title2.isEmpty() ) {
        font.setPointSize(12);
        painter->setFont(font);
        QFontMetrics fm2(font,paintDevice);
        QStringList lines = title2.split('\n', QString::SkipEmptyParts);
        if ( lines.size() == 1 ) {
            // single RUN
            x = (R.width()-fm2.boundingRect(title2).width())/2;
            painter->drawText(x,y,title2);
        } else if ( lines.size() > 1 ) {
            // multiple RUNs (show two RUNs and elide rest with elipsis)
            QString s1 = lines.at(0);
            QString s2 = lines.at(1);
            if ( lines.size() > 2 ) {
                if ( !s2.endsWith(',') ) {
                    s2 += ",";
                }
                s2 += "...)";
            }
            QString s = s1 + " " + s2;
            w = fm2.boundingRect(s).width();
            if ( w < 0.6*R.width() ) {
                // print on single line
                x = (R.width()-w)/2;
                painter->drawText(x,y,s);
            } else {
                // print on two lines
                QString s;
                if ( s1.size() > s2.size() ) {
                    x = (R.width()-fm2.boundingRect(s1).width())/2;
                } else {
                    x = (R.width()-fm2.boundingRect(s2).width())/2;
                }
                painter->drawText(x,y,s1);
                if ( s1.startsWith('(') ) {
                    x += fm2.width('(');
                }
                y += fm2.lineSpacing();
                painter->drawText(x,y,s2);
            }
        }
        y += fm2.lineSpacing();
    }

    //
    // Draw title3 and title4 (align on colon if possible)
    //
    font.setPointSize(10);
    painter->setFont(font);
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
        if ( s32.size() > s42.size() ) {
            w = fm3.boundingRect(s32).width();
        } else {
            w = fm3.boundingRect(s42).width();
        }
        x = R.width()-w-margin;
        painter->drawText(x,y,s32);
        y += fm3.lineSpacing();
        painter->drawText(x,y,s42);
        x -= fm3.boundingRect(" : ").width();
        y -= fm3.lineSpacing();
        painter->drawText(x,y," : ");
        y += fm3.lineSpacing();
        painter->drawText(x,y," : ");
        x -= fm3.boundingRect(s31).width();
        y -= fm3.lineSpacing();
        painter->drawText(x,y,s31);
        x += fm3.boundingRect(s31).width();
        x -= fm3.boundingRect(s41).width();
        y += fm3.lineSpacing();
        painter->drawText(x,y,s41);

    } else {
        // title3,title4 are custom (i.e. not colon delimited user/date)
        x = R.width() - fm3.boundingRect(title3).width() - margin;
        painter->drawText(x,y,title3);
        x = R.width() - fm3.boundingRect(title4).width() - margin;
        y += fm3.lineSpacing();
        painter->drawText(x,y,title4);
    }

    QRect bbox(0,0,R.width(),y+margin);
    QPen pen;
    pen.setWidth(4);
    painter->setPen(pen);
    //painter->drawRect(0,0,bbox.width(),bbox.height());

    painter->setFont(origFont);
    return bbox;
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

    int ytlw = 200; // TODO: Calculate!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

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
        if ( plotPresentation == "coplot" ) {
            _printCoplot(R,painter,plotIdx);
        } else if (plotPresentation == "error" || plotPresentation.isEmpty()) {
            _printErrorplot(R,painter,plotIdx);
        } else if ( plotPresentation == "error+coplot" ) {
            _printErrorplot(R,painter,plotIdx);
            _printCoplot(R,painter,plotIdx);
        } else {
            qDebug() << "snap [bad scoobs]: printCurves() : PlotPresentation="
                     << plotPresentation << "not recognized.";
            exit(-1);
        }
    } else {
        _printCoplot(R,painter,plotIdx);
    }

    // Restore the painter state off the painter stack
    painter->restore();
}

void BookView::_printCoplot(const QRect& R,
                            QPainter *painter, const QModelIndex &plotIdx)
{
    QTransform T = _coordToDotTransform(R,plotIdx);

    QList<QPainterPath*> paths;
    QRectF bbox;  // TODO: this has helped debugging but not needed
    QModelIndex curvesIdx = _bookModel()->getIndex(plotIdx,"Curves","Plot");
    int rc = model()->rowCount(curvesIdx);
    for ( int i = 0; i < rc; ++i ) {

        QModelIndex curveIdx = model()->index(i,0,curvesIdx);
        QModelIndex curveDataIdx = _bookModel()->getDataIndex(curveIdx,
                                                          "CurveData","Curve");
        QVariant v = model()->data(curveDataIdx);
        TrickCurveModel* curveModel =QVariantToPtr<TrickCurveModel>::convert(v);

        if ( curveModel ) {

            double ys = _yScale(curveModel,curveIdx);

            QPainterPath* path = new QPainterPath;
            paths << path;

            curveModel->map();
            TrickModelIterator it = curveModel->begin();
            const TrickModelIterator e = curveModel->end();

            QList<QPointF> pts;

            while (it != e) {

                QPointF p(it.x(),it.y()*ys);
                p = T.map(p);

                if ( pts.size() == 0 ) {
                    path->moveTo(p.x(),p.y()); // for first point only
                } else if ( pts.size() >= 2 ) {
                    QVector2D v0(pts.at(1)-pts.at(0));
                    QVector2D v1(p-pts.at(0));
                    v0.normalize();
                    v1.normalize();
                    double dot = QVector2D::dotProduct(v0,v1);
                    double ang = 180*acos(dot)/M_PI;
                    double dx = p.x()-pts.at(0).x();
                    double dy = p.y()-pts.at(0).y();
                    double d = sqrt(dx*dx+dy*dy);

                    // TODO!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! DO THIS!!!!
                    if ( ang > 60 || d > 0 ) {
                        // fuzzily non-colinear or
                        // distance between p0 and p over 1/6th inch
                        path->lineTo(pts.last());
                        pts.clear();
                    }
                }

                pts << p;

                ++it;
            }
            if ( !pts.isEmpty() ) {
                foreach ( QPointF p, pts ) {
                    path->lineTo(p);
                }
            }
            curveModel->unmap();

            QRectF curveBBox = path->boundingRect();
            bbox = bbox.united(curveBBox);
        }
    }

    painter->save();
    painter->setRenderHint(QPainter::Antialiasing);

    QList<QColor> colors = _bookModel()->createCurveColors(paths.size());
    QPen pen = painter->pen();

    int i = 0;
    foreach ( QPainterPath* path, paths ) {
        pen.setColor(colors.at(i));
        painter->setPen(pen);
        painter->drawPath(*path);
        delete path;
        ++i;
    }
    //painter->drawPath(*path2);
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
    TrickCurveModel* c0 = QVariantToPtr<TrickCurveModel>::convert(v0);
    TrickCurveModel* c1 = QVariantToPtr<TrickCurveModel>::convert(v1);

    if ( c0 == 0 || c1 == 0 ) {
        qDebug() << "snap [bad scoobies]:1: BookView::_printErrorplot()";
        exit(-1);
    }

    // Make list of scaled (e.g. unit) data points
    QList<QPointF> pts;
    double k0 = _bookModel()->getDataDouble(curveIdx0,"CurveYScale","Curve");
    double k1 = _bookModel()->getDataDouble(curveIdx1,"CurveYScale","Curve");
    double ys0 = _yScale(c0,curveIdx0);
    double ys1 = (k1/k0)*_yScale(c1,curveIdx0); // curveIdx0 for same unit
    c0->map();                                  // k1/k0 to correct book ysf
    c1->map();
    TrickModelIterator i0 = c0->begin();
    TrickModelIterator i1 = c1->begin();
    const TrickModelIterator e0 = c0->end();
    const TrickModelIterator e1 = c1->end();
    while (i0 != e0 && i1 != e1) {
        double t0 = i0.t();
        double t1 = i1.t();
        if ( qAbs(t1-t0) < 0.000001 ) {
            double d = ys0*i0.y() - ys1*i1.y();
            pts << QPointF(t0,d);
            ++i0;
            ++i1;
        } else {
            if ( t0 < t1 ) {
                ++i0;
            } else if ( t1 < t0 ) {
                ++i1;
            } else {
                qDebug() << "snap [bad scoobs]:1: _printErrorplot()";
                exit(-1);
            }
        }
    }
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

    // Print!
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing);
    QPen magentaPen(_bookModel()->createCurveColors(3).at(2));
    painter->setPen(magentaPen);
    painter->drawPath(path);
    painter->restore();
}


void BookView::_printXAxisLabel(const QRect& R,
                                QPainter *painter,
                                const QModelIndex &plotIdx)
{
    QString s = _bookModel()->getDataString(plotIdx,"PlotXAxisLabel","Plot");

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
    QFontMetrics fm = painter->fontMetrics();
    s = fm.elidedText(s, Qt::ElideLeft, R.height());
    QRect bbox = fm.boundingRect(s);

    // Draw!
    painter->save();
    painter->translate(R.x()+R.width()/2, R.y()+R.height()/2+bbox.width()/2);
    painter->rotate(270);
    painter->drawText(0,0,s);
    painter->restore();
}

void BookView::_nbCloseRequested(int tabId)
{
    if ( model() == 0 ) return;

    QModelIndex pageIdx = _tabIdToPageIdx(tabId);
    model()->removeRow(pageIdx.row(),pageIdx.parent()); // rowsAboutToBeRemoved deletes page
}

void BookView::_nbCurrentChanged(int tabId)
{
    if ( selectionModel() ) {
        QModelIndex pagesIdx = _bookModel()->getIndex(QModelIndex(),"Pages");
        QModelIndex pageIdx = model()->index(tabId, 0,pagesIdx);
        selectionModel()->setCurrentIndex(pageIdx,
                                          QItemSelectionModel::NoUpdate);
    }
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
    if ( model()->data(pidx).toString() != "Pages" &&
         model()->data(pidx).toString() != "Page" ) return;

    for ( int i = start; i <= end; ++i ) {
        QModelIndex idx = model()->index(i,0,pidx);
        QString cText = model()->data(idx).toString();
        if ( cText == "Page" ) {
            PageView* pageView = new PageView;
            _childViews << pageView;
            pageView->setModel(model());
            pageView->setRootIndex(idx);
            connect(pageView->selectionModel(),
                    SIGNAL(currentChanged(QModelIndex,QModelIndex)),
                    this,
                    SLOT(_pageViewCurrentChanged(QModelIndex,QModelIndex)));
            int tabId = _nb->addTab(pageView,"Page");
            _nb->setTabWhatsThis(tabId, "Page");
        } else if ( cText == "PageName" ) {
            QModelIndex pageNameIdx = _bookModel()->index(i,1,pidx);
            QString pageName = model()->data(pageNameIdx).toString();
            QFileInfo fi(pageName);
            QString fName = fi.fileName();
            int row = pidx.row();
            _nb->setTabToolTip(row,pageName);
            _nb->setTabText(row,fName);
        }
    }
}

void BookView::rowsAboutToBeRemoved(const QModelIndex &pidx, int start, int end)
{
    if ( start != end ) {
        qDebug() << "snap [bad scoobs]:1: BookView::rowsAboutToBeRemoved(): "
                    "TODO: support deleting multiple rows at once.";
        exit(-1);
    }
    if ( start < 0 || start >= _childViews.size() ) {
        qDebug() << "snap [bad scoobs]:2: BookView::rowsAboutToBeRemoved(): "
                    "childViews list not in sync with model.";
        exit(-1);
    }

    QModelIndex pageIdx = model()->index(start,0,pidx);
    if ( model()->data(pageIdx).toString() != "Page" ) {
        qDebug() << "snap [bad scoobs]:3 BookView::rowsAboutToBeRemoved(): "
                    "page deletion support only!!!";
    }

    int tabId = _pageIdxToTabId(pageIdx);
    _nb->removeTab(tabId);

    QAbstractItemView* pageView = _childViews.at(start);
    disconnect(pageView,0,0,0);
    _childViews.removeAt(start);
    delete pageView;
}
