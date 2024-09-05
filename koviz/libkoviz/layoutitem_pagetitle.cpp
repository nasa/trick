#include "layoutitem_pagetitle.h"

PageTitleLayoutItem::PageTitleLayoutItem(PlotBookModel* bookModel,
                                         const QModelIndex &pageIdx,
                                         const QFont& font) :
    QLayoutItem(),
    _bookModel(bookModel),
    _pageIdx(pageIdx),
    _font(font)
{
}

PageTitleLayoutItem::~PageTitleLayoutItem()
{
}

Qt::Orientations PageTitleLayoutItem::expandingDirections() const
{
    return Qt::Horizontal;
}

QRect PageTitleLayoutItem::geometry() const
{
    return _rect;
}

bool PageTitleLayoutItem::isEmpty() const
{
    return true;
}

QSize PageTitleLayoutItem::maximumSize() const
{
    return QSize(1000000,1000000);
}

QSize PageTitleLayoutItem::minimumSize() const
{
    return QSize(0,0);
}

void PageTitleLayoutItem::setGeometry(const QRect &r)
{
    _rect = r;
}

QSize PageTitleLayoutItem::sizeHint() const
{
    QSize size;

    bool isShowPageTitle = _bookModel->getDataBool(QModelIndex(),
                                                   "IsShowPageTitle","");
    if ( !isShowPageTitle ) {
        // If there is no page title, empty title is 10 pixels high
        size.setHeight(10);
        return size;
    }

    QFont font(_font);
    font.setPointSize(14); // title1
    QFontMetrics fm1(font);
    font.setPointSize(11); // title2
    QFontMetrics fm2(font);
    font.setPointSize(8);  // legend font
    QFontMetrics fml(font);

    int hl = _sizeLegend().height();

    int h = 0;
    h = fm1.xHeight();      // Top Margin
    h += fm1.lineSpacing();     // Bottom of Title1
    h += fm2.lineSpacing();     // Bottom of Title2
    h += fm1.xHeight();
    if ( hl > 0 ) {
        h += hl;                 // Legend
        h += fml.xHeight();  // Margin below legend
    }

    size.setHeight(h);

    return size;
}

void PageTitleLayoutItem::paint(QPainter *painter,
                                const QRect &R, const QRect &RG,
                                const QRect &C, const QRectF &M)
{
    Q_UNUSED(RG);
    Q_UNUSED(C);
    Q_UNUSED(M);

    bool isShowPageTitle = _bookModel->getDataBool(QModelIndex(),
                                                   "IsShowPageTitle","");
    if ( !isShowPageTitle ) {
        // If there is no page title, nothing to paint
        return;
    }

    // Save state
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing);
    QFont origFont = painter->font();

    // Local vars used in geometry calcs
    int x = 0;
    int y = 0;
    int w = 0;
    QFont font(origFont);

    // Get titles from model
    QString title1 = _pageTitles(_pageIdx).at(0);
    QString title2 = _pageTitles(_pageIdx).at(1);
    QString title3 = _pageTitles(_pageIdx).at(2);
    QString title4 = _pageTitles(_pageIdx).at(3);

    //
    // Draw main title
    //
    font.setPointSize(14);
    painter->setFont(font);
    QFontMetrics fm1(font,painter->device());
    w = fm1.boundingRect(title1).width();
    x = (R.width()-w)/2;
    y = fm1.xHeight() + fm1.ascent();
    if ( !title1.isEmpty() && !title1.trimmed().isEmpty() ) {
        painter->drawText(x,y,title1);
    }
    int yTitle1 = y;

    //
    // Draw subtitle with RUNs
    //
    font.setPointSize(11);
    painter->setFont(font);
    QFontMetrics fm2(painter->fontMetrics());
    if ( !title2.isEmpty() && !title2.trimmed().isEmpty() ) {
        QStringList lines = title2.split('\n', Qt::SkipEmptyParts);
        y += fm1.descent() + fm1.leading() + fm2.ascent();
        if ( lines.size() == 1 ) {
            // single RUN
            w = fm2.boundingRect(title2).width();
            x = (R.width()-w)/2;
            painter->drawText(x,y,title2);
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
            painter->drawText(x,y,s);
        }
    }
    int yTitle2 = y;

    //
    // Draw title3 and title4 (align on colon if possible)
    //
    int leftTitle34 = 0;
    font.setPointSize(11);
    painter->setFont(font);
    QFontMetrics fm3(painter->fontMetrics());
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
        if ( _bookModel->isPageLegend(_pageIdx) ) {
            y = yTitle2 + fm2.descent() + fm2.leading() + fm3.ascent();
        } else {
            y = yTitle1;
        }
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
        leftTitle34 = x;
        x += fm3.boundingRect(s31).width();
        x -= fm3.boundingRect(s41).width();
        y += fm3.lineSpacing();
        painter->drawText(x,y,s41);
        if ( x < leftTitle34 ) {
            leftTitle34 = x;
        }

    } else {
        // title3,title4 are custom (i.e. not colon delimited user/date)
        x = R.width() - fm3.boundingRect(title3).width();
        if ( _bookModel->isPageLegend(_pageIdx)) {
            y = yTitle2 + fm2.descent() + fm2.leading() + fm3.ascent();
        } else {
            y = yTitle1;
        }
        painter->drawText(x,y,title3);
        leftTitle34 = x;
        x = R.width() - fm3.boundingRect(title4).width();
        y += fm3.lineSpacing();
        painter->drawText(x,y,title4);
        if ( x < leftTitle34 ) {
            leftTitle34 = x;
        }
    }

    // Legend
    if ( _bookModel->isPageLegend(_pageIdx) ) {
        // Paint legend below title1 and title2
        QModelIndexList plotIdxs = _bookModel->plotIdxs(_pageIdx);
        QModelIndex curvesIdx = _bookModel->getIndex(plotIdxs.at(0),
                                                       "Curves","Plot");
        QFont legendFont = painter->font();
        legendFont.setPointSize(8);
        QFontMetrics fml(legendFont,painter->device());
        int c = fml.averageCharWidth();
        y = yTitle2 + fm2.descent() + fm2.leading() + fml.ascent();
        QRect S(R);
        S.setTopLeft(QPoint(R.x()+10*c,y));
        _paintPageLegend(S,curvesIdx,painter);
    }

    // Restore painter
    painter->setFont(origFont);
    painter->restore();
}

QSize PageTitleLayoutItem::_sizeLegend() const
{
    QSize size(0,0);

    if ( _bookModel->isPageLegend(_pageIdx) ) {

        // Since _isPlotLegendsSame==true, use first plot for labels
        QModelIndexList plotIdxs = _bookModel->plotIdxs(_pageIdx);

        if ( !plotIdxs.isEmpty() ) {

            QFont font = _font;
            font.setPointSize(8);
            QFontMetrics fm(font);
            const int c = fm.averageCharWidth();
            const int v = fm.leading(); // vertical space between legend entries
            const int s = c;   // spaceBetweenLineAndLabel
            const int l = 5*c; // line width
            const int q = 5*c; // space between legend columns

            QModelIndex plotIdx = plotIdxs.at(0);
            QStringList labels = _bookModel->legendLabels(plotIdx);
            int w = 0;
            int h = 0;
            if ( labels.size() == 1 ) {
                // 1 row 1 column
                int w0 = fm.boundingRect(labels.at(0)).width();
                w = w0;
                h = 2*fm.height()+v;
            } else if ( labels.size() == 2 ) {
                // 2 rows 1 column
                int w0 = fm.boundingRect(labels.at(0)).width();
                int w1 = fm.boundingRect(labels.at(1)).width();
                w = qMax(w0,w1)+l+s;
                h = 2*fm.height()+v;
            } else if ( labels.size() == 3 ) {
                // 3 rows 1 column
                int w0 = fm.boundingRect(labels.at(0)).width();
                int w1 = fm.boundingRect(labels.at(1)).width();
                int w2 = fm.boundingRect(labels.at(2)).width();
                w = qMax(qMax(w0,w1),w2)+l+s;
                h = 3*fm.height() + 2*v;
            } else if ( labels.size() == 4 ) {
                // 2 rows 2 columns
                int w0 = fm.boundingRect(labels.at(0)).width();
                int w1 = fm.boundingRect(labels.at(1)).width();
                int w2 = fm.boundingRect(labels.at(2)).width();
                int w3 = fm.boundingRect(labels.at(3)).width();
                w = qMax(w0,w1) + qMax(w2,w3) + 2*l + 2*s + q;
                h = 2*fm.height() + v;
            } else if ( labels.size() == 5 ) {
                // 3 rows 2 columns
                int w0 = fm.boundingRect(labels.at(0)).width();
                int w1 = fm.boundingRect(labels.at(1)).width();
                int w2 = fm.boundingRect(labels.at(2)).width();
                int w3 = fm.boundingRect(labels.at(3)).width();
                int w4 = fm.boundingRect(labels.at(4)).width();
                w = qMax(qMax(w0,w1),w2) + qMax(w3,w4) + 2*l + 2*s + q;
                h = 3*fm.height() + 2*v;
            } else if ( labels.size() == 6 ) {
                // 3 rows 2 columns
                int w0 = fm.boundingRect(labels.at(0)).width();
                int w1 = fm.boundingRect(labels.at(1)).width();
                int w2 = fm.boundingRect(labels.at(2)).width();
                int w3 = fm.boundingRect(labels.at(3)).width();
                int w4 = fm.boundingRect(labels.at(4)).width();
                int w5 = fm.boundingRect(labels.at(5)).width();
                w = qMax(qMax(w0,w1),w2) + qMax(qMax(w3,w4),w5) + 2*l + 2*s + q;
                h = 3*fm.height() + 2*v;
            } else if ( labels.size() == 7 ) {
                // 3 rows 3 columns
                int w0 = fm.boundingRect(labels.at(0)).width();
                int w1 = fm.boundingRect(labels.at(1)).width();
                int w2 = fm.boundingRect(labels.at(2)).width();
                int w3 = fm.boundingRect(labels.at(3)).width();
                int w4 = fm.boundingRect(labels.at(4)).width();
                int w5 = fm.boundingRect(labels.at(5)).width();
                int w6 = fm.boundingRect(labels.at(6)).width();
                w = qMax(qMax(w0,w1),w2) + qMax(qMax(w3,w4),w5) +
                    w6 + 3*l + 3*s + 2*q;
                h = 3*fm.height() + 2*v;
            }

            size.setWidth(w);
            size.setHeight(h);
        }
    }

    return size;
}

// Get title strings from model
QStringList PageTitleLayoutItem::_pageTitles(const QModelIndex &pageIdx) const
{
    QStringList titles;

    QModelIndex title1Idx;
    if ( _bookModel->isChildIndex(pageIdx,"Page","PageTitle")) {
        title1Idx = _bookModel->getDataIndex(pageIdx,"PageTitle","Page");
    } else {
        titles << " " << " " << " " << " ";
        return titles;
    }
    QModelIndex defTitlesIdx = _bookModel->getIndex(QModelIndex(),
                                                      "DefaultPageTitles");
    QModelIndex title2Idx = _bookModel->getDataIndex(defTitlesIdx,
                                                       "Title2",
                                                       "DefaultPageTitles");
    QModelIndex title3Idx = _bookModel->getDataIndex(defTitlesIdx,
                                                       "Title3",
                                                       "DefaultPageTitles");
    QModelIndex title4Idx = _bookModel->getDataIndex(defTitlesIdx,
                                                       "Title4",
                                                       "DefaultPageTitles");
    QString title1 = _bookModel->data(title1Idx).toString();
    QString t1 = _bookModel->getDataString(defTitlesIdx,
                                             "Title1","DefaultPageTitles");
    if ( title1.isEmpty() && t1.startsWith("koviz") ) {
        // Since subtitle has RUNs, don't use t1 (it has RUNs too)
        title1 = "Koviz Plots";
    } else if ( !t1.startsWith("koviz") ) {
        // DP page title overwritten by -t1 optional title
        title1 = t1;
    }
    QString title2 = _bookModel->data(title2Idx).toString();
    QString title3 = _bookModel->data(title3Idx).toString();
    QString title4 = _bookModel->data(title4Idx).toString();

    titles << title1 << title2 << title3 << title4;

    return titles;
}

void PageTitleLayoutItem::_paintPageLegend(const QRect &R,
                                           const QModelIndex &curvesIdx,
                                           QPainter* painter)
{
    QModelIndex plotIdx = curvesIdx.parent();
    QString pres = _bookModel->getDataString(plotIdx,
                                             "PlotPresentation","Plot");
    if ( pres == "error" ) {
        return;
    }

    QList<QPen*> pens = _bookModel->legendPens(plotIdx,
                                               painter->paintEngine()->type());
    QStringList symbols = _bookModel->legendSymbols(plotIdx);
    QStringList labels = _bookModel->legendLabels(plotIdx);

    if ( pres == "error+compare" ) {
        QPen* magentaPen = new QPen(_bookModel->errorLineColor());
        pens << magentaPen;
        symbols << "none";
        labels << "error";
    }

    __paintPageLegend(R,pens,symbols,labels,painter);

    // Clean up
    foreach ( QPen* pen, pens ) {
        delete pen;
    }
}

// pens,symbols and labels are ordered/collated foreach legend curve/label
// Returns bounding box of painted area (B)
void PageTitleLayoutItem::__paintPageLegend(const QRect &R,
                                            const QList<QPen *> &pens,
                                            const QStringList &symbols,
                                            const QStringList &labels,
                                            QPainter* painter)
{
    QFont origFont = painter->font();
    QPen origPen = painter->pen();

    QFont font(origFont);
    font.setPointSize(8);
    painter->setFont(font);
    QFontMetrics fm = painter->fontMetrics();

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

    painter->setFont(origFont);
    painter->setPen(origPen);
}

// Note: symbols can be drawn outside of R
void PageTitleLayoutItem::_paintLegendEntry(const QRect &R,
                                            int l,  // line width
                                            int s,  // spaceBetweenLineAndLabel
                                            const QPen &pen,
                                            const QString &symbol,
                                            const QString &label,
                                            QPainter* painter)
{
    QPen origPen = painter->pen();

    // Draw line segment
    painter->setPen(pen);
    QPoint p1(R.left(),R.center().y());
    QPoint p2(R.left()+l,R.center().y());
    painter->drawLine(p1,p2);

    // Draw symbols on line segment endpoints
    __paintSymbol(p1,symbol,painter);
    __paintSymbol(p2,symbol,painter);

    // Draw label
    QRect labelRect(R);
    QPoint p(R.topLeft().x()+l+s, R.topLeft().y());
    labelRect.moveTopLeft(p);
    painter->drawText(labelRect, Qt::AlignLeft, label);

    painter->setPen(origPen);
}

void PageTitleLayoutItem::__paintSymbol(const QPointF& p,
                                        const QString &symbol,
                                        QPainter* painter)
{

    QPen origPen = painter->pen();
    QPen pen = painter->pen();
    pen.setStyle(Qt::SolidLine);
    painter->setPen(pen);

    int sizex = painter->fontMetrics().xHeight();

    if ( symbol == "circle" ) {
        painter->drawEllipse(p,sizex/2,sizex/2);
    } else if ( symbol == "thick_circle" ) {
        pen.setWidthF(sizex/3.0);
        painter->setPen(pen);
        painter->drawEllipse(p,sizex/2,sizex/2);
    } else if ( symbol == "solid_circle" ) {
        QPainterPath circlePath;
        circlePath.addEllipse(p,sizex/2,sizex/2);
        painter->fillPath(circlePath,QBrush(pen.color()));
    } else if ( symbol == "square" ) {
        double x = p.x()-sizex/2.0;
        double y = p.y()-sizex/2.0;
        painter->drawRect(QRectF(x,y,sizex,sizex));
    } else if ( symbol == "thick_square") {
        pen.setWidthF(sizex/3.0);
        painter->setPen(pen);
        double x = p.x()-sizex/2.0;
        double y = p.y()-sizex/2.0;
        painter->drawRect(QRectF(x,y,sizex,sizex));
    } else if ( symbol == "solid_square" ) {
        double x = p.x()-sizex/2.0;
        double y = p.y()-sizex/2.0;
        painter->fillRect(QRectF(x,y,sizex,sizex),QBrush(pen.color()));
    } else if ( symbol == "star" ) { // *
        double r = sizex/1.5;
        QPointF a(p.x()+r*cos(18.0*M_PI/180.0),
                  p.y()-r*sin(18.0*M_PI/180.0));
        QPointF b(p.x(),p.y()-r);
        QPointF c(p.x()-r*cos(18.0*M_PI/180.0),
                  p.y()-r*sin(18.0*M_PI/180.0));
        QPointF d(p.x()-r*cos(54.0*M_PI/180.0),
                  p.y()+r*sin(54.0*M_PI/180.0));
        QPointF e(p.x()+r*cos(54.0*M_PI/180.0),
                  p.y()+r*sin(54.0*M_PI/180.0));
        painter->drawLine(p,a);
        painter->drawLine(p,b);
        painter->drawLine(p,c);
        painter->drawLine(p,d);
        painter->drawLine(p,e);
    } else if ( symbol == "xx" ) {
        double s = sizex/2.0;
        QPointF a(p.x()+s,p.y()+s);
        QPointF b(p.x()-s,p.y()+s);
        QPointF c(p.x()-s,p.y()-s);
        QPointF d(p.x()+s,p.y()-s);
        painter->drawLine(p,a);
        painter->drawLine(p,b);
        painter->drawLine(p,c);
        painter->drawLine(p,d);
    } else if ( symbol == "triangle" ) {
        double r = sizex/1.5;
        QPointF a(p.x(),p.y()-r);
        QPointF b(p.x()-r*cos(30.0*M_PI/180.0),
                  p.y()+r*sin(30.0*M_PI/180.0));
        QPointF c(p.x()+r*cos(30.0*M_PI/180.0),
                  p.y()+r*sin(30.0*M_PI/180.0));
        painter->drawLine(a,b);
        painter->drawLine(b,c);
        painter->drawLine(c,a);
    } else if ( symbol == "thick_triangle" ) {
        pen.setWidthF(2.0);
        painter->setPen(pen);
        double r = 4.0;
        QPointF a(p.x(),p.y()-r);
        QPointF b(p.x()-r*cos(30.0*M_PI/180.0),
                  p.y()+r*sin(30.0*M_PI/180.0));
        QPointF c(p.x()+r*cos(30.0*M_PI/180.0),
                  p.y()+r*sin(30.0*M_PI/180.0));
        painter->drawLine(a,b);
        painter->drawLine(b,c);
        painter->drawLine(c,a);
    } else if ( symbol == "solid_triangle" ) {
        pen.setWidthF(2.0);
        painter->setPen(pen);
        double r = 3.0;
        QPointF a(p.x(),p.y()-r);
        QPointF b(p.x()-r*cos(30.0*M_PI/180.0),
                  p.y()+r*sin(30.0*M_PI/180.0));
        QPointF c(p.x()+r*cos(30.0*M_PI/180.0),
                  p.y()+r*sin(30.0*M_PI/180.0));
        painter->drawLine(a,b);
        painter->drawLine(b,c);
        painter->drawLine(c,a);
    } else if ( symbol.startsWith("number_",Qt::CaseInsensitive) &&
                symbol.size() == 8 ) {

        QFont origFont = painter->font();
        QBrush origBrush = painter->brush();

        // Calculate bbox to draw text in
        QString number = symbol.right(1); // last char is '0'-'9'
        QFont font = painter->font();
        font.setPointSize(7);
        painter->setFont(font);
        QFontMetrics fm = painter->fontMetrics();
        QRectF bbox(fm.tightBoundingRect(number));
        bbox.moveCenter(p);

        // Draw solid circle around number
        QRectF box(bbox);
        double l = 3.0*qMax(box.width(),box.height())/2.0;
        box.setWidth(l);
        box.setHeight(l);
        box.moveCenter(p);
        QBrush brush(pen.color());
        painter->setBrush(brush);
        painter->drawEllipse(box);

        // Draw number in white in middle of circle
        QPen whitePen("white");
        painter->setPen(whitePen);
        painter->drawText(bbox,Qt::AlignCenter,number);

        painter->setFont(origFont);
        painter->setBrush(origBrush);
    }

    painter->setPen(origPen);
}
