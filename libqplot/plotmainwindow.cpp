#include <QTabWidget>
#include <QTableView>
#include <QHeaderView>
#include <QFrame>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QSplitter>
#include <QFileSystemModel>
#include <QTreeView>
#include <QHash>

#include "plotmainwindow.h"

PlotMainWindow::PlotMainWindow(
        const QString &presentation,
        const QString &dpDir,
        const QStringList& dpFiles,
        const QStringList& titles,
        MonteModel* monteModel,
        QStandardItemModel* varsModel,
        QStandardItemModel *monteInputsModel,
        QWidget *parent) :
    QMainWindow(parent),
    _presentation(presentation),
    _dpDir(dpDir),
    _dpFiles(dpFiles),
    _monteModel(monteModel),
    _varsModel(varsModel),
    _monteInputsModel(monteInputsModel),
    _monteInputsView(0),
    _dpTreeWidget(0)
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

    // Create models
    _plotModel = new PlotBookModel(monteModel,0,1,parent);
    _plotSelectModel = new QItemSelectionModel(_plotModel);

    // Create Plot Tabbed Notebook View Widget
    _plotBookView = new PlotBookView(_plotModel, titles, msplit);
    _plotBookView->setSelectionModel(_plotSelectModel);
    if ( _monteModel->rowCount() == 2 && _presentation != "compare" ) {
        // Two runs, and presentation is not compare - show diff/coplot
        _plotBookView->showCurveDiff(true);
    }
    connect(_plotModel,
            SIGNAL(rowsAboutToBeRemoved(QModelIndex,int,int)),
            this,
            SLOT(_plotModelRowsAboutToBeRemoved(QModelIndex,int,int)));
    connect(_plotSelectModel,
            SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
            this,
            SLOT(_plotSelectModelSelectionChanged(QItemSelection,QItemSelection)));
    msplit->addWidget(_plotBookView);

    // Monte inputs view (widget added later)
    if ( _monteInputsModel ) {
        _monteInputsView = new MonteInputsView(_plotBookView,lsplit);
        _monteInputsView->setModel(_monteInputsModel);
        _monteInputsHeaderView = _monteInputsView->horizontalHeader();
        connect(_monteInputsHeaderView,SIGNAL(sectionClicked(int)),
            this,SLOT(_monteInputsHeaderViewClicked(int)));

    }

    // Vars/DP Notebook
    _nbDPVars = new QTabWidget(lsplit);
    _nbDPVars->setFocusPolicy(Qt::ClickFocus);
    lsplit->addWidget(_nbDPVars);
    _nbDPVars->setAttribute(Qt::WA_AlwaysShowToolTips, false);

    // Vars Tab
    QFrame* varsFrame = new QFrame(lsplit);
    _varsWidget = new VarsWidget(_varsModel, _monteModel, _plotModel,
                                  _plotSelectModel, _monteInputsView,
                                 varsFrame);
    _nbDPVars->addTab(varsFrame,"Vars");

    // DP Tab
    // Due to what I think is a Qt bug, the DPTreeWidget which is placed
    // in this tab is created when the DP tab is clicked.
    // See PlotMainWindow::_nbCurrentChanged()
    _dpFrame = new QFrame(lsplit);
    _nbDPVars->addTab(_dpFrame,"DP");
    if ( ! _dpFiles.isEmpty() ) {
        // DP files specified on commandline
        _dpTreeWidget = new  DPTreeWidget(_dpDir, _dpFiles, _varsModel,
                                          _monteModel, _plotModel,
                                          _plotSelectModel, _dpFrame);
        _nbDPVars->setCurrentIndex(1);
    }
    connect(_nbDPVars,SIGNAL(currentChanged(int)),
            this,SLOT(_nbCurrentChanged(int)));

    // Vars/DP needs monteInputsView, but needs to be added after Vars/DP
    if ( _monteInputsModel ) {
        lsplit->addWidget(_monteInputsView);
    }

    // Start/Stop times input
    RangeInput* rangeInput = new RangeInput(this);
    QSizePolicy sp(QSizePolicy::Preferred,QSizePolicy::Preferred);
    rangeInput->setSizePolicy(sp);
    lsplit->addWidget(rangeInput);
    lsplit->setStretchFactor(0,1);
    lsplit->setStretchFactor(1,0);
    connect(rangeInput,SIGNAL(minChanged(double)),
            this, SLOT(_startTimeChanged(double)));
    connect(rangeInput,SIGNAL(maxChanged(double)),
            this, SLOT(_stopTimeChanged(double)));



    // Size main window
    QList<int> sizes;
    sizes << 420 << 1180;
    msplit->setSizes(sizes);
    msplit->setStretchFactor(0,0);
    msplit->setStretchFactor(1,1);
    resize(1600,900);
}

PlotMainWindow::~PlotMainWindow()
{
    delete _plotModel;
}

void PlotMainWindow::createMenu()
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

void PlotMainWindow::_nbCurrentChanged(int i)
{
    if ( i == 1 && _dpTreeWidget == 0 ) {
        //
        // The reason this is here is to work around what I think
        // is a bug within Qt. If the DPTreeWidget is created
        // earlier, for some reason, this warning message is issued:
        // QSortFilterProxyModel: index from wrong model passed to mapFromSource
        // I believe that, in a rare case, this causes a core dump.
        //
        // So instead of creating the DPTreeWidget alongside the VarsWidget,
        // the DPTreeWidget is created when the DP tab is clicked.
        //
        _dpTreeWidget = new  DPTreeWidget(_dpDir, _dpFiles, _varsModel,
                                          _monteModel, _plotModel,
                                          _plotSelectModel, _dpFrame);
    }
}

void PlotMainWindow::_plotSelectModelSelectionChanged(
                                               const QItemSelection &currSel,
                                               const QItemSelection &prevSel)
{

    Q_UNUSED(prevSel);

    if ( currSel.indexes().size() == 0 && prevSel.indexes().size() > 0 ) {
        // Deselecting a curve: so,
        //     1. deselect run in monte selection too
        //     2. deselect vars
        if ( _monteInputsView ) {
            _monteInputsView->selectionModel()->clear();
        }
        _varsWidget->clearSelection();
    }

    if ( _monteInputsView ) {

        QModelIndex curveIdx;
        if ( currSel.indexes().size() > 0 ) {
            curveIdx = currSel.indexes().at(0);
        }

        if ( _plotModel->isIndex(curveIdx,"Curve") ) {

            int runId = curveIdx.row();

            // Since model could be sorted, search for row with runId.
            // Assuming that runId is in column 0.
            int rc = _monteInputsView->model()->rowCount();
            int runRow = 0 ;
            for ( runRow = 0; runRow < rc; ++runRow) {
                QModelIndex runIdx = _monteInputsView->model()->index(runRow,0);
                if (runId == _monteInputsView->model()->data(runIdx).toInt()) {
                    break;
                }
            }

            QModelIndex midx = _monteInputsView->model()->index(runRow,0);
            _monteInputsView->selectionModel()->select(midx,
                                          QItemSelectionModel::ClearAndSelect|
                                          QItemSelectionModel::Rows);
            _monteInputsView->selectionModel()->setCurrentIndex(midx,
                                          QItemSelectionModel::ClearAndSelect|
                                          QItemSelectionModel::Rows);
        }
    }
}

void PlotMainWindow::_plotModelRowsAboutToBeRemoved(const QModelIndex &pidx,
                                                 int start, int end)
{
    Q_UNUSED(pidx);
    Q_UNUSED(start);
    Q_UNUSED(end);
    _varsWidget->clearSelection();
}

bool PlotMainWindow::_isRUN(const QString &fp)
{
    QFileInfo fi(fp);
    return ( fi.baseName().left(4) == "RUN_" && fi.isDir() ) ;
}

bool PlotMainWindow::_isMONTE(const QString &fp)
{
    QFileInfo fi(fp);
    return ( fi.baseName().left(6) == "MONTE_" && fi.isDir() ) ;
}

void PlotMainWindow::savePdf(const QString& fname)
{
    if ( ! fname.isEmpty() ) {
        _plotBookView->savePdf(fname);
    }
}

void PlotMainWindow::_savePdf()
{
    QString fname = QFileDialog::getSaveFileName(this,
                                                 QString("Save As PDF"),
                                                 QString(""),
                                                 tr("files (*.pdf)"));

    if ( ! fname.isEmpty() ) {
        _plotBookView->savePdf(fname);
    }
}

void PlotMainWindow::_startTimeChanged(double startTime)
{
    QModelIndex startIdx = _plotModel->getIndex(QModelIndex(),
                                                "SessionStartTime");
    if ( startIdx.isValid() ) {
        QStandardItem* item = _plotModel->itemFromIndex(startIdx);
        item->setData(startTime);
    }
}

void PlotMainWindow::_stopTimeChanged(double stopTime)
{
    QModelIndex stopIdx = _plotModel->getIndex(QModelIndex(),
                                               "SessionStopTime");
    if ( stopIdx.isValid() ) {
        QStandardItem* item = _plotModel->itemFromIndex(stopIdx);
        item->setData(stopTime);
    }
}

void PlotMainWindow::_monteInputsHeaderViewClicked(int section)
{
    Q_UNUSED(section);

    if ( !_plotModel ) return;

    QHash<int,QString> run2color;
    int rc = _monteInputsModel->rowCount();
    qreal sH = 0.1;
    qreal eH = 1.0-sH;
    qreal m = (eH-sH)/rc;
    for ( int i = 0; i < rc; ++i ) {
        QModelIndex runIdx = _monteInputsModel->index(i,0);
        int runId = _monteInputsModel->data(runIdx).toInt();
        qreal h = m*i + sH;
        QColor c = QColor::fromHsvF(h,1,0.85);
        QString s = c.name();
        run2color.insert(runId, s);
    }

    QModelIndexList pageIdxs = _plotModel->pageIdxs();
    foreach ( QModelIndex pageIdx, pageIdxs ) {
        QModelIndexList plotIdxs = _plotModel->plotIdxs(pageIdx);
        foreach ( QModelIndex plotIdx, plotIdxs ) {
            QModelIndex curvesIdx = _plotModel->getIndex(plotIdx,
                                                         "Curves", "Plot");
            QModelIndexList curveIdxs = _plotModel->curveIdxs(curvesIdx);
            bool isFirst = true;
            int r = 0;
            foreach ( QModelIndex curveIdx, curveIdxs ) {
                // This is to speed things up when montecarlo since
                // getIndex() is a bit expensive
                if ( isFirst ) {
                    QModelIndex rIdx = _plotModel->getIndex(curveIdx,
                                                    "CurveRunID",
                                                    "Curve");
                    r = rIdx.row();
                    isFirst = false;
                }
                QModelIndex runIdx = _plotModel->index(r,0,curveIdx);
                int runId = _plotModel->data(runIdx).toInt();
                QModelIndex colorIdx = _plotModel->getIndex(curveIdx,
                                                    "CurveColor", "Curve");
                QStandardItem* item = _plotModel->itemFromIndex(colorIdx);
                item->setData(run2color.value(runId));
            }
        }
    }

    _plotBookView->replot();
}
