#include "dptreewidget.h"

#ifdef __linux
#include "timeit_linux.h"
#endif

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
                           QStandardItemModel *dpVarsModel,
                           MonteModel* monteModel,
                           PlotBookModel *bookModel,
                           QItemSelectionModel *bookSelectModel,
                           MonteInputsView *monteInputsView,
                           QWidget *parent) :
    QWidget(parent),
    _idNum(0),
    _timeName(timeName),
    _dpDirName(dpDirName),
    _dpFiles(),
    _dpVarsModel(dpVarsModel),
    _monteModel(monteModel),
    _bookModel(bookModel),
    _bookSelectModel(bookSelectModel),
    _monteInputsView(monteInputsView),
    _gridLayout(0),
    _searchBox(0)
{
    _setupModel();

    _gridLayout = new QGridLayout(parent);

    _searchBox = new QLineEdit(parent);
    connect(_searchBox,SIGNAL(textChanged(QString)),
            this,SLOT(_searchBoxTextChanged(QString)));
    _gridLayout->addWidget(_searchBox,0,0);

    _dpTreeView = new DPTreeView(parent);
    _dpTreeView->setModel(_dpFilterModel);
    QModelIndex proxyRootIdx = _dpFilterModel->mapFromSource(_dpModelRootIdx);
    _dpTreeView->setRootIndex(proxyRootIdx);
    _dpTreeView->setFocusPolicy(Qt::ClickFocus);
    _gridLayout->addWidget(_dpTreeView,1,0);
    connect(_dpTreeView->selectionModel(),
            SIGNAL(currentChanged(QModelIndex,QModelIndex)),
            this, SLOT(_dpTreeViewCurrentChanged(QModelIndex,QModelIndex)));

    for ( int col = 1; col < _dpModel->columnCount(); ++col) {
        _dpTreeView->hideColumn(col);
    }
    _dpTreeView->expandAll();
    _dpTreeView->resizeColumnToContents(0);

    foreach (QString dp, dpFiles ) {
        _createDP(dp);
    }
}

DPTreeWidget::~DPTreeWidget()
{
    if ( _dpModel ) {
        delete _dpModel;
        _dpModel = 0;
    }

    if ( _dir ) {
        delete _dir;
        _dir = 0;
    }

    if ( _dpFilterModel ) {
        delete _dpFilterModel;
    }
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
                // Since cdUP returned same path, assume path1==path2=="/" (root)
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
                // Since cdUP returned same path, assume path1==path2=="/" (root)
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

    // The dp filter models takes a list of params that are common between runs.
    // Only DP_files which have params which are in all runs will show in tree
    QStringList dpParams;
    dpParams << _timeName; // always common,but may not be in dpVarsModel,so add
    for (int i = 0; i < _dpVarsModel->rowCount(); ++i ) {
        QModelIndex idx = _dpVarsModel->index(i,0);
        QString param = _dpVarsModel->data(idx).toString();
        dpParams.append(param);
    }
    _dpFilterModel = new DPFilterProxyModel(dpParams);

    _dpFilterModel->setDynamicSortFilter(true);
    _dpFilterModel->setSourceModel(_dpModel);
    QRegExp dprx(QString(".*"));  // DP_ and SET_ are filtered by _dpModel
    _dpFilterModel->setFilterRegExp(dprx);
    _dpFilterModel->setFilterKeyColumn(0);
}

void DPTreeWidget::_createDP(const QString &dpfile)
{
    _createDPPages(dpfile);
    _createDPTables(dpfile);
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
        bool isCreated = false;
        foreach (QModelIndex pageIdx, _bookModel->pageIdxs() ) {
            QString pageName = _bookModel->getDataString(pageIdx,
                                                         "PageName","Page");
            QString pageFileName = pageName.split(":").at(0);
            if ( pageFileName == fp ) {
                _bookSelectModel->setCurrentIndex(pageIdx,
                                                  QItemSelectionModel::Current);
                isCreated = true;
                break;
            }
        }
        if ( !isCreated ) {
            _createDP(fp);
            _bookSelectModel->setCurrentIndex(_bookModel->pageIdxs().last(),
                                              QItemSelectionModel::Current);
        }
    }
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
    int rc = _monteModel->rowCount();

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
        _addChild(pageItem, "PageBackgroundColor", page->backgroundColor());
        _addChild(pageItem, "PageForegroundColor", page->foregroundColor());

        // Plots
        QStandardItem *plotsItem = _addChild(pageItem, "Plots");

        foreach (DPPlot* plot, page->plots() ) {

            // Plot
            QStandardItem *plotItem = _addChild(plotsItem, "Plot");

            // Some plot children
            _addChild(plotItem, "PlotName", _descrPlotTitle(plot));
            _addChild(plotItem, "PlotTitle",      plot->title());
            _addChild(plotItem, "PlotMathRect", QRectF());
            _addChild(plotItem, "PlotXMinRange",  plot->xMinRange());
            _addChild(plotItem, "PlotXMaxRange",  plot->xMaxRange());
            _addChild(plotItem, "PlotYMinRange",  plot->yMinRange());
            _addChild(plotItem, "PlotYMaxRange",  plot->yMaxRange());
            if ( rc == 2 && plot->curves().size() == 1 ) {
                QString presentation = _bookModel->getDataString(QModelIndex(),
                                                               "Presentation");
                _addChild(plotItem, "PlotPresentation", presentation);
            } else {
                _addChild(plotItem, "PlotPresentation", "compare");
            }
            _addChild(plotItem, "PlotPointSize", 0.0);
            _addChild(plotItem, "PlotXAxisLabel", plot->xAxisLabel());
            _addChild(plotItem, "PlotYAxisLabel", plot->yAxisLabel());
            _addChild(plotItem, "PlotStartTime",  plot->startTime());
            _addChild(plotItem, "PlotStopTime",   plot->stopTime());
            _addChild(plotItem, "PlotGridOnOff",  plot->grid());
            _addChild(plotItem, "PlotGridColor",       plot->gridColor());
            _addChild(plotItem, "PlotBackgroundColor", plot->backgroundColor());
            _addChild(plotItem, "PlotForegroundColor", plot->foregroundColor());
            _addChild(plotItem, "PlotFont",            plot->font());

            // Curves
            QStandardItem *curvesItem = _addChild(plotItem,"Curves");
            int nColors = plot->curves().size()*rc;
            QList<QColor> colors = _bookModel->createCurveColors(nColors);

            QHash<int,QString> run2color;
            for ( int r = 0; r < rc; ++r ) {
                QModelIndex runIdx = _monteInputsView->model()->index(r,0);
                int runId = _monteInputsView->model()->data(runIdx).toInt();
                run2color.insert(runId, colors.at(r).name());
            }

            // Turn off model signals when adding children for speedup
            bool block = _bookModel->blockSignals(true);

            int colorId = 0;
            foreach (DPCurve* dpcurve, plot->curves() ) {

                // Setup progress bar dialog for time intensive loads
                QProgressDialog progress("Loading curves...", "Abort", 0, rc, this);
                progress.setWindowModality(Qt::WindowModal);
                progress.setMinimumDuration(500);

#ifdef __linux
                TimeItLinux timer;
                timer.start();
#endif
                for ( int r = 0; r < rc; ++r) {
                    QString color;
                    if ( plot->curves().size() == 1 ) {
                        color = run2color.value(r);
                    } else {
                        color = colors.at(colorId++).name();
                    }
                    _addCurve(curvesItem, dpcurve, _monteModel, r, color);
#ifdef __linux
                    int secs = qRound(timer.stop()/1000000.0);
                    div_t d = div(secs,60);
                    QString msg = QString("Loaded %1 of %2 curves "
                                          "(%3 min %4 sec)")
                                       .arg(r+1).arg(rc).arg(d.quot).arg(d.rem);
                    progress.setLabelText(msg);
#endif
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
            QModelIndex plotIdx = plotItem->index();
            QModelIndex plotMathRectIdx = _bookModel->getDataIndex(plotIdx,
                                                                 "PlotMathRect",
                                                                 "Plot");
            foreach ( QModelIndex siblingPlotIdx, siblingPlotIdxs ) {
                bool isXTime = _bookModel->isXTime(siblingPlotIdx);
                if ( isXTime ) {
                    QRectF sibPlotRect = _bookModel->getPlotMathRect(siblingPlotIdx);
                    bbox.setLeft(sibPlotRect.left());
                    bbox.setRight(sibPlotRect.right());
                    break;
                }
            }
            _bookModel->setData(plotMathRectIdx,bbox);

            // Reset monte carlo input view current idx to signal current changed
            int currRunId = -1;
            if ( _monteInputsView ) {
                currRunId = _monteInputsView->currentRun();
            }
            if ( currRunId >= 0 ) {
                QModelIndex curveIdx = _bookModel->index(currRunId,0,curvesIdx);
                int curveRunId = _bookModel->getDataInt(curveIdx,
                                                        "CurveRunID","Curve");
                if ( curveRunId == currRunId ) {
                    // Reset monte input view's current index which will set
                    // plot view's curr idx (by way of signal/slot connections)
                    QModelIndex currIdx = _monteInputsView->currentIndex();
                    _monteInputsView->setCurrentIndex(QModelIndex());
                    _monteInputsView->setCurrentIndex(currIdx);
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
    int numRuns = _monteModel->rowCount();
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
                TrickCurveModel* curveModel = _monteModel->curve(i, _timeName,
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

                QVariant v=PtrToQVariant<TrickCurveModel>::convert(curveModel);
                _addChild(varItem, "TableVarData",v);
            }

            foreach (DPVar* var, table->vars() ) {

                QStandardItem *varItem = _addChild(varsItem,"TableVar");

                // Children
                _addChild(varItem, "TableVarName",     var->name());
                _addChild(varItem, "TableVarLabel",    var->label());
                _addChild(varItem, "TableVarUnit",     var->unit());
                _addChild(varItem, "TableVarScale",    var->scaleFactor());
                _addChild(varItem, "TableVarBias",     var->bias());
                _addChild(varItem, "TableVarMinRange", var->minRange());
                _addChild(varItem, "TableVarMaxRange", var->maxRange());
                _addChild(varItem, "TableVarFormat",   var->format());

                // The actual data for the variable will be a trick curve model
                TrickCurveModel* curveModel = _monteModel->curve(i, _timeName,
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

                QVariant v=PtrToQVariant<TrickCurveModel>::convert(curveModel);
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

void DPTreeWidget::_addCurve(QStandardItem *curvesItem,
                             DPCurve *dpcurve, MonteModel *monteModel,
                             int runId, const QString& defaultColor)
{
    // Curve
    QStandardItem *curveItem = _addChild(curvesItem,"Curve");

    TrickCurveModel* curveModel = 0 ;

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
            xUnit = "s";
        }

        curveModel = monteModel->curve(runId, _timeName, xName, yName);
        if ( !curveModel ) {
            QString runDir = monteModel->
                                headerData(runId,Qt::Vertical).toString();
            _err_stream << "koviz [error]: could not find parameter: \n\n"
                        << "        " << "("
                        << _timeName << " , "
                        << xName << " , "
                        << yName << ") "
                        << "\n\nin RUN:\n\n "
                        << "         "
                        << runDir ;
            throw std::runtime_error(_err_string.toLatin1().constData());
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
            curveModel = _monteModel->curve(runId, _timeName, xName, yName);
            if ( curveModel ) {
                x = xyPair->x();
                y = xyPair->y();
                break;
            }
        }

        if ( !curveModel ) {
            QString runDir = monteModel->
                                headerData(runId,Qt::Vertical).toString();
            _err_stream << "koviz [error]: could not find matching xypair "
                           "parameter in RUN:\n\n"
                        << "        " << runDir << "\n\n"
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
    _addChild(curveItem, "CurveTimeUnit", dpcurve->t()->unit());
    _addChild(curveItem, "CurveXName", xName);
    _addChild(curveItem, "CurveXUnit", xUnit);
    _addChild(curveItem, "CurveYName", y->name());
    _addChild(curveItem, "CurveYUnit", y->unit());
    QString runDirName = QFileInfo(curveModel->trkFile()).dir().dirName();
    bool ok;
    int curveRunId = runDirName.mid(4).toInt(&ok);
    if ( ok ) {
        _addChild(curveItem, "CurveRunID", curveRunId);
    } else {
        _addChild(curveItem, "CurveRunID", runId);
    }
    _addChild(curveItem, "CurveXMinRange", x->minRange());
    _addChild(curveItem, "CurveXMaxRange", x->maxRange());
    _addChild(curveItem, "CurveXScale", x->scaleFactor());

    QHash<QString,QVariant> shifts = _bookModel->getDataHash(QModelIndex(),
                                                             "RunToShiftHash");
    QString curveRunDir = QFileInfo(curveModel->trkFile()).absolutePath();
    if ( shifts.contains(curveRunDir) ) {
        double shiftVal = shifts.value(curveRunDir).toDouble();
        _addChild(curveItem, "CurveXBias", shiftVal);
    } else {
        _addChild(curveItem, "CurveXBias", x->bias());
    }
    _addChild(curveItem, "CurveYMinRange",   y->minRange());
    _addChild(curveItem, "CurveYMaxRange",   y->maxRange());
    _addChild(curveItem, "CurveYScale",      y->scaleFactor());
    _addChild(curveItem, "CurveYBias",       y->bias());
    _addChild(curveItem, "CurveSymbolStyle", y->symbolStyle());
    _addChild(curveItem, "CurveSymbolSize",  y->symbolSize());
    _addChild(curveItem, "CurveLineStyle",   y->lineStyle());
    _addChild(curveItem, "CurveYLabel",      y->label());
    QString color = y->lineColor() ;
    if ( color.isEmpty() ) {
        color = defaultColor;
    }
    _addChild(curveItem, "CurveColor", color);

    // Finally, add actual curve model data with signals turned on
    QVariant v = PtrToQVariant<TrickCurveModel>::convert(curveModel);
    _addChild(curveItem, "CurveData", v);
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
