
#include "snapd.h"

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

SnapDiff::SnapDiff(const QString &run1, const QString &run2, QWidget *parent) :
    QMainWindow(parent),
    _run1(run1),
    _run2(run2)
{
    QStringList s1; s1 << _run1 ; _runs1 = new Runs(s1);
    QStringList s2; s2 << _run2 ; _runs2 = new Runs(s2);
    QStringList params1 = _runs1->params();
    foreach ( QString param2, _runs2->params() ) {
        if ( params1.contains(param2) ) {
            _commonParams.append(param2);
        }
    }
    _commonParams.sort();

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
    QStringList runs;
    runs.append(_run1);
    runs.append(_run2);
    _runs = new Runs(runs);
    _monteModel = new MonteModel(_runs);


    //
    // Create Plot Tabbed Notebook View Widget
    //
    _plotBookView = new PlotBookView(msplit);
    _plotBookView->setModel(_plotModel);
    _plotBookView->showCurveDiff(true);
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
    // DP&Vars notebook
    //
    _nbDPVars = new QTabWidget(lsplit);
    _nbDPVars->setFocusPolicy(Qt::ClickFocus);
    lsplit->addWidget(_nbDPVars);
    _nbDPVars->setAttribute(Qt::WA_AlwaysShowToolTips, false);

    // Vars Tab
    _varsModel = _createVarsModel(_commonParams);
    QFrame* varsFrame = new QFrame(lsplit);
    _varsWidget = new VarsWidget(_varsModel, _monteModel, _plotModel,
                                  _plotSelectModel, _plotBookView,
                                 0, varsFrame);
    _nbDPVars->addTab(varsFrame,"Vars");


    // DP Tab
    QFrame* frameDP = new QFrame(lsplit);
    _dpTreeWidget = new  DPTreeWidget(_run1,_monteModel,
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

SnapDiff::~SnapDiff()
{
    delete _monteModel;
    delete _plotModel;
}

void SnapDiff::createMenu()
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
QStandardItemModel *SnapDiff::_createVarsModel(const QStringList& params)
{
    QStandardItemModel* pm = new QStandardItemModel(0,1,this);

    QStandardItem *rootItem = pm->invisibleRootItem();
    for ( int i = 0; i < params.size(); ++i) {
        QStandardItem *varItem = new QStandardItem(params.at(i));
        rootItem->appendRow(varItem);
    }

    return pm;
}

void SnapDiff::_plotSelectModelSelectionChanged(const QItemSelection &currSel,
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

// _plotModel will have a curve on a branch like this:
//           root->page->plot->curves->curvei
//
bool SnapDiff::_isCurveIdx(const QModelIndex &idx) const
{
    if ( idx.model() != _plotModel ) return false;
    if ( idx.model()->data(idx.parent()).toString() != "Curves" ) {
        return false;
    } else {
        return true;
    }
}

//
// If page (tab) changed in plot book view, update DP tree and Var list
//
void SnapDiff::_plotSelectModelCurrentChanged(const QModelIndex &currIdx,
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

void SnapDiff::_plotModelRowsAboutToBeRemoved(const QModelIndex &pidx,
                                                 int start, int end)
{
    Q_UNUSED(pidx);
    Q_UNUSED(start);
    Q_UNUSED(end);
    _varsWidget->clearSelection();
}

void SnapDiff::_updateVarSelection(const QModelIndex& pageIdx)
{
    _nbDPVars->setCurrentIndex(0); // set tabbed notebook page to Var page
    _varsWidget->updateSelection(pageIdx);
}

void SnapDiff::_updateDPSelection(const QModelIndex &pageIdx)
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

bool SnapDiff::_isRUN(const QString &fp)
{
    QFileInfo fi(fp);
    return ( fi.baseName().left(4) == "RUN_" && fi.isDir() ) ;
}

bool SnapDiff::_isMONTE(const QString &fp)
{
    QFileInfo fi(fp);
    return ( fi.baseName().left(6) == "MONTE_" && fi.isDir() ) ;
}

void SnapDiff::_savePdf()
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
