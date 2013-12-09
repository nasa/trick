#ifdef SNAPGUI

#include "montewindow.h"

#include <QDockWidget>
#include <QTabWidget>
#include <QTableView>
#include <QHeaderView>
#include <QFrame>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QSplitter>
#include <QSortFilterProxyModel>
#include <QFileSystemModel>
#include <QTreeView>

MonteWindow::MonteWindow(const QString &montedir, QWidget *parent) :
    QMainWindow(parent),
    _montedir(montedir),
    _currQPIdx(0),
    _isSkip(false)
{
    setWindowTitle(tr("Snap!"));
    createMenu();

    // Central Widget and main layout
    QSplitter* msplit = new QSplitter;
    setCentralWidget(msplit);
    QFrame* lframe = new QFrame(msplit);
    QGridLayout* lgrid = new QGridLayout(lframe);
    QSplitter* lsplit = new QSplitter(lframe);
    lsplit->setOrientation(Qt::Vertical);
    lgrid->addWidget(lsplit,0,0);

    //
    // Create models
    //
    _plotModel = new QStandardItemModel(0,1,parent);
    _plotSelectModel = new QItemSelectionModel(_plotModel);
    _monte = new Monte(_montedir);
    _monteModel = new MonteModel(_monte);
    _monteInputsModel = _monte->inputModel();

    //
    // Vars Model for list of recorded trick vars (params)
    //
    _varsModel = _createVarsModel(_monteModel);;
    _varsFilterModel = new QSortFilterProxyModel;
    _varsFilterModel->setDynamicSortFilter(true);
    _varsFilterModel->setSourceModel(_varsModel);
    QRegExp rx(QString(".*"));
    _varsFilterModel->setFilterRegExp(rx);
    _varsFilterModel->setFilterKeyColumn(0);
    _varsSelectModel = new QItemSelectionModel(_varsFilterModel);

    //
    // Left tabbed notebook widget for DP&Vars
    //
    _nbDPVars = new QTabWidget(lsplit);
    lsplit->addWidget(_nbDPVars);
    _nbDPVars->setAttribute(Qt::WA_AlwaysShowToolTips, false);

    //
    // DP File/Sys Model and Filter Proxy Model
    //
    QDir topdir(_montedir);
    topdir.cdUp();
    _dpModel = new QFileSystemModel;
    QModelIndex dpRootIdx = _dpModel->setRootPath(topdir.path());
    QStringList filters;
    filters  << "DP_*" << "SET_*"; // _dpFilterModel does additional filtering
    _dpModel->setNameFilters(filters);
    _dpModel->setNameFilterDisables(false);
    _dpModel->setFilter(QDir::Dirs|QDir::Files);
    _dpFilterModel = new DPFilterProxyModel;
    _dpFilterModel->setDynamicSortFilter(true);
    _dpFilterModel->setSourceModel(_dpModel);
    QRegExp dprx(QString(".*"));  // DP_ and SET_ are filtered by _dpModel
    _dpFilterModel->setFilterRegExp(dprx);
    _dpFilterModel->setFilterKeyColumn(0);

    //
    // Vars view (list of searchable trick recorded vars)
    //
    QFrame* frameVars = new QFrame(lsplit);
    QGridLayout* varsGridLayout = new QGridLayout(frameVars);
    _varsSearchBox = new QLineEdit(frameVars);
    connect(_varsSearchBox,SIGNAL(textChanged(QString)),
            this,SLOT(_varsSearchBoxTextChanged(QString)));
    varsGridLayout->addWidget(_varsSearchBox,0,0);

    _varsListView = new QListView(frameVars);
    _varsListView->setModel(_varsFilterModel);
    varsGridLayout->addWidget(_varsListView,1,0);
    _nbDPVars->addTab(frameVars,"Vars");
    _varsListView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    _varsListView->setSelectionModel(_varsSelectModel);
    _varsListView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    connect(_varsSelectModel,
            SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
            this,
            SLOT(_varsSelectModelSelectionChanged(QItemSelection,QItemSelection)));

    //
    // DP TreeView with Search Box
    //
    QFrame* dpFrame = new QFrame(lsplit);
    QGridLayout* dpGridLayout = new QGridLayout(dpFrame);
    _dpSearchBox = new QLineEdit(dpFrame);
    connect(_dpSearchBox,SIGNAL(textChanged(QString)),
            this,SLOT(_dpSearchBoxTextChanged(QString)));
    dpGridLayout->addWidget(_dpSearchBox,0,0);
    _dpTreeView = new QTreeView(dpFrame);
    _dpTreeView->setModel(_dpFilterModel);
    QModelIndex proxyRootIdx = _dpFilterModel->mapFromSource(dpRootIdx);
    _dpTreeView->setRootIndex(proxyRootIdx);
    dpGridLayout->addWidget(_dpTreeView,1,0);
    connect(_dpTreeView,SIGNAL(clicked(QModelIndex)),
            this, SLOT(_dpTreeViewClicked(QModelIndex)));
    connect(_dpTreeView->selectionModel(),
            SIGNAL(currentChanged(QModelIndex,QModelIndex)),
            this, SLOT(_dpTreeViewCurrentChanged(QModelIndex,QModelIndex)));
    _nbDPVars->addTab(dpFrame,"DP");

    // This doesn't work :( Can't hide timestamp column
    for ( int col = 1; col < _dpModel->columnCount(); ++col) {
        _dpTreeView->hideColumn(col);
    }


    //
    // For Tim, show tree view of book of plots
    //
    //_plotTreeView = new QTreeView(lsplit);
    //_plotTreeView->setModel(_plotModel);
    //_plotTreeView->setHeaderHidden(true);
    //_plotTreeView->setSelectionModel(_plotSelectModel);
    //_plotTreeView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    //lsplit->addWidget(_plotTreeView);

    //
    // Monte inputs view
    //
    _monteInputsView = new QTableView(lsplit);
    _monteInputsView->setModel(_monteInputsModel);
    _monteInputsView->setSortingEnabled(true);
    _monteInputsView->sortByColumn(0,Qt::AscendingOrder);
    _monteInputsView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    lsplit->addWidget(_monteInputsView);
    _monteInputsView->setTextElideMode(Qt::ElideNone);
    QHeaderView* headerView = _monteInputsView->horizontalHeader();
    headerView->setTextElideMode(Qt::ElideLeft);
    headerView->setResizeMode(QHeaderView::ResizeToContents);
    connect(headerView,SIGNAL(sectionClicked(int)),
            this,SLOT(_monteInputsViewHeaderSectionClicked(int)));
    _monteInputsSelectModel = new QItemSelectionModel(_monteInputsModel);
    _monteInputsView->setSelectionModel(_monteInputsSelectModel);
    _monteInputsView->setSelectionMode(QAbstractItemView::SingleSelection);
    _monteInputsView->setSelectionBehavior(QAbstractItemView::SelectRows);
    connect(_monteInputsSelectModel,
            SIGNAL(currentChanged(QModelIndex,QModelIndex)),
            this,
            SLOT(_monteInputsSelectModelCurrentChanged(QModelIndex,QModelIndex)));

    //
    // Create Plot Tabbed Notebook View Widget
    //
    _plotBookView = new PlotBookView(msplit);
    _plotBookView->setModel(_plotModel);
    _plotBookView->setData(_monteModel);
    _plotBookView->setSelectionModel(_plotSelectModel);
    connect(_plotModel,
            SIGNAL(rowsAboutToBeRemoved(QModelIndex,int,int)),
            this,
            SLOT(_plotModelRowsAboutToBeRemoved(QModelIndex,int,int)));
    connect(_plotSelectModel,
            SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
            this,
            SLOT(_plotSelectModelSelectionChanged(QItemSelection,QItemSelection)));
    connect(_plotSelectModel,
            SIGNAL(currentChanged(QModelIndex,QModelIndex)),
            this,
            SLOT(_plotSelectModelCurrentChanged(QModelIndex,QModelIndex)));
    msplit->addWidget(_plotBookView);

    // Size main window
    QList<int> sizes;
    sizes << 420 << 1180;
    msplit->setSizes(sizes);
    msplit->setStretchFactor(0,0);
    msplit->setStretchFactor(1,1);
    resize(1600,900);
}

MonteWindow::~MonteWindow()
{
    delete _varsFilterModel;
    delete _dpModel;
    delete _monteModel;
    delete _plotModel;
}

void MonteWindow::createMenu()
{
    _menuBar = new QMenuBar;
    _fileMenu = new QMenu(tr("&File"), this);
    _pdfAction = _fileMenu->addAction(tr("P&DF"));
    _exitAction = _fileMenu->addAction(tr("E&xit"));
    _menuBar->addMenu(_fileMenu);
    connect(_pdfAction, SIGNAL(triggered()),this, SLOT(_savePdf()));
    connect(_exitAction, SIGNAL(triggered()),this, SLOT(close()));
    setMenuWidget(_menuBar);
}

//
// Monte Carlo Plot Page Widget
//
void MonteWindow::_createDPPages(const QString& dpfile)
{
    QStandardItem *rootItem = _plotModel->invisibleRootItem();
    QCursor currCursor = this->cursor();
    this->setCursor(QCursor(Qt::WaitCursor));

    DPProduct dp(dpfile);
    int numRuns = _monteModel->rowCount();
    int pageNum = 0 ;
    foreach (DPPage* page, dp.pages() ) {
        QString pageTitle = dpfile;
        if ( pageNum > 0 ) {
            pageTitle += QString("_%0").arg(pageNum);
        }
        QStandardItem *pageItem = new QStandardItem(pageTitle);
        rootItem->appendRow(pageItem);
        foreach (DPPlot* plot, page->plots() ) {

            QString plotTitle = _descrPlotTitle(plot);
            QStandardItem *plotItem = new QStandardItem(plotTitle);
            pageItem->appendRow(plotItem);

            QString xAxisLabel = plot->xAxisLabel();
            QStandardItem *xAxisLabelItem = new QStandardItem(xAxisLabel);
            plotItem->appendRow(xAxisLabelItem);

            QString yAxisLabel = plot->yAxisLabel();
            QStandardItem *yAxisLabelItem = new QStandardItem(yAxisLabel);
            plotItem->appendRow(yAxisLabelItem);

            QStandardItem *curvesItem = new QStandardItem("Curves");
            plotItem->appendRow(curvesItem);

            foreach (DPCurve* dpcurve, plot->curves() ) {
                for ( int run = 0; run < numRuns; ++run) {
                    QString curveTitle = QString("Curve_%0").arg(run);
                    QStandardItem *curveItem = new QStandardItem(curveTitle);
                    curvesItem->appendRow(curveItem);

                    QString tName = dpcurve->t()->name();
                    QString xName = dpcurve->x()->name();
                    QString yName = dpcurve->y()->name();
                    QString runName = _monteModel->headerData(run,Qt::Vertical).
                                                   toString();

                    QStandardItem *tItem   = new QStandardItem(tName);
                    QStandardItem *xItem   = new QStandardItem(xName);
                    QStandardItem *yItem   = new QStandardItem(yName);
                    QStandardItem *runItem = new QStandardItem(runName);

                    curveItem->appendRow(tItem);
                    curveItem->appendRow(xItem);
                    curveItem->appendRow(yItem);
                    curveItem->appendRow(runItem);
                }
            }
            _selectCurrentRunOnPageItem(pageItem);
        }
        pageNum++;
    }

    this->setCursor(currCursor);
}

//
// Just a simple list of vars grabbed off the MonteModel col headerData
//
QStandardItemModel *MonteWindow::_createVarsModel(MonteModel *mm)
{
    QStandardItemModel* pm = new QStandardItemModel(0,1,this);

    QStringList varList;
    for ( int c = 1; c < mm->columnCount(); ++c) {
        QString var = mm->headerData(c,Qt::Horizontal).toString();
        varList.append(var);
    }

    varList.sort();

    QStandardItem *rootItem = pm->invisibleRootItem();
    for ( int i = 0; i < varList.size(); ++i) {
        QStandardItem *varItem = new QStandardItem(varList.at(i));
        rootItem->appendRow(varItem);
    }

    return pm;
}

void MonteWindow::_dpTreeViewClicked(const QModelIndex &idx)
{
    Q_UNUSED(idx);

    TimeItLinux t; t.start();
    QModelIndexList idxs =  _dpTreeView->selectionModel()->selectedRows();
    foreach ( QModelIndex idx, idxs ) {
        QModelIndex srcIdx = _dpFilterModel->mapToSource(idx);
        QString fn = _dpModel->fileName(srcIdx);
        QString fp = _dpModel->filePath(srcIdx);
        if ( _isDP(fp) ) {
            bool isCreated = false;
            for ( int row = 0; row < _plotModel->rowCount(); ++row) {
                QModelIndex pageIdx = _plotModel->index(row,0);
                QString pageName = _plotModel->data(pageIdx).toString();
                if ( pageName == fp ) {
                    _plotSelectModel->setCurrentIndex(pageIdx,
                                                   QItemSelectionModel::Select);
                    isCreated = true;
                    break;
                }
            }
            if ( !isCreated ) {
                _createDPPages(fp);
            }
        }
        QString msg = fn + " t=";
        t.snap(msg.toAscii().constData()); /* uncomment to see load time */
    }
}

void MonteWindow::_dpTreeViewCurrentChanged(const QModelIndex &currIdx,
                                            const QModelIndex &prevIdx)
{
    Q_UNUSED(prevIdx);
    _dpTreeViewClicked(currIdx);
}

void MonteWindow::_varsSelectModelSelectionChanged(const QItemSelection &currVarSelection,
                                      const QItemSelection &prevVarSelection)
{
    Q_UNUSED(prevVarSelection); // TODO: handle deselection (prevSelection)

    if ( currVarSelection.size() == 0 ) return;
    if ( _isSkip ) return;

    QModelIndex qpIdx; // for new or selected qp page
    QModelIndexList selIdxs = _varsSelectModel->selection().indexes();

    if ( selIdxs.size() == 1 ) { // Single selection

        QString yName = _varsFilterModel->data(selIdxs.at(0)).toString();
        qpIdx = _findSinglePlotPageWithCurve(yName) ;

        if ( ! qpIdx.isValid() ) {
            // No page with single plot of selected var, so create plot of var
            QStandardItem* qpItem = _createQPItem();
            _addPlotOfVarToPageItem(qpItem,currVarSelection.indexes().at(0));
            _selectCurrentRunOnPageItem(qpItem);
        } else {
            _plotBookView->setCurrentPage(qpIdx.row());
        }

    } else {  // Multiple items selected.
        QModelIndex pageIdx = _plotBookView->currentPageIndex();
        QStandardItem* pageItem = _plotModel->itemFromIndex(pageIdx);
        QModelIndexList currVarIdxs = currVarSelection.indexes();
        while ( ! currVarIdxs.isEmpty() ) {
            QModelIndex varIdx = currVarIdxs.takeFirst();
            if ( pageItem->rowCount() >= 6 ) {
                pageItem = _createQPItem();
                pageIdx = _plotModel->indexFromItem(pageItem);
                _plotBookView->setCurrentPage(pageIdx.row());
            }
            _addPlotOfVarToPageItem(pageItem,varIdx);
            _selectCurrentRunOnPageItem(pageItem);
        }
    }
}

QStandardItem* MonteWindow::_createQPItem()
{
    QStandardItem* qpItem;
    QStandardItem *rootItem = _plotModel->invisibleRootItem();
    QString qpItemName = QString("QP_%0").arg(_currQPIdx++);
    qpItem = new QStandardItem(qpItemName);
    rootItem->appendRow(qpItem);
    return qpItem;
}

void MonteWindow::_addPlotOfVarToPageItem(QStandardItem* pageItem,
                          const QModelIndex &varIdx)
{
    int nPlots = pageItem->rowCount();

    QString tName("sys.exec.out.time");
    QString xName("sys.exec.out.time");
    QString yName = _varsFilterModel->data(varIdx).toString();

    QString plotTitle = QString("QPlot_%0").arg(nPlots);
    QStandardItem* plotItem = new QStandardItem(plotTitle);
    pageItem->appendRow(plotItem);

    QStandardItem *xAxisLabelItem = new QStandardItem(xName);
    plotItem->appendRow(xAxisLabelItem);

    QStandardItem *yAxisLabelItem = new QStandardItem(yName);
    plotItem->appendRow(yAxisLabelItem);

    QStandardItem *curvesItem = new QStandardItem("Curves");
    plotItem->appendRow(curvesItem);

    for ( int r = 0; r < _monteModel->rowCount(); ++r) {

        QString runName = _monteModel->headerData(r,Qt::Vertical).toString();

        //
        // Create curve
        //
        QString curveName = QString("Curve_%0").arg(r);
        QStandardItem *curveItem = new QStandardItem(curveName);
        curvesItem->appendRow(curveItem);

        QStandardItem *tItem   = new QStandardItem(tName);
        QStandardItem *xItem   = new QStandardItem(xName);
        QStandardItem *yItem   = new QStandardItem(yName);
        QStandardItem *runItem = new QStandardItem(runName);

        curveItem->appendRow(tItem);
        curveItem->appendRow(xItem);
        curveItem->appendRow(yItem);
        curveItem->appendRow(runItem);
    }
}

// Search for yparam with a *single plot* on a page with curve
// This is really a hackish helper for _varsSelectModelSelectionChanged()
QModelIndex MonteWindow::_findSinglePlotPageWithCurve(const QString &curveName)
{
    QModelIndex retIdx;

    bool isExists = false;
    QStandardItem *rootItem = _plotModel->invisibleRootItem();
    for ( int pageId = 0; pageId < rootItem->rowCount(); ++pageId) {
        QModelIndex pageIdx = _plotModel->index(pageId,0);
        if ( _plotModel->rowCount(pageIdx) > 1 ) continue;
        for ( int plotId = 0; plotId < _plotModel->rowCount(pageIdx); ++plotId) {
            QModelIndex plotIdx = _plotModel->index(plotId,0,pageIdx);
            // Only search one curve since assuming monte carlo runs
            // where all curves will have same param list
            if ( _plotModel->rowCount(plotIdx) > 0 ) {
                QModelIndex curveIdx = _plotModel->index(0,0,plotIdx);
                for ( int y = 0; y < _plotModel->rowCount(curveIdx); ++y) {
                    QModelIndex yIdx = _plotModel->index(y,0,curveIdx);
                    if ( curveName == _plotModel->data(yIdx).toString() ) {
                        retIdx = pageIdx;
                        isExists = true;
                        break;
                    }
                    if (isExists) break;
                }
            }
            if (isExists) break;
        }
        if (isExists) break;
    }

    return retIdx;
}

void MonteWindow::_selectCurrentRunOnPageItem(QStandardItem* pageItem)
{
    int runId = currSelectedRun();
    if ( runId >= 0 ) {
        QItemSelection currSel = _plotSelectModel->selection();
        QModelIndex pageIdx = _plotModel->indexFromItem(pageItem);
        for ( int i = 0; i < _plotModel->rowCount(pageIdx); ++i ) {
            QModelIndex plotIdx = pageIdx.model()->index(i,0,pageIdx);
            QModelIndex curvesIdx;
            for ( int j = 0; j < plotIdx.model()->rowCount(plotIdx); ++j) {
                QModelIndex idx = plotIdx.model()->index(j,0,plotIdx);
                if ( plotIdx.model()->data(idx).toString() == "Curves" ) {
                    curvesIdx = idx;
                    break;
                }
            }
            if ( !curvesIdx.isValid() ) {
                qDebug() << "snap [bad scoobies]: \"Curves\" item not found.";
                exit(-1);
            }
            QModelIndex curveIdx = pageIdx.model()->index(runId,0,curvesIdx);
            if ( ! currSel.contains(curveIdx) ) {
                QItemSelection curveSel(curveIdx,curveIdx) ;
                _plotSelectModel->select(curveSel,QItemSelectionModel::Select);
            }
        }
    }
}

int MonteWindow::currSelectedRun()
{
    int runId = -1;
    QItemSelection currSel = _plotSelectModel->selection();
    foreach ( QModelIndex i , currSel.indexes() ) {
        if ( _isCurveIdx(i) ) {
            runId = i.row();
            break;
        }
    }
    if ( runId < 0 ) {
        // If no curve selected in plotSelectModel, try monteInput selection
        QItemSelection monteInputSel = _monteInputsSelectModel->selection();
        if ( monteInputSel.size() > 0 ) {
            QModelIndex selIdx = monteInputSel.indexes().at(0);
            bool ok = false;
            runId = _monteInputsModel->data(selIdx).toInt(&ok);
            if ( !ok ) {
                runId = -1;
            }
        }
    }
    return runId;
}

// _plotModel will have a curve on a branch like this:
//           root->page->plot->curves->curvei
//
bool MonteWindow::_isCurveIdx(const QModelIndex &idx) const
{
    if ( idx.model() != _plotModel ) return false;
    if ( idx.model()->data(idx.parent()).toString() != "Curves" ) {
        return false;
    } else {
        return true;
    }
}

void MonteWindow::_varsSearchBoxTextChanged(const QString &rx)
{
    _varsFilterModel->setFilterRegExp(rx);
}

void MonteWindow::_dpSearchBoxTextChanged(const QString &rx)
{
    _dpTreeView->expandAll();
    _dpFilterModel->setFilterRegExp(rx);
}


void MonteWindow::_monteInputsViewHeaderSectionClicked(int section)
{
    Q_UNUSED(section);

    QModelIndexList selIdxs = _monteInputsSelectModel->selectedIndexes();
    if ( selIdxs.size() > 0 ) {
        QModelIndex idx = selIdxs.at(0);
        _monteInputsView->scrollTo(idx, QAbstractItemView::PositionAtCenter);
    }
}

void MonteWindow::_monteInputsSelectModelCurrentChanged(const QModelIndex &curr,
                                             const QModelIndex &prev)
{
    Q_UNUSED(prev);

    if ( ! curr.model() ) return ;

    QModelIndex runIdx = curr.model()->index(curr.row(),0);
    int runId = curr.model()->data(runIdx).toInt();
    _plotBookView->selectRun(runId);
}


void MonteWindow::_plotSelectModelSelectionChanged(const QItemSelection &currSel,
                                                 const QItemSelection &prevSel)
{
    Q_UNUSED(prevSel);

    if ( currSel.indexes().size() == 0 && prevSel.indexes().size() > 0 ) {
        // Deselecting a curve: so,
        //     1. deselect run in monte selection too
        //     2. deselect vars
        _monteInputsSelectModel->clear();
        _varsSelectModel->clear();
    }

    QModelIndex curveIdx;
    if ( currSel.indexes().size() > 0 ) {
        curveIdx = currSel.indexes().at(0);
    }

    if ( _isCurveIdx(curveIdx) ) {

        int runId = curveIdx.row();

        // Since model could be sorted, search for row with runId.
        // Assuming that runId is in column 0.
        int rc = _monteInputsModel->rowCount();
        int runRow = 0 ;
        for ( runRow = 0; runRow < rc; ++runRow) {
            QModelIndex runIdx = _monteInputsModel->index(runRow,0);
            if ( runId == _monteInputsModel->data(runIdx).toInt() ) {
                break;
            }
        }

        QModelIndex midx = _monteInputsModel->index(runRow,0);
        _monteInputsSelectModel->select(midx,
                                       QItemSelectionModel::ClearAndSelect|
                                       QItemSelectionModel::Rows);
        _monteInputsSelectModel->setCurrentIndex(midx,
                                       QItemSelectionModel::ClearAndSelect|
                                       QItemSelectionModel::Rows);
    }
}

//
// If page (tab) changed in plot book view, update DP tree and Var list
//
void MonteWindow::_plotSelectModelCurrentChanged(const QModelIndex &currIdx,
                                                const QModelIndex &prevIdx)
{
    Q_UNUSED(prevIdx);

    if ( currIdx.isValid() && !currIdx.parent().isValid() ) {
        QModelIndex pageIdx = currIdx;
        QString pageTitle = pageIdx.model()->data(pageIdx).toString();
        if ( pageTitle.left(2) == "QP" ) {
            _updateVarSelection(pageIdx);
        } else {
            _updateDPSelection(pageIdx);
        }
    }
}

void MonteWindow::_plotModelRowsAboutToBeRemoved(const QModelIndex &pidx,
                                                 int start, int end)
{
    Q_UNUSED(pidx);
    Q_UNUSED(start);
    Q_UNUSED(end);
    _varsSelectModel->clear();
}

void MonteWindow::_updateVarSelection(const QModelIndex& pageIdx)
{
    _nbDPVars->setCurrentIndex(0); // set tabbed notebook page to Var page

    const QAbstractItemModel* m = pageIdx.model();
    int nPlots = m->rowCount(pageIdx);
    QItemSelection varSelection;
    for ( int plotRow = 0; plotRow < nPlots; ++plotRow) {
        QModelIndex plotIdx = m->index(plotRow,0,pageIdx);
        QModelIndex curveIdx = m->index(0,0,plotIdx);
        QModelIndex yVarIdx = m->index(2,0,curveIdx);
        QString yVarName = m->data(yVarIdx).toString();
        for ( int i = 0; i < _varsFilterModel->rowCount(); ++i) {
            QModelIndex varIdx = _varsFilterModel->index(i,0);
            QString name = _varsFilterModel->data(varIdx).toString();
            if ( name == yVarName ) {
                QItemSelection itemSel(varIdx,varIdx);
                varSelection.merge(itemSel, QItemSelectionModel::Select);
                break;
            }
        }
    }
    if ( varSelection.size() > 0 ) {
        _isSkip = true;
        _varsSelectModel->select(varSelection,
                                 QItemSelectionModel::ClearAndSelect);
        _varsListView->scrollTo(varSelection.indexes().at(0));
        _isSkip = false;
    }
}

void MonteWindow::_updateDPSelection(const QModelIndex &pageIdx)
{
    _nbDPVars->setCurrentIndex(1); // set tabbed notebook page to DP page

    QString dpPath = pageIdx.model()->data(pageIdx).toString();
    QModelIndex dpIdx = _dpModel->index(dpPath); // not on help page!
    if ( dpIdx.isValid() ) {
        QModelIndex dpProxyIdx = _dpFilterModel->mapFromSource(dpIdx);
        _dpTreeView->selectionModel()->setCurrentIndex(
                    dpProxyIdx,
                    QItemSelectionModel::ClearAndSelect);
    }
}

bool MonteWindow::_isDP(const QString& fp)
{
    QFileInfo fi(fp);
    return (fi.baseName().left(3) == "DP_" && fi.suffix() == "xml" );
}

bool MonteWindow::_isRUN(const QString &fp)
{
    QFileInfo fi(fp);
    return ( fi.baseName().left(4) == "RUN_" && fi.isDir() ) ;
}

bool MonteWindow::_isMONTE(const QString &fp)
{
    QFileInfo fi(fp);
    return ( fi.baseName().left(6) == "MONTE_" && fi.isDir() ) ;
}

QString MonteWindow::_descrPlotTitle(DPPlot *plot)
{
    QString plotTitle = "Plot_";
    if ( plot->title() != "Plot" )  {
        plotTitle += plot->title();
    } else {
        QStringList vars;
        foreach ( DPCurve* curve, plot->curves() ) {
            vars.append(curve->y()->name());
        }
        QString var0 = vars.at(0);
        int dotCnt = 0 ;
        QString sub;
        for ( int i = 1 ; i < var0.size(); ++i) {
            sub = var0.right(i);
            if ( sub.at(0) == '.' ) {
                dotCnt++;
            }
            bool is = true;
            foreach ( QString var, vars ) {
                if ( ! var.endsWith(sub) ) {
                    is = false;
                    break;
                }
            }
            if ( ! is || dotCnt == 2 ) {
                break;
            }
        }
        if ( dotCnt == 2 ) {
            sub.remove(0,1);
        }
        plotTitle += sub;
    }

    return plotTitle;
}

void MonteWindow::_savePdf()
{
    qDebug() << "Implement me! MonteWindow::_savePdf!" ;

    QString fname = QFileDialog::getSaveFileName(this,
                                                 QString("PDF"),
                                                 QString(""),
                                                 tr("files (*.pdf)"));

    if ( ! fname.isEmpty() ) {
        _plotBookView->savePdf(fname);
    }
}

#endif // SNAPGUI2
