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
#include <QProcess>
#include <QFileInfo>

#include "plotmainwindow.h"

PlotMainWindow::PlotMainWindow(
        const QString& timeName,
        const QString &presentation,
        const QString &dpDir,
        const QStringList& dpFiles,
        const QStringList& titles,
        MonteModel* monteModel,
        QStandardItemModel* varsModel,
        QStandardItemModel *monteInputsModel,
        QWidget *parent) :
    QMainWindow(parent),
    _timeName(timeName),
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
    _bookModel = new PlotBookModel(monteModel,0,1,parent);
    _bookSelectModel = new QItemSelectionModel(_bookModel);
    if ( titles.size() == 4 ) {
        QStandardItem *rootItem = _bookModel->invisibleRootItem();
        QStandardItem *citem;
        QStandardItem *gitem;
        citem = _bookModel->addChild(rootItem, "DefaultPageTitles","");
        gitem = _bookModel->addChild(citem, "Title1",titles.at(0));
        gitem = _bookModel->addChild(citem, "Title2",titles.at(1));
        gitem = _bookModel->addChild(citem, "Title3",titles.at(2));
        gitem = _bookModel->addChild(citem, "Title4",titles.at(3));
    }


    // Create Plot Tabbed Notebook View Widget
    _bookView = new BookView();
    _bookView->setModel(_bookModel);
    _bookView->setSelectionModel(_bookSelectModel);
    /*
    connect(_bookSelectModel,SIGNAL(currentChanged(QModelIndex,QModelIndex)),
            this, SLOT(_bookCurrentChanged(QModelIndex,QModelIndex)));
    */
    msplit->addWidget(_bookView);

    // Monte inputs view (widget added later)
    if ( _monteInputsModel ) {
        _monteInputsView = new MonteInputsView(_bookView,lsplit);
        _monteInputsView->setModel(_monteInputsModel);
        _monteInputsHeaderView = _monteInputsView->horizontalHeader();
#if 0
        connect(_monteInputsHeaderView,SIGNAL(sectionClicked(int)),
            this,SLOT(_monteInputsHeaderViewClicked(int)));
#endif
    }

    _plotTreeView = new QTreeView(lsplit);
    _plotTreeView->setModel(_bookModel);

    // Vars/DP Notebook
    _nbDPVars = new QTabWidget(lsplit);
    _nbDPVars->setFocusPolicy(Qt::ClickFocus);
    lsplit->addWidget(_nbDPVars);
    _nbDPVars->setAttribute(Qt::WA_AlwaysShowToolTips, false);

    // Vars Tab
    QFrame* varsFrame = new QFrame(lsplit);
    _varsWidget = new VarsWidget(_varsModel, _monteModel, _bookModel,
                                  _bookSelectModel, _monteInputsView,
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
        _dpTreeWidget = new  DPTreeWidget(_timeName, _dpDir, _dpFiles, _varsModel,
                                          _monteModel, _bookModel,
                                          _bookSelectModel, _dpFrame);
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
    resize(1300,720);
}

PlotMainWindow::~PlotMainWindow()
{
    delete _bookModel;
}

void PlotMainWindow::createMenu()
{
    _menuBar = new QMenuBar;
    _fileMenu = new QMenu(tr("&File"), this);
    _pdfAction = _fileMenu->addAction(tr("Save As P&DF"));
    _vectorPdfAction = _fileMenu->addAction(tr("Save As High Q&uality PDF"));
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
        _dpTreeWidget = new  DPTreeWidget(_timeName, _dpDir, _dpFiles,
                                          _varsModel, _monteModel, _bookModel,
                                          _bookSelectModel, _dpFrame);
    }
}

void PlotMainWindow::_bookCurrentChanged(const QModelIndex &currIdx,
                                         const QModelIndex &prevIdx)
{
    Q_UNUSED(prevIdx);
    qDebug() << "TODO: PlotMainWindow::_bookCurrentChanged=";
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
        _bookView->savePdf(fname);
    }
}

void PlotMainWindow::_savePdf()
{
    /*
    QString fname = QFileDialog::getSaveFileName(this,
                                                 QString("Save As PDF"),
                                                 QString(""),
                                                 tr("files (*.pdf)"));

    */
    QString fname = "/home/vetter/dev/trick-07.23.1/trick_sims/SIM_ball_L1/dog.pdf";
    if ( ! fname.isEmpty() ) {
        _bookView->savePdf(fname);
        //exit(-1);

        QString program = "evince";
        QStringList arguments;
        arguments << fname;
        QProcess *myProcess = new QProcess(this);
        myProcess->start(program, arguments);

        QFileInfo fi(fname);
        double l = log10(fi.size());
        if ( l >= 3 && l < 6 ) {
            qDebug() << "dog.pdf.size=" << fi.size()/1000 << "K";
        } else if ( l >= 6 && l < 9 ) {
            qDebug() << "dog.pdf.size=" << fi.size()/1000000 << "MB";
        } else {
            qDebug() << "dog.pdf.size=" << fi.size() << "bytes";
        }
    }
}

void PlotMainWindow::_startTimeChanged(double startTime)
{
    QModelIndex startIdx = _bookModel->getIndex(QModelIndex(),
                                                "SessionStartTime");
    if ( startIdx.isValid() ) {
        QStandardItem* item = _bookModel->itemFromIndex(startIdx);
        item->setData(startTime);
    }
}

void PlotMainWindow::_stopTimeChanged(double stopTime)
{
    QModelIndex stopIdx = _bookModel->getIndex(QModelIndex(),
                                               "SessionStopTime");
    if ( stopIdx.isValid() ) {
        QStandardItem* item = _bookModel->itemFromIndex(stopIdx);
        item->setData(stopTime);
    }
}

void PlotMainWindow::_monteInputsHeaderViewClicked(int section)
{
    Q_UNUSED(section);

    if ( !_bookModel ) return;

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

    QModelIndexList pageIdxs = _bookModel->pageIdxs();
    foreach ( QModelIndex pageIdx, pageIdxs ) {
        QModelIndexList plotIdxs = _bookModel->plotIdxs(pageIdx);
        foreach ( QModelIndex plotIdx, plotIdxs ) {
            QModelIndex curvesIdx = _bookModel->getIndex(plotIdx,
                                                         "Curves", "Plot");
            QModelIndexList curveIdxs = _bookModel->curveIdxs(curvesIdx);
            bool isFirst = true;
            int r = 0;
            foreach ( QModelIndex curveIdx, curveIdxs ) {
                // This is to speed things up when montecarlo since
                // getIndex() is a bit expensive
                if ( isFirst ) {
                    QModelIndex rIdx = _bookModel->getIndex(curveIdx,
                                                    "CurveRunID",
                                                    "Curve");
                    r = rIdx.row();
                    isFirst = false;
                }
                QModelIndex runIdx = _bookModel->index(r,0,curveIdx);
                int runId = _bookModel->data(runIdx).toInt();
                QModelIndex colorIdx = _bookModel->getIndex(curveIdx,
                                                    "CurveColor", "Curve");
                QStandardItem* item = _bookModel->itemFromIndex(colorIdx);
                item->setData(run2color.value(runId));
            }
        }
    }

#if 0
    _plotBookView->replot();
#endif
}
