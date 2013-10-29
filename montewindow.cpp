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
    // Model which holds data for notebook view of pages of plots
    //
    _bookOfPlotsModel = new QStandardItemModel(0,1,parent);
    //_bookTreeView = new QTreeView(s);
    //_bookTreeView->setModel(_bookOfPlotsModel);
    //_bookTreeView->setHeaderHidden(true);

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
    // Create Plot Tabbed Notebook View Widget
    //
    _monteModel = new MonteModel(_montedir);
    _plotBookView = new PlotBookView(s);
    _plotBookView->setModel(_bookOfPlotsModel);
    _plotBookView->setData(_monteModel);
    s->addWidget(_plotBookView);

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
    delete _bookOfPlotsModel;
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
    // If already created, just make page current
    //
    QStandardItem *rootItem = _bookOfPlotsModel->invisibleRootItem();
    for ( int row = 0; row < rootItem->rowCount(); ++row) {
        QModelIndex pageIdx = _bookOfPlotsModel->index(row,0);
        QString pageName = _bookOfPlotsModel->data(pageIdx).toString();
        if ( pageName == dpfile ) {
            qDebug() << pageName << dpfile;
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

void MonteWindow::_slotDirTreeClicked(const QModelIndex &idx)
{
    Q_UNUSED(idx);

    //TimeItLinux t; t.start();
    QModelIndexList idxs =  _treeview->selectionModel()->selectedRows();
    foreach ( QModelIndex idx, idxs ) {
        QString fn = _treemodel->fileName(idx);
        QString fp = _treemodel->filePath(idx);
        if ( _isDP(fp) ) {
            _createMontePages(fp);
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
