#include "trickview.h"

#ifdef __linux
#include "timeit_linux.h"
#endif

TrickView::TrickView(SieListModel* sieModel,
                     TVModel* tvModel,
                     PlotBookModel* bookModel,
                     QItemSelectionModel*  bookSelectModel,
                     QWidget* parent) :
    QWidget(parent),
    _sieModel(sieModel),
    _tvModel(tvModel),
    _bookModel(bookModel),
    _bookSelectModel(bookSelectModel)
{
    // Search box
    _gridLayout = new QGridLayout(parent);
    _waitLabel = new QLabel(parent);
    _searchBox = new QLineEdit(parent);
    //connect(_searchBox,SIGNAL(textChanged(QString)),
    //        this,SLOT(_tvSearchBoxTextChanged(QString)));
    connect(_searchBox,SIGNAL(returnPressed()),
            this,SLOT(_tvSearchBoxReturnPressed()));
    _gridLayout->addWidget(_waitLabel,0,0);
    _gridLayout->addWidget(_searchBox,1,0);

    // SIE model
    connect(_sieModel, SIGNAL(sendMessage(QString)),
            this, SLOT(_setMessageLabel(QString)));

    // Vars list view
    _listView = new QListView(parent);
    _listView->setDragDropMode(QAbstractItemView::DragOnly);
    _listView->setDragEnabled(false);
    _listView->setModel(_sieModel);
    _listView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    _listView->setFocusPolicy(Qt::ClickFocus);
    _varsSelectModel = new QItemSelectionModel(_sieModel);
    _listView->setSelectionModel(_varsSelectModel);
    _listView->setSelectionMode(QAbstractItemView::SingleSelection);
    connect(_varsSelectModel,
         SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
         this,
         SLOT(_tvSelectionChanged(QItemSelection,QItemSelection)));
    connect(_sieModel, SIGNAL(modelLoaded()),
            _listView, SLOT(reset()));
    _gridLayout->addWidget(_listView,2,0);

    // TV Model
    connect(_tvModel, SIGNAL(rowsInserted(QModelIndex,int,int)),
            this,SLOT(_tvModelRowAppended(QModelIndex,int,int)));
    connect(_tvModel, SIGNAL(sendMessage(QString)),
            this, SLOT(_setMessageLabel(QString)));

    // Book Model
    connect(_bookModel,
            SIGNAL(rowsAboutToBeRemoved(QModelIndex,int,int)),
            this,
            SLOT(_bookModelRowsAboutToBeRemoved(QModelIndex,int,int)));
}

TrickView::~TrickView()
{
}

void TrickView::setDragEnabled(bool isEnabled)
{
    _varsSelectModel->clear();
    _listView->setDragEnabled(isEnabled);
}

void TrickView::slotDropEvent(QDropEvent *event, const QModelIndex &idx)
{
    QString tag = _bookModel->data(idx.sibling(idx.row(),0)).toString();
    QString mimeType("application/x-qabstractitemmodeldatalist");
    if ( event->mimeData()->hasFormat(mimeType) ) {
        QByteArray bytes = event->mimeData()->data(mimeType);
        QDataStream stream(&bytes,QIODevice::ReadOnly);
        int row,col;
        QMap<int, QVariant> valueMap;
        stream >> row >> col >> valueMap;
        if ( !valueMap.isEmpty() ) {
            QString dropString = valueMap.value(0).toString();
            if ( tag == "PlotXAxisLabel" ) {
                Qt::KeyboardModifiers kmods = event->keyboardModifiers();
                bool isCtl = (kmods & Qt::ControlModifier);
                bool isAlt = (kmods & Qt::AltModifier);
                QString unit = _sieModel->paramUnit(dropString);
                QStringList params = _sieModel->expandParam(dropString);
                if ( !isCtl && !isAlt && params.size() > 0 ) {
                    _changeXOnPlot(params.at(0),unit,idx);      // X
                } else if ( isCtl && params.size() == 2  ) {
                    _changeXOnPlot(params.at(1),unit,idx);     // Y
                } else if ( params.size() == 3 ) {
                    if ( isCtl ) {
                        _changeXOnPlot(params.at(1),unit,idx); // Y
                    } else if ( isAlt ) {
                        _changeXOnPlot(params.at(2),unit,idx); // Z
                    }
                } else {
                    fprintf(stderr, "koviz TODO: handle x drop when sie "
                            "param is dimensioned greater than 3.\n");
                }
            } else if ( ! idx.isValid() ) {
                _addParamToBook(dropString);
            }
        }
    }
}

void TrickView::_changeXOnPlot(const QString &xName, const QString &xUnit,
                               const QModelIndex &xAxisLabelIdx)
{
    QModelIndex plotIdx = xAxisLabelIdx.parent();
    _bookModel->setData(xAxisLabelIdx,xName);
    QModelIndex curvesIdx = _bookModel->getIndex(plotIdx, "Curves","Plot");
    QModelIndexList curveIdxs = _bookModel->getIndexList(curvesIdx,
                                                         "Curve","Curves");
    bool isError = false;
    bool block = _bookModel->blockSignals(true);
    foreach ( QModelIndex curveIdx, curveIdxs ) {
        QModelIndex xNameIdx = _bookModel->getDataIndex(curveIdx,
                                                        "CurveXName","Curve");
        QModelIndex xUnitIdx = _bookModel->getDataIndex(curveIdx,
                                                        "CurveXUnit","Curve");
        QModelIndex curveDataIdx = _bookModel->getDataIndex(curveIdx,
                                                           "CurveData","Curve");
        QString tName = _bookModel->getDataString(curveIdx,
                                                  "CurveTimeName", "Curve");
        QString yName = _bookModel->getDataString(curveIdx,
                                                  "CurveYName", "Curve");

        int tcol = _tvModel->paramColumn(tName);
        int xcol = _tvModel->paramColumn(xName);
        if ( xcol < 0 ) {
            _tvModel->addParam(xName, xUnit);
            xcol = _tvModel->paramColumn(xName);
        }
        int ycol = _tvModel->paramColumn(yName);

        if ( tcol < 0 || ycol < 0 ) {
            QMessageBox msgBox;
            QString msg = QString("Sorry!  Unable to change x because "
                                  "time, x and y not all in the same "
                                  "model e.g. x is from recorded data "
                                  "and y from the TV");
            msgBox.setText(msg);
            msgBox.exec();
            isError = true;
            break;
        }

        CurveModel* curveModel = new CurveModel(_tvModel,tcol,xcol,ycol);

        _bookModel->setData(xNameIdx,xName);
        _bookModel->setData(xUnitIdx,xUnit);

        QVariant v = PtrToQVariant<CurveModel>::convert(curveModel);
        _bookModel->setData(curveDataIdx,v);
    }
    _bookModel->blockSignals(block);

    if ( !isError ) {
        // Reset plot math rect
        QRectF bbox = _bookModel->calcCurvesBBox(curvesIdx);
        plotIdx = curvesIdx.parent();
        QModelIndex pageIdx = plotIdx.parent().parent();
        QModelIndex plotMathRectIdx = _bookModel->getDataIndex(plotIdx,
                                                               "PlotMathRect",
                                                               "Plot");
        QModelIndexList siblingPlotIdxs = _bookModel->plotIdxs(pageIdx);
        foreach ( QModelIndex siblingPlotIdx, siblingPlotIdxs ) {
            bool isXTime = _bookModel->isXTime(siblingPlotIdx);
            if ( isXTime ) {
                QRectF sibPlotRect= _bookModel->getPlotMathRect(siblingPlotIdx);
                if ( sibPlotRect.width() > 0 ) {
                    bbox.setLeft(sibPlotRect.left());
                    bbox.setRight(sibPlotRect.right());
                }
                break;
            }
        }
        _bookModel->setData(plotMathRectIdx,bbox);
    }
}

void TrickView::_tvSelectionChanged(
                                const QItemSelection &currVarSelection,
                                const QItemSelection &prevVarSelection)
{
    Q_UNUSED(prevVarSelection); // TODO: handle deselection (prevSelection)

    if ( _listView->dragEnabled() ) {
        return;
    }

    QModelIndexList idxs = currVarSelection.indexes();
    if ( idxs.size() == 1 ) {
        QModelIndex idx = idxs.at(0);
        QString param = _sieModel->data(idx).toString();
        int nParamsAdded = _addParamToBook(param);
        if ( nParamsAdded == 0 ) {
            QMessageBox msgBox;
            QString msg = QString("Unable to plot parameter=\"%1\" "
                                  "because it is not in Trick "
                                  "managed memory.").arg(param);
            msgBox.setText(msg);
            msgBox.exec();
        }
    }
}

void TrickView::_tvSearchBoxTextChanged(const QString &rx)
{
    //_varsFilterModel->setFilterRegExp(rx);
}

void TrickView::_setMessageLabel(const QString &msg)
{
    _waitLabel->setText(msg);
}

void TrickView::_tvModelRowAppended(const QModelIndex &parent,int start,int end)
{
    Q_UNUSED(parent);
    Q_UNUSED(start);
    Q_UNUSED(end);
    //QModelIndex idx = _tvModel->index(start,0);
    //QVariant v = _tvModel->data(idx);
    //QString msg = QString("Time = %1").arg(v.toDouble());
    //_setMessageLabel(msg);

    foreach (QModelIndex pageIdx, _bookModel->pageIdxs()) {
        foreach (QModelIndex plotIdx, _bookModel->plotIdxs(pageIdx)) {
            QModelIndex curvesIdx = _bookModel->getIndex(plotIdx,
                                                         "Curves","Plot");
            foreach (QModelIndex curveIdx, _bookModel->curveIdxs(curvesIdx)) {
                QModelIndex dataIdx = _bookModel->getDataIndex(curveIdx,
                                                           "CurveData","Curve");
                CurveModel* curveModel = _bookModel->getCurveModel(curveIdx);
                QVariant v = PtrToQVariant<CurveModel>::convert(curveModel);
                _bookModel->setData(dataIdx,v,PlotBookModel::AppendData);
            }
            QRectF bbox = _bookModel->calcCurvesBBox(curvesIdx);
            _bookModel->setPlotMathRect(bbox,plotIdx);
        }
    }
}

void TrickView::_bookModelRowsAboutToBeRemoved(const QModelIndex &parent,
                                               int first, int last)
{
    _varsSelectModel->clear();
}

void TrickView::_tvSearchBoxReturnPressed()
{
    QString rx = _searchBox->text();
    int nMatches = _sieModel->setRegexp(rx);
    QString msg;
    if ( nMatches == 0 ) {
        msg = QString("No matches");
    } else if ( nMatches == 1 ) {
        msg = QString("1 match!!!");
    } else {
        msg = QString("%1 matches!").arg(nMatches);
    }
    _setMessageLabel(msg);
}

// Returns the number of params added after expanding the param
// For example, if param is ball.state.out.position
// 2 is returned since the following params would be added:
//     ball.state.out.position[0]
//     ball.state.out.position[1]
int TrickView::_addParamToBook(const QString &param)
{
    Qt::KeyboardModifiers kmods = QApplication::keyboardModifiers();
    bool isSft = (kmods & Qt::ShiftModifier);
    bool isAlt = (kmods & Qt::AltModifier);
    bool isCtl = (kmods & Qt::ControlModifier);

    QModelIndex currIdx = _bookSelectModel->currentIndex();
    QModelIndex pageIdx;
    QModelIndex plotIdx;
    if ( !isSft && _bookModel->data(currIdx).toString() == "Plot" ) {
        plotIdx = currIdx;
        pageIdx = _bookModel->getIndex(plotIdx,"Page");
    }
    int i = 0;
    QString unit = _sieModel->paramUnit(param);
    foreach ( QString p, _sieModel->expandParam(param) ) {
        if ( !plotIdx.isValid() ) {
            pageIdx = _createPage();
            plotIdx = _addPlotToPage(pageIdx,p,unit);
        } else {
            if ( isAlt && !isCtl ) {
                _addCurveToPlot(plotIdx,p,unit);
            } else if ( !isAlt && isCtl ) {
                plotIdx = _addPlotToPage(pageIdx,p,unit);
            } else if ( isAlt && isCtl ) {
                if ( i == 0 ) {
                    plotIdx = _addPlotToPage(pageIdx,p,unit);
                } else {
                    _addCurveToPlot(plotIdx,p,unit);
                }
            } else {
                pageIdx = _createPage();
                plotIdx = _addPlotToPage(pageIdx,p,unit);
            }
        }
        ++i;
        _bookSelectModel->setCurrentIndex(plotIdx,
                                          QItemSelectionModel::Current);
    }

    return i;
}

QModelIndex TrickView::_createPage()
{
    QStandardItem* pageItem = _bookModel->createPageItem();
    QModelIndex pageIdx = _bookModel->indexFromItem(pageItem);
    return pageIdx;
}

QModelIndex TrickView::_addPlotToPage(const QModelIndex &pageIdx,
                                      const QString &yName,const QString &yUnit)
{
    QModelIndex plotsIdx = _bookModel->getIndex(pageIdx, "Plots", "Page");
    QStandardItem* plotsItem = _bookModel->itemFromIndex(plotsIdx);
    QStandardItem* plotItem = _bookModel->addChild(plotsItem, "Plot");

    int plotId = plotItem->row();
    QString plotName = QString("tv.%0").arg(plotId);

    _bookModel->addChild(plotItem, "PlotName", plotName);
    _bookModel->addChild(plotItem, "PlotTitle", "");
    _bookModel->addChild(plotItem, "PlotMathRect", QRectF());
    _bookModel->addChild(plotItem, "PlotStartTime", -DBL_MAX);
    _bookModel->addChild(plotItem, "PlotStopTime",   DBL_MAX);
    _bookModel->addChild(plotItem, "PlotGrid", true);
    _bookModel->addChild(plotItem, "PlotRatio", "");
    _bookModel->addChild(plotItem, "PlotXScale", "linear");
    _bookModel->addChild(plotItem, "PlotYScale", "linear");
    _bookModel->addChild(plotItem, "PlotXMinRange", -DBL_MAX);
    _bookModel->addChild(plotItem, "PlotXMaxRange",  DBL_MAX);
    _bookModel->addChild(plotItem, "PlotYMinRange", -DBL_MAX);
    _bookModel->addChild(plotItem, "PlotYMaxRange",  DBL_MAX);
    _bookModel->addChild(plotItem, "PlotBackgroundColor", "#FFFFFF");
    _bookModel->addChild(plotItem, "PlotForegroundColor", "#000000");
    _bookModel->addChild(plotItem, "PlotPresentation", "compare");
    _bookModel->addChild(plotItem, "PlotXAxisLabel", "Time");
    _bookModel->addChild(plotItem, "PlotYAxisLabel", yName);
    _bookModel->addChild(plotItem, "PlotRect", QRect(0,0,0,0));

    QModelIndex plotIdx = _bookModel->indexFromItem(plotItem);
    QModelIndex curveIdx = _addCurveToPlot(plotIdx,yName,yUnit);
    Q_UNUSED(curveIdx);

    return plotIdx;
}

QModelIndex TrickView::_addCurveToPlot(const QModelIndex &plotIdx,
                                       const QString &yName,
                                       const QString &yUnit)
{
    QStandardItem* curvesItem;
    if ( _bookModel->isChildIndex(plotIdx, "Plot", "Curves") ) {
        QModelIndex curvesIdx = _bookModel->getIndex(plotIdx,"Curves","Plot");
        curvesItem = _bookModel->itemFromIndex(curvesIdx);
    }  else {
        QStandardItem* plotItem = _bookModel->itemFromIndex(plotIdx);
        curvesItem = _bookModel->addChild(plotItem,"Curves");
    }

    QStandardItem *curveItem = _bookModel->addChild(curvesItem,"Curve");

    _tvModel->addParam(yName, yUnit);

    int tcol = _tvModel->paramColumn("time");
    int xcol = tcol;
    int ycol = _tvModel->paramColumn(yName);
    CurveModel* curveModel = new CurveModel(_tvModel,tcol,xcol,ycol);

    QModelIndex curvesIdx = _bookModel->indexFromItem(curvesItem);
    int nCurves = _bookModel->rowCount(curvesIdx);
    QList<QColor> colors = _bookModel->createCurveColors(nCurves);
    QString color = colors.last().name();

    bool block = _bookModel->blockSignals(true);

    _bookModel->addChild(curveItem, "CurveRunID", -1);
    _bookModel->addChild(curveItem, "CurveTimeName", curveModel->t()->name());
    _bookModel->addChild(curveItem, "CurveTimeUnit", curveModel->t()->unit());
    _bookModel->addChild(curveItem, "CurveXName", curveModel->t()->name());
    _bookModel->addChild(curveItem, "CurveXUnit", curveModel->t()->unit());
    _bookModel->addChild(curveItem, "CurveXScale", 1.0);
    _bookModel->addChild(curveItem, "CurveXBias", 0.0);
    _bookModel->addChild(curveItem, "CurveYName", yName);
    _bookModel->addChild(curveItem, "CurveYUnit", curveModel->y()->unit());
    _bookModel->addChild(curveItem, "CurveYScale", 1.0);
    _bookModel->addChild(curveItem, "CurveYBias", 0.0);
    _bookModel->addChild(curveItem, "CurveXMinRange", -DBL_MAX);
    _bookModel->addChild(curveItem, "CurveXMaxRange",  DBL_MAX);
    _bookModel->addChild(curveItem, "CurveYMinRange", -DBL_MAX);
    _bookModel->addChild(curveItem, "CurveYMaxRange",  DBL_MAX);
    _bookModel->addChild(curveItem, "CurveSymbolSize", "");
    _bookModel->addChild(curveItem, "CurveYLabel", yName);
    _bookModel->addChild(curveItem, "CurveColor", color);
    _bookModel->addChild(curveItem, "CurveLineStyle","Plain");
    _bookModel->addChild(curveItem, "CurveSymbolStyle", "none");

    QVariant v = PtrToQVariant<CurveModel>::convert(curveModel);
    _bookModel->addChild(curveItem, "CurveData", v);

    _bookModel->blockSignals(block);

    QModelIndex curveIdx = _bookModel->indexFromItem(curveItem);
    return curveIdx;
}
