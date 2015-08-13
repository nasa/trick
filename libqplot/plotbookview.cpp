#include "plotbookview.h"
#include "libsnapdata/trickcurvemodel.h"
#include "libopts/options.h"
#include <QFileInfo>
#include <QHeaderView>

#include <QDebug>
#include "libsnapdata/timeit_linux.h"

PlotBookView::PlotBookView(PlotBookModel *plotModel, const QStringList &titles, QWidget *parent) :
    QAbstractItemView(parent),
    _titles(titles),
    _plotModel(plotModel),
    _currSelectedRun(-1),
    _isShowCurveDiff(false)
{
    setModel(_plotModel);

    _bookFrame = new QFrame(parent);
    _bookGridLayout = new QGridLayout(_bookFrame);

    _buttonCloseAll = new QPushButton("Close All",_bookFrame);
    connect(_buttonCloseAll,SIGNAL(clicked()),
            this,SLOT(_closeAllPlots()));
    _bookGridLayout->addWidget(_buttonCloseAll,1,0);
    _buttonToggleDiff = new QPushButton("Toggle Diff",_bookFrame);
    connect(_buttonToggleDiff,SIGNAL(clicked()),
            this,SLOT(_toggleDiffPlots()));

     _buttonLayout = new QHBoxLayout;
     _buttonLayout->addWidget(_buttonCloseAll);
     _buttonLayout->addWidget(_buttonToggleDiff);
     _buttonFrame = new QFrame(_bookFrame);
     _buttonFrame->setLayout(_buttonLayout);
    _bookGridLayout->addWidget(_buttonFrame,1,0);
    _buttonToggleDiff->setHidden(true);


    this->setFocusPolicy(Qt::ClickFocus);
    _nb = new QTabWidget(_bookFrame);
    _nb->setTabsClosable(true);
    _nb->setFocusPolicy(Qt::StrongFocus);
    _nb->setMovable(true);
    connect(_nb,SIGNAL(tabCloseRequested(int)),
            this,SLOT(tabCloseRequested(int)));
    connect(_nb,SIGNAL(currentChanged(int)),
            this,SLOT(tabCurrentChanged(int)));
    _bookGridLayout->addWidget(_nb,0,0);
}

// This is private
PlotBookView::PlotBookView(QWidget *parent) :
    QAbstractItemView(parent),
    _plotModel(0),
    _currSelectedRun(-1),
    _isShowCurveDiff(false)
{
}

PlotBookView::~PlotBookView()
{
    // I think, not sure, the grids, plots frames are deleted with notebook
    // so nothing to do
}

//
// This is bogus.  Untested.  And don't really know what it is.
// For now just return frame rect
//
QRect PlotBookView::visualRect(const QModelIndex &index) const
{
    QRect rect;
    if ( model() && index.isValid()) {
        QWidget* page = _idx2Page(index);
        if ( page )  {
            rect = page->rect();
        }
    }

    return rect;
}

void PlotBookView::scrollTo(const QModelIndex &index,
                            QAbstractItemView::ScrollHint hint)
{
    // TODO
    Q_UNUSED(index);
    Q_UNUSED(hint);
    update();
}

// For now, return index for a plot at point
// Point is relative to page
QModelIndex PlotBookView::indexAt(const QPoint &point) const
{
    if ( !model() ) return QModelIndex();

    QModelIndex idx;
    QWidget* page = _nb->currentWidget();
    QGridLayout* grid = _page2grid.value(page);

    for ( int i = 0; i < grid->count(); ++i) {
        QWidget* w = grid->itemAt(i)->widget();
        if ( ! w->isVisible() ) {
            continue;
        }
        if ( w->geometry().contains(point) ) {
            Plot* plot = static_cast<Plot*>(w);
            idx = _plot2Idx(plot);
            break;
        }
    }

    return idx;
}

void PlotBookView::setSelectionModel(QItemSelectionModel *selectionModel)
{
    connect(selectionModel,
            SIGNAL(currentChanged(QModelIndex,QModelIndex)),
            this,
            SLOT(_plotSelectModelCurrentChanged(QModelIndex,QModelIndex)));
    connect(selectionModel,
            SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
            this,
            SLOT(_plotSelectModelSelectChanged(QItemSelection,QItemSelection)));
    QAbstractItemView::setSelectionModel(selectionModel);
}

QModelIndex PlotBookView::currentPageIndex()
{
    QModelIndex pageIdx;
    QWidget* page = _nb->currentWidget();
    pageIdx = _page2Idx(page);
    return pageIdx;
}

void PlotBookView::setCurrentPage(const QModelIndex& pageIdx)
{
    int pageId = -1;
    QWidget* page = _idx2Page(pageIdx);
    for ( int i = 0 ; i < _nb->count(); ++i ) {
        QWidget* w = _nb->widget(i);
        if ( w == page ) {
            pageId = i;
            break;
        }
    }
    if ( pageId >= 0 ) {
        _nb->setCurrentIndex(pageId);
    }

}

bool PlotBookView::savePdf(const QString &fileName)
{
    bool isVectorize = true;
    for ( int pageId = 0; pageId < _pages.size(); ++pageId) {
        QWidget* page = _pages.at(pageId);
        QVector<Plot*> plots = _page2Plots.value(page);
        foreach (Plot* plot, plots ) {
            int nCurves = plot->axisRect()->curveCount();
            if ( nCurves > 10 ) {
                isVectorize = false;
                break;
            }
        }
    }

    bool ret = false;
    if ( isVectorize ) {
        // Vectorizing causes huge pdfs. Use pixmaps for now
        // but leave in hook for vectorizing.
        //ret = _savePdfVectorized(fileName);
        ret = _savePdfPixmapped(fileName);
    } else {
        ret = _savePdfPixmapped(fileName);
    }

    return ret;
}

bool PlotBookView::_savePdfVectorized(const QString &fileName)
{
    // Page size and title header size
    int ww = 1200;
    int hh = 900;

    // Save current widget
    QWidget* originalPage = _nb->currentWidget();

    // Header height
    int heightHeader = 0;
    for ( int i = 0 ; i < _nb->count(); ++i ) {
        QWidget* widget = _nb->widget(i);
        if ( !_pages.contains(widget) ) {
            // Skip tables
            continue;
        }
        QWidget* page = widget;
        _nb->setCurrentWidget(page);  // for layout when offscreen
        PageTitleWidget* pw = _page2pagewidget.value(page);
        if ( pw->sizeHint().height() > heightHeader ) {
            heightHeader = pw->height() ;
        }
    }

    // Plots rect
    QRect plotsRect(0,heightHeader,ww,hh-heightHeader);

    QPrinter printer(QPrinter::ScreenResolution);
    printer.setOutputFileName(fileName);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setFullPage(true);
    //printer.setPaperSize(QPrinter::A4);
    printer.setPaperSize(QSizeF(ww,hh),QPrinter::DevicePixel);
    //printer.setOrientation(QPrinter::Landscape);

    QCPPainter printpainter;
    if (! printpainter.begin(&printer)) {
        return false;
    }
    printpainter.setMode(QCPPainter::pmVectorized);
    printpainter.setMode(QCPPainter::pmNoCaching);
    printpainter.setMode(QCPPainter::pmNonCosmetic,false);
    QRect printerRect = QRect(0,0,printer.width(),printer.height());
    printpainter.setWindow(printerRect);
    printpainter.fillRect(printerRect, QBrush(Qt::white));

    int pageCnt = 0;
    for ( int i = 0; i < _pages.size(); ++i) {

        QWidget* widget = _nb->widget(i);
        if ( !_pages.contains(widget) ) {
            // Skip tables
            continue;
        }
        QWidget* page = widget;

        QVector<Plot*> plots = _page2Plots.value(page);
        QVector<QRect> origPlotViewports;
        foreach (Plot* plot, plots ) {
            origPlotViewports.append(plot->viewport());
            plot->setViewport(plotsRect);
        }

        _layoutPdfPlots(plots);

        // Draw then restore plot layout
        int plotId = 0 ;
        foreach (Plot* plot, plots ) {
            plot->drawMe(&printpainter);
            plot->setViewport(origPlotViewports.at(plotId));
            plot->plotLayout()->simplify();  // get rid of empty cells
            plotId++;
        }

        // Draw header (resize for offscreen rendering)
        PageTitleWidget* pw = _page2pagewidget.value(page);
        pw->setFixedWidth(ww);
        pw->setFixedHeight(heightHeader);
        pw->render(&printpainter);

        // Insert new page in pdf booklet
        if ( pageCnt < _pages.size()-1 ) {
            printer.newPage();
        }

        ++pageCnt;
    }

    printpainter.end();

    _nb->setCurrentWidget(originalPage);

    return true;
}

//
// To speed up printing and reduce pdf size,
// instead of printing directly to the pdf,
// print pixmaps of curves to pdf
//
bool PlotBookView::_savePdfPixmapped(const QString &fileName)
{
    // Page size and title header size
    int ww = 1200;
    int hh = 900;

    // Save current widget
    QWidget* originalPage = _nb->currentWidget();

    // Header height
    int heightHeader = 0;
    for ( int i = 0 ; i < _nb->count(); ++i ) {
        QWidget* widget = _nb->widget(i);
        if ( !_pages.contains(widget) ) {
            // Skip tables
            continue;
        }
        QWidget* page = widget;
        _nb->setCurrentWidget(page);  // for layout when offscreen
        PageTitleWidget* pw = _page2pagewidget.value(page);
        if ( pw->sizeHint().height() > heightHeader ) {
            heightHeader = pw->height() ;
        }
    }

    // Setup pdf printer
    QPrinter pdfPrinter(QPrinter::ScreenResolution);
    pdfPrinter.setOutputFileName(fileName);
    pdfPrinter.setOutputFormat(QPrinter::PdfFormat);
    pdfPrinter.setFullPage(true);
    pdfPrinter.setPaperSize(QSizeF(ww,hh),QPrinter::DevicePixel);

    // Setup Painter
    QCPPainter pdfPainter;
    pdfPainter.setMode(QCPPainter::pmVectorized);
    pdfPainter.setMode(QCPPainter::pmNoCaching);
    pdfPainter.setMode(QCPPainter::pmNonCosmetic,false);
    if (! pdfPainter.begin(&pdfPrinter)) {
        return false;
    }
    QRect paintRect = QRect(0,0,pdfPrinter.width(),pdfPrinter.height());
    pdfPainter.setWindow(paintRect);

    // Init pixmap for header
    QRect headerRect(0,0,ww,heightHeader);
    QPixmap pixmapHeader(ww,heightHeader);
    QCPPainter pixPainterHeader;
    pixPainterHeader.setMode(QCPPainter::pmNoCaching);
    pixPainterHeader.setMode(QCPPainter::pmNonCosmetic,false);
    if (! pixPainterHeader.begin(&pixmapHeader)) {
        pdfPainter.end();
        return false;
    }
    pixPainterHeader.setWindow(headerRect);

    // Init pixmap for plots
    QRect plotsRect(0,heightHeader,ww,hh-heightHeader);
    QPixmap pixmapPlots(ww,hh-heightHeader);
    QCPPainter pixPainterPlots;
    pixPainterPlots.setMode(QCPPainter::pmNoCaching);
    pixPainterPlots.setMode(QCPPainter::pmNonCosmetic,false);
    if (! pixPainterPlots.begin(&pixmapPlots)) {
        pdfPainter.end();
        pixPainterHeader.end();
        return false;
    }
    pixPainterPlots.setWindow(plotsRect);

    int pageCnt = 0;
    for ( int i = 0 ; i < _nb->count(); ++i ) {

        QWidget* widget = _nb->widget(i);
        if ( !_pages.contains(widget) ) {
            // Skip tables
            continue;
        }
        QWidget* page = widget;

        PageTitleWidget* pw = _page2pagewidget.value(page);

        QVector<Plot*> plots = _page2Plots.value(page);
        QVector<QRect> origPlotViewports;
        foreach (Plot* plot, plots ) {
            origPlotViewports.append(plot->viewport());
            plot->setViewport(plotsRect);
        }

        _layoutPdfPlots(plots);

        // Draw plots (and restore plot layout)
        pixmapPlots.fill(Qt::white);
        int plotId = 0 ;
        foreach (Plot* plot, plots ) {
            plot->drawMe(&pixPainterPlots);
            plot->setViewport(origPlotViewports.at(plotId));
            plot->plotLayout()->simplify();  // get rid of empty cells
            plotId++;
        }
        pdfPainter.drawPixmap(0,heightHeader,pixmapPlots);

        // Draw header (resize for offscreen rendering)
        pixmapHeader.fill(Qt::white);
        pixPainterHeader.end();
        pw->setFixedWidth(ww);
        pw->setFixedHeight(heightHeader);
        pw->render(&pixmapHeader);
        pw->setFixedSize(QSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX));
        pdfPainter.drawPixmap(0,0,pixmapHeader);
        pixPainterHeader.begin(&pixmapHeader);

        // Insert new page in pdf booklet
        if ( pageCnt < _pages.size()-1 ) {
            pdfPrinter.newPage();
        }

        ++pageCnt;
    }

    pdfPainter.end();
    pixPainterHeader.end();
    pixPainterPlots.end();

    _nb->setCurrentWidget(originalPage);

    return true;
}


void PlotBookView::_layoutPdfPlots(const QVector<Plot*>& plots)
{
        // Plotbookview's page is a Qt QGrid of QCustomplot Widgets.
        // QCustomPlot can make a "page", but for interactive use,
        // I found it better to use Qt's QGrid.  When printing it is better
        // to use QCustomPlot's layout.  The following switch statement
        // adds empty cells, later restored, in each plot's internal grid layout.
        switch ( plots.size() )
        {
        case 1:
        {
            break;
        }
        case 2:
        {
            plots.at(0)->plotLayout()->insertRow(1);
            plots.at(1)->plotLayout()->insertRow(0);
            break;
        }
        case 3:
        {
            plots.at(0)->plotLayout()->insertRow(1);
            plots.at(0)->plotLayout()->insertRow(1);

            plots.at(1)->plotLayout()->insertRow(0);
            plots.at(1)->plotLayout()->insertRow(2);

            plots.at(2)->plotLayout()->insertRow(0);
            plots.at(2)->plotLayout()->insertRow(0);
            break;
        }
        case 4:
        {
            plots.at(0)->plotLayout()->insertRow(1);
            plots.at(0)->plotLayout()->insertColumn(1);

            plots.at(1)->plotLayout()->insertRow(1);
            plots.at(1)->plotLayout()->insertColumn(0);

            plots.at(2)->plotLayout()->insertRow(0);
            plots.at(2)->plotLayout()->insertColumn(1);

            plots.at(3)->plotLayout()->insertRow(0);
            plots.at(3)->plotLayout()->insertColumn(0);
            break;
        }
        case 5:
        {
            plots.at(0)->plotLayout()->insertRow(1);
            plots.at(0)->plotLayout()->insertRow(1);
            plots.at(0)->plotLayout()->insertColumn(1);

            plots.at(1)->plotLayout()->insertRow(1);
            plots.at(1)->plotLayout()->insertRow(1);
            plots.at(1)->plotLayout()->insertColumn(0);

            plots.at(2)->plotLayout()->insertRow(0);
            plots.at(2)->plotLayout()->insertRow(2);
            plots.at(2)->plotLayout()->insertColumn(1);

            plots.at(3)->plotLayout()->insertRow(0);
            plots.at(3)->plotLayout()->insertRow(2);
            plots.at(3)->plotLayout()->insertColumn(0);

            plots.at(4)->plotLayout()->insertRow(0);
            plots.at(4)->plotLayout()->insertRow(0);
            break;
        }
        case 6:
        {

            plots.at(0)->plotLayout()->insertRow(1);
            plots.at(0)->plotLayout()->insertRow(1);
            plots.at(0)->plotLayout()->insertColumn(1);

            plots.at(1)->plotLayout()->insertRow(0);
            plots.at(1)->plotLayout()->insertRow(2);
            plots.at(1)->plotLayout()->insertColumn(1);

            plots.at(2)->plotLayout()->insertRow(0);
            plots.at(2)->plotLayout()->insertRow(0);
            plots.at(2)->plotLayout()->insertColumn(1);

            plots.at(3)->plotLayout()->insertRow(1);
            plots.at(3)->plotLayout()->insertRow(1);
            plots.at(3)->plotLayout()->insertColumn(0);

            plots.at(4)->plotLayout()->insertRow(0);
            plots.at(4)->plotLayout()->insertRow(2);
            plots.at(4)->plotLayout()->insertColumn(0);

            plots.at(5)->plotLayout()->insertRow(0);
            plots.at(5)->plotLayout()->insertRow(0);
            plots.at(5)->plotLayout()->insertColumn(0);
            break;
        }
        case 7:
        {
            plots.at(0)->plotLayout()->insertRow(1);
            plots.at(0)->plotLayout()->insertRow(1);
            plots.at(0)->plotLayout()->insertRow(1);
            plots.at(0)->plotLayout()->insertColumn(1);

            plots.at(1)->plotLayout()->insertRow(1);
            plots.at(1)->plotLayout()->insertRow(1);
            plots.at(1)->plotLayout()->insertRow(1);
            plots.at(1)->plotLayout()->insertColumn(0);

            plots.at(2)->plotLayout()->insertRow(0);
            plots.at(2)->plotLayout()->insertRow(2);
            plots.at(2)->plotLayout()->insertRow(2);
            plots.at(2)->plotLayout()->insertColumn(1);

            plots.at(3)->plotLayout()->insertRow(0);
            plots.at(3)->plotLayout()->insertRow(2);
            plots.at(3)->plotLayout()->insertRow(2);
            plots.at(3)->plotLayout()->insertColumn(0);

            plots.at(4)->plotLayout()->insertRow(0);
            plots.at(4)->plotLayout()->insertRow(0);
            plots.at(4)->plotLayout()->insertRow(3);
            plots.at(4)->plotLayout()->insertColumn(1);

            plots.at(5)->plotLayout()->insertRow(0);
            plots.at(5)->plotLayout()->insertRow(0);
            plots.at(5)->plotLayout()->insertRow(3);
            plots.at(5)->plotLayout()->insertColumn(0);

            plots.at(6)->plotLayout()->insertRow(0);
            plots.at(6)->plotLayout()->insertRow(0);
            plots.at(6)->plotLayout()->insertRow(0);
            break;
        }
        default:
        {
        }
        }
}

void PlotBookView::showCurveDiff(bool isShow)
{
    _isShowCurveDiff = isShow;
    _buttonToggleDiff->setHidden(false);

}

void PlotBookView::replot()
{
    foreach ( QWidget* page, _pages ) {
        foreach ( Plot* plot, _page2Plots.value(page) ) {
            plot->replot();
        }
    }
}

QModelIndex PlotBookView::moveCursor(QAbstractItemView::CursorAction cursorAction,
                                     Qt::KeyboardModifiers modifiers)
{
    // TODO
    Q_UNUSED(cursorAction);
    Q_UNUSED(modifiers);
    return QModelIndex();
}

int PlotBookView::horizontalOffset() const
{
    return horizontalScrollBar()->value();
}

int PlotBookView::verticalOffset() const
{
    return verticalScrollBar()->value();
}

// TODO: Untested
bool PlotBookView::isIndexHidden(const QModelIndex &index) const
{
    bool isHidden = false;
    Plot* plot = _idx2Plot(index);
    if ( plot ) {
        isHidden = plot->isVisible();
    }
    return isHidden;
}

// TODO
void PlotBookView::setSelection(const QRect &rect,
                                QItemSelectionModel::SelectionFlags command)
{
    Q_UNUSED(rect);
    Q_UNUSED(command);
    qDebug() << "setSelection!!";
}

//
// This is really bogus right now
//
QRegion PlotBookView::visualRegionForSelection(
                               const QItemSelection &selection) const
{
    QModelIndexList idxs = selection.indexes();
    foreach ( QModelIndex idx, idxs ) {
        QWidget* page = _idx2Page(idx);
        if ( page ) {
            return QRegion(page->rect());
        }
        break; // do first one for now
    }
    return QRegion();
}

//
// This is really bogus right now
//
QItemSelectionModel::SelectionFlags PlotBookView::selectionCommand(
                           const QModelIndex &index, const QEvent *event) const
{
    Q_UNUSED(index);
    Q_UNUSED(event);
    return QItemSelectionModel::Select;
}

void PlotBookView::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_Up:_selectNextCurve(); break;
    case Qt::Key_Down: _selectPrevCurve();break;
    default: ; // do nothing
    }
    QAbstractItemView::keyPressEvent(event);
}

//
// This takes makes the plot at idx the only plot on the page
//
void PlotBookView::maximize(const QModelIndex &idx)
{
    if ( _plotModel->isIndex(idx, "Plot") ) {

        QGridLayout* grid = _idx2Grid(idx);
        Plot* plot = _idx2Plot(idx);

        for ( int i = 0; i < grid->count(); ++i) {
            QWidget* w = grid->itemAt(i)->widget();
            if ( w != plot ) {
                w->setVisible(false);
            } else {
                w->setVisible(true);
            }
        }
    }
}

void PlotBookView::minimize(const QModelIndex &idx)
{
    if ( _plotModel->isIndex(idx, "Plot") ) {
        QGridLayout* grid = _idx2Grid(idx);
        for ( int i = 0; i < grid->count(); ++i) {
            grid->itemAt(i)->widget()->setVisible(true);
        }
    }
}

void PlotBookView::dataChanged(const QModelIndex &topLeft,
                          const QModelIndex &bottomRight)
{

    if ( !topLeft.isValid() ) return;

    if ( topLeft == bottomRight ) {

        QModelIndex idx = topLeft;
        QStandardItem* item = _plotModel->itemFromIndex(idx);
        QString itemText = item->text();

        if ( itemText == "SessionStartTime" ||
             itemText == "SessionStopTime" ||
             itemText == "PageStartTime" ||
             itemText == "PageStopTime" ) {

            foreach ( QModelIndex pageIdx, _plotModel->pageIdxs() ) {
                QWidget* page = _idx2Page(pageIdx);
                foreach ( Plot* plot, _page2Plots.value(page) ) {
                    if ( itemText == "SessionStartTime" ||
                         itemText == "PageStartTime" ) {
                        plot->setStartTime(item->data().toDouble());
                    } else {
                        plot->setStopTime(item->data().toDouble());
                    }
                    plot->axisRect()->zoomToFit();
                    plot->replot();
                }
            }

        } else if ( itemText == "CurveColor" ) {

            TrickCurve* curve = _idx2Curve(idx.parent());
            QColor color(item->data().toString());
            QPen pen = curve->pen();
            pen.setColor(color);
            curve->setPen(pen);
        }
    }

    QAbstractItemView::dataChanged(topLeft, bottomRight);
    viewport()->update();
}

void PlotBookView::_plotSelectModelCurrentChanged(const QModelIndex &currIdx,
                                                 const QModelIndex &prevIdx)
{
    Q_UNUSED(prevIdx);
    if ( _plotModel->isIndex(currIdx, "Page") ) {
        setCurrentPage(currIdx);
    }
}

void PlotBookView::selectRun(int runId)
{
    if ( !model() || !selectionModel() ) return ;
    if ( model()->rowCount() <= 0 ) return;
    if (_plotModel->pageIdxs().empty()) return;

    QModelIndex page0Idx = _plotModel->pageIdxs().at(0);
    if ( model()->rowCount(page0Idx) <= 0 ) return ;

    // Build a selection of all curves that this RUN maps to
    QItemSelection runSelection;
    foreach ( QModelIndex pageIdx, _plotModel->pageIdxs() ) {
        foreach ( QModelIndex plotIdx, _plotModel->plotIdxs(pageIdx) ) {
            QModelIndex curvesIdx = _plotModel->getIndex(plotIdx,
                                                         "Curves", "Plot");
            QModelIndex curveIdx = model()->index(runId,0,curvesIdx);
            QItemSelection curveSelection(curveIdx,curveIdx);
            runSelection.merge(curveSelection, QItemSelectionModel::Select);
        }
    }

    // Make the selection
    selectionModel()->select(runSelection,QItemSelectionModel::ClearAndSelect);
}

void PlotBookView::_plotSelectModelSelectChanged(const QItemSelection &curr,
                                    const QItemSelection &prev)
{
    if ( curr.indexes().size() > 0 ) {
         // when tab changes, noop
        if ( _plotModel->isIndex(curr.indexes().at(0), "Page") ) return;
    }

    foreach ( QModelIndex prevIdx, prev.indexes() ) {
        TrickCurve* prevCurve = _idx2Curve(prevIdx);
        if ( prevCurve ) {
            prevCurve->setSelected(false);
            if ( curr.indexes().size() == 0 ) {
                // If curr > 0, replot done in next block
                QModelIndex plotIdx = _plotModel->getIndex(prevIdx, "Plot");
                Plot* plot = _idx2Plot(plotIdx);
                if ( plot->isVisible() ) {
                        plot->replot();
                }
            }
        }
    }

    foreach ( QModelIndex currIdx, curr.indexes() ) {
        TrickCurve* currCurve = _idx2Curve(currIdx);
        if ( currCurve ) {
            currCurve->setSelected(true);
            QModelIndex plotIdx = _plotModel->getIndex(currIdx, "Plot");
            Plot* plot = _idx2Plot(plotIdx);

            // If plot is on current page (not hidden) replot
            if ( plot->isVisible() ) {
                plot->replot();
            }
        }
    }

}

void PlotBookView::_slotCurveClicked(TrickCurve *curve)
{
    if ( curve == 0 ) {
        // Deselect
        selectionModel()->clear();
    } else {
        QModelIndex curveIdx = _curve2Idx(curve);
        int runId = curveIdx.row();
        selectRun(runId);
    }
}

void PlotBookView::_selectNextCurve()
{
    if ( ! selectionModel() || !model() ) return ;

    QItemSelection currSel = selectionModel()->selection();
    if ( currSel.size() > 0 ) {
        QModelIndex currIdx = currSel.indexes().at(0);
        if ( _plotModel->isIndex(currIdx, "Curve") ) {
            QModelIndex plotIdx = _plotModel->getIndex(currIdx, "Plot");
            int currRow = currIdx.row();
            int nextRow = currRow+1;
            if ( nextRow >= model()->rowCount(plotIdx) ) {
                nextRow = currRow;
            }
            selectRun(nextRow);
        }
    }
}

void PlotBookView::_selectPrevCurve()
{
    if ( ! selectionModel() || !model() ) return ;

    QItemSelection currSel = selectionModel()->selection();
    if ( currSel.size() > 0 ) {
        QModelIndex currIdx = currSel.indexes().at(0);
        if ( _plotModel->isIndex(currIdx, "Curve") ) {
            int currRow = currIdx.row();
            int prevRow = currRow-1;
            if ( prevRow < 0 ) {
                prevRow = 0;
            }
            selectRun(prevRow);
        }
    }
}

void PlotBookView::tabCloseRequested(int tabId)
{
    if ( model() == 0 ) return;

    QWidget* widget = _nb->widget(tabId);
    QModelIndex pageIdx = _page2Idx(widget);
    if ( pageIdx.isValid() ) {
        // Plot page
        QModelIndex pagesIdx = _plotModel->getIndex(QModelIndex(), "Pages");
        int row = pageIdx.row();
        model()->removeRow(row,pagesIdx);
    } else {
        // Table
        QModelIndex tableIdx = _table2Idx(widget);
        if ( tableIdx.isValid() ) {
            QModelIndex tablesIdx = _plotModel->getIndex(QModelIndex(),
                                                         "Tables");
            int row = tableIdx.row();
            model()->removeRow(row,tablesIdx);
        }
    }
}

void PlotBookView::tabCurrentChanged(int tabId)
{
    if ( !model() ) return;

    QWidget* page = _nb->widget(tabId);
    foreach (Plot* plot, _page2Plots.value(page) ) {
        if ( plot ) {
            plot->replot();
        }
    }
}

void PlotBookView::_closeAllPlots()
{
    int nTabs = _nb->count();
    for ( int i = nTabs ; i >= 0; --i ) {
        tabCloseRequested(i);
    }
}

void PlotBookView::_toggleDiffPlots()
{
    foreach ( QWidget* page, _pages ) {
        QVector<Plot*> plots = _page2Plots.value(page);
        foreach ( Plot* plot, plots ) {
            AxisRect* axisRect = (AxisRect*) plot->axisRect();
            axisRect->toggleCurveDiff();
        }
    }
}

void PlotBookView::doubleClick(QMouseEvent *event)
{
    if ( !model() ) return ;

    QPoint globalPageOrigin(_nb->mapToGlobal(QPoint(0,0)));
    QPoint pagePos = event->globalPos()-globalPageOrigin;
    QModelIndex plotIdx = indexAt(pagePos);
    if ( _plotModel->isIndex(plotIdx, "Plot") ) {
        QModelIndex pageIdx = _plotModel->getIndex(plotIdx, "Page");
        bool isExpanded = false;
        foreach ( QModelIndex idx, _plotModel->plotIdxs(pageIdx) ) {
            if ( idx != plotIdx ) {
                Plot* plot = _idx2Plot(idx);
                if ( plot->isVisible() ) {
                    isExpanded = false;
                } else {
                    isExpanded = true;
                }
                break;
            }
        }

        if ( isExpanded ) {
            minimize(plotIdx);
        } else {
            maximize(plotIdx);
        }
    }
}


void PlotBookView::plotKeyPress(QKeyEvent *e)
{
    switch (e->key()) {
    case Qt::Key_Up:{
        _selectPrevCurve();
        break;
    }
    case Qt::Key_Down: {
        _selectNextCurve();
        break;
    }
    case Qt::Key_Space:{
        _toggleDiffPlots();
        break;
    }
    }
}

void PlotBookView::_nbTabAboutToBeRemoved(int tabId)
{
    QList<QWidget*> widgets = _widget2notebookTab.keys();
    foreach ( QWidget* widget, widgets ) {
        int tid = _widget2notebookTab.value(widget);
        if ( tid > tabId ) {
            _widget2notebookTab[widget] = tid-1;
        }
    }
}

void PlotBookView::rowsInserted(const QModelIndex &pidx, int start, int end)
{
    QModelIndex gpidx = model()->parent(pidx);

    for ( int row = start; row < end+1; ++row) {

        QModelIndex idx = model()->index(row,0,pidx);
        QStandardItem* item = _plotModel->itemFromIndex(idx);
        QString itemText = item->text();

        if ( itemText == "Page" ) {

            // Do nothing right now

        } else  if ( itemText == "PageName" ) {

            QString pageName = model()->data(idx).toString();
            _insertPage(pageName);
            setCurrentIndex(idx);

        } else  if ( itemText == "PageTitle" ) {

            QString title = model()->data(idx).toString();
            QWidget* page = _idx2Page(pidx);
            _insertPageTitle(page,title);

        } else if ( itemText == "PageStartTime" ) {

            QWidget* page = _idx2Page(pidx);
            double pageStartTime = model()->data(idx).toDouble();
            _page2startTime.insert(page,pageStartTime);

        } else if ( itemText == "PageStopTime" ) {

            QWidget* page = _idx2Page(pidx);
            double pageStopTime = model()->data(idx).toDouble();
            _page2stopTime.insert(page,pageStopTime);

        } else if ( itemText == "PageBackgroundColor" ) {

            QWidget* page = _idx2Page(pidx);
            QString bgColor = model()->data(idx).toString();
            QPalette pal = page->palette();
            QColor qcolor(bgColor);
            pal.setColor(QPalette::Background, qcolor);
            page->setPalette(pal);
            PageTitleWidget* pw = _page2pagewidget.value(page);
            pw->setBackgroundColor(bgColor);

        } else if ( itemText == "PageForegroundColor" ) {

            QWidget* page = _idx2Page(pidx);
            QString fgColor = model()->data(idx).toString();
            QPalette pal = page->palette();
            QColor qcolor(fgColor);
            pal.setColor(QPalette::Foreground, qcolor);
            page->setPalette(pal);

        } else if ( itemText == "Plot" ) {

            QWidget* page = _idx2Page(pidx);
            QModelIndex pmIdx = _plotModel->getIndex(
                                       QModelIndex(), "SessionStartTime");
            double sessionStartTime = _plotModel->data(pmIdx).toDouble();
            pmIdx = _plotModel->getIndex(QModelIndex(),"SessionStopTime");
            double sessionStopTime = _plotModel->data(pmIdx).toDouble();
            QModelIndex pgIdx = _plotModel->getIndex(idx, "Page");
            pmIdx = _plotModel->getIndex(pgIdx, "PageBackgroundColor");
            QString pageBGColor = _plotModel->data(pmIdx).toString();
            _insertPlot(page, sessionStartTime, sessionStopTime, pageBGColor);

        }  else if ( itemText == "PlotXAxisLabel" ) {

            QString xAxisLabel = model()->data(idx).toString();
            Plot* plot = _idx2Plot(pidx);
            plot->setXAxisLabel(xAxisLabel);

        }  else if ( itemText == "PlotYAxisLabel" ) {

            QString yAxisLabel = model()->data(idx).toString();
            Plot* plot = _idx2Plot(pidx);
            plot->setYAxisLabel(yAxisLabel);

        } else if ( itemText == "CurveData" ) {

            QVariant v = model()->data(idx);
            TrickCurveModel* curveModel;
            curveModel = QVariantToPtr<TrickCurveModel>::convert(v);
            QString xunit = curveModel->x()->unit();
            QString yunit = curveModel->y()->unit();

            Plot* plot = _idx2Plot(gpidx);
            TrickCurve* curve = plot->axisRect()->addCurve(curveModel);
            plot->axisRect()->zoomToFit();
            _plot2Curves[plot].append(curve);

            QModelIndex plotIdx = _plotModel->getIndex(idx, "Plot");
            QModelIndex xAxisLabelIdx =
                    _plotModel->getIndex(plotIdx,"PlotXAxisLabel", "Plot");
            QString xAxisLabel = _plotModel->data(xAxisLabelIdx).toString();
            xAxisLabel = _appendUnitToLabel(xAxisLabel,xunit);
            plot->setXAxisLabel(xAxisLabel);

            QModelIndex yAxisLabelIdx =
                    _plotModel->getIndex(plotIdx,"PlotYAxisLabel", "Plot");
            QString yAxisLabel = _plotModel->data(yAxisLabelIdx).toString();
            yAxisLabel = _appendUnitToLabel(yAxisLabel,yunit);
            plot->setYAxisLabel(yAxisLabel);

            if ( pidx.row() == 1 && _isShowCurveDiff ) {
                plot->axisRect()->showCurveDiff();
            }

        } else if ( itemText == "CurveColor" ) {

            QString colorStr = model()->data(idx).toString();
            TrickCurve* curve =  _idx2Curve(pidx);
            if ( colorStr.isEmpty() ) {
                QString color = curve->pen().color().name();
                model()->setData(idx,color);
            } else {
                if ( !_isShowCurveDiff ) {
                    QColor color(colorStr);
                    QPen pen = curve->pen();
                    pen.setColor(color);
                    curve->setPen(pen);
                }
            }

        } else if ( itemText == "CurveXScale" ) {

            TrickCurve* curve =  _idx2Curve(pidx);
            double xsf = model()->data(idx).toDouble();
            curve->setXScaleFactor(xsf);

        } else if ( itemText == "CurveYScale" ) {

            TrickCurve* curve =  _idx2Curve(pidx);
            double ysf = model()->data(idx).toDouble();
            curve->setYScaleFactor(ysf);

        } else if ( itemText == "CurveXBias" ) {

            TrickCurve* curve =  _idx2Curve(pidx);
            double xbias = model()->data(idx).toDouble();
            curve->setXBias(xbias);

        } else if ( itemText == "CurveYBias" ) {

            TrickCurve* curve =  _idx2Curve(pidx);
            double ybias = model()->data(idx).toDouble();
            curve->setYBias(ybias);

        } else if ( itemText == "CurveSymbolStyle" ) {

            TrickCurve* curve =  _idx2Curve(pidx);
            QString symbol = model()->data(idx).toString();
            curve->setSymbolStyle(symbol);

        } else if ( itemText == "CurveSymbolSize" ) {

            TrickCurve* curve =  _idx2Curve(pidx);
            QString size = model()->data(idx).toString();
            curve->setSymbolSize(size);

        } else if ( itemText == "CurveLineStyle" ) {

            TrickCurve* curve =  _idx2Curve(pidx);
            QString lineStyle = model()->data(idx).toString();
            curve->setCurveStyle(lineStyle);

        } else if ( itemText == "CurveYLabel" ) {

            int nCurves = model()->rowCount(gpidx);

            // No labels if nCurves > 10 (helps speedup monte carlo immensely)
            if ( nCurves > 10 ) {
                return ;
            }

            // Get TrickCurve for this yLabelIndex
            TrickCurve* curve =  _idx2Curve(pidx);

            if ( _isShowCurveDiff && nCurves == 2 ) {

                QModelIndex crvsIdx = gpidx;

                QModelIndex crv0Idx = model()->index(0,0,crvsIdx);
                TrickCurve* c0 = _idx2Curve(crv0Idx);
                QFileInfo info0(c0->trkFile());
                QString run0 = info0.dir().dirName();

                QModelIndex crv1Idx = model()->index(1,0,crvsIdx);
                TrickCurve* c1 = _idx2Curve(crv1Idx);
                QFileInfo info1(c1->trkFile());
                QString run1 = info1.dir().dirName();

                if ( run0 != run1 ) {
                    // RUN_names are different
                    c0->setName(run0);
                    c1->setName(run1);
                } else {
                    // RUN_names are the same, so in different parent dirs
                    // UNLESS they are the exact same directory!
                    QString r0 = info0.absoluteDir().absolutePath();
                    QString r1 = info1.absoluteDir().absolutePath();
                    if ( r0 == r1 ) {
                        // Identical RUNs being compared!
                        c0->setName(run0);
                        c1->setName(run1);
                    } else {
                        QStringList rl0 = r0.split('/');
                        QStringList rl1 = r1.split('/');
                        r0.clear();
                        r1.clear();
                        while ( 1 ) {
                            QString s0 = rl0.takeLast();
                            QString s1 = rl1.takeLast();
                            if ( r0.isEmpty() ) {
                                r0 = s0;
                                r1 = s1;
                            } else {
                                r0 = s0 + "/" + r0;
                                r1 = s1 + "/" + r1;
                            }
                            if ( s0 != s1 ) {
                                break;
                            }
                            if ( rl0.isEmpty() ) {
                                if ( !rl1.isEmpty() ) {
                                    s1 = rl1.takeLast();
                                    r1 = s1 + "/" + r1;
                                }
                                break;
                            }
                            if ( rl1.isEmpty() ) {
                                if ( !rl0.isEmpty() ) {
                                    s0 = rl0.takeLast();
                                    r0 = s0 + "/" + r0;
                                }
                                break;
                            }
                        }

                        c0->setName(r0);
                        c1->setName(r1);
                    }
                }

            } else {

                // Do all the curves have the same param name?
                bool isAllVarsTheSame = true;
                QString var0 ;
                for ( int i = 0; i < nCurves; ++i ) {
                    QModelIndex crvIdx = model()->index(i,0,gpidx);
                    QModelIndex yIdx = _plotModel->getIndex(crvIdx,
                                                     "CurveYName", "Curve");
                    QString yVar = model()->data(yIdx).toString();
                    if ( i == 0 ) {
                        var0 = yVar;
                    } else {
                        if ( var0 != yVar ) {
                            isAllVarsTheSame = false;
                            break;
                        }
                    }
                }

                // Check if plot curves originate from different RUNs
                bool isRunsDifferent = false;
                QFileInfo trkFileInfo(curve->trkFile());
                QString runDir = trkFileInfo.dir().dirName();
                for ( int i = 0; i < nCurves; ++i ) {
                    QModelIndex curveIdx = model()->index(i,0,gpidx);
                    TrickCurve* c = _idx2Curve(curveIdx);
                    QFileInfo tInfo(c->trkFile());
                    if ( tInfo.dir().dirName() != runDir ) {
                        isRunsDifferent = true;
                        break;
                    }
                }

                if ( isRunsDifferent ) {
                    if ( isAllVarsTheSame ) {
                        // just show the RUN to avoid
                        // repetively showing the same param
                        for ( int i = 0; i < nCurves; ++i ) {
                            QModelIndex curveIdx = model()->index(i,0,gpidx);
                            TrickCurve* c = _idx2Curve(curveIdx);
                            QFileInfo tInfo(c->trkFile());
                            QString runDir = tInfo.dir().dirName();
                            c->setName(runDir);
                        }
                    } else {
                        for ( int i = 0; i < nCurves; ++i ) {
                            QModelIndex crvIdx = model()->index(i,0,gpidx);
                            QModelIndex labelIdx = _plotModel->getIndex(crvIdx,
                                                     "CurveYLabel", "Curve");
                            QString label = model()->data(labelIdx).toString();
                            if ( label.isEmpty() ) {
                                QModelIndex yIdx = _plotModel->getIndex(crvIdx,
                                                     "CurveYName", "Curve");
                                QString yVar = model()->data(yIdx).toString();
                                QStringList yVarList = yVar.split(".");
                                label = yVarList.last();
                            }
                            TrickCurve* c = _idx2Curve(crvIdx);
                            QFileInfo tInfo(c->trkFile());
                            QString runDir = tInfo.dir().dirName();
                            QString yLabel = label + " (" + runDir + ")";
                            c->setName(yLabel);
                        }
                    }
                } else {

                    // All the same RUN, then only show label

                    // Legend label as specified in DP file
                    QString yLabel = model()->data(idx).toString();
                    if ( yLabel.isEmpty() ) {
                        // If label is empty, use last part of parameter name
                        // E.g. if parameter is ball.state.out.position[0],
                        //      use position[0]
                        QModelIndex yIdx = _plotModel->getIndex(pidx,
                                                "CurveYName", "Curve");
                        QString yVar = model()->data(yIdx).toString();
                        QStringList yVarList = yVar.split(".");
                        yLabel = yVarList.last();
                    }
                    curve->setName(yLabel);
                }
            }

            if ( nCurves == 1 || nCurves > 8 ) {
                curve->parentPlot()->legend->setVisible(false);
            } else {
                curve->parentPlot()->legend->setVisible(true);
            }

            Plot* plot = _idx2Plot(pidx);
            QCPItemText* plotTitle = plot->title();
            QColor fgColor = plotTitle->color();
            curve->parentPlot()->legend->setTextColor(fgColor);

        } else if ( itemText == "PlotTitle" ) {

            QString title = model()->data(idx).toString();
            Plot* plot = _idx2Plot(pidx);
            plot->setTitle(title);

        } else if ( itemText == "PlotXMinRange" ) {

            double xMin = model()->data(idx).toDouble();
            Plot* plot = _idx2Plot(pidx);
            plot->setXMinRange(xMin);

        } else if ( itemText == "PlotXMaxRange" ) {

            double xMax = model()->data(idx).toDouble();
            Plot* plot = _idx2Plot(pidx);
            plot->setXMaxRange(xMax);

        } else if ( itemText == "PlotYMinRange" ) {

            double yMin = model()->data(idx).toDouble();
            Plot* plot = _idx2Plot(pidx);
            plot->setYMinRange(yMin);

        } else if ( itemText == "PlotYMaxRange" ) {

            double yMax = model()->data(idx).toDouble();
            Plot* plot = _idx2Plot(pidx);
            plot->setYMaxRange(yMax);

        } else if ( itemText == "PlotStartTime" ) {

            QWidget* page = _idx2Page(pidx);
            Plot* plot = _idx2Plot(pidx);
            QModelIndex sIdx = _plotModel->getIndex(QModelIndex(),
                                                    "SessionStartTime");
            double sessionStartTime = _plotModel->data(sIdx).toDouble();
            double plotStartTime = model()->data(idx).toDouble();
            double pageStartTime = _page2startTime.value(page);
            if ( sessionStartTime > -1.0e30 ) {
                plot->setStartTime(sessionStartTime);
            } else if ( plotStartTime > -1.0e30 ) {
                plot->setStartTime(plotStartTime);
            } else if ( pageStartTime > -1.0e30 ) {
                plot->setStartTime(pageStartTime);
            }

        } else if ( itemText == "PlotStopTime" ) {

            QWidget* page = _idx2Page(pidx);
            Plot* plot = _idx2Plot(pidx);
            QModelIndex sIdx = _plotModel->getIndex(QModelIndex(),
                                                    "SessionStopTime");
            double sessionStopTime = _plotModel->data(sIdx).toDouble();
            double plotStopTime = model()->data(idx).toDouble();
            double pageStopTime = _page2stopTime.value(page);
            if ( sessionStopTime < 1.0e30 ) {
                plot->setStopTime(sessionStopTime);
            } else if ( plotStopTime < 1.0e30 ) {
                plot->setStopTime(plotStopTime);
            } else if ( pageStopTime < 1.0e30 ) {
                plot->setStopTime(pageStopTime);
            }

        } else if ( itemText == "PlotGrid" ) {

            Plot* plot = _idx2Plot(pidx);
            QString isGridStr = model()->data(idx).toString();
            bool ok;
            bool isGrid = Options::stringToBool(isGridStr,&ok);
            plot->setGrid(isGrid);

        } else if ( itemText == "PlotGridColor" ) {

            Plot* plot = _idx2Plot(pidx);
            QString gridColor = model()->data(idx).toString();
            plot->setGridColor(gridColor);

        } else if ( itemText == "PlotBackgroundColor" ) {

            Plot* plot = _idx2Plot(pidx);
            QString bgColor = model()->data(idx).toString();
            if ( bgColor == "#FFFFFF" ) {
                // This assumes that the plot was not explicitly set to white
                QModelIndex pgIdx = _plotModel->getIndex(pidx, "Page");
                QModelIndex pageBGColorIdx = _plotModel->getIndex(pgIdx,
                                                         "PageBackgroundColor",
                                                         "Page");
                bgColor = model()->data(pageBGColorIdx).toString();
            }
            plot->setBackgroundColor(bgColor);

        } else if ( itemText == "PlotForegroundColor" ) {

            QString col = model()->data(idx).toString();
            if ( col.isEmpty() ) {  // TODO - use empty str for bg color def
                QWidget* page = _idx2Page(pidx);
                QPalette pal = page->palette();
                col = pal.color(QPalette::Foreground).name();
            }
            QColor fgColor(col);
            QPen fgPen(fgColor);

            Plot* plot = _idx2Plot(pidx);

            // Axis colors
            QCPAxis* xAxisBot  = plot->axisRect()->axis(QCPAxis::atBottom,0);
            QCPAxis* xAxisTop = plot->axisRect()->axis(QCPAxis::atTop,0);
            QCPAxis* yAxisLeft  = plot->axisRect()->axis(QCPAxis::atLeft,0);
            QCPAxis* yAxisRight = plot->axisRect()->axis(QCPAxis::atRight,0);
            QList<QCPAxis*> axes;
            axes << xAxisBot << xAxisTop << yAxisLeft << yAxisRight;
            foreach ( QCPAxis* axis, axes ) {
                axis->setTickLabelColor(fgColor);
                axis->setTickPen(fgPen);
                axis->setSubTickPen(fgPen);
                axis->setLabelColor(fgColor);
                axis->setBasePen(fgPen);
            }

            // Plot title
            QCPItemText* plotTitle = plot->title();
            plotTitle->setColor(fgColor);

        } else if ( itemText == "PlotFont" ) {
            QString fontStr = model()->data(idx).toString();
            if ( !fontStr.isEmpty() ) {

                // Font string should be the font *family* e.g.
                // helvetica, utopia, Times, lucida, courier, bitstream,
                // liberation sans l, urw bookman l, nimbus sans l
                // Do NOT use the full raw name that xlsfonts gives

                Plot* plot = _idx2Plot(pidx);
                QCPAxis* xAxis = plot->axisRect()->axis(QCPAxis::atBottom,0);
                QCPAxis* yAxis = plot->axisRect()->axis(QCPAxis::atLeft,0);

                // Ticks
                QFont tickFont = xAxis->tickLabelFont();
                tickFont.setFamily(fontStr);
                xAxis->setTickLabelFont(tickFont);
                yAxis->setTickLabelFont(tickFont);

                // XY Axis Labels
                QFont xAxisFont = xAxis->labelFont();
                xAxisFont.setFamily(fontStr);
                xAxis->setLabelFont(xAxisFont);
                QFont yAxisFont = yAxis->labelFont();
                yAxisFont.setFamily(fontStr);
                yAxis->setLabelFont(yAxisFont);

                // Four page titles
                QWidget* page = _idx2Page(pidx);
                PageTitleWidget* pw = _page2pagewidget.value(page);
                QFont pageTitleFont;
                pageTitleFont = pw->font();
                pageTitleFont.setFamily(fontStr);
                pw->setFont(pageTitleFont);

                // Plot title
                QCPItemText* plotTitle = plot->title();
                QFont plotTitleFont = plotTitle->font();
                plotTitleFont = plotTitle->font();
                plotTitleFont.setFamily(fontStr);
                plotTitle->setFont(pageTitleFont);

                // Plot legend
                plot->legend->setFont(yAxisFont);
            }

        } else if ( itemText == "Table" ) {
        } else if ( itemText == "TableVarCount" ) {
        } else if ( itemText == "TableVars" ) {
            // When TableVars item is inserted into the tree,
            // the table is created, so things like TableName is accessed
            // and assumed present

            // Create table view
            QTableView* table = new QTableView(this);

            // Create table model
            QModelIndex tableIdx = _plotModel->getIndex(idx, "Table",
                                                        "TableVars");
            QModelIndex varCountIdx = _plotModel->getIndex(tableIdx,
                                                           "TableVarCount",
                                                           "Table");
            int varCount = _plotModel->data(varCountIdx).toInt();
            _tables.append(table);
            QStandardItemModel* tableModel = new QStandardItemModel(0,
                                                                    varCount+1);
            table->setModel(tableModel);

            QModelIndex tableNameIdx = _plotModel->getIndex(tableIdx,
                                                           "TableName",
                                                           "Table");
            QString tableName = model()->data(tableNameIdx).toString();
            _nb->addTab(table,QFileInfo(tableName).baseName());
            int nbIdx = _nb->count()-1;
            _widget2notebookTab.insert(table,nbIdx);
            _nb->setCurrentIndex(nbIdx);
            _nb->setAttribute(Qt::WA_AlwaysShowToolTips, true);

        } else if ( itemText == "TableVarRunId" ) {
        } else if ( itemText == "TableCurveData" ) {


            QVariant v = model()->data(idx);
            TrickCurveModel* curveModel;
            curveModel = QVariantToPtr<TrickCurveModel>::convert(v);
            QString yunit = curveModel->y()->unit();
            QString yName = curveModel->y()->name();
            QString yHeaderLabel = _appendUnitToLabel(yName,yunit);

            QModelIndex tableIdx = _plotModel->getIndex(idx, "Table",
                                                        "TableCurveData");
            int tableRow = tableIdx.row();
            QTableView* table = _tables.at(tableRow);
            QStandardItemModel* tableModel =  (QStandardItemModel*)
                                              table->model();

            // Get table column number for data
            QModelIndex varsIdx = _plotModel->getIndex(tableIdx,
                                                       "TableVars",
                                                       "Table");
            int ycol = model()->rowCount(varsIdx);

            // Set table column header to name
            QStandardItem* yItem = new QStandardItem(yHeaderLabel);

            // Set column width to what header label text width
            tableModel->setHorizontalHeaderItem(ycol,yItem);
            QHeaderView* header = table->horizontalHeader();
            int width = header->sectionSizeHint(ycol);
            table->setColumnWidth(ycol,width);

            // Scale/bias
            QModelIndex sfIdx = _plotModel->getIndex(pidx, "TableVarScale",
                                                     "Curve");
            double sf = _plotModel->data(sfIdx).toDouble();
            QModelIndex biasIdx = _plotModel->getIndex(pidx, "TableVarBias",
                                                     "Curve");
            double bias = _plotModel->data(biasIdx).toDouble();

            // Min/Max Range
            QModelIndex minRangeIdx = _plotModel->getIndex(pidx,
                                                           "TableVarMinRange",
                                                           "Curve");
            QModelIndex maxRangeIdx = _plotModel->getIndex(pidx,
                                                           "TableVarMaxRange",
                                                           "Curve");
            double minRange = _plotModel->data(minRangeIdx).toDouble();
            double maxRange = _plotModel->data(maxRangeIdx).toDouble();


//#define TIME_ME
#ifdef TIME_ME
                TimeItLinux timer;
                 timer.start();
#endif

            if ( ycol == 1 ) {

                // Set time label on column 0
                QString tunit = curveModel->t()->unit();
                QString tHeaderLabel = _appendUnitToLabel("Time",tunit);
                QStandardItem* tItem = new QStandardItem(tHeaderLabel);
                tableModel->setHorizontalHeaderItem(0,tItem);

                // First column
                tableModel->setRowCount(curveModel->rowCount());
                curveModel->map();
                TrickModelIterator it = curveModel->begin();
                const TrickModelIterator e = curveModel->end();
                int row = 0;
                while (it != e) {
                    QModelIndex tIdx = tableModel->index(row,0);
                    tableModel->setData(tIdx,it.t());
                    _setTableData(tableModel,row,ycol,
                                  it.y(),sf,bias,minRange,maxRange);
                    ++row;
                    ++it;
                }
                curveModel->unmap();
            } else {

                // Insert data from curve, making sure timestamps line up
                curveModel->map();
                TrickModelIterator it = curveModel->begin();
                const TrickModelIterator e = curveModel->end();

                for ( int i = 0; i < tableModel->rowCount(); ++i ) {
                    // rowCount() changes, keep checking rowCount() in for loop
                    QModelIndex tIdx = tableModel->index(i,0);
                    double t0 = tableModel->data(tIdx).toDouble();
                    double ty = it.t();
                    if ( qAbs(ty-t0) < 1.0e-9 ) {
                        _setTableData(tableModel,i,ycol,
                                      it.y(),sf,bias,minRange,maxRange);
                        ++it;
                    } else if ( ty < t0 ) {
                        tableModel->insertRow(i);
                        tIdx = tableModel->index(i,0);
                        tableModel->setData(tIdx,ty);
                        _setTableData(tableModel,i,ycol,
                                      it.y(),sf,bias,minRange,maxRange);
                        ++it;
                    } else  {
                       // t0 < t1
                    }
                    if ( it == e ) {
                        break;
                    }
                }

                int i = tableModel->rowCount();
                while (it != e) {
                    tableModel->insertRow(i);
                    QModelIndex tIdx = tableModel->index(i,0);
                    tableModel->setData(tIdx,it.t());
                    _setTableData(tableModel,i,ycol,
                                  it.y(),sf,bias,minRange,maxRange);
                    ++it;
                    ++i;
                }
                curveModel->unmap();
            }

#ifdef TIME_ME
                 timer.snap("loadtime(tablevar)=");
#endif

            /*
            if ( pidx.row() == 1 && _isShowCurveDiff ) {
                plot->axisRect()->showCurveDiff();
            }
            */

        } else {
#if 0
            qDebug() << "snap [bad scoobies] : PlotBookView::rowInserted() "
                      "Could not handle index " << idx << " with item text "
                        << itemText ;
            exit(-1);
#endif
        }
    }
}

void PlotBookView::_setTableData(QStandardItemModel *table,
                                 int row, int col,
                                 double val,
                                 double sf, double bias,
                                 double minRange, double maxRange)
{
    QModelIndex idx = table->index(row,col);
    double y = val*sf+bias;
    if ( y < minRange ) {
        y = minRange;
    } else if ( y > maxRange ) {
        y = maxRange;
    }
    table->setData(idx,y);
}

void PlotBookView::rowsAboutToBeRemoved(const QModelIndex &pidx,
                                        int start, int end)
{
    for ( int row = start; row < end+1; ++row) {

        QModelIndex idx = model()->index(row,0,pidx);

        if ( _plotModel->isIndex(pidx, "Pages") ) {
            // Page
            QWidget* page = _idx2Page(idx);
            int row = _plotModel->pageIdxs().indexOf(idx);
            _pages.remove(row);
            _page2grid.remove(page);
            _page2pagewidget.remove(page);
            _page2startTime.remove(page);
            _page2stopTime.remove(page);
            foreach ( Plot* plot, _page2Plots.value(page) ) {
                _plot2Curves.remove(plot);
            }
            _page2Plots.remove(page);
            int tabId = _widget2notebookTab.value(page);
            _nbTabAboutToBeRemoved(tabId);
            page->deleteLater();

        } else if ( _plotModel->isIndex(pidx, "Tables") ) {
            QTableView* tableView = _tables.at(row);
            int tabId = _widget2notebookTab.value(tableView);
            _nbTabAboutToBeRemoved(tabId);
            _tables.remove(row);
            delete tableView->model();
            tableView->deleteLater();
        } else if ( _plotModel->isIndex(idx, "Plot") ) {
            // Plot
            QWidget* page = _idx2Page(pidx);
            QGridLayout* grid = static_cast<QGridLayout*>(page->layout());

            Plot* plot = _idx2Plot(idx);
            QVector<TrickCurve*> curves = _plot2Curves.value(plot);
            foreach ( TrickCurve* curve, curves ) {
                disconnect(curve,SIGNAL(selectionChanged(TrickCurve*)));
            }
            int plotId = _page2Plots[page].indexOf(plot);
            _page2Plots[page].remove(plotId);
            _plot2Curves.remove(plot);
            grid->removeWidget(plot);
            delete plot;

            QList<QWidget*> plots;
            int nPlots = grid->count();
            for ( int i = nPlots-1; i >= 0 ; --i ) {
                QWidget* plot = grid->takeAt(i)->widget();
                plots << plot;
            }

            switch (nPlots)
            {
            case 0:
            {
                break;
            }
            case 1:
            {
                grid->addWidget(plots.at(0),0,0);
                break;
            }
            case 2:
            {
                grid->addWidget(plots.at(0),0,0);
                grid->addWidget(plots.at(1),1,0);
                break;
            }
            case 3:
            {
                grid->addWidget(plots.at(0),0,0);
                grid->addWidget(plots.at(1),1,0);
                grid->addWidget(plots.at(2),2,0);
                break;
            }
            case 4:
            {
                grid->addWidget(plots.at(0),0,0);
                grid->addWidget(plots.at(1),0,1);
                grid->addWidget(plots.at(2),1,0);
                grid->addWidget(plots.at(3),1,1);
                break;
            }
            case 5:
            {
                grid->addWidget(plots.at(0),0,0);
                grid->addWidget(plots.at(1),0,1);
                grid->addWidget(plots.at(2),1,0);
                grid->addWidget(plots.at(3),1,1);
                grid->addWidget(plots.at(4),2,0,1,2);
                break;
            }
            case 6:
            {
                grid->addWidget(plots.at(0),0,0);
                grid->addWidget(plots.at(1),0,1);
                grid->addWidget(plots.at(2),1,0);
                grid->addWidget(plots.at(3),1,1);
                grid->addWidget(plots.at(4),2,0);
                grid->addWidget(plots.at(5),2,1);
                break;
            }
            case 7:
            {
                grid->addWidget(plots.at(0),0,0);
                grid->addWidget(plots.at(1),0,1);
                grid->addWidget(plots.at(2),1,0);
                grid->addWidget(plots.at(3),1,1);
                grid->addWidget(plots.at(4),2,0);
                grid->addWidget(plots.at(5),2,1);
                grid->addWidget(plots.at(6),3,0,1,2);
                break;
            }
            default:
            {
                qDebug() << "snap [error]: can't handle more than 7 plots!";
            }
            }

        } else if ( _plotModel->isIndex(pidx, "Curves") ) {

            // TODO: untested
            Plot* plot = _idx2Plot(pidx);
            TrickCurve* curve = _idx2Curve(idx);
            disconnect(curve,SIGNAL(selectionChanged(TrickCurve*)));
            plot->removePlottable(curve);
            _plot2Curves[plot].remove(idx.row());
            plot->replot();
        }
    }
}

QModelIndex PlotBookView::_curve2Idx(TrickCurve *curve)
{
    Plot* plot = static_cast<Plot*>(curve->parentPlot());
    QModelIndex plotIdx = _plot2Idx(plot);
    int rowCurve = _plot2Curves.value(plot).indexOf(curve);
    QModelIndex curvesIdx = _plotModel->getIndex(plotIdx, "Curves", "Plot");
    QModelIndex curveIdx = model()->index(rowCurve,0,curvesIdx);
    return curveIdx;
}

QWidget *PlotBookView::_idx2Page(const QModelIndex &idx) const
{
    QModelIndex pageIdx;

    pageIdx = _plotModel->getIndex(idx, "Page");
    if ( !pageIdx.isValid() )  return 0;

    int r = _plotModel->pageIdxs().indexOf(pageIdx);
    QWidget* page = _pages.at(r);

    return page;
}

QGridLayout *PlotBookView::_idx2Grid(const QModelIndex &idx) const
{
    QWidget* page = _idx2Page(idx);
    QGridLayout* grid = _page2grid.value(page);
    return grid;
}

Plot *PlotBookView::_idx2Plot(const QModelIndex &idx) const
{
    Plot* plot = 0 ;

    QModelIndex plotIdx = _plotModel->getIndex(idx, "Plot");
    if ( !plotIdx.isValid() )  return 0;

    QModelIndex pageIdx = _plotModel->getIndex(plotIdx, "Page");
    int iPlot = _plotModel->plotIdxs(pageIdx).indexOf(plotIdx);
    if ( iPlot >= 0 ) {
        QWidget* page = _idx2Page(pageIdx);
        plot = _page2Plots.value(page).at(iPlot);
    }

    return plot;
}

TrickCurve *PlotBookView::_idx2Curve(const QModelIndex &idx) const
{
    TrickCurve* curve = 0 ;
    Plot* plot = _idx2Plot(idx);
    if ( plot ) {
        curve = _plot2Curves.value(plot).at(idx.row());
    }
    return curve;
}

QModelIndex PlotBookView::_page2Idx(QWidget *page) const
{
    if ( !model() || !page) return QModelIndex();

    QModelIndex idx;
    int row = _pages.indexOf(page);
    if ( row >= 0 ) {
        idx = _plotModel->pageIdxs().at(row);
    }
    return idx;
}

QModelIndex PlotBookView::_table2Idx(QWidget *tableView) const
{
    if ( !model() || !tableView) return QModelIndex();

    QModelIndex idx;
    int row = _tables.indexOf((QTableView*)tableView);
    if ( row >= 0 ) {
        QModelIndex tablesIdx = _plotModel->getIndex(QModelIndex(), "Tables");
        idx = _plotModel->index(row,0,tablesIdx);
    }
    return idx;
}


QModelIndex PlotBookView::_plot2Idx(Plot *plot) const
{
    if ( !model() || !plot ) return QModelIndex();

    QModelIndex plotIdx;
    QWidget* page = plot->parentWidget();
    int rowPage = _pages.indexOf(page);
    QModelIndex pageIdx = _plotModel->pageIdxs().at(rowPage);
    QModelIndexList plotIdxs = _plotModel->plotIdxs(pageIdx);
    int iPlot =  _page2Plots.value(page).indexOf(plot);
    if ( iPlot >= 0 ) {
        plotIdx = plotIdxs.at(iPlot);
    }
    return plotIdx;
}

QString PlotBookView::_appendUnitToLabel(const QString& labelInput,
                                            const QString& unit ) const
{
    QString label = labelInput;
    QString curveUnit = unit;

    if ( label.contains('(') ) {
        // assumes that label parens are for units
        QString labelUnit;
        int a = label.lastIndexOf('(');
        int b = label.lastIndexOf(')');
        if ( b > 0 ) {
            labelUnit = label.mid(a+1,b-a-1);
        }
        if ( labelUnit != curveUnit && labelUnit !="--" ) {
            // units not the same across curves, so make unit (--)
            label.replace(a+1,labelUnit.size(),"--");
        }
    } else {

        label += " (";
        label += curveUnit;
        label += ")";
    }

    return label;
}

void PlotBookView::_insertPage(const QString &dpFileName)
{
    // Page background
    QPalette pal(palette());
    pal.setColor(QPalette::Background, Qt::white);

    // Create/configure page widget
    QFrame* page = new QFrame;
    page->setAutoFillBackground(true);
    page->setPalette(pal);
    _pages.append(page);

    // Create/configure gridlayout for page
    QGridLayout* grid = new QGridLayout(page);
    grid->setContentsMargins(0, 0, 0, 0);
    grid->setSpacing(0);
    _page2grid.insert(page,grid);

    // Add Title Widget to page
    PageTitleWidget* pw = new PageTitleWidget(page);
    _page2pagewidget.insert(page,pw);
    grid->addWidget(pw,0,0,1,100);

    // Add Page to Notebook and make this page current (show page)
    _nb->addTab(page,QFileInfo(dpFileName).baseName());
    _widget2notebookTab.insert(page,_nb->count()-1);
    int nbIdx = _nb->count()-1;
    _nb->setCurrentIndex(nbIdx);
    _nb->setAttribute(Qt::WA_AlwaysShowToolTips, true);
}

void PlotBookView::_insertPageTitle(QWidget *page, const QString &title)
{
    PageTitleWidget* pw = _page2pagewidget.value(page);
    if ( !_titles.at(0).isEmpty() ) {
        pw->setTitle1(_titles.at(0));
    } else if ( !title.startsWith("QP_")  && title != "Page" ) {
        pw->setTitle1(title);
    } else {
        pw->setTitle1("Snap Plot");
    }
    if ( !_titles.at(1).isEmpty() ) {
        pw->setTitle2(_titles.at(1));
    }
    if ( !_titles.at(2).isEmpty() ) {
        pw->setTitle3(_titles.at(2));
    }
    if ( !_titles.at(3).isEmpty() ) {
        pw->setTitle4(_titles.at(3));
    }
}

void PlotBookView::_insertPlot(QWidget *page,
                               double startTime, double stopTime,
                               const QString& pageBGColor )
{

    // Create/configure plot
    Plot* plot = new Plot(page);
    plot->setStartTime(startTime);
    plot->setStopTime(stopTime);
    plot->setBackgroundColor(pageBGColor);
    connect(plot,SIGNAL(mouseDoubleClick(QMouseEvent*)),
            this,SLOT(doubleClick(QMouseEvent*)));
    connect(plot, SIGNAL(keyPress(QKeyEvent*)),
            this, SLOT(plotKeyPress(QKeyEvent*)));
    connect(plot, SIGNAL(curveClicked(TrickCurve*)),
            this, SLOT(_slotCurveClicked(TrickCurve*)));
    _page2Plots[page].append(plot);


    QGridLayout* grid = _page2grid.value(page);
    int nPlots = _page2Plots[page].size();
    switch ( nPlots ) {
    case 1: {
        grid->addWidget(plot,1,0);
        break;
    }
    case 2: {
        grid->addWidget(plot,2,0);
        break;
    }
    case 3: {
        grid->addWidget(plot,3,0);
        break;
    }
    case 4: {
        QWidget* w2 = grid->itemAtPosition(2,0)->widget();
        QWidget* w3 = grid->itemAtPosition(3,0)->widget();
        grid->removeWidget(w2);
        grid->removeWidget(w3);
        grid->addWidget(w2,1,1);
        grid->addWidget(w3,2,0);
        grid->addWidget(plot,2,1);
        break;
    }
    case 5: {
        grid->addWidget(plot,3,0,1,2);
        break;
    }
    case 6: {
        QWidget* w2 = grid->itemAtPosition(1,1)->widget();
        QWidget* w3 = grid->itemAtPosition(2,0)->widget();
        QWidget* w4 = grid->itemAtPosition(2,1)->widget();
        QWidget* w5 = grid->itemAtPosition(3,0)->widget();
        grid->removeWidget(w2);
        grid->removeWidget(w3);
        grid->removeWidget(w4);
        grid->removeWidget(w5);
        grid->addWidget(w2,2,0);
        grid->addWidget(w3,3,0);
        grid->addWidget(w4,1,1);
        grid->addWidget(w5,2,1);
        grid->addWidget(plot,3,1);
        break;
    }
    case 7: {
        grid->addWidget(plot,4,0,1,2);
        break;
    }
    default: {
        qDebug() << "snap limitation: 7 plots max on DP :(";
        qDebug() << "snap will probably crash now!";
    }
    }
}
