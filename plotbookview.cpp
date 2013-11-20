#include "plotbookview.h"
#include <QDebug>

PlotBookView::PlotBookView(QWidget *parent) :
    QAbstractItemView(parent),
    _monteModel(0),
    _isTabCloseRequested(false),
    _currSelectedRun(-1)
{
    _nb = new QTabWidget(parent);
    _nb->setTabsClosable(true);
    connect(_nb,SIGNAL(tabCloseRequested(int)),
            this,SLOT(tabCloseRequested(int)));
}

PlotBookView::~PlotBookView()
{
    // I think, not sure, the grids, plots frames are deleted with notebook
    // so nothing to do
}

void PlotBookView::setData(MonteModel *monteModel)
{
    _monteModel = monteModel;
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
            SLOT(plotBookViewCurrentChanged(QModelIndex,QModelIndex)));
    connect(selectionModel,
            SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
            this,
            SLOT(plotBookViewSelectionChanged(QItemSelection,QItemSelection)));
    /*
    connect(_nb, SIGNAL(currentChanged(int)),
            this, SLOT(plotBookViewTabWidgetCurrentChanged(int)));
        */
    QAbstractItemView::setSelectionModel(selectionModel);
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
    if ( _isPlotIdx(idx) ) {

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
    if ( _isPlotIdx(idx) ) {
        QGridLayout* grid = _idx2Grid(idx);
        for ( int i = 0; i < grid->count(); ++i) {
            grid->itemAt(i)->widget()->setVisible(true);
        }
    }
}

void PlotBookView::selectRun(int runId)
{
    if ( !model() || !selectionModel() ) return ;
    if ( model()->rowCount() <= 0 ) return;
    QModelIndex page0Idx = model()->index(0,0);
    if ( model()->rowCount(page0Idx) <= 0 ) return ;

    // If run already selected, do not reselect
    if ( runId == _currSelectedRun ) {
        return;
    }
    _currSelectedRun = runId;


    // Build a selection of all curves that this RUN maps to
    QItemSelection runSelection;
    int nPages = model()->rowCount();
    for ( int pageRow = 0; pageRow < nPages; ++pageRow) {
        QModelIndex pageIdx = model()->index(pageRow,0);
        int nPlots = model()->rowCount(pageIdx);
        for ( int plotRow = 0; plotRow < nPlots; ++plotRow) {
            QModelIndex plotIdx = model()->index(plotRow,0,pageIdx);
            QModelIndex curveIdx = model()->index(runId,0,plotIdx);
            QItemSelection curveSelection(curveIdx,curveIdx);
            runSelection.merge(curveSelection, QItemSelectionModel::Select);
        }
    }

    // Make the selection
    selectionModel()->select(runSelection,QItemSelectionModel::ClearAndSelect);
    selectionModel()->setCurrentIndex(runSelection.indexes().at(0),
                                      QItemSelectionModel::ClearAndSelect);
}

//
// This is really bogus right now
//
void PlotBookView::dataChanged(const QModelIndex &topLeft,
                          const QModelIndex &bottomRight)
{
    QAbstractItemView::dataChanged(topLeft, bottomRight);
    viewport()->update();
}

void PlotBookView::plotBookViewCurrentChanged(const QModelIndex &currIdx,
                                  const QModelIndex &prevIdx)
{
    Q_UNUSED(prevIdx);

    //
    // Update notebook widget index
    //
    int level = 0 ;
    QModelIndex idx(currIdx);
    while ( idx.parent().isValid() ) {
        idx = idx.parent();
        level++;
    }

    // If page selection updated, make tab widget page current
    if ( level == 0 ) {
        _nb->setCurrentIndex(idx.row()); // modelIdx.row == tabbedWidgetIndex
    }

    //
    // If currIdx is a curve index, select curve
    //
    if ( level == 2 ) {
        TrickCurve* currCurve = _idx2Curve(currIdx);
        if ( currCurve ) {
            int runId = currIdx.row();
            selectRun(runId);
        }
    }
}

void PlotBookView::plotBookViewSelectionChanged(const QItemSelection &curr,
                                    const QItemSelection &prev)
{
    if ( curr.isEmpty() ) return;

    foreach ( QModelIndex prevIdx, prev.indexes() ) {
        TrickCurve* prevCurve = _idx2Curve(prevIdx);
        if ( prevCurve ) {
            prevCurve->setSelected(false);
        }
    }

    foreach ( QModelIndex currIdx, curr.indexes() ) {
        TrickCurve* currCurve = _idx2Curve(currIdx);
        if ( currCurve ) {
            currCurve->setSelected(true);
            QModelIndex plotIdx = currIdx.parent();
            Plot* plot = _idx2Plot(plotIdx);
            plot->replot();
        }
    }
}

void PlotBookView::plotBookViewTabWidgetCurrentChanged(int currTab)
{
    if ( ! selectionModel() ) {
        return;
    }

    QModelIndex idx = _nbId2Idx(currTab);
    selectionModel()->setCurrentIndex(idx,QItemSelectionModel::ClearAndSelect);
}

void PlotBookView::selectionChangedTrickCurve(TrickCurve* curve)
{
    if ( model() == 0 || selectionModel() == 0 )  {
        return;
    }

    if ( curve->selected() ) {

        QModelIndex curveIdx = _curve2Idx(curve);

        selectionModel()->setCurrentIndex(curveIdx,
                                         QItemSelectionModel::ClearAndSelect);
    }
}

void PlotBookView::tabCloseRequested(int tabId)
{
    if ( model() == 0 ) return;

    _isTabCloseRequested = true;
    model()->removeRow(tabId);
    _isTabCloseRequested = false;
}

void PlotBookView::doubleClick(QMouseEvent *event)
{
    if ( !model() ) return ;

    QPoint globalPageOrigin(_nb->mapToGlobal(QPoint(0,0)));
    QPoint pagePos = event->globalPos()-globalPageOrigin;
    QModelIndex plotIdx = indexAt(pagePos);
    if ( _isPlotIdx(plotIdx) ) {
        QModelIndex pageIdx = plotIdx.parent();
        int rc = model()->rowCount(pageIdx);
        bool isExpanded = false;
        for ( int r = 0; r < rc; ++r) {
            QModelIndex idx = model()->index(r,0,pageIdx);
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
    }
}

void PlotBookView::rowsInserted(const QModelIndex &pidx, int start, int end)
{
    QModelIndex gpidx = model()->parent(pidx);
    QModelIndex g2pidx = model()->parent(gpidx);
    QModelIndex g3pidx = model()->parent(g2pidx);

    for ( int row = start; row < end+1; ++row) {
        QModelIndex idx = model()->index(row,0,pidx);
        if ( ! pidx.isValid() ) {
            // Page (below root idx)
            QString dpfile = model()->data(idx).toString();
            QFrame* page = new QFrame;
            _pages.append(page);
            QGridLayout* grid = new QGridLayout(page);
            grid->setContentsMargins(0, 0, 0, 0);
            grid->setSpacing(0);
            _page2grid.insert(page,grid);
            _grids.append(grid);
            _nb->addTab(page,QFileInfo(dpfile).baseName());
            int nbIdx = _nb->count()-1;
            _nb->setCurrentIndex(nbIdx);
            _nb->setAttribute(Qt::WA_AlwaysShowToolTips, true);
        } else if ( ! gpidx.isValid() ) {
            // Plot
            QWidget* page = _idx2Page(pidx);
            QGridLayout* grid = _grids.at(pidx.row()) ;
            Plot* plot = new Plot(idx,page);
            connect(plot,SIGNAL(mouseDoubleClick(QMouseEvent*)),
                    this,SLOT(doubleClick(QMouseEvent*)));
            connect(plot, SIGNAL(keyPress(QKeyEvent*)),
                    this, SLOT(plotKeyPress(QKeyEvent*)));
            _page2Plots[page].append(plot);
            int nPlots = model()->rowCount(pidx);
            switch ( nPlots ) {
            case 1: {
                grid->addWidget(plot,0,0);
                break;
            }
            case 2: {
                grid->addWidget(plot,1,0);
                break;
            }
            case 3: {
                grid->addWidget(plot,2,0);
                break;
            }
            case 4: {
                QWidget* w2 = grid->itemAtPosition(1,0)->widget();
                QWidget* w3 = grid->itemAtPosition(2,0)->widget();
                grid->removeWidget(w2);
                grid->removeWidget(w3);
                grid->addWidget(w2,0,1);
                grid->addWidget(w3,1,0);
                grid->addWidget(plot,1,1);
                break;
            }
            case 5: {
                grid->addWidget(plot,2,0,1,2);
                break;
            }
            case 6: {
                QWidget* w5 = grid->itemAtPosition(2,0)->widget();
                grid->removeWidget(w5);
                grid->addWidget(w5,2,0);
                grid->addWidget(plot,2,1);
                break;
            }
            case 7: {
                grid->addWidget(plot,3,0,1,2);
                break;
            }
            default: {
                qDebug() << "snap limitation: 7 plots max on DP :(";
                qDebug() << "snap will probably crash now!";
            }
            }

        } else if ( ! g2pidx.isValid() ) {
            // Curve
        } else if ( ! g3pidx.isValid() ) {
            // t,x,y,run
            if ( idx.row() == 3 ) {
                // Run
                QModelIndex yidx = model()->index(2,0,pidx);
                QString yparam = model()->data(yidx).toString();
                TrickCurveModel* curveModel = _monteModel->curve(pidx.row(),
                                                                yparam);
                Plot* plot = _idx2Plot(gpidx);
                TrickCurve* curve = plot->addCurve(curveModel);
                _plot2Curves[plot].append(curve);
                connect(curve,SIGNAL(selectionChanged(TrickCurve*)),
                        this,SLOT(selectionChangedTrickCurve(TrickCurve*)));
            }
        }
    }
}

void PlotBookView::rowsAboutToBeRemoved(const QModelIndex &pidx,
                                        int start, int end)
{
    QModelIndex gpidx = model()->parent(pidx);
    QModelIndex g2pidx = model()->parent(gpidx);
    QModelIndex g3pidx = model()->parent(g2pidx);

    for ( int row = start; row < end+1; ++row) {

        QModelIndex idx = model()->index(row,0,pidx);

        if ( ! pidx.isValid() ) {
            // Page
            QWidget* page = _idx2Page(idx);
            _grids.remove(idx.row());
            _pages.remove(idx.row());
            _page2grid.remove(page);
            _page2Plots.remove(page);
            if ( ! _isTabCloseRequested ) {
                // Tab widget will remove its own tab
                _nb->removeTab(idx.row());
            }
            page->deleteLater();
        } else if ( ! gpidx.isValid() ) {
            // Plot
            QWidget* page = _idx2Page(pidx);
            QGridLayout* grid = static_cast<QGridLayout*>(page->layout());

            Plot* plot = _idx2Plot(idx);
            QVector<TrickCurve*> curves = _plot2Curves.value(plot);
            foreach ( TrickCurve* curve, curves ) {
                disconnect(curve,SIGNAL(selectionChanged(TrickCurve*)));
            }
            _page2Plots[page].remove(idx.row());
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
        } else if ( ! g2pidx.isValid() ) {
            // Curve
            Plot* plot = _idx2Plot(pidx);
            TrickCurve* curve = _idx2Curve(idx);
            disconnect(curve,SIGNAL(selectionChanged(TrickCurve*)));
            plot->removePlottable(curve);
            _plot2Curves[plot].remove(idx.row());
            plot->replot();
        } else if ( ! g3pidx.isValid() ) {
            // t,x,y,run
        }
    }
}

QModelIndex PlotBookView::_curve2Idx(TrickCurve *curve)
{
    Plot* plot = static_cast<Plot*>(curve->parentPlot());
    QModelIndex plotIdx = _plot2Idx(plot);
    int rowCurve = _plot2Curves.value(plot).indexOf(curve);
    QModelIndex curveIdx = model()->index(rowCurve,0,plotIdx);
    return curveIdx;
}

QWidget *PlotBookView::_idx2Page(const QModelIndex &idx) const
{
    if ( !idx.isValid() ) return 0;

    QModelIndex pageIdx(idx);
    while ( pageIdx.parent().isValid() ) {
        pageIdx = pageIdx.parent();
    }
    QWidget* page = _pages.at(pageIdx.row());
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
    if ( !idx.isValid() || !idx.parent().isValid() ) {
        // root or page
        return 0;
    }

    Plot* plot = 0 ;
    QWidget* page = _idx2Page(idx);
    if ( page ) {
        QModelIndex plotIdx(idx);
        while ( plotIdx.parent().parent().isValid() ) {
            plotIdx = plotIdx.parent();
        }
        plot = _page2Plots.value(page).at(plotIdx.row());
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

QModelIndex PlotBookView::_nbId2Idx(int id) const
{
    QModelIndex idx ;

    if ( model() ) {
        idx = model()->index(id,0);
    }

    return idx;
}

QModelIndex PlotBookView::_page2Idx(QWidget *page) const
{
    if ( !model() || !page) return QModelIndex();

    QModelIndex idx;
    int row = _pages.indexOf(page);
    if ( row >= 0 ) {
        idx = model()->index(row,0);
    }
    return idx;
}

QModelIndex PlotBookView::_plot2Idx(Plot *plot) const
{
    if ( !model() || !plot ) return QModelIndex();

    QModelIndex plotIdx;

    QWidget* page = plot->parentWidget();
    int rowPage = _pages.indexOf(page);
    int rowPlot = _page2Plots.value(page).indexOf(plot);
    QModelIndex pageIdx = model()->index(rowPage,0);
    plotIdx = model()->index(rowPlot,0,pageIdx);

    return plotIdx;
}

bool PlotBookView::_isPlotIdx(const QModelIndex &idx)
{
    if ( idx.isValid() && idx.parent().isValid() &&
         !idx.parent().parent().isValid() ) {
        return true;
    } else {
        return false;
    }
}

bool PlotBookView::_isCurveIdx(const QModelIndex &idx)
{
    if ( idx.isValid() && idx.parent().isValid() &&
         idx.parent().parent().isValid() &&
         !idx.parent().parent().parent().isValid() ) {
        return true;
    } else {
        return false;
    }
}

void PlotBookView::_selectNextCurve()
{
    if ( ! selectionModel() || !model() ) return ;

    QModelIndex currIdx = selectionModel()->currentIndex();
    if ( _isCurveIdx(currIdx) ) {
        QModelIndex plotIdx = currIdx.parent();
        int currRow = currIdx.row();
        int nextRow = currRow+1;
        if ( nextRow >= model()->rowCount(plotIdx) ) {
            nextRow = currRow;
        }
        QModelIndex nextCurveIdx = model()->index(nextRow,0,plotIdx);
        selectionModel()->setCurrentIndex(nextCurveIdx,
                                 QItemSelectionModel::ClearAndSelect);
    }
}

void PlotBookView::_selectPrevCurve()
{
    if ( ! selectionModel() || !model() ) return ;

    QModelIndex currIdx = selectionModel()->currentIndex();
    if ( _isCurveIdx(currIdx) ) {
        QModelIndex plotIdx = currIdx.parent();
        int currRow = currIdx.row();
        int prevRow = currRow-1;
        if ( prevRow < 0 ) {
            prevRow = 0;
        }
        QModelIndex prevCurveIdx = model()->index(prevRow,0,plotIdx);
        selectionModel()->setCurrentIndex(prevCurveIdx,
                                         QItemSelectionModel::ClearAndSelect);
    }
}
