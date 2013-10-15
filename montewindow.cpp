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

MonteWindow::MonteWindow(QWidget *parent) :
    QMainWindow(parent)
{
    setWindowTitle(tr("Snap!"));
    createMenu();

    // Central Widget
    QSplitter* s = new QSplitter;
    setCentralWidget(s);

    //
    // DPs, SETs, RUNs, SIMs, MONTE Dir Tree
    //
    QDir setdir("/home/vetter/dev/SET_Series30xx");
    QFileSystemModel *model = new QFileSystemModel;
    model->setRootPath(setdir.path());
    _treeview = new QTreeView(s);
    _treeview->setModel(model);
    _treeview->setRootIndex(model->index(setdir.path()));
    //_treeview->setSelectionMode(QAbstractItemView::MultiSelection);
    //_treeview->setSelectionMode(QAbstractItemView::ExtendedSelection);
    _treeview->hideColumn(1);
    _treeview->hideColumn(2);
    _treeview->hideColumn(3);
    QStringList filters;
    filters  << "DP_*" << "RUN_*" << "SET_*" << "MONTE_*" << "SIM_*";
    model->setNameFilters(filters);
    model->setNameFilterDisables(false);
    model->setFilter(QDir::Dirs|QDir::Files);
    connect(_treeview,SIGNAL(clicked(QModelIndex)),
            this, SLOT(_slotDirTreeClicked(QModelIndex)));

    //
    // Monte Carlo Plot Page Widget
    //
    _nb = new QTabWidget;
    QString montedir("/home/vetter/dev/SET_Series30xx/");
    montedir += "MONTE_RUN_M_3027_i15T_i350T_IDSS-N1_iLIDS-DTS4C1.1000r";
    _monte = new Monte(montedir);
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
}

void MonteWindow::createMenu()
{
    _menuBar = new QMenuBar;
    _fileMenu = new QMenu(tr("&File"), this);
    _exitAction = _fileMenu->addAction(tr("E&xit"));
    _menuBar->addMenu(_fileMenu);
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
    // Qt::BusyCursor
    QCursor currCursor = this->cursor();
    this->setCursor(QCursor(Qt::WaitCursor));
    PlotPage* plot = new PlotPage(page);
    plot->setData(_monte);
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
