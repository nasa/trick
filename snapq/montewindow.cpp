
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
    _montedir(montedir)
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
    QStringList runDirs;
    runDirs.append(_montedir);
#if 0
    QString setDir("/home/vetter/dev/SET_Series30xx");
    QString monteDir = setDir + '/' +
                      "MONTE_RUN_M_3027_i15T_i350T_IDSS-N1_iLIDS-DTS4C1.1000r";
    QString run0 = monteDir + "/RUN_00000";
    QString run1 = monteDir + "/RUN_00001";
    QString run2 = monteDir + "/RUN_00002";
    QString run3 = monteDir + "/RUN_00003";
    runDirs << run0 << run1 << run2 << run3;
#endif
    _runs = new Runs(runDirs);
    _monteModel = new MonteModel(_runs);

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

    //
    // Vars/DP Notebook
    //
    _nbDPVars = new QTabWidget(lsplit);
    _nbDPVars->setFocusPolicy(Qt::ClickFocus);
    lsplit->addWidget(_nbDPVars);
    _nbDPVars->setAttribute(Qt::WA_AlwaysShowToolTips, false);

    // Vars Tab
    _varsModel = _createVarsModel(_monteModel);;
    QFrame* varsFrame = new QFrame(lsplit);
    _varsWidget = new VarsWidget(_varsModel, _monteModel, _plotModel,
                                  _plotSelectModel, _plotBookView,
                                 0,
                                 varsFrame);
    _nbDPVars->addTab(varsFrame,"Vars");

    // DP Tab
    QFrame* frameDP = new QFrame(lsplit);
    _dpTreeWidget = new  DPTreeWidget(_montedir,_monteModel,
                                      _plotModel,_plotSelectModel,
                                      frameDP);
    _nbDPVars->addTab(frameDP,"DP");

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
    delete _monteModel;
    delete _plotModel;
}

void MonteWindow::createMenu()
{
    _menuBar = new QMenuBar;
    _fileMenu = new QMenu(tr("&File"), this);
    _pdfAction = _fileMenu->addAction(tr("Save As P&DF"));
    _exitAction = _fileMenu->addAction(tr("E&xit"));
    _menuBar->addMenu(_fileMenu);
    connect(_pdfAction, SIGNAL(triggered()),this, SLOT(_savePdf()));
    connect(_exitAction, SIGNAL(triggered()),this, SLOT(close()));
    setMenuWidget(_menuBar);
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


void MonteWindow::_plotSelectModelSelectionChanged(const QItemSelection &currSel,
                                                 const QItemSelection &prevSel)
{
    Q_UNUSED(prevSel);

    if ( currSel.indexes().size() == 0 && prevSel.indexes().size() > 0 ) {
        // Deselecting a curve: so,
        //     1. deselect run in monte selection too
        //     2. deselect vars
        _varsWidget->clearSelection();
    }

    QModelIndex curveIdx;
    if ( currSel.indexes().size() > 0 ) {
        curveIdx = currSel.indexes().at(0);
    }

    if ( _isCurveIdx(curveIdx) ) {
        int runId = curveIdx.row();
        Q_UNUSED(runId);
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
    _varsWidget->clearSelection();
}

void MonteWindow::_updateVarSelection(const QModelIndex& pageIdx)
{
    _nbDPVars->setCurrentIndex(0); // set tabbed notebook page to Var page
    _varsWidget->updateSelection(pageIdx);
}

void MonteWindow::_updateDPSelection(const QModelIndex &pageIdx)
{
#if 0
    _nbDPVars->setCurrentIndex(1); // set tabbed notebook page to DP page

    QString dpPath = pageIdx.model()->data(pageIdx).toString();
    QModelIndex dpIdx = _dpModel->index(dpPath); // not on help page!
    if ( dpIdx.isValid() ) {
        QModelIndex dpProxyIdx = _dpFilterModel->mapFromSource(dpIdx);
        _dpTreeView->selectionModel()->setCurrentIndex(
                    dpProxyIdx,
                    QItemSelectionModel::ClearAndSelect);
    }
#endif
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

void MonteWindow::_savePdf()
{
    QString* selectedFilter = new QString;
    QString fname = QFileDialog::getSaveFileName(this,
                                              QString("Save As PDF"),
                                              QString(""),
                                              tr("files (*.pdf)"),
                                              selectedFilter,
                                              QFileDialog::DontUseNativeDialog);
    delete selectedFilter;

    if ( ! fname.isEmpty() ) {
        _plotBookView->savePdf(fname);
    }
}
