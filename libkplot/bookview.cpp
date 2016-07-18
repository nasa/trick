#include "bookview.h"

#include "libsnapdata/timeit_linux.h"
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

void BookView::_update()
{
}

void BookView::currentChanged(const QModelIndex &current,
                              const QModelIndex &previous)
{
    Q_UNUSED(previous);
    _nb->setCurrentIndex(current.row());
}

void BookView::selectionChanged(const QItemSelection &selected,
                                const QItemSelection &deselected)
{
    Q_UNUSED(selected);
    Q_UNUSED(deselected);
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
    if ( pageCnt == 1 ) {
        printer.setOrientation(QPrinter::Landscape);
    } else {
        printer.setOrientation(QPrinter::Portrait);
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

    QRect pageTitleRect = __printPageTitle(painter,pageIdx);

    QModelIndex plotsIdx = _bookModel()->getIndex(pageIdx,"Plots", "Page");

    int nPlots = model()->rowCount(plotsIdx);

    QRect R(pageTitleRect.bottomLeft().x(),
            pageTitleRect.bottomLeft().y(),
            paintDevice->width(),
            paintDevice->height()-pageTitleRect.height());

    if ( nPlots == 0 ) {
        return;
    } else if ( nPlots == 1 ) {
        QModelIndex plotIdx = model()->index(0,0,plotsIdx);
        __printCurves(R,painter,plotIdx);
    } else if ( nPlots == 6 ) {
        R.setWidth(R.width()/2);
        R.setHeight(R.height()/3);
        for ( int i = 0; i < nPlots; ++i ) {
            int x,y;
            if ( i < 3 ) {
                x = 0;
                y = i*R.height();
            } else if ( i >= 3 ) {
                x = R.width();
                y = (i%3)*R.height();
            }
            QRect R2 = R.translated(x,y);
            QModelIndex plotIdx = model()->index(i,0,plotsIdx);
            __printCurves(R2,painter,plotIdx);
        }
    } else {
        qDebug() << "snap [error]: _printPage() : cannot print more than "
                    "7 plots on a page.";
        exit(-1);
    }


    //__printXAxisLabel(painter,pageIdx);
}

// Returns a bounding box of page title
QRect BookView::__printPageTitle(QPainter* painter, const QModelIndex &pageIdx)
{
    //
    // Init
    //
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
    QString title2 = model()->data(title2Idx).toString();
    QString title3 = model()->data(title3Idx).toString();
    QString title4 = model()->data(title4Idx).toString();

    // Draw main title
    //
    QFont font;
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
    painter->drawRect(0,0,bbox.width(),bbox.height());

    return bbox;
}

void BookView::__printCurves(const QRect& R,
                             QPainter *painter, const QModelIndex &plotIdx)
{

    painter->drawRect(R);

    QModelIndex plotMathRectIdx = _bookModel()->getDataIndex(plotIdx,
                                                        "PlotMathRect", "Plot");
    QRectF M = model()->data(plotMathRectIdx).toRectF();
    if ( M.topLeft().y() < M.bottomLeft().y() ) {
        // Flip if y-axis not directed "up" (this happens with bboxes)
        M = QRectF(M.bottomLeft(),M.topRight());
    }

    double a = R.width()/M.width();
    double b = R.height()/M.height();
    double c = R.topLeft().x()-a*M.topLeft().x();
    double d = R.topLeft().y()-b*M.topLeft().y();
    QTransform T( a,    0,
                  0,    b,
                  c,    d);


    QList<QPainterPath*> paths;
    QRectF bbox;
    QModelIndex curvesIdx = _bookModel()->getIndex(plotIdx,"Curves","Plot");
    int rc = model()->rowCount(curvesIdx);
    for ( int i = 0; i < rc; ++i ) {

        QModelIndex curveIdx = model()->index(i,0,curvesIdx);
        QModelIndex curveDataIdx = _bookModel()->getDataIndex(curveIdx,
                                                          "CurveData","Curve");
        QVariant v = model()->data(curveDataIdx);
        TrickCurveModel* curveModel =QVariantToPtr<TrickCurveModel>::convert(v);

        if ( curveModel ) {

            QPainterPath* path = new QPainterPath;
            paths << path;

            curveModel->map();
            TrickModelIterator it = curveModel->begin();
            const TrickModelIterator e = curveModel->end();

            QList<QPointF> pts;

            while (it != e) {

                QPointF p(it.x(),it.y());
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

                    if ( ang > 60 || d > 200 ) {
                        // fuzzily non-colinear or
                        // distance between p0 and p over 1/6th inch
                        path->lineTo(pts.last());
                        pts.clear();
                    }
                }

                pts << p;

                ++it;
            }
            curveModel->unmap();

            QRectF curveBBox = path->boundingRect();
            bbox = bbox.united(curveBBox);
        }
    }

    painter->save();
    painter->setRenderHint(QPainter::Antialiasing);

    QPen pen(QColor(Qt::black));
    pen.setWidth(2);
    painter->setPen(pen);

    QList<QColor> colors = _bookModel()->createCurveColors(paths.size());

    int i = 0;
    foreach ( QPainterPath* path, paths ) {
        pen.setColor(colors.at(i));
        painter->setPen(pen);
        painter->drawPath(*path);
        delete path;
        ++i;
    }
    //painter->drawPath(*path2);

    // Restore the painter state off the painter stack
    painter->restore();
}

void BookView::__printXAxisLabel(QPainter *painter,
                                     const QModelIndex &pageIdx)
{
    QPaintDevice* paintDevice = painter->device();


    QString s("sys.exec.out.time {s}");

    QFont font;
    font.setPointSize(12);
    painter->setFont(font);

    QFontMetrics fm(font,paintDevice);
    QRect R = fm.boundingRect(s);
    int textWidth = R.width();
    int margin = paintDevice->logicalDpiY()/8; // 8th inch
    int x = paintDevice->width()/2 - textWidth/2;
    int y = paintDevice->height() - fm.descent() - margin;

    painter->drawText(x,y,s);
}

void BookView::__printYAxisLabel(QPainter *painter,
                                     const QModelIndex &pageIdx)
{
    QPaintDevice* paintDevice = painter->device();

    QString s("ball.state.out.acceleration[0] {m/s2}");

    QFont font;
    font.setPointSize(12);
    painter->setFont(font);

#if 0
    QFontMetrics fm(font,paintDevice);
    QRect R = fm.boundingRect(s);
    int textWidth = R.width();
    int margin = paintDevice->logicalDpiY()/8; // 8th inch
    int x = paintDevice->width()/2 - textWidth/2;
    int y = paintDevice->height() - fm.descent() - margin;


    int vw = viewport()->width();
    int vh = viewport()->height();
    QFontMetrics fm = viewport()->fontMetrics();
    QString txt = fm.elidedText(_yAxisLabelText, Qt::ElideLeft, vh);
    QRect bb = fm.tightBoundingRect(txt);
    int bw = bb.width();
    int bh = bb.height();

    // Draw!
    QPainter painter(viewport());
    painter.save();
    painter.translate( vw, (vh+bw)/2-5);
    painter.rotate(270);
    painter.drawText(0,0, txt);
    painter.restore();

    painter->drawText(x,y,s);
#endif
}

void BookView::_nbCloseRequested(int idx)
{
    if ( model() == 0 ) return;

    QWidget* tabWidget = _nb->widget(idx);
    QString tabToolTip = _nb->tabToolTip(idx);
    QString wt = _nb->tabWhatsThis(idx);
    if ( wt == "Page" ) {
        QModelIndex pagesIdx = _bookModel()->getIndex(QModelIndex(), "Pages");
        int row = -1;
        int rc = model()->rowCount(pagesIdx);
        for ( int i = 0; i < rc ; ++i ) {
            QModelIndex pageIdx = model()->index(i,0,pagesIdx);
            QModelIndex pageNameIdx = _bookModel()->getIndex(pageIdx,
                                                             "PageName","Page");
            pageNameIdx = model()->index(pageNameIdx.row(),1,pageIdx);
            QString pageName = model()->data(pageNameIdx).toString();
            if ( pageName == tabToolTip ) {
                row = i;
                break;
            }
        }
        if ( row < 0 ) {
            qDebug() << "snap [bad scoobs]: BookView::_nbCloseRequested() "
                        "Could not find page to remove!  Aborting!";
            exit(-1);
        }
        model()->removeRow(row,pagesIdx);
        _nb->removeTab(idx);
    } else if ( wt == "Table" ) {
        // TODO: Table
    } else {
        qDebug() << "snap [bad scoobs]: BookView::_nbCloseRequested() "
                    "tabWhatsThis should have been set.";
        exit(-1);
    }

    delete tabWidget;
}

void BookView::_nbCurrentChanged(int idx)
{
    if ( selectionModel() ) {
        QModelIndex pagesIdx = _bookModel()->getIndex(QModelIndex(),"Pages");
        QModelIndex pageIdx = model()->index(idx, 0,pagesIdx);
        selectionModel()->setCurrentIndex(pageIdx,QItemSelectionModel::Current);
    }
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
            PageView* pw = new PageView;
            pw->setModel(model());
            pw->setRootIndex(idx);
            int tabId = _nb->addTab(pw,"Page");
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
