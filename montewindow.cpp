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
    _montedir(montedir)
{
    setWindowTitle(tr("Snap!"));
    createMenu();

    // Central Widget
    QSplitter* s = new QSplitter;
    setCentralWidget(s);

    //
    // DPs, SETs, RUNs, SIMs, MONTE Dir Tree
    //
    QDir topdir(_montedir);
    topdir.cdUp();
    _treemodel = new QFileSystemModel;
    _treemodel->setRootPath(topdir.path());
    _treeview = new QTreeView(s);
    _treeview->setModel(_treemodel);
    _treeview->setRootIndex(_treemodel->index(topdir.path()));
    //_treeview->setSelectionMode(QAbstractItemView::MultiSelection);
    //_treeview->setSelectionMode(QAbstractItemView::ExtendedSelection);
    _treeview->hideColumn(1);
    _treeview->hideColumn(2);
    _treeview->hideColumn(3);
    QStringList filters;
    //filters  << "DP_*" << "RUN_*" << "SET_*" << "MONTE_*" << "SIM_*";
    filters  << "DP_*" << "SET_*";
    _treemodel->setNameFilters(filters);
    _treemodel->setNameFilterDisables(false);
    _treemodel->setFilter(QDir::Dirs|QDir::Files);
    connect(_treeview,SIGNAL(clicked(QModelIndex)),
            this, SLOT(_slotDirTreeClicked(QModelIndex)));

    //
    // Monte Carlo Plot Page Widget
    //
    _nb = new QTabWidget;
    _monteModel = new MonteModel(_montedir);
    s->addWidget(_nb);

    // Size main window
    QList<int> sizes;
    sizes << 420 << 1180;
    s->setSizes(sizes);
    s->setStretchFactor(0,0);
    s->setStretchFactor(1,1);
    resize(1600,900);
}

MonteWindow::~MonteWindow()
{
    delete _treemodel;
    delete _monteModel;
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
void MonteWindow::_createMontePages(const QString& dpfile,
                           const QString& montedir)
{
    int c = _nb->count();
    for ( int i = 0; i < c; ++i) {
        if ( _nb->tabToolTip(i) == dpfile ) {
            _nb->setCurrentIndex(i);
            return;
        }
    }

    DPProduct dp(dpfile);
    //Monte* monte = new Monte(montedir);
    DPPage* page = dp.pages().at(0);
    QCursor currCursor = this->cursor();
    this->setCursor(QCursor(Qt::WaitCursor));
    PlotPage* plot = new PlotPage(page);
    plot->setData(_monteModel);
    _nb->addTab(plot,QFileInfo(dpfile).baseName());
    int idx = _nb->count()-1;
    _nb->setCurrentIndex(idx);
    _nb->setAttribute(Qt::WA_AlwaysShowToolTips, true);
    _nb->setTabToolTip(idx,dpfile);
    this->setCursor(currCursor);
}

void MonteWindow::_slotDirTreeClicked(const QModelIndex &idx)
{
    Q_UNUSED(idx);

    //TimeItLinux t; t.start();
    QModelIndexList idxs =  _treeview->selectionModel()->selectedRows();
    foreach ( QModelIndex idx, idxs ) {
        QString fn = _treemodel->fileName(idx);
        QString fp = _treemodel->filePath(idx);
        if ( _isDP(fp) ) {
            _createMontePages(fp,fp);
        } else if ( _isRUN(fp) ) {
        } else if ( _isMONTE(fp) ) {
        }
    }
    //t.snap("PlotLoadTime=");
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

#endif // SNAPGUI2
