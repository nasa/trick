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

PlotMainWindow::PlotMainWindow(bool isDebug,
        const QStringList &timeNames,
        double startTime, double stopTime,
        const QHash<QString,QVariant>& shifts, // rundir->shiftvalue
        const QString &presentation,
        const QString &dpDir,
        const QStringList& dpFiles,
        const QStringList& titles,
        const QStringList &legends,
        MonteModel* monteModel,
        QStandardItemModel* varsModel,
        QStandardItemModel *monteInputsModel,
        QWidget *parent) :
    QMainWindow(parent),
    _isDebug(isDebug),
    _timeNames(timeNames),
    _presentation(presentation),
    _dpDir(dpDir),
    _dpFiles(dpFiles),
    _monteModel(monteModel),
    _varsModel(varsModel),
    _monteInputsModel(monteInputsModel),
    _monteInputsView(0),
    _dpTreeWidget(0)
{
    // Window title
    if ( titles.size() >= 1 && !titles.at(0).isEmpty() ) {
        // Set window title to -t1 title
        setWindowTitle(titles.at(0));
    } else {
        setWindowTitle(tr("koviz!"));
    }

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
    _bookModel = new PlotBookModel(_timeNames,monteModel,0,1,parent);
    if ( titles.size() == 4 ) {
        QStandardItem *rootItem = _bookModel->invisibleRootItem();
        QStandardItem *citem;
        citem = _bookModel->addChild(rootItem, "DefaultPageTitles","");
        _bookModel->addChild(citem, "Title1",titles.at(0));
        _bookModel->addChild(citem, "Title2",titles.at(1));
        _bookModel->addChild(citem, "Title3",titles.at(2));
        _bookModel->addChild(citem, "Title4",titles.at(3));
        _bookModel->addChild(rootItem, "LiveCoordTime","");
        _bookModel->addChild(rootItem, "StartTime",startTime);
        _bookModel->addChild(rootItem, "StopTime",stopTime);
        if ( shifts.isEmpty() ) {
            _bookModel->addChild(rootItem, "Presentation",_presentation);
        } else {
            _bookModel->addChild(rootItem, "Presentation","compare");
        }
        _bookModel->addChild(rootItem, "IsShowLiveCoord",true);
        _bookModel->addChild(rootItem, "RunToShiftHash",shifts);
    }
    if ( legends.size() == 7 ) {
        QStandardItem *rootItem = _bookModel->invisibleRootItem();
        QStandardItem *citem;
        citem = _bookModel->addChild(rootItem, "LegendLabels","");
        _bookModel->addChild(citem, "Label1",legends.at(0));
        _bookModel->addChild(citem, "Label2",legends.at(1));
        _bookModel->addChild(citem, "Label3",legends.at(2));
        _bookModel->addChild(citem, "Label4",legends.at(3));
        _bookModel->addChild(citem, "Label5",legends.at(4));
        _bookModel->addChild(citem, "Label6",legends.at(5));
        _bookModel->addChild(citem, "Label7",legends.at(6));
    }

    // Create Plot Tabbed Notebook View Widget
    _bookView = new BookView();
    _bookView->setModel(_bookModel);
    connect(_bookView->selectionModel(),
            SIGNAL(currentChanged(QModelIndex,QModelIndex)),
            this,
            SLOT(_bookViewCurrentChanged(QModelIndex,QModelIndex)));
    connect(_bookModel,
            SIGNAL(rowsInserted(QModelIndex,int,int)),
            this,
            SLOT(_bookModelRowsInserted(QModelIndex,int,int)));

    msplit->addWidget(_bookView);

    // Monte inputs view (widget added later)
    if ( _monteInputsModel ) {
        _monteInputsView = new MonteInputsView(lsplit);
        _monteInputsView->setModel(_monteInputsModel);
        connect(_monteInputsView->selectionModel(),
                SIGNAL(currentChanged(QModelIndex,QModelIndex)),
                this,
                SLOT(_monteInputsViewCurrentChanged(QModelIndex,QModelIndex)));
        _monteInputsHeaderView = _monteInputsView->horizontalHeader();
        if ( _monteInputsModel->rowCount() < 3 ) {
            // Hide if viewing only a single or two RUNs
            _monteInputsView->hide();
        }
        connect(_monteInputsHeaderView,SIGNAL(sectionClicked(int)),
            this,SLOT(_monteInputsHeaderViewClicked(int)));
    }

    if ( _isDebug ) {
        _plotTreeView = new QTreeView(lsplit);
        _plotTreeView->setModel(_bookModel);
    }

    // Vars/DP Notebook
    _nbDPVars = new QTabWidget(lsplit);
    _nbDPVars->setFocusPolicy(Qt::ClickFocus);
    lsplit->addWidget(_nbDPVars);
    _nbDPVars->setAttribute(Qt::WA_AlwaysShowToolTips, false);

    // Vars Tab
    QFrame* varsFrame = new QFrame(lsplit);
    _varsWidget = new VarsWidget(_timeNames.at(0), _varsModel,
                                 _monteModel, _bookModel,
                                  _bookView->selectionModel(), _monteInputsView,
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
        _dpTreeWidget = new  DPTreeWidget(_timeNames.at(0), _dpDir,
                                          _dpFiles, _varsModel,
                                          _monteModel, _bookModel,
                                          _bookView->selectionModel(),
                                          _monteInputsView, _dpFrame);
        _nbDPVars->setCurrentIndex(1);
    }
    connect(_nbDPVars,SIGNAL(currentChanged(int)),
            this,SLOT(_nbCurrentChanged(int)));

    // Vars/DP needs monteInputsView, but needs to be added after Vars/DP
    if ( _monteInputsModel ) {
        lsplit->addWidget(_monteInputsView);
    }

    // Start/Stop times input
#ifdef TODO
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
#endif



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
    _optsMenu = new QMenu(tr("&Options"), this);
    _pdfAction  = _fileMenu->addAction(tr("Save As P&DF"));
    _exitAction = _fileMenu->addAction(tr("E&xit"));
    _showLiveCoordAction = _optsMenu->addAction(tr("ShowLiveCoord"));
    _clearPlotsAction          = _optsMenu->addAction(tr("ClearPlots"));
    _showLiveCoordAction->setCheckable(true);
    _showLiveCoordAction->setChecked(true);
    _menuBar->addMenu(_fileMenu);
    _menuBar->addMenu(_optsMenu);
    connect(_pdfAction, SIGNAL(triggered()),this, SLOT(_savePdf()));
    connect(_exitAction, SIGNAL(triggered()),this, SLOT(close()));
    connect(_showLiveCoordAction, SIGNAL(triggered()),
            this, SLOT(_toggleShowLiveCoord()));
    connect(_clearPlotsAction, SIGNAL(triggered()),
            this, SLOT(_clearPlots()));
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
        _dpTreeWidget = new  DPTreeWidget(_timeNames.at(0), _dpDir, _dpFiles,
                                          _varsModel, _monteModel, _bookModel,
                                          _bookView->selectionModel(),
                                          _monteInputsView,_dpFrame);
    }
}

void PlotMainWindow::_bookViewCurrentChanged(const QModelIndex &currIdx,
                                             const QModelIndex &prevIdx)
{
    Q_UNUSED(prevIdx);

    if ( _monteInputsView ) {
        if ( _bookModel->isIndex(currIdx,"Curve") ) {
            // Make row current in monte inputs view that goes with bview curve
            int runId = _bookModel->getDataInt(currIdx,"CurveRunID","Curve");
            int rc = _monteInputsView->model()->rowCount();
            for (int i = 0; i < rc; ++i ) {
                // note: next line assumes that run is in column 0
                QModelIndex idx = _monteInputsView->model()->index(i,0);
                if ( runId == _monteInputsView->model()->data(idx).toInt() ) {
                    _monteInputsView->setCurrentIndex(idx);
                    break;
                }
            }
        }
    }

    if ( !currIdx.isValid() && prevIdx.isValid() ) {
        if ( _monteInputsView ) {
            if ( _bookModel->isIndex(prevIdx,"Curve") ) {
                // Clicked off curves in _bookView,
                // so clear current in monte and all CurveViews
                QModelIndex invalidIdx;
                _monteInputsView->setCurrentIndex(invalidIdx);
                _bookView->setCurrentCurveRunID(-1);

            }
        }
    }
}

void PlotMainWindow::_bookModelRowsInserted(const QModelIndex &pidx,
                                            int start, int end)
{
    Q_UNUSED(start);
    Q_UNUSED(end);

    if ( _monteInputsView ) {
        if ( _bookModel->isIndex(pidx,"Curve") ) {
            QModelIndex miCurrIdx = _monteInputsView->currentIndex();
            if ( miCurrIdx.isValid() ) {
                int row = miCurrIdx.row();
                QModelIndex runIdx =  _monteInputsView->model()->index(row,0);
                int runID = _monteInputsView->model()->data(runIdx).toInt();
                int curveRunID = _bookModel->getDataInt(pidx,
                                                        "CurveRunID", "Curve");
                if ( runID == curveRunID ) {
                    _bookView->setCurrentCurveRunID(runID);
                }
            }
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
        _bookView->savePdf(fname);
    }
}

void PlotMainWindow::_savePdf()
{
    QString fname = QFileDialog::getSaveFileName(this,
                                                 QString("Save As PDF"),
                                                 QString(""),
                                                 tr("files (*.pdf)"));

    if ( ! fname.isEmpty() ) {

        int ret = QMessageBox::Save;
        if ( !fname.endsWith(".pdf") ) {
            fname += ".pdf";
            QFileInfo fi(fname);
            if ( fi.exists() ) {
                QMessageBox msgBox;
                msgBox.setStandardButtons(QMessageBox::Save|
                                          QMessageBox::Cancel);
                QString msg;
                msg = "Overwrite: " + fi.fileName() + "?";
                msgBox.setText(msg);
                msgBox.setDefaultButton(QMessageBox::Save);
                ret = msgBox.exec();
            }
        }

        if ( ret == QMessageBox::Save ) {
            //QString fname = "/users/kvetter/dev/dog.pdf";
            _bookView->savePdf(fname);
            /*
            QString program = "evince";
            QStringList arguments;
            arguments << fname;
            QProcess *myProcess = new QProcess(this);
            myProcess->start(program, arguments);

            QFileInfo fi(fname);
            double l = log10(fi.size());
            if ( l >= 3 && l < 6 ) {
                fprintf(stderr,"dog.pdf.size=%lf K\n", fi.size()/1000);
            } else if ( l >= 6 && l < 9 ) {
                fprintf(stderr,"dog.pdf.size=%lf MB\n", fi.size()/1000000);
            } else {
                fprintf(stderr,"dog.pdf.size=%lf bytes\n", fi.size());
            }
            */
        }
    }
}

void PlotMainWindow::_toggleShowLiveCoord()
{
    QModelIndex isShowIdx = _bookModel->getDataIndex(QModelIndex(),
                                                     "IsShowLiveCoord");
    bool isShowLiveCoord = _bookModel->data(isShowIdx).toBool();
    if ( isShowLiveCoord ) {
        _bookModel->setData(isShowIdx,false);  // hide
        _showLiveCoordAction->setChecked(false);
    } else {
        _bookModel->setData(isShowIdx,true);   // show
        _showLiveCoordAction->setChecked(true);
    }
}

void PlotMainWindow::_clearPlots()
{
    QModelIndex pagesIdx = _bookModel->getIndex(QModelIndex(),"Pages");
    int nPages = _bookModel->rowCount(pagesIdx);
    for (int i = nPages-1; i >= 0; --i) {
        _bookModel->removeRow(i,pagesIdx);
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

    int rc = _monteInputsModel->rowCount();
    QList<QColor> colors = _bookModel->createCurveColors(rc);

    QHash<int,QString> run2color;
    for ( int r = 0; r < rc; ++r ) {
        QModelIndex runIdx = _monteInputsModel->index(r,0);
        int runId = _monteInputsModel->data(runIdx).toInt();
        run2color.insert(runId, colors.at(r).name());
    }

    QModelIndexList pageIdxs = _bookModel->pageIdxs();
    foreach ( QModelIndex pageIdx, pageIdxs ) {
        QModelIndexList plotIdxs = _bookModel->plotIdxs(pageIdx);
        foreach ( QModelIndex plotIdx, plotIdxs ) {
            // Turn off signals so pixmaps are not created as each color changes
            bool block = _bookModel->blockSignals(true);
            QModelIndex changeColorIdx;
            QString     changeColorFrom;
            QString     changeColorTo;
            QModelIndex curvesIdx = _bookModel->getIndex(plotIdx,
                                                         "Curves", "Plot");
            QModelIndexList curveIdxs = _bookModel->curveIdxs(curvesIdx);
            int r = 0;
            foreach ( QModelIndex curveIdx, curveIdxs ) {
                int runId = _bookModel->getDataInt(curveIdx,
                                                   "CurveRunID",
                                                   "Curve");
                QString nextColor = run2color.value(runId);
                QModelIndex colorIdx = _bookModel->getIndex(curveIdx,
                                                            "CurveColor",
                                                            "Curve");
                colorIdx = _bookModel->sibling(colorIdx.row(),1,colorIdx);
                QString currColor = _bookModel->data(colorIdx).toString();
                if ( nextColor != currColor ) {
                    changeColorIdx  = colorIdx;
                    changeColorFrom = currColor;
                    changeColorTo   = nextColor;
                }
                _bookModel->setData(colorIdx,nextColor);
                ++r;
            }

            // Set one curve color back to its original color
            // before turning the signals back on
            _bookModel->setData(changeColorIdx,changeColorFrom);

            // Turn signals back on
            block = _bookModel->blockSignals(block);

            // Set one curve color while signals on so that the dataChanged
            // signal will fire and a new pixmap will be created
            _bookModel->setData(changeColorIdx,changeColorTo);
        }
    }
}

void PlotMainWindow::_monteInputsViewCurrentChanged(const QModelIndex &currIdx,
                                                    const QModelIndex &prevIdx)
{
    Q_UNUSED(prevIdx);

    if ( currIdx.column() == 0 ) {  // column 0 is runID by convention
        // set all curves in bookview with runID to current
        int runID = _monteInputsView->model()->data(currIdx).toInt();
        _bookView->setCurrentCurveRunID(runID);
    }
}
