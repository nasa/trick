#include "dptreewidget.h"

#include "libsnapdata/timeit_linux.h"

QString DPTreeWidget::_err_string;
QTextStream DPTreeWidget::_err_stream(&DPTreeWidget::_err_string);

DPTreeWidget::DPTreeWidget(const QString& timeName,
                           const QString &dpDirName,
                           const QStringList &dpFiles,
                           QStandardItemModel *dpVarsModel,
                           MonteModel* monteModel,
                           PlotBookModel *plotModel,
                           QItemSelectionModel *plotSelectModel,
                           QWidget *parent) :
    QWidget(parent),
    _timeName(timeName),
    _dpDirName(dpDirName),
    _dpFiles(),
    _dpVarsModel(dpVarsModel),
    _monteModel(monteModel),
    _plotModel(plotModel),
    _plotSelectModel(plotSelectModel),
    _gridLayout(0),
    _searchBox(0)
{
    _setupModel();

    _gridLayout = new QGridLayout(parent);

    _searchBox = new QLineEdit(parent);
    connect(_searchBox,SIGNAL(textChanged(QString)),
            this,SLOT(_searchBoxTextChanged(QString)));
    _gridLayout->addWidget(_searchBox,0,0);

    _dpTreeView = new QTreeView(parent);
    _dpTreeView->setModel(_dpFilterModel);
    QModelIndex proxyRootIdx = _dpFilterModel->mapFromSource(_dpModelRootIdx);
    _dpTreeView->setRootIndex(proxyRootIdx);
    _dpTreeView->setFocusPolicy(Qt::ClickFocus);
    _gridLayout->addWidget(_dpTreeView,1,0);
    connect(_dpTreeView,SIGNAL(clicked(QModelIndex)),
            this, SLOT(_dpTreeViewClicked(QModelIndex)));
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
    delete _dpModel;
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
    dpParams << "sys.exec.out.time";  // this is always common, but may not be
                                      // in dpVarsModel - so add it
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
    // Disable tables since Tim needs DP pages now
    // and the tables are still awful slow
    //_createDPTables(dpfile);
}

void DPTreeWidget::_searchBoxTextChanged(const QString &rx)
{
    _dpTreeView->expandAll();
    _dpFilterModel->setFilterRegExp(rx);
}

void DPTreeWidget::_dpTreeViewClicked(const QModelIndex &idx)
{
    Q_UNUSED(idx);

    QModelIndexList idxs =  _dpTreeView->selectionModel()->selectedRows();
    foreach ( QModelIndex idx, idxs ) {
        QModelIndex srcIdx = _dpFilterModel->mapToSource(idx);
        QString fp = _dpModel->filePath(srcIdx);
        if ( _isDP(fp) ) {
            bool isCreated = false;
            foreach (QModelIndex pageIdx, _plotModel->pageIdxs() ) {
                QModelIndex pageNameIdx = _plotModel->getIndex(pageIdx,
                                                               "PageName",
                                                               "Page");
                QModelIndex pageNameDataIdx = pageNameIdx.sibling(
                                                    pageNameIdx.row(),1);
                QString pageName = _plotModel->data(pageNameDataIdx).toString();
                if ( pageName == fp ) {
                    _plotSelectModel->setCurrentIndex(pageIdx,
                                                  QItemSelectionModel::Current);
                    isCreated = true;
                    break;
                }
            }
            if ( !isCreated ) {
                _createDP(fp);
            }
        }
    }
}

void DPTreeWidget::_dpTreeViewCurrentChanged(const QModelIndex &currIdx,
                                             const QModelIndex &prevIdx)
{
    Q_UNUSED(prevIdx);
    _dpTreeViewClicked(currIdx);
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
    int numRuns = _monteModel->rowCount();
    int pageNum = 0 ;

    // Pages
    QModelIndex pagesIdx = _plotModel->getIndex(QModelIndex(), "Pages");

    QStandardItem *pagesItem = _plotModel->itemFromIndex(pagesIdx);

//#define TIME_ME
#ifdef TIME_ME
                TimeItLinux timer;
                 timer.start();
#endif
    foreach (DPPage* page, dp.pages() ) {

        // Page
        QStandardItem *pageItem = _addChild(pagesItem,"Page");
        QModelIndex pageIdx = _plotModel->indexFromItem(pageItem);
        _plotSelectModel->setCurrentIndex(pageIdx,QItemSelectionModel::Current);

        // PageName
        QString pageName = dpfile;
        if ( pageNum > 0 ) {
            pageName += QString("_%0").arg(pageNum);
        }
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
            _addChild(plotItem, "PlotPointSize", 0.0);
            _addChild(plotItem, "PlotXAxisLabel", plot->xAxisLabel());
            _addChild(plotItem, "PlotYAxisLabel", plot->yAxisLabel());
            _addChild(plotItem, "PlotXMinRange",  plot->xMinRange());
            _addChild(plotItem, "PlotXMaxRange",  plot->xMaxRange());
            _addChild(plotItem, "PlotYMinRange",  plot->yMinRange());
            _addChild(plotItem, "PlotYMaxRange",  plot->yMaxRange());
            _addChild(plotItem, "PlotStartTime",  plot->startTime());
            _addChild(plotItem, "PlotStopTime",   plot->stopTime());
            _addChild(plotItem, "PlotGridOnOff",  plot->grid());
            _addChild(plotItem, "PlotGridColor",       plot->gridColor());
            _addChild(plotItem, "PlotBackgroundColor", plot->backgroundColor());
            _addChild(plotItem, "PlotForegroundColor", plot->foregroundColor());
            _addChild(plotItem, "PlotFont",            plot->font());

            // Curves
            QStandardItem *curvesItem = _addChild(plotItem,"Curves");
            QList<QColor> colors = _plotModel->createCurveColors(numRuns);

            int curveId = -1;
            foreach (DPCurve* dpcurve, plot->curves() ) {
                ++curveId;
                for ( int runId = 0; runId < numRuns; ++runId) {
                    _addCurve(curvesItem, dpcurve, _monteModel,
                              runId, curveId, colors);
                }
            }
        }
        pageNum++;
    }
#ifdef TIME_ME
    timer.snap("loadtime(dptreewidget)=");
#endif

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
    QModelIndex tablesIdx = _plotModel->getIndex(QModelIndex(), "Tables");
    QStandardItem *tablesItem = _plotModel->itemFromIndex(tablesIdx);

    foreach (DPTable* table, dp.tables() ) {

        // Table
        QStandardItem *tableItem = _addChild(tablesItem,"Table");

        // TableName
        QString tableName = dpfile;
        if ( tableNum > 0 ) {
            tableName += QString("_%0").arg(tableNum);
        }
        _addChild(tableItem, "TableName", tableName);

        _addChild(tableItem, "TableTitle", table->title());
        _addChild(tableItem, "TableStartTime", table->startTime());
        _addChild(tableItem, "TableStopTime", table->stopTime());
        _addChild(tableItem, "TableDelimiter", table->delimiter());

        // This is used to initially allocate the number of table columns.
        // It doesn't have to match the actual count...
        // It speeds up the table creation
        _addChild(tableItem, "TableVarCount", numRuns*table->vars().count());

        // Vars
        QStandardItem *varsItem = _addChild(tableItem, "TableVars");

        for ( int i = 0 ; i < numRuns ; ++i ) {

            // Make the table vars curves so handled the same way datawise
            foreach (DPVar* var, table->vars() ) {

                // Curve (not to be confused with a plot curve)
                // TODO: need to change this, but plotmodel->data() uses it
                QStandardItem *curveItem = _addChild(varsItem, "Curve");

                // Get time name for this variable
                QString tName = "sys.exec.out.time";
                if ( !_timeName.isEmpty() ) {
                    tName = _timeName;
                }
                if ( !var->timeName().isEmpty() && _timeName.isEmpty() ) {
                    tName = var->timeName();
                }

                // Children
                _addChild(curveItem, "CurveTime",        tName);
                _addChild(curveItem, "CurveXName",       var->name());
                _addChild(curveItem, "CurveXUnit",       var->unit());
                _addChild(curveItem, "CurveYName",       var->name());
                _addChild(curveItem, "CurveYUnit",       var->unit());
                _addChild(curveItem, "TableVarScale",    var->scaleFactor());
                _addChild(curveItem, "TableVarBias",     var->bias());
                _addChild(curveItem, "TableVarMinRange", var->minRange());
                _addChild(curveItem, "TableVarMaxRange", var->maxRange());
                _addChild(curveItem, "CurveRunID",       i);
                _addChild(curveItem, "TableCurveData");
                _addChild(curveItem, "TableVarFormat",     var->format());
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
    return(_plotModel->addChild(parentItem,childTitle,childValue));
}

void DPTreeWidget::_addCurve(QStandardItem *curvesItem,
                             DPCurve *dpcurve, MonteModel *monteModel,
                             int runId, int curveId,const QList<QColor>& colors)
{
    // Begin blocking signals to speed up curve insertion
    bool block = _plotModel->blockSignals(true);

    // Curve
    QStandardItem *curveItem = _addChild(curvesItem,"Curve");

    // Curve Name
    QString curveName = QString("Curve_%0_%1").arg(curveId).arg(runId);
    _addChild(curveItem, "CurveName", curveName);

    TrickCurveModel* curveModel = 0 ;

    // Get x&y params that match this run
    DPVar* x = 0;
    DPVar* y = 0;
    QString tName = "sys.exec.out.time";  // can be reset in this if block
    if ( !_timeName.isEmpty() ) {
        tName = _timeName;
    }
    QString xName;
    QString xUnit;
    QString yName;
    if ( dpcurve->xyPairs().isEmpty() ) {
        // Find out what x&y to use for curve
        // It can be in xypairs
        x = dpcurve->x();
        y = dpcurve->y();
        xName = x->name();
        xUnit = x->unit();
        yName = y->name();
        if ( !y->timeName().isEmpty() && _timeName.isEmpty() ) {
            tName = y->timeName();  // note that y supercedes x
        } else if ( !x->timeName().isEmpty() && _timeName.isEmpty() ) {
            tName = x->timeName();
        }
        if ( xName.isEmpty() ) {
            xName = tName;
            xUnit = dpcurve->t()->unit();
        }
        curveModel = monteModel->curve(runId, tName, xName, yName);
        if ( !curveModel ) {
            QString runDir = monteModel->
                                headerData(runId,Qt::Vertical).toString();
            _err_stream << "snap [error]: could not find parameter: \n\n"
                        << "        " << "("
                        << tName << " , "
                        << xName << " , "
                        << yName << ") "
                        << "\n\nin RUN:\n\n "
                        << "         "
                        << runDir ;
            throw std::runtime_error(_err_string.toAscii().constData());
        }
    } else {

        // Search through xypairs
        QStringList txyParams;  // in case error, use this in message
        foreach ( DPXYPair* xyPair, dpcurve->xyPairs() ) {
            xName = xyPair->x()->name();
            xUnit = xyPair->x()->unit();
            yName = xyPair->y()->name();
            if ( !xyPair->y()->timeName().isEmpty() && _timeName.isEmpty() ) {
                tName = xyPair->y()->timeName();
            } else if ( !xyPair->x()->timeName().isEmpty()
                        && _timeName.isEmpty() ) {
                tName = xyPair->x()->timeName();
            }
            if ( xName.isEmpty() ) {
                xName = tName;
                xUnit = "--";
            }
            txyParams << "(" + tName  + " , " + xName + " , " + yName + ")";
            curveModel = _monteModel->curve(runId, tName, xName, yName);
            if ( curveModel ) {
                x = xyPair->x();
                y = xyPair->y();
                break;
            } else {
                // Reset tName back to default
                tName = "sys.exec.out.time";
            }
        }

        if ( !curveModel ) {
            QString runDir = monteModel->
                                headerData(runId,Qt::Vertical).toString();
            _err_stream << "snap [error]: could not find matching xypair "
                           "parameter in RUN:\n\n"
                        << "        " << runDir << "\n\n"
                           "Tried the following :\n\n";
            foreach ( QString txy, txyParams ) {
                _err_stream << "        " << txy << "\n";
            }
            throw std::runtime_error(_err_string.toAscii().constData());
        }
    }

    // Curve children
    _addChild(curveItem, "CurveTime", tName);
    _addChild(curveItem, "CurveTimeUnit", dpcurve->t()->unit());
    _addChild(curveItem, "CurveXName", xName);
    _addChild(curveItem, "CurveXUnit", xUnit);
    _addChild(curveItem, "CurveYName", y->name());
    _addChild(curveItem, "CurveYUnit", y->unit());
    _addChild(curveItem, "CurveRunID", runId);
    _addChild(curveItem, "CurveXScale",      x->scaleFactor());
    _addChild(curveItem, "CurveXBias",       x->bias());
    _addChild(curveItem, "CurveYScale",      y->scaleFactor());
    _addChild(curveItem, "CurveYBias",       y->bias());
    _addChild(curveItem, "CurveSymbolStyle", y->symbolStyle());
    _addChild(curveItem, "CurveSymbolSize",  y->symbolSize());
    _addChild(curveItem, "CurveLineStyle",   y->lineStyle());
    _addChild(curveItem, "CurveYLabel",      y->label());
    QString color = y->lineColor() ;
    if ( color.isEmpty() || colors.size() >= 2 ) {
        color = colors.at(runId).name();
    }
    _addChild(curveItem, "CurveColor", color);

    // End blocking signals to speed up curve insertion
    block = _plotModel->blockSignals(block);

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
