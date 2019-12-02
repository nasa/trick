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
        const QString& trickhost,
        uint trickport,
        double trickoffset,
        const QString& excludePattern,
        const QString& filterPattern,
        const QString& scripts,
        bool isDebug,
        bool isPlotAllVars,
        const QStringList &timeNames,
        double startTime, double stopTime,
        double timeMatchTolerance,
        double frequency,
        const QHash<QString,QVariant>& shifts, // rundir->shiftvalue
        const QString &presentation,
        const QString &dpDir,
        const QStringList& dpFiles,
        const QStringList& titles,
        const QStringList &legends, const QStringList &colors,
        const QStringList &linestyles,
        const QStringList &symbolstyles,
        const QString &orient, bool isLegend,
        const QString &foreground, const QString &background,
        bool isShowTables,
        QString map, QString mapFile,
        Runs* runs,
        QStandardItemModel* varsModel,
        QStandardItemModel *monteInputsModel,
        QWidget *parent) :
    QMainWindow(parent),
    _trickhost(trickhost),
    _trickport(trickport),
    _trickoffset(trickoffset),
    _excludePattern(excludePattern),
    _filterPattern(filterPattern),
    _scripts(scripts),
    _isDebug(isDebug),
    _timeNames(timeNames),
    _presentation(presentation),
    _dpDir(dpDir),
    _dpFiles(dpFiles),
    _titles(titles),
    _foreground(foreground),
    _background(background),
    _isShowTables(isShowTables),
    _map(map),
    _mapFile(mapFile),
    _runs(runs),
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
    _bookModel = new PlotBookModel(_timeNames,_runs,0,1,parent);
    if ( titles.size() == 4 ) {
        QStandardItem *rootItem = _bookModel->invisibleRootItem();
        QStandardItem *citem;
        citem = _bookModel->addChild(rootItem, "DefaultPageTitles","");
        _bookModel->addChild(citem, "Title1",titles.at(0));
        _bookModel->addChild(citem, "Title2",titles.at(1));
        _bookModel->addChild(citem, "Title3",titles.at(2));
        _bookModel->addChild(citem, "Title4",titles.at(3));
        _bookModel->addChild(rootItem, "LiveCoordTime","");
        _bookModel->addChild(rootItem, "LiveCoordTimeIndex",0);
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
    QStandardItem *rootItem = _bookModel->invisibleRootItem();
    _bookModel->addChild(rootItem, "Orientation", orient);
    _bookModel->addChild(rootItem, "TimeMatchTolerance", timeMatchTolerance);
    _bookModel->addChild(rootItem, "Frequency", frequency);
    _bookModel->addChild(rootItem, "IsLegend", isLegend);
    if ( colors.size() == 7 ) {
        QStandardItem *rootItem = _bookModel->invisibleRootItem();
        QStandardItem *citem;
        citem = _bookModel->addChild(rootItem, "LegendColors","");
        _bookModel->addChild(citem, "Color1",colors.at(0));
        _bookModel->addChild(citem, "Color2",colors.at(1));
        _bookModel->addChild(citem, "Color3",colors.at(2));
        _bookModel->addChild(citem, "Color4",colors.at(3));
        _bookModel->addChild(citem, "Color5",colors.at(4));
        _bookModel->addChild(citem, "Color6",colors.at(5));
        _bookModel->addChild(citem, "Color7",colors.at(6));
    }
    _bookModel->addChild(rootItem, "ForegroundColor", foreground);
    _bookModel->addChild(rootItem, "BackgroundColor", background);
    if ( linestyles.size() == 7 ) {
        QStandardItem *rootItem = _bookModel->invisibleRootItem();
        QStandardItem *citem;
        citem = _bookModel->addChild(rootItem, "Linestyles","");
        _bookModel->addChild(citem, "Linestyle1",linestyles.at(0));
        _bookModel->addChild(citem, "Linestyle2",linestyles.at(1));
        _bookModel->addChild(citem, "Linestyle3",linestyles.at(2));
        _bookModel->addChild(citem, "Linestyle4",linestyles.at(3));
        _bookModel->addChild(citem, "Linestyle5",linestyles.at(4));
        _bookModel->addChild(citem, "Linestyle6",linestyles.at(5));
        _bookModel->addChild(citem, "Linestyle7",linestyles.at(6));
    }
    if ( symbolstyles.size() == 7 ) {
        QStandardItem *rootItem = _bookModel->invisibleRootItem();
        QStandardItem *citem;
        citem = _bookModel->addChild(rootItem, "Symbolstyles","");
        _bookModel->addChild(citem, "Symbolstyle1",symbolstyles.at(0));
        _bookModel->addChild(citem, "Symbolstyle2",symbolstyles.at(1));
        _bookModel->addChild(citem, "Symbolstyle3",symbolstyles.at(2));
        _bookModel->addChild(citem, "Symbolstyle4",symbolstyles.at(3));
        _bookModel->addChild(citem, "Symbolstyle5",symbolstyles.at(4));
        _bookModel->addChild(citem, "Symbolstyle6",symbolstyles.at(5));
        _bookModel->addChild(citem, "Symbolstyle7",symbolstyles.at(6));
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
    connect(_bookModel,
            SIGNAL(rowsAboutToBeRemoved(QModelIndex,int,int)),
            this,
            SLOT(_bookModelRowsAboutToBeRemoved(QModelIndex,int,int)));
    connect(_bookModel,
            SIGNAL(dataChanged(QModelIndex,QModelIndex,QVector<int>)),
            this,
            SLOT(_bookModelDataChanged(QModelIndex,QModelIndex,QVector<int>)));

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
        if ( _monteInputsModel->rowCount() == 1 ) {
            // Hide if only viewing a single RUN
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
                                 _runs->runDirs(), _bookModel,
                                 _bookView->selectionModel(), _monteInputsView,
                                 varsFrame);
    if ( isPlotAllVars ) {
        _varsWidget->selectAllVars();
        _varsWidget->clearSelection();
    }
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
                                          _runs->runDirs(), _bookModel,
                                          _bookView->selectionModel(),
                                          _monteInputsView, _isShowTables,
                                          _dpFrame);
        _nbDPVars->setCurrentIndex(1);
    }
    connect(_nbDPVars,SIGNAL(currentChanged(int)),
            this,SLOT(_nbCurrentChanged(int)));

    // Vars/DP needs monteInputsView, but needs to be added after Vars/DP
    if ( _monteInputsModel ) {
        lsplit->addWidget(_monteInputsView);
    }

    // Start/Live/Stop times input
    _timeInput = new TimeInput(this);
    _timeInput->hideLiveTime();
    QSizePolicy sp(QSizePolicy::Preferred,QSizePolicy::Preferred);
    _timeInput->setSizePolicy(sp);
    lsplit->addWidget(_timeInput);
    lsplit->setStretchFactor(0,1);
    lsplit->setStretchFactor(1,0);
    connect(_timeInput,SIGNAL(startTimeChanged(double)),
            this, SLOT(_startTimeChanged(double)));
    connect(_timeInput,SIGNAL(liveTimeChanged(double)),
            this, SLOT(_liveTimeChanged(double)));
    connect(_timeInput,SIGNAL(liveTimeNext()),
            this, SLOT(_liveTimeNext()));
    connect(_timeInput,SIGNAL(liveTimePrev()),
            this, SLOT(_liveTimePrev()));
    connect(_timeInput,SIGNAL(stopTimeChanged(double)),
            this, SLOT(_stopTimeChanged(double)));
    connect(_bookModel,
            SIGNAL(dataChanged(QModelIndex,QModelIndex,QVector<int>)),
            _timeInput,
            SLOT(_slotDataChanged(QModelIndex,QModelIndex,QVector<int>)));

    /*
     * To run with the ball2 sim, add the following to the input file:
     * trick.var_server_set_port(<_trickport>)
     */
    _vsSocket = new QTcpSocket();
    connect(_vsSocket,SIGNAL(readyRead()),
            this,SLOT(_vsRead()));
    _vsSocket->connectToHost(_trickhost,_trickport);
    if (_vsSocket->waitForConnected(500)) {
        fprintf(stderr,"Connected To The Trick Variable Server!\n");
        _vsSocket->write("trick.var_add(\"trick_sys.sched.time_tics\")\n");
    } else {
        fprintf(stderr, "koviz [todo]: Timed out waiting "
                                      "on Trick var server\n");
    }

    // creating bviscom to send commands to bvis
    bviscom = new TimeCom(this);

    // sending run command if there is only one run
    if ( _monteInputsModel->rowCount() == 1 ) {
        QString sendRun = QString("%1/").arg(
                    QDir::current().absoluteFilePath(
                        _runs->runDirs().at(0)));
        bviscom->sendFirstRun(sendRun);
    }

    // creating a test videowindow
    vidView = new VideoWindow(this);
    vidView->show();
    this->setFocusPolicy(Qt::StrongFocus);
    connect(vidView,SIGNAL(timechangedByMpv(double)),
                           this, SLOT(setTimeFromVideo(double)));

    // Size main window
    QList<int> sizes;
    sizes << 420 << 1180;
    msplit->setSizes(sizes);
    msplit->setStretchFactor(0,0);
    msplit->setStretchFactor(1,1);

    // Read "INI" (may resize window)
    _readSettings();
}

void PlotMainWindow::setTimeFromVideo(double time) {
    QModelIndex liveIdx = _bookModel->getDataIndex(QModelIndex(),
                                                   "LiveCoordTime");
    _bookModel->setData(liveIdx,time);
}

PlotMainWindow::~PlotMainWindow()
{
    delete _bookModel;
    _vsSocket->close();
    delete _vsSocket;
}

void PlotMainWindow::createMenu()
{
    _menuBar = new QMenuBar;
    _fileMenu = new QMenu(tr("&File"), this);
    _optsMenu = new QMenu(tr("&Options"), this);
    _pdfAction  = _fileMenu->addAction(tr("Save &PDF"));
    _dpAction  = _fileMenu->addAction(tr("Save &DP"));
    _sessionAction = _fileMenu->addAction(tr("Save &Session"));
    _exitAction = _fileMenu->addAction(tr("E&xit"));
    _showLiveCoordAction = _optsMenu->addAction(tr("ShowLiveCoord"));
    _clearPlotsAction  = _optsMenu->addAction(tr("ClearPlots"));
    _clearTablesAction = _optsMenu->addAction(tr("ClearTables"));
    _plotAllVarsAction = _optsMenu->addAction(tr("PlotAllVars"));
    _showLiveCoordAction->setCheckable(true);
    _showLiveCoordAction->setChecked(true);
    _menuBar->addMenu(_fileMenu);
    _menuBar->addMenu(_optsMenu);
    if ( !_scripts.isEmpty() ) {
        _scriptsMenu = new QMenu(tr("&Scripts"), this);
        QStringList scripts = _scripts.split(',',QString::SkipEmptyParts);
        foreach ( QString script, scripts ) {
            QAction* action = _scriptsMenu->addAction(script);
            Q_UNUSED(action);
        }
        connect(_scriptsMenu,SIGNAL(triggered(QAction*)),
                this,SLOT(_launchScript(QAction*)));
        _menuBar->addMenu(_scriptsMenu);
    }
    connect(_dpAction, SIGNAL(triggered()),this, SLOT(_saveDP()));
    connect(_pdfAction, SIGNAL(triggered()),this, SLOT(_savePdf()));
    connect(_sessionAction, SIGNAL(triggered()),this, SLOT(_saveSession()));
    connect(_exitAction, SIGNAL(triggered()),this, SLOT(close()));
    connect(_showLiveCoordAction, SIGNAL(triggered()),
            this, SLOT(_toggleShowLiveCoord()));
    connect(_clearPlotsAction, SIGNAL(triggered()),
            this, SLOT(_clearPlots()));
    connect(_clearTablesAction, SIGNAL(triggered()),
            this, SLOT(_clearTables()));

    connect(_plotAllVarsAction, SIGNAL(triggered()),
            this, SLOT(_plotAllVars()));
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
                                          _varsModel, _runs->runDirs(), _bookModel,
                                          _bookView->selectionModel(),
                                          _monteInputsView, _isShowTables,
                                          _dpFrame);
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

    if ( !currIdx.isValid() ) {
        _timeInput->hideLiveTime();
        if ( _monteInputsView ) {
            // Clicked into whitespace in a CurvesView,
            // so clear current run in monte input view and all CurveViews
            QModelIndex invalidIdx;
            _monteInputsView->setCurrentIndex(invalidIdx);
            _bookView->setCurrentCurveRunID(-1);
        }
    } else {
        bool isShowLiveTime = false;
        if ( _bookModel->isIndex(currIdx,"Curve") ) {
            isShowLiveTime = true;
        } else if ( _bookModel->isIndex(currIdx,"Plot") ) {
            if (_bookModel->getDataString(currIdx,"PlotPresentation")=="error"){
                isShowLiveTime = true;
            }
        }
        if ( isShowLiveTime ) {
            _timeInput->showLiveTime();
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

void PlotMainWindow::_bookModelDataChanged(const QModelIndex &topLeft,
                                           const QModelIndex &bottomRight,
                                           const QVector<int> &roles)
{
    Q_UNUSED(roles);
    if ( topLeft != bottomRight ) return;
    const QAbstractItemModel* model = topLeft.model();
    if ( !model ) return;
    if ( topLeft.column() != 1 ) return;

    QString tag = model->data(topLeft.sibling(topLeft.row(),0)).toString();
    if ( tag == "LiveCoordTime" ) {
        bool ok;
        double liveTime = model->data(topLeft).toDouble(&ok);
        if ( ok ) {
            vidView->seek_time(liveTime);
            bviscom->sendTime2Bvis(liveTime);
        }
    }
}

void PlotMainWindow::_scriptError(QProcess::ProcessError error)
{
    Q_UNUSED(error);
    QMessageBox msgBox;
    QString msg = QString("Error launching user script!");
    msgBox.setText(msg);
    msgBox.exec();
}

void PlotMainWindow::_bookModelRowsAboutToBeRemoved(const QModelIndex &pidx,
                                                    int start, int end)
{
    Q_UNUSED(pidx);
    Q_UNUSED(start);
    Q_UNUSED(end);
    _varsWidget->clearSelection();
    if ( _dpTreeWidget ) {
        _dpTreeWidget->clearSelection();
    }
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

void PlotMainWindow::_saveDP()
{
    QString fname = QFileDialog::getSaveFileName(this,
                                                 QString("Save DP"),
                                                 QString(""),
                                                 tr("files (DP*)"));

    if ( fname.isEmpty() ) {
        return;
    }

    QString i1("  ");
    QString i2("    ");
    QString i3("      ");
    QString i4("        ");

    // Open DP file for writing
    QFile dp(fname);
    if (!dp.open(QIODevice::WriteOnly)) {
        fprintf(stderr,"koviz: [error] could not open %s\n",
                fname.toLatin1().constData());
        return;
    }
    QTextStream out(&dp);

    // Title
    out << "Koviz\n\n";

    // Plots
    out << "PLOTS:\n";

    // Pages
    int pageNum = 1;
    QModelIndexList pageIdxs = _bookModel->pageIdxs();
    foreach ( QModelIndex pageIdx, pageIdxs ) {
        QString pageTitle = _bookModel->getDataString(pageIdx,
                                                      "PageTitle","Page");
        if ( pageTitle == "Koviz" ) {
            pageTitle.clear();
        }
        out << i1 << "Page " << pageNum << ":" << " \"" << pageTitle << "\"\n";

        double pageStart = _bookModel->getDataDouble(pageIdx,
                                                     "PageStartTime","Page");
        if ( pageStart != -DBL_MAX ) {
            out << i2 << "start: " << pageStart << "\n";
        }

        double pageStop = _bookModel->getDataDouble(pageIdx,
                                                    "PageStopTime","Page");
        if ( pageStop != DBL_MAX ) {
            out << i2 << "stop: " << pageStop << "\n";
        }

        QString pageBG = _bookModel->getDataString(pageIdx,
                                                   "PageBackgroundColor",
                                                   "Page");
        if ( pageBG != "#FFFFFF") {
            out << i2 << "background_color: \"" << pageBG << "\"\n";
        }

        QString pageFG = _bookModel->getDataString(pageIdx,
                                                   "PageForegroundColor",
                                                   "Page");
        if ( pageFG != "#000000") {
            out << i2  << "foreground_color: \"" << pageFG << "\"\n";
        }

        out << "\n";

        int plotNum = 1;
        QModelIndexList plotIdxs = _bookModel->plotIdxs(pageIdx);
        foreach ( QModelIndex plotIdx, plotIdxs ) {
            QString plotTitle = _bookModel->getDataString(plotIdx,
                                                          "PlotTitle","Plot");
            out << i2 << "Plot " << plotNum << ":"
                << " \"" << plotTitle << "\"\n";

            double plotStart = _bookModel->getDataDouble(plotIdx,
                                                         "PlotStartTime",
                                                         "Plot");
            if ( plotStart != -DBL_MAX ) {
                out << i3 << "start: " << plotStart << "\n";
            }

            double plotStop = _bookModel->getDataDouble(plotIdx,
                                                        "PlotStopTime","Plot");
            if ( plotStop != DBL_MAX ) {
                out << i3 << "stop: " << plotStop << "\n";
            }

            double plotXMin = _bookModel->getDataDouble(plotIdx,
                                                        "PlotXMinRange","Plot");
            if ( plotXMin != -DBL_MAX ) {
                out << i3 << "x_min_range: " << plotXMin << "\n";
            }

            double plotXMax = _bookModel->getDataDouble(plotIdx,
                                                        "PlotXMaxRange","Plot");
            if ( plotXMax != DBL_MAX ) {
                out << i3 << "x_max_range: " << plotXMax << "\n";
            }

            double plotYMin = _bookModel->getDataDouble(plotIdx,
                                                        "PlotYMinRange","Plot");
            if ( plotYMin != -DBL_MAX ) {
                out << i3 << "y_min_range: " << plotYMin << "\n";
            }

            double plotYMax = _bookModel->getDataDouble(plotIdx,
                                                        "PlotYMaxRange","Plot");
            if ( plotYMax != DBL_MAX ) {
                out << i3 << "y_max_range: " << plotYMax << "\n";
            }

            QString plotBG = _bookModel->getDataString(plotIdx,
                                                       "PlotBackgroundColor",
                                                       "Plot");
            if ( plotBG != "#FFFFFF") {
                out << i3 << "background_color: \"" << plotBG << "\"\n";
            }

            QString plotFG = _bookModel->getDataString(plotIdx,
                                                       "PlotForegroundColor",
                                                       "Plot");
            if ( plotFG != "#000000" ) {
                out << i3 << "foreground_color: \"" << plotFG << "\"\n";
            }

            QString plotXScale = _bookModel->getDataString(plotIdx,
                                                           "PlotXScale",
                                                           "Plot");
            if ( plotXScale != "linear" ) {
                out << i3 << "plot_x_scale: \"" << plotXScale << "\"\n";
            }

            QString plotYScale = _bookModel->getDataString(plotIdx,
                                                           "PlotYScale",
                                                           "Plot");
            if ( plotYScale != "linear" ) {
                out << i3 << "plot_y_scale: \"" << plotYScale << "\"\n";
            }

            QModelIndex curvesIdx = _bookModel->getIndex(plotIdx,
                                                         "Curves","Plot");
            QModelIndexList curveIdxs = _bookModel->curveIdxs(curvesIdx);

            QString xAxisLabel = _bookModel->getDataString(plotIdx,
                                                           "PlotXAxisLabel",
                                                           "Plot");
            bool isXAxisLabel = true;
            foreach ( QModelIndex curveIdx, curveIdxs ) {
                QString xName = _bookModel->getDataString(curveIdx,
                                                          "CurveXName",
                                                          "Curve");
                if ( xName == xAxisLabel ||
                     (xName == "sys.exec.out.time" && xAxisLabel == "Time")) {
                    isXAxisLabel = false;
                    break;
                }
            }
            if ( isXAxisLabel ) {
                out << i3 << "x_axis_label: \"" << xAxisLabel << "\"\n" ;
            }

            QString yAxisLabel = _bookModel->getDataString(plotIdx,
                                                           "PlotYAxisLabel",
                                                           "Plot");
            bool isYAxisLabel = true;
            foreach ( QModelIndex curveIdx, curveIdxs ) {
                QString yName = _bookModel->getDataString(curveIdx,
                                                          "CurveYName",
                                                          "Curve");
                if ( yName == yAxisLabel ) {
                    isYAxisLabel = false;
                    break;
                }
            }
            if ( isYAxisLabel ) {
                out << i3 << "y_axis_label: \"" << yAxisLabel << "\"\n" ;
            }

            QStringList ynames;
            QString lastXName;
            foreach ( QModelIndex curveIdx, curveIdxs ) {
                QString xName = _bookModel->getDataString(curveIdx,
                                                          "CurveXName",
                                                          "Curve");
                QString yName = _bookModel->getDataString(curveIdx,
                                                          "CurveYName",
                                                          "Curve");
                if ( ynames.contains(yName) ) {
                    continue;
                }
                ynames << yName;

                if ( lastXName != xName ) {

                    out << i3 << "x_variable: \"" << xName << "\"\n";

                    QString xUnit = _bookModel->getDataString(curveIdx,
                                                              "CurveXUnit",
                                                              "Curve");
                    if ( !xUnit.isEmpty() ) {
                        out << i4 << "units: \"" << xUnit << "\"\n";
                    }

                    double curveXMin = _bookModel->getDataDouble(curveIdx,
                                                               "CurveXMinRange",
                                                               "Curve");
                    if ( curveXMin != -DBL_MAX ) {
                        out << i4 << "min_range: " << curveXMin << "\n";
                    }

                    double curveXMax = _bookModel->getDataDouble(curveIdx,
                                                               "CurveXMaxRange",
                                                               "Curve");
                    if ( curveXMax != DBL_MAX ) {
                        out << i4 << "max_range: " << curveXMax << "\n";
                    }

                    double curveXScale = _bookModel->getDataDouble(curveIdx,
                                                                  "CurveXScale",
                                                                   "Curve");
                    if ( curveXScale != 1.0 ) {
                        out << i4 << "scale_factor: " << curveXScale << "\n";
                    }

                    double curveXBias = _bookModel->getDataDouble(curveIdx,
                                                                  "CurveXBias",
                                                                  "Curve");
                    if ( curveXBias != 0.0 ) {
                        out << i4 << "bias: " << curveXBias << "\n";
                    }

                }

                out << i3 << "y_variable: \"" << yName << "\"\n";

                QString yUnit = _bookModel->getDataString(curveIdx,
                                                          "CurveYUnit",
                                                          "Curve");
                if ( !yUnit.isEmpty() ) {
                    out << i4 << "units: \"" << yUnit << "\"\n";
                }

                double curveYMin = _bookModel->getDataDouble(curveIdx,
                                                             "CurveYMinRange",
                                                             "Curve");
                if ( curveYMin != -DBL_MAX ) {
                    out << i4 << "min_range: " << curveYMin << "\n";
                }

                double curveYMax = _bookModel->getDataDouble(curveIdx,
                                                             "CurveYMaxRange",
                                                             "Curve");
                if ( curveYMax != DBL_MAX ) {
                    out << i4 << "max_range: " << curveYMax << "\n";
                }

                double curveYScale = _bookModel->getDataDouble(curveIdx,
                                                               "CurveYScale",
                                                               "Curve");
                if ( curveYScale != 1.0 ) {
                    out << i4 << "scale_factor: " << curveYScale << "\n";
                }

                double curveYBias = _bookModel->getDataDouble(curveIdx,
                                                              "CurveYBias",
                                                              "Curve");
                if ( curveYBias != 0.0 ) {
                    out << i4 << "bias: " << curveYBias << "\n";
                }

                QString symbolStyle = _bookModel->getDataString(curveIdx,
                                                             "CurveSymbolStyle",
                                                             "Curve");
                if ( !symbolStyle.isEmpty() ) {
                    out << i4 << "symbol_style: \"" << symbolStyle << "\"\n";
                }

                QString symbolSize = _bookModel->getDataString(curveIdx,
                                                              "CurveSymbolSize",
                                                              "Curve");
                if ( !symbolSize.isEmpty() ) {
                    out << i4 << "symbol_size: \"" << symbolSize << "\"\n";
                }

                QString lineStyle = _bookModel->getDataString(curveIdx,
                                                              "CurveLineStyle",
                                                              "Curve");
                if ( !lineStyle.isEmpty() && lineStyle != "plain" ) {
                    out << i4 << "line_style: \"" << lineStyle << "\"\n";
                }

                // Only do color if it is a named color (not #NNNNNN)
                QString lineColor = _bookModel->getDataString(curveIdx,
                                                              "CurveColor",
                                                              "Curve");
                if ( !lineColor.isEmpty() && !lineColor.startsWith("#")) {
                    out << i4 << "line_color: \"" << lineColor << "\"\n";
                }

                QString yLabel = _bookModel->getDataString(curveIdx,
                                                           "CurveYLabel",
                                                           "Curve");
                if ( !yLabel.isEmpty() ) {
                    out << i4 << "label: \"" << yLabel << "\"\n";
                }

                lastXName = xName;
            }

            out << "\n";
            ++plotNum;
        }
        out << "\n";

        ++pageNum;
    }

    // Tables
    int tableNum = 1;
    QModelIndex tablesIdx = _bookModel->getIndex(QModelIndex(),"Tables");
    QModelIndexList tableIdxs = _bookModel->getIndexList(tablesIdx,"Table");
    foreach ( QModelIndex tableIdx, tableIdxs ) {
        if ( tableNum == 1 ) {
            out << "TABLES:\n";
        }
        // TODO Save DP Tables !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    }

    dp.close();
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
                                                 QString("Save PDF"),
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

void PlotMainWindow::_saveSession()
{
    QString fname = QFileDialog::getSaveFileName(this,
                                                 QString("Save Session"),
                                                 QString("session_"),
                                                 tr("files (session*)"));

    if ( ! fname.isEmpty() ) {

        // Open trk file for writing
        QFile f(fname);
        if (!f.open(QIODevice::WriteOnly)) {
            fprintf(stderr,"koviz: [error] could not open %s\n",
                    fname.toLatin1().constData());
            exit(-1);
        }
        QTextStream out(&f);

        // Session Title
        QFileInfo fi(".");
        QString userName = fi.owner();
        QDate date = QDate::currentDate();
        QString fmt("MMMM d, yyyy");
        QString dateStr = date.toString(fmt);
        QString sessionTitle = "Session - " + userName + " " + dateStr ;
        out << sessionTitle;
        out << "\n\n";

        // RUNs
        foreach ( QString run, _runs->runDirs() ) {
            out << "RUN: " << run << "\n";
        }

        // DPs
        foreach ( QString dp, _dpFiles ) {
            out << "PRODUCT: " << dp << "\n";
        }

        // Title1
        QModelIndex titlesIdx = _bookModel->getIndex(QModelIndex(),
                                                     "DefaultPageTitles");
        QString title1 = _bookModel->getDataString(titlesIdx,
                                                   "Title1",
                                                   "DefaultPageTitles");
        if ( !title1.startsWith("koviz") ) {
            out << "t1: " << title1 << "\n";
        } else {
            out << "t1: \"\"\n";
        }

        // Title2
        if ( !_titles.at(1).contains('\n') ) {
            out << "t2: " << _titles.at(1) << "\n";
        } else {
            out << "t2: \"\"\n";
        }

        // Title3
        if ( !_titles.at(2).startsWith("User:") ) {
             out << "t3: " << _titles.at(2) << "\n";
        } else {
            out << "t3: \"\"\n";
        }

        // Title4
        if ( !_titles.at(3).startsWith("Date:") ) {
             out << "t4: " << _titles.at(3) << "\n";
        } else {
            out << "t4: \"\"\n";
        }

        // Foreground
        if ( !_foreground.isEmpty() ) {
             out << "fg: " << _foreground << "\n";
        }

        // Background
        if ( !_background.isEmpty() ) {
             out << "bg: " << _background << "\n";
        }

        // RUN/log Exclude and Filter patterns
        if ( !_excludePattern.isEmpty() ) {
            out << "exclude:" << _excludePattern << "\n";
        }
        if ( !_filterPattern.isEmpty() ) {
            out << "filter:" << _filterPattern << "\n";
        }

        // Time match tolerance
        double tmt = _bookModel->getDataDouble(QModelIndex(),
                                               "TimeMatchTolerance","");
        out << "time_match_tolerance:" << tmt << "\n";

        // Legend/Curve Colors
        QModelIndex clrIdx = _bookModel->getIndex(QModelIndex(),
                                                  "LegendColors","");
        QString c1 = _bookModel->getDataString(clrIdx,"Color1","LegendColors");
        QString c2 = _bookModel->getDataString(clrIdx,"Color2","LegendColors");
        QString c3 = _bookModel->getDataString(clrIdx,"Color3","LegendColors");
        QString c4 = _bookModel->getDataString(clrIdx,"Color4","LegendColors");
        QString c5 = _bookModel->getDataString(clrIdx,"Color5","LegendColors");
        QString c6 = _bookModel->getDataString(clrIdx,"Color6","LegendColors");
        QString c7 = _bookModel->getDataString(clrIdx,"Color7","LegendColors");
        out << "c1: \"" << c1 << "\"\n" ;
        out << "c2: \"" << c2 << "\"\n" ;
        out << "c3: \"" << c3 << "\"\n" ;
        out << "c4: \"" << c4 << "\"\n" ;
        out << "c5: \"" << c5 << "\"\n" ;
        out << "c6: \"" << c6 << "\"\n" ;
        out << "c7: \"" << c7 << "\"\n" ;

        // Legend labels
        QModelIndex legIdx = _bookModel->getIndex(QModelIndex(),"LegendLabels");
        QString l1 = _bookModel->getDataString(legIdx,"Label1","LegendLabels");
        QString l2 = _bookModel->getDataString(legIdx,"Label2","LegendLabels");
        QString l3 = _bookModel->getDataString(legIdx,"Label3","LegendLabels");
        QString l4 = _bookModel->getDataString(legIdx,"Label4","LegendLabels");
        QString l5 = _bookModel->getDataString(legIdx,"Label5","LegendLabels");
        QString l6 = _bookModel->getDataString(legIdx,"Label6","LegendLabels");
        QString l7 = _bookModel->getDataString(legIdx,"Label7","LegendLabels");
        out << "l1: \"" << l1 << "\"\n" ;
        out << "l2: \"" << l2 << "\"\n" ;
        out << "l3: \"" << l3 << "\"\n" ;
        out << "l4: \"" << l4 << "\"\n" ;
        out << "l5: \"" << l5 << "\"\n" ;
        out << "l6: \"" << l6 << "\"\n" ;
        out << "l7: \"" << l7 << "\"\n" ;

        // Linestyles
        QModelIndex lsIdx = _bookModel->getIndex(QModelIndex(),"Linestyles");
        QString ls1 =_bookModel->getDataString(lsIdx,"Linestyle1","Linestyles");
        QString ls2 =_bookModel->getDataString(lsIdx,"Linestyle2","Linestyles");
        QString ls3 =_bookModel->getDataString(lsIdx,"Linestyle3","Linestyles");
        QString ls4 =_bookModel->getDataString(lsIdx,"Linestyle4","Linestyles");
        QString ls5 =_bookModel->getDataString(lsIdx,"Linestyle5","Linestyles");
        QString ls6 =_bookModel->getDataString(lsIdx,"Linestyle6","Linestyles");
        QString ls7 =_bookModel->getDataString(lsIdx,"Linestyle7","Linestyles");
        out << "ls1: \"" << ls1 << "\"\n" ;
        out << "ls2: \"" << ls2 << "\"\n" ;
        out << "ls3: \"" << ls3 << "\"\n" ;
        out << "ls4: \"" << ls4 << "\"\n" ;
        out << "ls5: \"" << ls5 << "\"\n" ;
        out << "ls6: \"" << ls6 << "\"\n" ;
        out << "ls7: \"" << ls7 << "\"\n" ;

        // Symbolstyles
        QModelIndex ssIdx = _bookModel->getIndex(QModelIndex(),"Symbolstyles");
        QString s1 =_bookModel->getDataString(ssIdx,"Symbolstyle1",
                                                    "Symbolstyles");
        QString s2 =_bookModel->getDataString(ssIdx,"Symbolstyle2",
                                                    "Symbolstyles");
        QString s3 =_bookModel->getDataString(ssIdx,"Symbolstyle3",
                                                    "Symbolstyles");
        QString s4 =_bookModel->getDataString(ssIdx,"Symbolstyle4",
                                                    "Symbolstyles");
        QString s5 =_bookModel->getDataString(ssIdx,"Symbolstyle5",
                                                    "Symbolstyles");
        QString s6 =_bookModel->getDataString(ssIdx,"Symbolstyle6",
                                                    "Symbolstyles");
        QString s7 =_bookModel->getDataString(ssIdx,"Symbolstyle7",
                                                    "Symbolstyles");
        out << "s1: \"" << s1 << "\"\n" ;
        out << "s2: \"" << s2 << "\"\n" ;
        out << "s3: \"" << s3 << "\"\n" ;
        out << "s4: \"" << s4 << "\"\n" ;
        out << "s5: \"" << s5 << "\"\n" ;
        out << "s6: \"" << s6 << "\"\n" ;
        out << "s7: \"" << s7 << "\"\n" ;

        // Show Legend
        QString isLegend = _bookModel->getDataString(QModelIndex(),"IsLegend");
        out << "legend: " << isLegend << "\n";

        // Orientation
        QString orient = _bookModel->getDataString(QModelIndex(),"Orientation");
        out << "orient: " << orient << "\n";

        // Presentation
        QString pres = _bookModel->getDataString(QModelIndex(),"Presentation");
        out << "presentation: " << pres << "\n";

        // Shift
        QHash<QString,QVariant> shifts = _bookModel->getDataHash(QModelIndex(),
                                                             "RunToShiftHash");
        QString shift;
        foreach ( QString run, shifts.keys() ) {
            QString shiftVal = shifts.value(run).toString();
            shift += run + ":" + shiftVal + ",";
        }
        if ( !shift.isEmpty() ) {
            shift.chop(1); // chop off comma
            out << "shift: " << shift << "\n";
        }

        // Show Tables
        out << "showTables: ";
        if ( _isShowTables ) {
            out << "on\n";
        } else {
            out << "off\n";
        }

        // Start/Stop Times
        double start = _bookModel->getDataDouble(QModelIndex(),"StartTime");
        double stop  = _bookModel->getDataDouble(QModelIndex(),"StopTime");
        if ( start != -DBL_MAX ) {
            out << "start: " << start << "\n";
        }
        if ( stop != DBL_MAX ) {
            out << "stop: "  << stop << "\n";
        }

        // Timename(s)
        QString timeNames;
        foreach ( QString timeName, _timeNames ) {
            timeNames += timeName + "=";
        }
        if ( !timeNames.isEmpty() ) {
            timeNames.chop(1);
            out << "timeName: " << timeNames << "\n";
        }

        // Map or mapfile
        if ( !_map.isEmpty() && !_mapFile.isEmpty() ) {
            fprintf(stderr, "koviz [bad scoobs]: PlotMainWindow::_saveSession()"
                            " commandline options map=%s and mapFile=%s cannot "
                            "be used together.\n",
                            _map.toLatin1().constData(),
                            _mapFile.toLatin1().constData());
            exit(-1);
        }
        if ( !_map.isEmpty() ) {
            out << "map: " << _map;
        }
        if ( !_mapFile.isEmpty() ) {
            out << "mapFile: " << _mapFile;
        }


        f.close();
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

void PlotMainWindow::_clearTables()
{
    QModelIndex tablesIdx = _bookModel->getIndex(QModelIndex(),"Tables");
    int nTables = _bookModel->rowCount(tablesIdx);
    for (int i = nTables-1; i >= 0; --i) {
        _bookModel->removeRow(i,tablesIdx);
    }
}

void PlotMainWindow::_launchScript(QAction* action)
{
    int i = _monteInputsView->currentRun();
    QString rundir;
    if ( i >= 0 ) {
        rundir = _runs->runDirs().at(i);
    } else {
        if ( _runs->runDirs().size() == 1 ) {
            rundir = _runs->runDirs().at(0);
        } else {
            QMessageBox msgBox;
            msgBox.setText("Please select a run before launching your script.");
            msgBox.exec();
        }
    }
    if ( !rundir.isEmpty() ) {
        QStringList fields = action->text().split(' ',
                                                  QString::SkipEmptyParts);
        QString program = fields.takeAt(0);
        program = program.remove('&');
        QStringList arguments;
        arguments << rundir;
        foreach ( QString field, fields ) {
            arguments << field;
        }
        QProcess *proc = new QProcess(this);
        connect(proc,SIGNAL(errorOccurred(QProcess::ProcessError)),
                this,SLOT(_scriptError(QProcess::ProcessError)));
        proc->start(program, arguments);
    }
}

void PlotMainWindow::_plotAllVars()
{
    _varsWidget->selectAllVars();
    _varsWidget->clearSelection();
}

void PlotMainWindow::_startTimeChanged(double startTime)
{
    QModelIndex startTimeIdx = _bookModel->getDataIndex(QModelIndex(),
                                                        "StartTime");
    _bookModel->setData(startTimeIdx,startTime);

    QModelIndexList pageIdxs = _bookModel->pageIdxs();
    foreach ( QModelIndex pageIdx, pageIdxs ) {
        foreach ( QModelIndex plotIdx, _bookModel->plotIdxs(pageIdx) ) {
            QModelIndex curvesIdx = _bookModel->getIndex(plotIdx,
                                                         "Curves","Plot");
            QRectF bbox = _bookModel->calcCurvesBBox(curvesIdx);
            _bookModel->setPlotMathRect(bbox,plotIdx);
        }
    }

    // If live time is less than start time, change live time to start time
    QModelIndex liveIdx = _bookModel->getDataIndex(QModelIndex(),
                                                   "LiveCoordTime");
    double liveTime = _bookModel->data(liveIdx).toDouble();
    if ( liveTime < startTime ) {
        _bookModel->setData(liveIdx,startTime);
    }
}

void PlotMainWindow::_liveTimeChanged(double liveTime)
{
    QModelIndex curveIdx = _currCurveIdx();
    CurveModel* curveModel = _bookModel->getCurveModel(curveIdx);

    if ( curveModel ) {

        curveModel->map();

        // Get curve index i of liveTime
        double xs = _bookModel->xScale(curveIdx);
        double xb = _bookModel->xBias(curveIdx);
        int i = 0;
        bool isXTime = (curveModel->x()->name() == curveModel->t()->name());
        if ( isXTime ) {
            i = curveModel->indexAtTime((liveTime-xb)/xs);
        } else {
            // e.g. ball xy curve where x is position[0]
            i = curveModel->indexAtTime(liveTime);
        }

        ModelIterator* it = curveModel->begin();
        double t = it->at(i)->t();
        if ( isXTime ) {
            t = t*xs+xb;
        }
        delete it;
        curveModel->unmap();

        QModelIndex rootIdx;
        double start = _bookModel->getDataDouble(rootIdx,"StartTime");
        double stop  = _bookModel->getDataDouble(rootIdx,"StopTime");
        t = (t < start) ? start : t;
        t = (t > stop)  ? stop  : t;
        QModelIndex liveIdx = _bookModel->getDataIndex(QModelIndex(),
                                                       "LiveCoordTime");
        _bookModel->setData(liveIdx,t);
        _timeInput->setLiveTime(t);
    }
}

void PlotMainWindow::_stopTimeChanged(double stopTime)
{
    QModelIndex stopTimeIdx = _bookModel->getDataIndex(QModelIndex(),
                                                       "StopTime");
    _bookModel->setData(stopTimeIdx,stopTime);

    QModelIndexList pageIdxs = _bookModel->pageIdxs();
    foreach ( QModelIndex pageIdx, pageIdxs ) {
        foreach ( QModelIndex plotIdx, _bookModel->plotIdxs(pageIdx) ) {
            QModelIndex curvesIdx = _bookModel->getIndex(plotIdx,
                                                         "Curves","Plot");
            QRectF bbox = _bookModel->calcCurvesBBox(curvesIdx);
            _bookModel->setPlotMathRect(bbox,plotIdx);
        }
    }

    // If live time is greater than stop time, change live time to stop time
    QModelIndex liveIdx = _bookModel->getDataIndex(QModelIndex(),
                                                   "LiveCoordTime");
    double liveTime = _bookModel->data(liveIdx).toDouble();
    if ( liveTime > stopTime ) {
        _bookModel->setData(liveIdx,stopTime);
    }
}

void PlotMainWindow::_liveTimeNext()
{
    QModelIndex curveIdx = _currCurveIdx();
    CurveModel* curveModel = _bookModel->getCurveModel(curveIdx);

    if ( curveModel ) {

        curveModel->map();

        ModelIterator* it = curveModel->begin();

        // Calculate curve time index
        int i = 0;
        double xs = _bookModel->xScale(curveIdx);
        double xb = _bookModel->xBias(curveIdx);
        QModelIndex liveIdx = _bookModel->getDataIndex(QModelIndex(),
                                                       "LiveCoordTime");
        double liveTime = _bookModel->data(liveIdx).toDouble();
        bool isXTime = (curveModel->x()->name() == curveModel->t()->name());
        if ( isXTime ) {
            i = curveModel->indexAtTime((liveTime-xb)/xs);
        } else {
            // e.g. ball xy curve where x is position[0]
            i = curveModel->indexAtTime(liveTime);
        }

        /* Timestamps may duplicate, go to first in series */
        double itTime = it->at(i)->t();
        while ( i > 0 ) {
            if ( it->at(i-1)->t() == itTime ) {
                --i;
            } else {
                break;
            }
        }


        QModelIndex lctIdx = _bookModel->getDataIndex(QModelIndex(),
                                                      "LiveCoordTimeIndex","");
        int ii = _bookModel->getDataInt(QModelIndex(),
                                          "LiveCoordTimeIndex","");

        // Calculate nextTime after liveTime
        double nextTime = liveTime;
        it = it->at(i+1+ii);
        while ( !it->isDone() ) {
            if ( isXTime ) {
                nextTime = it->x()*xs+xb;
            } else {
                nextTime = it->t();
            }
            double dt = qAbs(nextTime-liveTime);
            if ( dt == 0.0 ) {
                // Multiple points for same timestamp,
                // move to next point on this curve for this same timestamp
                _bookModel->setData(lctIdx,++ii);
                break;
            } else {
                if ( ii != 0 ) {
                    _bookModel->setData(lctIdx,0);
                }
            }
            if ( dt > 1.0e-16 ) {
                break;
            }
            it->next();
        }
        delete it;

        // nextTime should not exceed stop time
        double stop  = _bookModel->getDataDouble(QModelIndex(),"StopTime");
        nextTime = (nextTime > stop)  ? stop  : nextTime;

        // Update liveTime to nextTime
        _bookModel->setData(liveIdx,nextTime);

        curveModel->unmap();
    }
}

void PlotMainWindow::_liveTimePrev()
{
    QModelIndex curveIdx = _currCurveIdx();
    CurveModel* curveModel = _bookModel->getCurveModel(curveIdx);

    if ( curveModel ) {

        curveModel->map();

        // Calculate curve time index
        int i = 0;
        double xs = _bookModel->xScale(curveIdx);
        double xb = _bookModel->xBias(curveIdx);
        QModelIndex liveIdx = _bookModel->getDataIndex(QModelIndex(),
                                                       "LiveCoordTime");
        double liveTime = _bookModel->data(liveIdx).toDouble();
        bool isXTime = (curveModel->x()->name() == curveModel->t()->name());
        if ( isXTime ) {
            i = curveModel->indexAtTime((liveTime-xb)/xs);
        } else {
            // e.g. ball xy curve where x is position[0]
            i = curveModel->indexAtTime(liveTime);
        }

        double prevTime = liveTime;
        ModelIterator* it = curveModel->begin();

        /* Timestamps may be identical, set i to first duplicate */
        double itTime = it->at(i)->t();
        while ( i >= 1 ) {
            if ( it->at(i-1)->t() == itTime ) {
                --i;
            } else {
                break;
            }
        }

        /* Get current index for possible duplicate timestamps (ii) */
        QModelIndex idx = _bookModel->getDataIndex(QModelIndex(),
                                                   "LiveCoordTimeIndex","");
        int ii = _bookModel->getDataInt(QModelIndex(),
                                        "LiveCoordTimeIndex","");


        while ( i+ii > 0 ) {
            it = it->at(i+ii-1);
            if ( isXTime ) {
                prevTime = it->x()*xs+xb;
            } else {
                prevTime = it->t();
            }
            double dt = qAbs(liveTime-prevTime);
            if ( dt == 0 ) {
                // Multiple points for same timestamp,
                // move to prev point on this curve for this same timestamp
                _bookModel->setData(idx,--ii);
                break;
            } else {
                int jj = 0;  // Last index of possible duplicate timestamps
                int j = i-1;
                double jTime = it->at(j+ii)->t();
                while ( j+ii-1 >= 0 ) {
                    if ( it->at(j+ii-1)->t() == jTime ) {
                        ++jj;
                        --j;
                    } else {
                        break;
                    }
                }
                _bookModel->setData(idx,jj);
            }
            if ( dt > 1.0e-16 ) {
                break;
            }
            --i;
        }
        delete it;

        // prevTime should not precede start time
        double start  = _bookModel->getDataDouble(QModelIndex(),"StartTime");
        prevTime = (prevTime < start)  ? start  : prevTime;

        // Update liveTime to prevTime
        _bookModel->setData(liveIdx,prevTime);

        curveModel->unmap();
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
            _bookModel->blockSignals(block);

            // Set one curve color while signals on so that the dataChanged
            // signal will fire and a new pixmap will be created
            _bookModel->setData(changeColorIdx,changeColorTo);
        }
    }
}

void PlotMainWindow::_monteInputsViewCurrentChanged(const QModelIndex &currIdx,
                                                    const QModelIndex &prevIdx)
{
    if ( currIdx.isValid() ) {
        // set all curves in bookview with runID to current
        // Note: column 0 is runID by convention
        QModelIndex runIDIdx = _monteInputsView->model()->sibling(currIdx.row(),
                                                                  0,currIdx);
        int runID = _monteInputsView->model()->data(runIDIdx).toInt();
        _bookView->setCurrentCurveRunID(runID);
        if (currIdx.row() != prevIdx.row())
        {
            QString sendRun = QString("%1/").arg(
                        QDir::current().absoluteFilePath(
                            _runs->runDirs().at(runID)));
            bviscom->sendRun2Bvis(sendRun);
        }
    }
}

void PlotMainWindow::_writeSettings()
{
    QSettings settings("JSC", "koviz");

    settings.beginGroup("PlotMainWindow");
    settings.setValue("size", size());
    settings.setValue("pos", pos());
    settings.endGroup();
}

// On linux the settings file is in ~/.config/JSC/koviz.conf
void PlotMainWindow::_readSettings()
{
    QSettings settings("JSC", "koviz");

    settings.beginGroup("PlotMainWindow");
    resize(settings.value("size", QSize(1300, 720)).toSize());
    move(settings.value("pos", QPoint(200, 200)).toPoint());
    settings.endGroup();
}

void PlotMainWindow::closeEvent(QCloseEvent *event)
{
    _writeSettings();
    event->accept();
}

QModelIndex PlotMainWindow::_currCurveIdx()
{
    QModelIndex curveIdx;

    QModelIndex currIdx = _bookView->currentIndex();
    if ( currIdx.isValid() ) {
        QString tag = _bookModel->data(currIdx).toString();
        if ( tag == "Curve" ) {
            curveIdx = currIdx;
        } else {
            QModelIndex pageIdx;
            if ( tag ==  "Page" ) {
                pageIdx = currIdx;
            } else {
                QModelIndexList pageIdxs = _bookModel->pageIdxs();
                if ( !pageIdxs.isEmpty() ) {
                    pageIdx = pageIdxs.at(0);
                }
            }
            if ( pageIdx.isValid() ) {
                QModelIndex monteIdx;
                if ( _monteInputsView ) {
                    monteIdx = _monteInputsView->currentIndex();
                }
                if ( monteIdx.isValid() ) {
                    // Find first curve in first plot with matching runID
                    QModelIndex runIdx = monteIdx.sibling(monteIdx.row(),0);
                    int runID = _monteInputsView->model()->data(runIdx).toInt();
                    QModelIndex plotsIdx = _bookModel->getIndex(pageIdx,
                                                                "Plots","Page");
                    QModelIndexList plotIdxs = _bookModel->getIndexList(
                                plotsIdx,
                                "Plot",
                                "Plots");
                    foreach ( QModelIndex plotIdx, plotIdxs ) {
                        bool isFound = false;
                        QModelIndex curvesIdx = _bookModel->getIndex(plotIdx,
                                                                     "Curves",
                                                                     "Plot");
                        QModelIndexList curveIdxs = _bookModel->getIndexList(
                                    curvesIdx,
                                    "Curve",
                                    "Curves");
                        foreach ( QModelIndex cIdx, curveIdxs ) {
                            int curveRunID = _bookModel->getDataInt(cIdx,
                                                                   "CurveRunID",
                                                                   "Curve");
                            if ( curveRunID == runID ) {
                                curveIdx = cIdx;
                                isFound = true;
                                break;
                            }
                        }
                        if ( isFound ) {
                            break;
                        }
                    }
                } else {
                    // Find first curve on page regardless of runID match
                    QModelIndexList plotIdxs = _bookModel->plotIdxs(pageIdx);
                    if ( ! plotIdxs.isEmpty() ) {
                        QModelIndex plotIdx0 = plotIdxs.at(0);
                        QModelIndex curvesIdx = _bookModel->getIndex(plotIdx0,
                                                                     "Curves",
                                                                     "Plot");
                        QModelIndexList curveIdxs = _bookModel->curveIdxs(
                                                                     curvesIdx);
                        if ( !curveIdxs.isEmpty() ) {
                            curveIdx = curveIdxs.at(0);
                        }
                    }
                }
            }
        }
    }

    return curveIdx;
}

void PlotMainWindow::_vsRead()
{
    QByteArray bytes = _vsSocket->readLine();
    QString msg(bytes);
    QStringList fields = msg.split('\t');
    double time = fields.at(1).toDouble()/1.0e6;

    static double lasttime = -1.0;
    if ( time != lasttime ) {
        vidView->seek_time(time+_trickoffset);
        lasttime = time;
    }
}
