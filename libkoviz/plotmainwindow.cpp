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
        double timeMatchTolerance,
        double frequency,
        const QHash<QString,QVariant>& shifts, // rundir->shiftvalue
        const QString &presentation,
        const QString &dpDir,
        const QStringList& dpFiles,
        const QStringList& titles,
        const QStringList &legends, const QStringList &colors,
        const QString &orient, bool isLegend,
        const QString &foreground, const QString &background,
        bool isShowTables,
        Runs* runs,
        QStandardItemModel* varsModel,
        QStandardItemModel *monteInputsModel,
        QWidget *parent) :
    QMainWindow(parent),
    _isDebug(isDebug),
    _timeNames(timeNames),
    _presentation(presentation),
    _dpDir(dpDir),
    _dpFiles(dpFiles),
    _isShowTables(isShowTables),
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

    // Read "INI" (may resize window)
    _readSettings();
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
    _pdfAction  = _fileMenu->addAction(tr("Save &PDF"));
    _dpAction  = _fileMenu->addAction(tr("Save &DP"));
    _exitAction = _fileMenu->addAction(tr("E&xit"));
    _showLiveCoordAction = _optsMenu->addAction(tr("ShowLiveCoord"));
    _clearPlotsAction  = _optsMenu->addAction(tr("ClearPlots"));
    _clearTablesAction = _optsMenu->addAction(tr("ClearTables"));
    _showLiveCoordAction->setCheckable(true);
    _showLiveCoordAction->setChecked(true);
    _menuBar->addMenu(_fileMenu);
    _menuBar->addMenu(_optsMenu);
    connect(_dpAction, SIGNAL(triggered()),this, SLOT(_saveDP()));
    connect(_pdfAction, SIGNAL(triggered()),this, SLOT(_savePdf()));
    connect(_exitAction, SIGNAL(triggered()),this, SLOT(close()));
    connect(_showLiveCoordAction, SIGNAL(triggered()),
            this, SLOT(_toggleShowLiveCoord()));
    connect(_clearPlotsAction, SIGNAL(triggered()),
            this, SLOT(_clearPlots()));
    connect(_clearTablesAction, SIGNAL(triggered()),
            this, SLOT(_clearTables()));
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
        if ( _monteInputsView ) {
            // Clicked into whitespace in a CurvesView,
            // so clear current run in monte input view and all CurveViews
            QModelIndex invalidIdx;
            _monteInputsView->setCurrentIndex(invalidIdx);
            _bookView->setCurrentCurveRunID(-1);
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

void PlotMainWindow::_bookModelRowsAboutToBeRemoved(const QModelIndex &pidx,
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
                if ( !lineStyle.isEmpty() ) {
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
    Q_UNUSED(prevIdx);

    if ( currIdx.isValid() ) {
        // set all curves in bookview with runID to current
        // Note: column 0 is runID by convention
        QModelIndex runIDIdx = _monteInputsView->model()->sibling(currIdx.row(),
                                                                  0,currIdx);
        int runID = _monteInputsView->model()->data(runIDIdx).toInt();
        _bookView->setCurrentCurveRunID(runID);
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
