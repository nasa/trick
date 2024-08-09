#include "dptreewidget.h"

QString DPTreeWidget::_err_string;
QTextStream DPTreeWidget::_err_stream(&DPTreeWidget::_err_string);

DPTreeView::DPTreeView(QWidget *parent) : QTreeView(parent) {}

void DPTreeView::currentChanged(const QModelIndex &current,
                                const QModelIndex &previous)
{
    QAbstractItemView::currentChanged(current,previous);
    if ( allColumnsShowFocus() ) {
        if ( previous.isValid() ) {
            QRect prevRect = visualRect(previous);
            prevRect.setX(0);
            prevRect.setWidth(viewport()->width());
            viewport()->update(prevRect);
        }
        if ( current.isValid() ) {
            QRect currRect = visualRect(current);
            currRect.setX(0);
            currRect.setWidth(viewport()->width());
            viewport()->update(currRect);
        }
    }
}

void DPTreeView::selectionChanged(const QItemSelection &selected,
                                  const QItemSelection &deselected)
{
    QAbstractItemView::selectionChanged(selected,deselected);
}

DPTreeWidget::DPTreeWidget(const QString& timeName,
                           const QString &dpDirName,
                           const QStringList &dpFiles,
                           Runs* runs,
                           PlotBookModel *bookModel,
                           QItemSelectionModel *bookSelectModel,
                           MonteInputsView *monteInputsView,
                           SieListModel *sieModel,
                           TVModel *tvModel,
                           bool isShowTables,
                           const QStringList &unitOverrides,
                           QWidget *parent) :
    QWidget(parent),
    _idNum(0),
    _timeName(timeName),
    _dpDirName(dpDirName),
    _dpFiles(dpFiles),
    _runs(runs),
    _bookModel(bookModel),
    _bookSelectModel(bookSelectModel),
    _monteInputsView(monteInputsView),
    _sieModel(sieModel),
    _tvModel(tvModel),
    _isShowTables(isShowTables),
    _unitOverrides(unitOverrides),
    _gridLayout(0),
    _msgLabel(0),
    _searchBox(0)
{
    _setupModel();

    _gridLayout = new QGridLayout(parent);

    _msgLabel = new QLabel(parent);
    _gridLayout->addWidget(_msgLabel,0,0);

    _searchBox = new QLineEdit(parent);
    connect(_searchBox,SIGNAL(textChanged(QString)),
            this,SLOT(_searchBoxTextChanged(QString)));
    _gridLayout->addWidget(_searchBox,1,0);

    _dpTreeView = new DPTreeView(parent);
    _dpTreeView->setModel(_dpFilterModel);
    QModelIndex proxyRootIdx = _dpFilterModel->mapFromSource(_dpModelRootIdx);
    _dpTreeView->setRootIndex(proxyRootIdx);
    _dpTreeView->setFocusPolicy(Qt::ClickFocus);
    _gridLayout->addWidget(_dpTreeView,2,0);
    connect(_dpTreeView->selectionModel(),
            SIGNAL(currentChanged(QModelIndex,QModelIndex)),
            this, SLOT(_dpTreeViewCurrentChanged(QModelIndex,QModelIndex)));

    if ( _sieModel && _tvModel ) {
        connect(_sieModel, SIGNAL(modelLoaded()),
                this,SLOT(_loadDPFiles()),
                Qt::QueuedConnection);
        connect(_sieModel, SIGNAL(sendMessage(QString)),
                this, SLOT(_setMsgLabel(QString)));
        connect(_tvModel, SIGNAL(rowsInserted(QModelIndex,int,int)),
                this,SLOT(_tvModelRowAppended(QModelIndex,int,int)));
        connect(_tvModel, SIGNAL(modelAboutToBeReset()),
                this, SLOT(_tvModelAboutToBeReset()));

    } else {
        _msgLabel->hide();
        _loadDPFiles();
    }
}

DPTreeWidget::~DPTreeWidget()
{
    delete _dpModel;
    delete _dir;
    delete _dpFilterModel;

    foreach ( ProgramModel* program, _programModels ) {
        delete program;
    }
    _programModels.clear();
}

//
// DP File/Sys Model and Filter Proxy Model
//
void DPTreeWidget::_setupModel()
{
    _dir = new QDir(_dpDirName);

    // Change directory from RUN/MONTE dir to DP_Product dir (if possible)
    bool isFound = false;
    while ( 1 ) {
        QString path1 = QDir::cleanPath(_dir->absolutePath());
        QStringList dpDirFilter;
        dpDirFilter << "DP_Product";
        QStringList dpdirs = _dir->entryList(dpDirFilter);
        if ( !dpdirs.isEmpty() ) {
            _dir->cd(dpdirs.at(0));
            isFound = true;
            break;
        }
        _dir->cdUp();
        QString path2 = QDir::cleanPath(_dir->absolutePath());
        if ( path1 == path2 ) {
            // Since cdUP returned same path, assume path1==path2=="/" (root)
            break;
        }
    }

    if ( isFound == false ) {
        // No DP_Product dir found at or above data dir
        // Search for DP_ files at or above data dir
        delete _dir;
        _dir = new QDir(_dpDirName);
        while ( 1 ) {
            QString path1 = QDir::cleanPath(_dir->absolutePath());
            QStringList dpDirFilter;
            dpDirFilter << "DP_*";
            QStringList dps = _dir->entryList(dpDirFilter);
            if ( !dps.isEmpty() ) {
                isFound = true;
                break;
            }
            _dir->cdUp();
            QString path2 = QDir::cleanPath(_dir->absolutePath());
            if ( path1 == path2 ) {
                // Since cdUP returned same path, assume path1==path2=="/"(root)
                break;
            }
        }
    }

    if ( isFound == false ) {
        // No DP_* files or Data_Product dirs found
        // Try for parent SIM_ dir
        delete _dir;
        _dir = new QDir(_dpDirName);
        while ( 1 ) {
            QString path1 = QDir::cleanPath(_dir->absolutePath());
            QStringList simDirFilter;
            simDirFilter << "SIM_*";
            QStringList sims = _dir->entryList(simDirFilter);
            if ( !sims.isEmpty() ) {
                isFound = true;
                break;
            }
            _dir->cdUp();
            QString path2 = QDir::cleanPath(_dir->absolutePath());
            if ( path1 == path2 ) {
                // Since cdUP returned same path, assume path1==path2=="/"(root)
                break;
            }
        }
    }

    if ( isFound == false ) {
        // No DP_* files, Data_Product dirs or SIM_ dirs found
        // Use directory above as last ditch try
        delete _dir;
        _dir = new QDir(_dpDirName);
        _dir->cdUp();
    }

    _dpModel = new QFileSystemModel;
    _dpModelRootIdx = _dpModel->setRootPath(_dir->path());
    QStringList filters;
    //filters  << "DP_*" << "SET_*"; // _dpFilterModel does additional filtering
    _dpModel->setNameFilters(filters);
    _dpModel->setNameFilterDisables(false);
    _dpModel->setFilter(QDir::Dirs|QDir::Files|QDir::NoDotAndDotDot);

    // Create param filter for DP tree
    _dpFilterModel = new DPFilterProxyModel(_timeName,_runs,_sieModel);
    _dpFilterModel->setDynamicSortFilter(true);
    _dpFilterModel->setSourceModel(_dpModel);
    QRegExp dprx(QString(".*"));  // DP_ and SET_ are filtered by _dpModel
    _dpFilterModel->setFilterRegExp(dprx);
    _dpFilterModel->setFilterKeyColumn(0);
}

void DPTreeWidget::_createDP(const QString &dpfile)
{
    _createDPPages(dpfile);
    if ( _isShowTables ) {
        _createDPTables(dpfile);
    }
}

void DPTreeWidget::_searchBoxTextChanged(const QString &rx)
{
    _dpTreeView->expandAll();
    _dpFilterModel->setFilterRegExp(rx);
}

void DPTreeWidget::_dpTreeViewCurrentChanged(const QModelIndex &currIdx,
                                             const QModelIndex &prevIdx)
{
    Q_UNUSED(prevIdx);

    QModelIndex srcIdx = _dpFilterModel->mapToSource(currIdx);
    QString fp = _dpModel->filePath(srcIdx);
    if ( _isDP(fp) ) {
        bool isPage = false;
        bool isTable = false;
        foreach (QModelIndex pageIdx, _bookModel->pageIdxs() ) {
            // Search pages
            QString pageName = _bookModel->getDataString(pageIdx,
                                                         "PageName","Page");
            QString pageFileName = pageName.split(":").at(0);
            if ( pageFileName == fp ) {
                _bookSelectModel->setCurrentIndex(pageIdx,
                                                  QItemSelectionModel::Current);
                isPage = true;
                break;
            }
        }
        if ( !isPage ) {
            // Search tables
            foreach (QModelIndex tableIdx, _bookModel->tableIdxs() ) {
                // Search pages
                QString tableName = _bookModel->getDataString(tableIdx,
                                                           "TableName","Table");
                QString tableFileName = tableName.split(":").at(0);
                if ( tableFileName == fp ) {
                    _bookSelectModel->setCurrentIndex(tableIdx,
                                                  QItemSelectionModel::Current);
                    isTable = true;
                    break;
                }
            }
        }
        if ( !isPage && !isTable ) {
            int nPages0 = _bookModel->pageIdxs().size();
            int nTables0 = _bookModel->tableIdxs().size();
            _createDP(fp);
            int nPages1 = _bookModel->pageIdxs().size();
            int nTables1 = _bookModel->tableIdxs().size();
            if ( nPages0 < nPages1 ) {
                // A page was added, set tab to new page
                _bookSelectModel->setCurrentIndex(_bookModel->pageIdxs().last(),
                                                  QItemSelectionModel::Current);
            } else if ( nTables0 < nTables1 ) {
                // A table was added, set tab to new table
                _bookSelectModel->setCurrentIndex(
                                                 _bookModel->tableIdxs().last(),
                                                 QItemSelectionModel::Current);
            }
        }
    }
}

void DPTreeWidget::_loadDPFiles()
{
    foreach (QString dp, _dpFiles ) {
        _createDP(dp);
    }

    // Uncomment if one doesn't want to load DP with new data on sim reconnects
    //disconnect(_sieModel, SIGNAL(modelLoaded()), this, SLOT(_loadDPFiles()));
}

void DPTreeWidget::_setMsgLabel(const QString &msg)
{
    _msgLabel->setText(msg);
}

void DPTreeWidget::_tvModelRowAppended(const QModelIndex &parent,
                                       int start, int end)
{
    Q_UNUSED(parent);
    Q_UNUSED(end);
    QModelIndex idx = _tvModel->index(start,0);
    QVariant v = _tvModel->data(idx);
    QString msg = QString("Time = %1").arg(v.toDouble());
    _setMsgLabel(msg);

    _bookModel->appendDataToCurves(_tvCurveModels);
}

void DPTreeWidget::_tvModelAboutToBeReset()
{
    _bookModel->replaceCurveModelsWithCopies(_tvCurveModels);
    _tvCurveModels.clear();
}

//
// The DP tree that this creates must be coordinated with
//       * plotbookmodel
//       * plotbookview (rowInserted(), dataChanged() etc.
//       * vars widget page creator
//
void DPTreeWidget::_createDPPages(const QString& dpfile)
{
    QCursor currCursor = this->cursor();
    this->setCursor(QCursor(Qt::WaitCursor));

    DPProduct dp(dpfile);
    int rc = _runs->runPaths().size();
    if ( _tvModel && rc == 0 ) {
        // If there are no runs, count the tv model as one run
        rc = 1;
    }

    // Program
    DPProgram* dpprogram = dp.program();

    // Pages
    QModelIndex pagesIdx = _bookModel->getIndex(QModelIndex(), "Pages");
    QStandardItem *pagesItem = _bookModel->itemFromIndex(pagesIdx);

    // Page0 plots
    QModelIndex page0Idx = _bookModel->index(0,0,pagesIdx);
    QModelIndexList siblingPlotIdxs = _bookModel->plotIdxs(page0Idx);


    foreach (DPPage* page, dp.pages() ) {

        // Page
        QStandardItem *pageItem = _addChild(pagesItem,"Page");

        // PageName
        QString pageName = dpfile;
        pageName += QString(":dp.page.%0").arg(_idNum++);
        _addChild(pageItem, "PageName", pageName);

        _addChild(pageItem, "PageTitle", page->title());
        _addChild(pageItem, "PageStartTime", page->startTime());
        _addChild(pageItem, "PageStopTime", page->stopTime());
        QString bg = _bookModel->getDataString(QModelIndex(),"BackgroundColor");
        if ( !bg.isEmpty() ) {
            _addChild(pageItem, "PageBackgroundColor", bg);
        } else {
            _addChild(pageItem, "PageBackgroundColor", page->backgroundColor());
        }
        QString fg = _bookModel->getDataString(QModelIndex(),"ForegroundColor");
        if ( !fg.isEmpty() ) {
            _addChild(pageItem, "PageForegroundColor", fg);
        } else {
            _addChild(pageItem, "PageForegroundColor", page->foregroundColor());
        }

        // Plots
        QStandardItem *plotsItem = _addChild(pageItem, "Plots");

        foreach (DPPlot* plot, page->plots() ) {

            // Plot
            QStandardItem *plotItem = _addChild(plotsItem, "Plot");
            QModelIndex plotIdx = plotItem->index();

            // Some plot children
            _addChild(plotItem, "PlotName", _descrPlotTitle(plot));
            _addChild(plotItem, "PlotTitle",      plot->title());
            _addChild(plotItem, "PlotMathRect", QRectF());
            _addChild(plotItem, "PlotXScale", plot->plotXScale());
            _addChild(plotItem, "PlotYScale", plot->plotYScale());
            _addChild(plotItem, "PlotRatio", "");
            QModelIndex plotRatioIdx = _bookModel->getDataIndex(plotIdx,
                                                            "PlotRatio","Plot");
            _bookModel->setData(plotRatioIdx,plot->plotRatio());// why set 2x?
            _addChild(plotItem, "PlotXMinRange",  plot->xMinRange());
            _addChild(plotItem, "PlotXMaxRange",  plot->xMaxRange());
            _addChild(plotItem, "PlotYMinRange",  plot->yMinRange());
            _addChild(plotItem, "PlotYMaxRange",  plot->yMaxRange());
            if ( rc == 2 && plot->curves().size() == 1 ) {
                QString presentation = _bookModel->getDataString(QModelIndex(),
                                                               "Presentation");
                if ( !presentation.isEmpty() ) {
                    // Commandline argument -pres given
                    _addChild(plotItem, "PlotPresentation", presentation);
                } else {
                    if ( !plot->presentation().isEmpty() ) {
                        // Presentation specified in DP file
                        if ( plot->presentation() != "compare" &&
                             plot->presentation() != "error" &&
                             plot->presentation() != "error+compare" ) {
                            fprintf(stderr,
                                    "koviz [error]: bad presentation=%s "
                                    "in dpfile=%s \n",
                                    plot->presentation().toLatin1().constData(),
                                    dpfile.toLatin1().constData());
                            exit(-1);

                        }
                        _addChild(plotItem, "PlotPresentation",
                                  plot->presentation());
                    } else {
                        _addChild(plotItem, "PlotPresentation","compare");
                    }
                }
            } else {
                _addChild(plotItem, "PlotPresentation", "compare");
            }
            _addChild(plotItem, "PlotPointSize", 0.0);
            _addChild(plotItem, "PlotXAxisLabel", plot->xAxisLabel());
            _addChild(plotItem, "PlotYAxisLabel", plot->yAxisLabel());
            _addChild(plotItem, "PlotStartTime",  plot->startTime());
            _addChild(plotItem, "PlotStopTime",   plot->stopTime());
            _addChild(plotItem, "PlotGrid",       plot->grid());
            _addChild(plotItem, "PlotGridColor",       plot->gridColor());
            _addChild(plotItem, "PlotBackgroundColor", plot->backgroundColor());
            _addChild(plotItem, "PlotForegroundColor", plot->foregroundColor());
            _addChild(plotItem, "PlotFont",            plot->font());
            QVariantList listMajorXTics;
            foreach ( double tic, plot->majorXTics() ) {
                listMajorXTics << tic;
            }
            _addChild(plotItem, "PlotMajorXTics", listMajorXTics);
            QVariantList listMajorYTics;
            foreach ( double tic, plot->majorYTics() ) {
                listMajorYTics << tic;
            }
            _addChild(plotItem, "PlotMajorYTics", listMajorYTics);
            QVariantList listMinorXTics;
            foreach ( double tic, plot->minorXTics() ) {
                listMinorXTics << tic;
            }
            _addChild(plotItem, "PlotMinorXTics", listMinorXTics);
            QVariantList listMinorYTics;
            foreach ( double tic, plot->minorYTics() ) {
                listMinorYTics << tic;
            }
            _addChild(plotItem, "PlotMinorYTics", listMinorYTics);
            _addChild(plotItem, "PlotRect", plot->rect());

            // Curves
            QStandardItem *curvesItem = _addChild(plotItem,"Curves");
            QList<QColor> colors;
            int nCurves = plot->curves().size();
            colors = _bookModel->createCurveColors(rc*nCurves);

            QStringList styles = _bookModel->lineStyles();

            // Turn off model signals when adding children for speedup
            bool block = _bookModel->blockSignals(true);

            int i = 0;
            foreach (DPCurve* dpcurve, plot->curves() ) {

                // Setup progress bar dialog for time intensive loads
                QProgressDialog progress("Loading curves...", "Abort",
                                         0, rc, this);
                progress.setWindowModality(Qt::WindowModal);
                progress.setMinimumDuration(500);

                QElapsedTimer timer;
                timer.start();

                QString default_style = "plain";

                QString ux0;
                QString uy0;
                QString r0;
                for ( int r = 0; r < rc; ++r) {

                    QString color = colors.at(i++).name();

                    CurveModel* curveModel = _addCurve(curvesItem,dpcurve,
                                                       dpprogram,r,
                                                       color,default_style);

                    if ( r == 0 ) {
                        ux0 = curveModel->x()->unit();
                        uy0 = curveModel->y()->unit();
                        r0 = QFileInfo(curveModel->fileName()).
                                                             absoluteFilePath();
                    } else {
                        QString ux1 = curveModel->x()->unit();
                        QString uy1 = curveModel->y()->unit();
                        QString r1 = QFileInfo(curveModel->fileName()).
                                                             absoluteFilePath();
                        if ( !Unit::canConvert(ux0,ux1) ) {
                            fprintf(stderr,
                                 "koviz [error]: Unit mismatch for param=%s "
                                 "between the following paths:\n"
                                 "        %s {%s}\n"
                                 "        %s {%s}\n",
                                 curveModel->x()->name().toLatin1().constData(),
                                 r0.toLatin1().constData(),
                                 ux0.toLatin1().constData(),
                                 r1.toLatin1().constData(),
                                 ux1.toLatin1().constData());
                            exit(-1);
                        }
                        if ( !Unit::canConvert(uy0,uy1) ) {
                            fprintf(stderr,
                                 "koviz [error]: Unit mismatch for param=%s "
                                 "between the following paths:\n"
                                 "        %s {%s}\n"
                                 "        %s {%s}\n",
                                 curveModel->y()->name().toLatin1().constData(),
                                 r0.toLatin1().constData(),
                                 uy0.toLatin1().constData(),
                                 r1.toLatin1().constData(),
                                 uy1.toLatin1().constData());
                            exit(-1);
                        }
                    }

                    int secs = qRound(timer.nsecsElapsed()/1.0e9);
                    div_t d = div(secs,60);
                    QString msg = QString("Loaded %1 of %2 curves "
                                          "(%3 min %4 sec)")
                                       .arg(r+1).arg(rc).arg(d.quot).arg(d.rem);
                    progress.setLabelText(msg);
                    progress.setValue(r);
                    if (progress.wasCanceled()) {
                        break;
                    }
                }

                // Update progress dialog
                progress.setValue(rc);
            }

            // Turn signals back on before adding curveModel
            _bookModel->blockSignals(block);

            // Initialize plot math rect
            QModelIndex curvesIdx = curvesItem->index();
            QRectF bbox = _bookModel->calcCurvesBBox(curvesIdx);
            QModelIndex plotMathRectIdx = _bookModel->getDataIndex(plotIdx,
                                                                 "PlotMathRect",
                                                                 "Plot");
            if ( _bookModel->isXTime(plotIdx) ) {
                foreach ( QModelIndex siblingPlotIdx, siblingPlotIdxs ) {
                    bool isXTime = _bookModel->isXTime(siblingPlotIdx);
                    if ( isXTime ) {
                        QRectF sibPlotRect = _bookModel->getPlotMathRect(
                                                                siblingPlotIdx);
                        bbox.setLeft(sibPlotRect.left());
                        bbox.setRight(sibPlotRect.right());
                        break;
                    }
                }
            }
            if ( bbox.width() > 0.0 ) {
                _bookModel->setData(plotMathRectIdx,bbox);
            }

            // Reset monte carlo input view current idx to signal curr changed
            int currRunId = -1;
            if ( _monteInputsView ) {
                currRunId = _monteInputsView->currentRun();
            }
            if ( currRunId >= 0 ) {
                foreach (QModelIndex curveIdx,_bookModel->curveIdxs(curvesIdx)){
                    int curveRunId = _bookModel->getDataInt(curveIdx,
                                                          "CurveRunID","Curve");
                    if ( curveRunId == currRunId ) {
                        // Reset monte input view's current index which will set
                        // plot view's current index (by way of signal/slots)
                        QModelIndex currIdx = _monteInputsView->currentIndex();
                        _monteInputsView->setCurrentIndex(QModelIndex());
                        _monteInputsView->setCurrentIndex(currIdx);
                        break;
                    }
                }
            }
        }
    }

    this->setCursor(currCursor);
}

void DPTreeWidget::_createDPTables(const QString &dpfile)
{
    QCursor currCursor = this->cursor();
    this->setCursor(QCursor(Qt::WaitCursor));

    DPProduct dp(dpfile);
    int numRuns = _runs->runPaths().size();
    int tableNum = 0 ;

    // Tables
    QModelIndex tablesIdx = _bookModel->getIndex(QModelIndex(), "Tables");
    QStandardItem *tablesItem = _bookModel->itemFromIndex(tablesIdx);

    foreach (DPTable* table, dp.tables() ) {

        // Table
        QStandardItem *tableItem = _addChild(tablesItem,"Table");

        // TableName
        QString tableName = dpfile;
        tableName += QString(":dp.table.%0").arg(_idNum++);
        _addChild(tableItem, "TableName", tableName);

        _addChild(tableItem, "TableTitle", table->title());
        _addChild(tableItem, "TableTimeName", _timeName);
        _addChild(tableItem, "TableStartTime", table->startTime());
        _addChild(tableItem, "TableStopTime", table->stopTime());
        _addChild(tableItem, "TableDelimiter", table->delimiter());

        // Vars
        QStandardItem *varsItem = _addChild(tableItem, "TableVars");

        // If time is not first variable in table var list, set flag to add it
        bool isTime = false;
        if ( !table->vars().isEmpty() ) {
            if (table->vars().at(0)->name() == _timeName ) {
                isTime = true;
            }
        }

        for ( int i = 0 ; i < numRuns ; ++i ) {

            if ( !isTime ) {
                QStandardItem *varItem = _addChild(varsItem,"TableVar");

                // Children
                _addChild(varItem, "TableVarName",     _timeName);
                _addChild(varItem, "TableVarLabel",    _timeName);
                _addChild(varItem, "TableVarUnit",     "s");
                _addChild(varItem, "TableVarScale",    1.0);
                _addChild(varItem, "TableVarBias",     0.0);
                _addChild(varItem, "TableVarMinRange", -DBL_MAX);
                _addChild(varItem, "TableVarMaxRange", DBL_MAX);
                _addChild(varItem, "TableVarFormat",   "");

                // The actual data for the variable will be a trick curve model
                CurveModel* curveModel = _bookModel->createCurve(i, _timeName,
                                                           _timeName,_timeName);
                if ( !curveModel ) {
                    _err_stream << "koviz [error]: couldn't find parameter:\n\n"
                                << "        " << "("
                                << _timeName << " , "
                                << _timeName << " , "
                                << _timeName << ") \n";
                    throw std::runtime_error(
                                            _err_string.toLatin1().constData());
                }

                QVariant v=PtrToQVariant<CurveModel>::convert(curveModel);
                _addChild(varItem, "TableVarData",v);
            }

            foreach (DPVar* var, table->vars() ) {

                QStandardItem *varItem = _addChild(varsItem,"TableVar");

                // The actual data for the variable will be a trick curve model
                CurveModel* curveModel = _bookModel->createCurve(i, _timeName,
                                                      var->name(), var->name());
                if ( !curveModel ) {
                    _err_stream << "koviz [error]: couldn't find parameter:\n\n"
                                << "        " << "("
                                << _timeName << " , "
                                << var->name() << " , "
                                << var->name() << ") \n";
                    throw std::runtime_error(
                                            _err_string.toLatin1().constData());
                }

                // Children
                _addChild(varItem, "TableVarName",     var->name());
                _addChild(varItem, "TableVarLabel",    var->label());
                QString vUnit = var->unit();
                if ( !_unitOverrides.isEmpty() ) {
                    foreach ( QString overrideUnit, _unitOverrides ) {
                        Unit mUnit = Unit::map(curveModel->y()->unit(),
                                               overrideUnit);
                        if ( !mUnit.isEmpty() ) {
                            // No break if found, so last override in list used
                            vUnit = mUnit.name();
                        }
                    }
                }
                _addChild(varItem, "TableVarUnit",     vUnit);
                _addChild(varItem, "TableVarScale",    var->scaleFactor());
                _addChild(varItem, "TableVarBias",     var->bias());
                _addChild(varItem, "TableVarMinRange", var->minRange());
                _addChild(varItem, "TableVarMaxRange", var->maxRange());
                _addChild(varItem, "TableVarFormat",   var->format());
                _addChild(varItem, "TableVarRunID",    i);


                QVariant v=PtrToQVariant<CurveModel>::convert(curveModel);
                _addChild(varItem, "TableVarData",v);
            }
        }

        tableNum++;
    }

    this->setCursor(currCursor);
}

QStandardItem *DPTreeWidget::_addChild(QStandardItem *parentItem,
                             const QString &childTitle,
                             const QVariant& childValue)
{
    return(_bookModel->addChild(parentItem,childTitle,childValue));
}

CurveModel* DPTreeWidget::_addCurve(QStandardItem *curvesItem,
                             DPCurve *dpcurve,
                             DPProgram *dpprogram,
                             int runId, const QString& defaultColor,
                             const QString& defaultLineStyle)
{
    // Curve
    QStandardItem *curveItem = _addChild(curvesItem,"Curve");

    CurveModel* curveModel = 0 ;

    // Get x&y params that match this run
    DPVar* x = 0;
    DPVar* y = 0;
    QString xName;
    QString xUnit;
    QString yName;
    if ( dpcurve->xyPairs().isEmpty() ) {
        // Find out what x&y to use for curve
        x = dpcurve->x();
        y = dpcurve->y();
        xName = x->name();
        xUnit = x->unit();
        yName = y->name();

        if ( xName.isEmpty() ) {
            xName = _timeName;
        }

        if ( _sieModel && _tvModel ) {
            // Search TV
            QString sieXName = xName;
            QString sieYName = yName;
            if ( xName == "sys.exec.out.time" ) {
                sieXName = "time";
            }
            if ( yName == "sys.exec.out.time" ) {
                sieYName = "time";
            }
            if ( _sieModel->isParamExists(sieXName) &&
                 _sieModel->isParamExists(sieYName) ) {

                QString xUnit = _sieModel->paramUnit(sieXName);
                QString yUnit = _sieModel->paramUnit(sieYName);

                _tvModel->addParam(sieXName, xUnit);
                _tvModel->addParam(sieYName, yUnit);

                int tcol = _tvModel->paramColumn("time");
                int xcol = _tvModel->paramColumn(sieXName);
                int ycol = _tvModel->paramColumn(sieYName);
                curveModel = new CurveModel(_tvModel,tcol,xcol,ycol);
                _tvCurveModels.append(curveModel);
            }
        }

        if ( !curveModel ) {
            curveModel = _bookModel->createCurve(runId,
                                                 _timeName, xName, yName);
        }

        if ( !curveModel ) {
            // Search external programs

            QString outputCurveName;
            foreach ( Parameter outputParam, dpprogram->outputParams() ) {
                if ( outputParam.name() == yName ) {
                    outputCurveName = yName;
                    break;
                }
            }

            if ( !outputCurveName.isEmpty() ) {

                QList<CurveModel*> inputCurves;
                foreach ( Parameter inputParam, dpprogram->inputParams() ) {
                    CurveModel* inputCurve = _bookModel->createCurve(runId,
                                                             _timeName,
                                                             _timeName,
                                                             inputParam.name());
                    if ( inputCurve ) {
                        inputCurves << inputCurve;
                    } else {
                        fprintf(stderr, "koviz [error]: DP Program input "
                                "variable not found:\n"
                                "    DPProgramInputName=%s\n",
                                inputParam.name().toLatin1().constData());
                        exit(-1);
                    }
                }

                QStringList timeNames;
                timeNames << _timeName;
                ProgramModel* programModel = new ProgramModel(inputCurves,
                                                       dpprogram->inputParams(),
                                                       dpprogram->outputs(),
                                                       timeNames,
                                                       dpprogram->fileName());
                _programModels << programModel;
                int ycol = programModel->paramColumn(outputCurveName);
                curveModel = new CurveModel(programModel,0,0,ycol);
            }
        }

        if ( !curveModel ) {

            _err_stream << "koviz [error]: could not find parameter: \n\n"
                        << "        " << "("
                        << _timeName << " , "
                        << xName << " , "
                        << yName << ") ";
            if ( runId < _runs->runPaths().size() ) {
                _err_stream << "\n\nin RUN:\n\n "
                            << "         "
                            << _runs->runPaths().at(runId) ;
            } else if ( runId >= _runs->runPaths().size()
                        && _sieModel && _tvModel ) {
                _err_stream << "\n\nin the Trick SIE database.\n";
            }
            fprintf(stderr, "%s\n",
                           _err_string.toLatin1().constData());
            exit(-1);
        }
    } else {

        // Search through xypairs
        QStringList txyParams;  // in case error, use this in message
        foreach ( DPXYPair* xyPair, dpcurve->xyPairs() ) {
            xName = xyPair->x()->name();
            xUnit = xyPair->x()->unit();
            yName = xyPair->y()->name();
            if ( !xyPair->y()->timeName().isEmpty() &&
                  xyPair->y()->timeName() != _timeName ) {
                fprintf(stderr,"koviz [todo]: DPTreeWidget::_addCurve() "
                               "xyPair->y()->timeName() != _timeName \n");
            }
            if ( !xyPair->x()->timeName().isEmpty() &&
                  xyPair->x()->timeName() != _timeName ) {
                fprintf(stderr,"koviz [todo]: DPTreeWidget::_addCurve() "
                               "xyPair->x()->timeName() != _timeName \n");
            }
            if ( xName.isEmpty() ) {
                xName = _timeName;
                xUnit = "--";
            }
            txyParams << "(" + _timeName  + " , " + xName + " , " + yName + ")";
            curveModel = _bookModel->createCurve(runId,_timeName,xName,yName);
            if ( curveModel ) {
                x = xyPair->x();
                y = xyPair->y();
                break;
            }
        }

        if ( !curveModel ) {
            QString runPath = _runs->runPaths().at(runId);
            _err_stream << "koviz [error]: could not find matching xypair "
                           "parameter in RUN:\n\n"
                        << "        " << runPath << "\n\n"
                           "Tried the following :\n\n";
            foreach ( QString txy, txyParams ) {
                _err_stream << "        " << txy << "\n";
            }
            _err_stream << "\n        Try using the -map option.\n";
            throw std::runtime_error(_err_string.toLatin1().constData());
        }
    }

    // Curve children
    _addChild(curveItem, "CurveTimeName", _timeName);
    _addChild(curveItem, "CurveXName", xName);

    if ( !_unitOverrides.isEmpty() ) {
        foreach ( QString overrideUnit, _unitOverrides ) {
            Unit mUnit = Unit::map(curveModel->x()->unit(),
                                   overrideUnit);
            if ( !mUnit.isEmpty() ) {
                // No break if found, so last override in list used
                xUnit = mUnit.name();
            }
        }
    }
    _addChild(curveItem, "CurveXUnit", xUnit);

    _addChild(curveItem, "CurveYName", y->name());

    QString yUnit = y->unit();
    if ( !_unitOverrides.isEmpty() ) {
        foreach ( QString overrideUnit, _unitOverrides ) {
            Unit mUnit = Unit::map(curveModel->y()->unit(),
                                   overrideUnit);
            if ( !mUnit.isEmpty() ) {
                // No break if found, so last override in list used
                yUnit = mUnit.name();
            }
        }
    }
    _addChild(curveItem, "CurveYUnit", yUnit);
    _addChild(curveItem, "CurveRunID", runId);
    _addChild(curveItem, "CurveRunPath", _runs->runPaths().at(runId));
    _addChild(curveItem, "CurveXMinRange", x->minRange());
    _addChild(curveItem, "CurveXMaxRange", x->maxRange());
    _addChild(curveItem, "CurveXScale",
                          x->scaleFactor()*curveModel->x()->scale());
    QHash<QString,QVariant> shifts = _bookModel->getDataHash(QModelIndex(),
                                                             "RunToShiftHash");
    QFileInfo fi(curveModel->runPath());
    QString curveRun = fi.absoluteFilePath();
    if ( shifts.contains(curveRun) ) {
        double shiftVal = shifts.value(curveRun).toDouble();
        _addChild(curveItem, "CurveXBias", shiftVal);
    } else {
        _addChild(curveItem, "CurveXBias", x->bias() + curveModel->x()->bias());
    }
    _addChild(curveItem, "CurveYMinRange",   y->minRange());
    _addChild(curveItem, "CurveYMaxRange",   y->maxRange());
    _addChild(curveItem, "CurveYScale",
                         y->scaleFactor()*curveModel->y()->scale());
    _addChild(curveItem, "CurveYBias", y->bias() + curveModel->y()->bias());
    _addChild(curveItem, "CurveSymbolSize",  y->symbolSize());

    int row = curveItem->row();

    bool isGroups = false;
    QStringList groups;
    QModelIndex groupsIdx = _bookModel->getIndex(QModelIndex(),"Groups","");
    int nGroups = _bookModel->rowCount(groupsIdx);
    for ( int i = 0; i < nGroups; ++i ) {
        QModelIndex groupIdx = _bookModel->index(i,1,groupsIdx);
        QString group = _bookModel->data(groupIdx).toString();
        groups << group;
        if ( !group.isEmpty() ) {
            isGroups = true;
        }
    }

    // Symbol Style
    QString symbolStyle = y->symbolStyle(); // DP symbol style
    QModelIndex ssIdx = _bookModel->getIndex(QModelIndex(),
                                             "Symbolstyles","");
    if ( row < _bookModel->rowCount(ssIdx) ) {
        QModelIndex symbolStyleIdx = _bookModel->index(row,1,ssIdx);
        QString ss = _bookModel->data(symbolStyleIdx).toString();
        if ( !ss.isEmpty() ) {
            QString group;
            if ( row < groups.size() ) {
                group = groups.at(row);
            }
            if ( group.isEmpty() ) {
                // Use symbolstyle from cmdline, otherwise see group logic below
                symbolStyle = ss;
            }
        }
    }
    if ( symbolStyle.isEmpty() ) {
        symbolStyle = "none";
    }

    // Symbol End
    QString symbolEnd = y->symbolEnd();
    QModelIndex seIdx = _bookModel->getIndex(QModelIndex(),
                                             "Symbolends","");
    if ( row < _bookModel->rowCount(seIdx) ) {
        QModelIndex symbolEndIdx = _bookModel->index(row,1,seIdx);
        QString se = _bookModel->data(symbolEndIdx).toString();
        if ( !se.isEmpty() ) {
            QString group;
            if ( row < groups.size() ) {
                group = groups.at(row);
            }
            if ( group.isEmpty() ) {
                symbolEnd = se;
            }
        }
    }
    if ( symbolEnd.isEmpty() ) {
        symbolEnd = "none";
    }

    // Linestyle
    QString lineStyle = y->lineStyle() ; // DP linestyle
    QModelIndex lsIdx = _bookModel->getIndex(QModelIndex(),"Linestyles","");
    if ( row < _bookModel->rowCount(lsIdx) ) {
        // Possible commandline linestyle override
        QModelIndex lineStyleIdx = _bookModel->index(row,1,lsIdx);
        QString ls = _bookModel->data(lineStyleIdx).toString();
        if ( !ls.isEmpty() ) {
            QString group;
            if ( row < groups.size() ) {
                group = groups.at(row);
            }
            if ( group.isEmpty() ) {
                // E.g. if -l4 dog -ls4 thick_line -g4 99,
                //      then override handled in the group logic below
                lineStyle = ls;
            }
        }
    }
    if ( lineStyle.isEmpty() ) {
        lineStyle = defaultLineStyle;
    }

    // Label
    QString yLabel = y->label();
    QModelIndex llIdx = _bookModel->getIndex(QModelIndex(),"LegendLabels","");
    if ( row < _bookModel->rowCount(llIdx) ) {
        QString llTag = QString("Label%1").arg(row+1);
        QString ll = _bookModel->getDataString(llIdx,llTag,"LegendLabels");
        if ( !ll.isEmpty() ) {
            QString group;
            if ( row < groups.size() ) {
                group = groups.at(row);
            }
            if ( group.isEmpty() ) {
                // Use commandline label
                yLabel = ll;
            }
        }
    }

    // Color
    QString color = y->lineColor() ;
    QModelIndex lcsIdx = _bookModel->getIndex(QModelIndex(),"LegendColors","");
    if ( row < _bookModel->rowCount(lcsIdx) ) {
        // Possible commandline color override
        QModelIndex legendColorIdx = _bookModel->index(row,1,lcsIdx);
        QString legendColor = _bookModel->data(legendColorIdx).toString();
        if ( !legendColor.isEmpty() ) {
            QString group;
            if ( row < groups.size() ) {
                group = groups.at(row);
            }
            if ( group.isEmpty() ) {
                color = legendColor;
            }
        }
    }
    if ( color.isEmpty()) {
        color = defaultColor;
    }

    // Handle groups (-g1, -g2... -g7 options)
    if ( isGroups ) {
        int i = 0;
        foreach ( QString group, groups ) {
            if ( !group.isEmpty() ) {
                if ( _bookModel->isMatch(curveRun,group) ) {
                    // Color
                    QModelIndex idx = _bookModel->index(i,1,lcsIdx);
                    QString cc = _bookModel->data(idx).toString();
                    if ( !cc.isEmpty() ) {
                        color = cc ;
                    }

                    // Label
                    QString llTag = QString("Label%1").arg(i+1);
                    QString ll = _bookModel->getDataString(llIdx,llTag,
                                                           "LegendLabels");
                    if ( !ll.isEmpty() ) {
                        yLabel = ll ;
                    } else {
                        yLabel = group;
                    }

                    // Linestyle
                    idx = _bookModel->index(i,1,lsIdx);
                    QString ls = _bookModel->data(idx).toString();
                    if ( !ls.isEmpty() ) {
                        lineStyle = ls;
                    }

                    // Symbolstyle
                    idx = _bookModel->index(i,1,ssIdx);
                    QString ss = _bookModel->data(idx).toString();
                    if ( !ss.isEmpty() ) {
                        symbolStyle = ss;
                    }

                    // Symbol End
                    idx = _bookModel->index(i,1,seIdx);
                    QString se = _bookModel->data(idx).toString();
                    if ( !se.isEmpty() ) {
                        symbolEnd = se;
                    }

                    // Match found and handled
                    break;
                }
            }
            ++i;
        }
    }

    _addChild(curveItem, "CurveYLabel", yLabel);
    _addChild(curveItem, "CurveColor", color);
    _addChild(curveItem, "CurveLineStyle",  lineStyle);
    _addChild(curveItem, "CurveSymbolStyle", symbolStyle);
    _addChild(curveItem, "CurveSymbolEnd", symbolEnd);

    // Finally, add actual curve model data with signals turned on
    QVariant v = PtrToQVariant<CurveModel>::convert(curveModel);
    _addChild(curveItem, "CurveData", v);

    return curveModel;
}

bool DPTreeWidget::_isDP(const QString &fp)
{
    bool ret = false ;
    QFileInfo fi(fp);
    if ( (fi.baseName().left(3) == "DP_" && fi.suffix() == "xml" ) ) {
        ret = true;
    } else if ( fi.baseName().left(3) == "DP_" &&
                fi.suffix().isEmpty() && fi.isFile()) {
        ret = true;
    }
    return ret;
}

QString DPTreeWidget::_descrPlotTitle(DPPlot *plot)
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

void DPTreeWidget::clearSelection()
{
    _dpTreeView->selectionModel()->clear();
}
