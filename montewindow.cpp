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
    _currQPIdx(0)
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
    connect(_plotSelectModel,
            SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
            this,
            SLOT(_plotSelectModelSelectionChanged(QItemSelection,QItemSelection)));
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
    _exitAction = _fileMenu->addAction(tr("E&xit"));
    _menuBar->addMenu(_fileMenu);
    connect(_exitAction, SIGNAL(triggered()),this, SLOT(close()));
    setMenuWidget(_menuBar);
}

//
// Monte Carlo Plot Page Widget
//
void MonteWindow::_createMontePages(const QString& dpfile)
{
    //
    // If already created, make page current
    //
    QStandardItem *rootItem = _plotModel->invisibleRootItem();
    for ( int row = 0; row < rootItem->rowCount(); ++row) {
        QModelIndex pageIdx = _plotModel->index(row,0);
        QString pageName = _plotModel->data(pageIdx).toString();
        if ( pageName == dpfile ) {
            _plotSelectModel->setCurrentIndex(pageIdx,
                                          QItemSelectionModel::ClearAndSelect);
                                          //QItemSelectionModel::Current);
            return;
        }
    }


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
            foreach (DPCurve* dpcurve, plot->curves() ) {
                for ( int run = 0; run < numRuns; ++run) {
                    QString curveTitle = QString("Curve_%0").arg(run);
                    QStandardItem *curveItem = new QStandardItem(curveTitle);
                    plotItem->appendRow(curveItem);

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
            _createMontePages(fp);
        } else if ( _isRUN(fp) ) {
        } else if ( _isMONTE(fp) ) {
        }
        QString msg(fn);
        msg += " t=";
        t.snap(msg.toAscii().constData());
    }

}

// TODO: if multiselect, do smart groupings i.e.
//       plot like variables together e.g. a.b[0].forceX, a.b[1].forceX
void MonteWindow::_varsSelectModelSelectionChanged(const QItemSelection &currVarSelection,
                                      const QItemSelection &prevVarSelection)
{
    Q_UNUSED(prevVarSelection); // TODO: handle deselection (prevSelection)

    if ( currVarSelection.size() == 0 ) {
        return;
    }

    QModelIndex qpIdx; // for new or selected qp page
    QModelIndexList selIdxs = _varsSelectModel->selection().indexes();

    if ( selIdxs.size() == 1 ) {

        //
        // Single selection
        //

        QString yName = _varsFilterModel->data(selIdxs.at(0)).toString();
        qpIdx = _findPageWithCurve(yName) ;

        if ( ! qpIdx.isValid() ) {
            // No current plot associated with var selection,
            // create a single page with one plot
            QStandardItem* qpItem = _createQPItem();
            qpIdx = _plotModel->indexFromItem(qpItem);
            _addPlotOfVarQPItem(qpItem,currVarSelection.indexes().at(0));
        } else {
            // at bottom of func, select page done for all cases
        }

    } else {

        //
        // Multiple items selected.
        //

        // Get or create qp page
        QModelIndex currIndex = _plotSelectModel->currentIndex();
        if ( currIndex.isValid() && !currIndex.parent().isValid() ) {
            // currIndex is a page since it's valid and parent root
            QString pageTitle = _plotModel->data(currIndex).toString();
            if ( pageTitle.left(2) == "QP" ) {
                qpIdx = currIndex;
            }
        }
        QStandardItem* qpItem = 0;
        if ( ! qpIdx.isValid() ) {
            qpItem = _createQPItem();
            qpIdx = _plotModel->indexFromItem(qpItem);
        } else {
            qpItem = _plotModel->itemFromIndex(qpIdx);
        }

        QModelIndexList currIdxs = currVarSelection.indexes();
        while ( ! currIdxs.isEmpty() ) {
            QModelIndex idx = currIdxs.takeFirst();
            if ( qpItem->rowCount() >= 6 ) {
                qpItem = _createQPItem();
                qpIdx = _plotModel->indexFromItem(qpItem);
            }
            _addPlotOfVarQPItem(qpItem,idx);
        }
    }

    _plotSelectModel->setCurrentIndex(qpIdx,
                                     QItemSelectionModel::ClearAndSelect);
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

void MonteWindow::_addPlotOfVarQPItem(QStandardItem* qpItem,
                          const QModelIndex &varIdx)
{
    int nPlots = qpItem->rowCount();

    QString tName("sys.exec.out.time");
    QString xName("sys.exec.out.time");
    QString yName = _varsFilterModel->data(varIdx).toString();

    QString plotName = QString("QPlot_%0").arg(nPlots);
    QStandardItem* qPlotItem = new QStandardItem(plotName);
    qpItem->appendRow(qPlotItem);

    for ( int r = 0; r < _monteModel->rowCount(); ++r) {

        QString runName = _monteModel->headerData(r,Qt::Vertical).toString();

        //
        // Create curve
        //
        QString curveName = QString("Curve_%0").arg(r);
        QStandardItem *curveItem = new QStandardItem(curveName);
        qPlotItem->appendRow(curveItem);

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

// Search (albeit ugly) for yparam
QModelIndex MonteWindow::_findPageWithCurve(const QString &curveName)
{
    QModelIndex pageIdx;

    bool isExists = false;
    QStandardItem *rootItem = _plotModel->invisibleRootItem();
    for ( int pg = 0; pg < rootItem->rowCount(); ++pg) {
        QModelIndex pgIdx = _plotModel->index(pg,0);
        for ( int p = 0; p < _plotModel->rowCount(pgIdx); ++p) {
            QModelIndex pIdx = _plotModel->index(p,0,pgIdx);
            // Only search one curve since assuming monte carlo runs
            // where all curves will have same param list
            if ( _plotModel->rowCount(pIdx) > 0 ) {
                QModelIndex cIdx = _plotModel->index(0,0,pIdx);
                for ( int y = 0; y < _plotModel->rowCount(cIdx); ++y) {
                    QModelIndex yIdx = _plotModel->index(y,0,cIdx);
                    if ( curveName == _plotModel->data(yIdx).toString() ) {
                        isExists = true;
                        pageIdx = pgIdx;
                        break;
                    }
                    if (isExists) break;
                }
            }
            if (isExists) break;
        }
        if (isExists) break;
    }

    return pageIdx;
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

    QModelIndex curveIdx;
    if ( currSel.indexes().size() > 0 ) {
        curveIdx = currSel.indexes().at(0);
    }

    bool isCurveIdx = false;
    if ( curveIdx.isValid() && curveIdx.parent().isValid()
         && curveIdx.parent().parent().isValid()
         && !curveIdx.parent().parent().parent().isValid()  ) {

            isCurveIdx = true;
    }

    if ( isCurveIdx ) {

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

#endif // SNAPGUI2
