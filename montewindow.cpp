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

    // Central Widget and main layout
    QSplitter* s = new QSplitter;
    setCentralWidget(s);
    QFrame* lframe = new QFrame(s);
    QGridLayout* lgrid = new QGridLayout(lframe);
    _nbDPParam = new QTabWidget(lframe);
    lgrid->addWidget(_nbDPParam,0,0);
    _nbDPParam->setAttribute(Qt::WA_AlwaysShowToolTips, false);

    //
    // Create models
    //
    _bookOfPlotsModel = new QStandardItemModel(0,1,parent);
    _selectModel = new QItemSelectionModel(_bookOfPlotsModel);
    _monteModel = new MonteModel(_montedir);
    _paramsModel = _createParamsModel(_monteModel);;
    _selectParamModel = new QItemSelectionModel(_paramsModel);

    //
    // DP Model/TreeView
    //
    QDir topdir(_montedir);
    topdir.cdUp();
    _treemodel = new QFileSystemModel;
    _treemodel->setRootPath(topdir.path());
    _treeview = new QTreeView(lframe);
    _treeview->setModel(_treemodel);
    _treeview->setRootIndex(_treemodel->index(topdir.path()));
    _treeview->hideColumn(1);
    _treeview->hideColumn(2);
    _treeview->hideColumn(3);
    QStringList filters;
    filters  << "DP_*" << "SET_*";
    _treemodel->setNameFilters(filters);
    _treemodel->setNameFilterDisables(false);
    _treemodel->setFilter(QDir::Dirs|QDir::Files);
    connect(_treeview,SIGNAL(clicked(QModelIndex)),
            this, SLOT(_slotDirTreeClicked(QModelIndex)));
    _nbDPParam->addTab(_treeview,"DP");

    //
    // Param list view
    //
    _paramListView = new QListView(lframe);
    _paramListView->setModel(_paramsModel);
    _paramListView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    _paramListView->setSelectionModel(_selectParamModel);
    _nbDPParam->addTab(_paramListView,"Vars");
    connect(_selectParamModel,
            SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
            this,
            SLOT(_selectParamChanged(QItemSelection,QItemSelection)));

    //
    // For Tim, show tree view of book of plots
    //
    _bookTreeView = new QTreeView(lframe);
    _bookTreeView->setModel(_bookOfPlotsModel);
    _bookTreeView->setHeaderHidden(true);
    _bookTreeView->setSelectionModel(_selectModel);
    lgrid->addWidget(_bookTreeView,1,0);

    //
    // Create Plot Tabbed Notebook View Widget
    //
    _plotBookView = new PlotBookView(s);
    _plotBookView->setModel(_bookOfPlotsModel);
    _plotBookView->setData(_monteModel);
    _plotBookView->setSelectionModel(_selectModel);
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
    // If already created, make page current
    //
    QStandardItem *rootItem = _bookOfPlotsModel->invisibleRootItem();
    for ( int row = 0; row < rootItem->rowCount(); ++row) {
        QModelIndex pageIdx = _bookOfPlotsModel->index(row,0);
        QString pageName = _bookOfPlotsModel->data(pageIdx).toString();
        if ( pageName == dpfile ) {
            _selectModel->setCurrentIndex(pageIdx,
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
// Just a simple list of params grabbed off the MonteModel col headerData
//
QStandardItemModel *MonteWindow::_createParamsModel(MonteModel *mm)
{
    QStandardItemModel* pm = new QStandardItemModel(0,1,this);

    QStringList paramList;
    for ( int c = 1; c < mm->columnCount(); ++c) {
        QString param = mm->headerData(c,Qt::Horizontal).toString();
        paramList.append(param);
    }

    paramList.sort();

    QStandardItem *rootItem = pm->invisibleRootItem();
    for ( int i = 0; i < paramList.size(); ++i) {
        QStandardItem *paramItem = new QStandardItem(paramList.at(i));
        rootItem->appendRow(paramItem);
    }

    return pm;
}

void MonteWindow::_slotDirTreeClicked(const QModelIndex &idx)
{
    Q_UNUSED(idx);

    TimeItLinux t; t.start();
    QModelIndexList idxs =  _treeview->selectionModel()->selectedRows();
    foreach ( QModelIndex idx, idxs ) {
        QString fn = _treemodel->fileName(idx);
        QString fp = _treemodel->filePath(idx);
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

void MonteWindow::_selectParamChanged(const QItemSelection &currSelection,
                                      const QItemSelection &prevSelection)
{
    // Find or Create "QP" Quick Product
    QModelIndex qpIndex;
    int rc = _bookOfPlotsModel->rowCount();
    for ( int r = 0; r < rc; ++r) {
        QModelIndex dpIdx = _bookOfPlotsModel->index(r,0);
        QString dpName = _bookOfPlotsModel->data(dpIdx).toString();
        if ( dpName == "QP" ) {
            qpIndex = dpIdx;
            break;
        }
    }
    QStandardItem* qpItem = 0;
    if ( ! qpIndex.isValid() ) {
        QStandardItem *rootItem = _bookOfPlotsModel->invisibleRootItem();
        qpItem = new QStandardItem("QP");
        rootItem->appendRow(qpItem);
    } else {
        qpItem = _bookOfPlotsModel->itemFromIndex(qpIndex);
    }

    foreach ( QModelIndex idx, prevSelection.indexes() ) {
        qDebug() << "<<" << _paramsModel->data(idx).toString();
    }

    //
    // Add selected curves
    //
    int nPlots = qpItem->rowCount();
    if ( nPlots > 7 ) {
        qDebug() << "Sorry!  This is demo only! No support for 8+ plots!";
        return;
    }
    foreach ( QModelIndex idx, currSelection.indexes() ) {

        QString plotName = QString("QPlot_%0").arg(nPlots);
        QStandardItem* qPlotItem = new QStandardItem(plotName);
        qpItem->appendRow(qPlotItem);

        for ( int r = 0; r < _monteModel->rowCount(); ++r) {
            QString curveName = QString("Curve_%0").arg(r);
            QStandardItem *curveItem = new QStandardItem(curveName);
            qPlotItem->appendRow(curveItem);

            QString tName("sys.exec.out.time");
            QString xName("sys.exec.out.time");
            QString yName = _paramsModel->data(idx).toString();
            QString runName = _monteModel->headerData(r,Qt::Vertical).toString();

            QStandardItem *tItem   = new QStandardItem(tName);
            QStandardItem *xItem   = new QStandardItem(xName);
            QStandardItem *yItem   = new QStandardItem(yName);
            QStandardItem *runItem = new QStandardItem(runName);

            curveItem->appendRow(tItem);
            curveItem->appendRow(xItem);
            curveItem->appendRow(yItem);
            curveItem->appendRow(runItem);
        }

        nPlots++;
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
