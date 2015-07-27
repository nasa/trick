#include "dptreewidget.h"

DPTreeWidget::DPTreeWidget(const QString &dpDirName,
                           const QStringList &dpFiles,
                           QStandardItemModel *dpVarsModel,
                           MonteModel* monteModel,
                           PlotBookModel *plotModel,
                           QItemSelectionModel *plotSelectModel,
                           QWidget *parent) :
    QWidget(parent),
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
        _createDPPages(dp);
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

    // Change directory from RUN/MONTE dir to DP_Product dir
    _dir->cdUp();
    QStringList dpDirFilter;
    dpDirFilter << "DP_Product";
    QStringList dpdirs = _dir->entryList(dpDirFilter);
    if ( !dpdirs.isEmpty() ) {
        _dir->cd(dpdirs.at(0));
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
                QString pageName = _plotModel->data(pageNameIdx).toString();
                if ( pageName == fp ) {
                    _plotSelectModel->setCurrentIndex(pageIdx,
                                                  QItemSelectionModel::Select);
                    isCreated = true;
                    break;
                }
            }
            if ( !isCreated ) {
                _createDPPages(fp);
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

    foreach (DPPage* page, dp.pages() ) {

        // Page
        QStandardItem *pageItem = _addChild(pagesItem,"Page");

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

            int curveId = -1;
            foreach (DPCurve* dpcurve, plot->curves() ) {
                ++curveId;
                for ( int runId = 0; runId < numRuns; ++runId) {

                    // Curve
                    QStandardItem *curveItem = _addChild(curvesItem,"Curve");

                    // Curve children
                    QString curveName = QString("Curve_%0_%1")
                                        .arg(curveId).arg(runId);
                    _addChild(curveItem, "CurveName", curveName);
                    _addChild(curveItem, "CurveTime", dpcurve->t()->name());
                    _addChild(curveItem, "CurveTimeUnit", dpcurve->t()->unit());
                    _addChild(curveItem, "CurveXName", dpcurve->x()->name());
                    _addChild(curveItem, "CurveXUnit", dpcurve->x()->unit());
                    _addChild(curveItem, "CurveYName", dpcurve->y()->name());
                    _addChild(curveItem, "CurveYUnit", dpcurve->y()->unit());
                    _addChild(curveItem, "CurveRunID", runId);
                    _addChild(curveItem, "CurveData","");
                    _addChild(curveItem, "CurveXScale", dpcurve->x()->scaleFactor());
                    _addChild(curveItem, "CurveXBias", dpcurve->x()->bias());
                    _addChild(curveItem, "CurveYScale", dpcurve->y()->scaleFactor());
                    _addChild(curveItem, "CurveYBias", dpcurve->y()->bias());
                    _addChild(curveItem, "CurveColor", dpcurve->lineColor());
                    _addChild(curveItem, "CurveSymbolStyle", dpcurve->y()->symbolStyle());
                    _addChild(curveItem, "CurveSymbolSize", dpcurve->y()->symbolSize());
                    _addChild(curveItem, "CurveLineStyle", dpcurve->y()->lineStyle());
                    _addChild(curveItem, "CurveYLabel", dpcurve->y()->label());
                }
            }
        }
        pageNum++;
    }

    this->setCursor(currCursor);
}

QStandardItem *DPTreeWidget::_addChild(QStandardItem *parentItem,
                             const QString &childTitle,
                             const QVariant& childValue)
{
    return(_plotModel->addChild(parentItem,childTitle,childValue));
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
