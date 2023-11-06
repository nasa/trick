#include "trickview.h"

#ifdef __linux
#include "timeit_linux.h"
#endif

TrickView::TrickView(const QString &trickhost, int trickport,
                     PlotBookModel *bookModel,
                     QItemSelectionModel*  bookSelectModel,
                     QWidget *parent) :
    QWidget(parent),
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

    // Setup models
    _sieListModel = new SieListModel();
    _sieListModel->setParams(&_params);

    // Vars list view
    _listView = new QListView(parent);
    _listView->setDragDropMode(QAbstractItemView::DragOnly);
    _listView->setDragEnabled(false);
    _listView->setModel(_sieListModel);
    _listView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    _listView->setFocusPolicy(Qt::ClickFocus);
    _varsSelectModel = new QItemSelectionModel(_sieListModel);
    _listView->setSelectionModel(_varsSelectModel);
    _listView->setSelectionMode(QAbstractItemView::SingleSelection);
    connect(_varsSelectModel,
         SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
         this,
         SLOT(_tvSelectionChanged(QItemSelection,QItemSelection)));
    _gridLayout->addWidget(_listView,2,0);

    QString host("localhost");
    if ( !trickhost.isEmpty() ) {
        host = trickhost;
    }
    QFuture<void> future = QtConcurrent::run(this,&TrickView::_createTVModel,
                                             host,trickport);
    Q_UNUSED(future);

    _tvModel = new TVModel(host,trickport);
    connect(_tvModel, SIGNAL(rowsInserted(QModelIndex,int,int)),
            this,SLOT(_tvModelRowAppended(QModelIndex,int,int)));

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
                QString unit = _paramUnit(dropString);
                QStringList params = _expandParam(dropString);
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

        CurveModel* curveModel = new CurveModel(_tvModel,tcol,xcol,ycol);

        _bookModel->setData(xNameIdx,xName);
        _bookModel->setData(xUnitIdx,xUnit);

        QVariant v = PtrToQVariant<CurveModel>::convert(curveModel);
        _bookModel->setData(curveDataIdx,v);
    }
    _bookModel->blockSignals(block);

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
            QRectF sibPlotRect = _bookModel->getPlotMathRect(siblingPlotIdx);
            if ( sibPlotRect.width() > 0 ) {
                bbox.setLeft(sibPlotRect.left());
                bbox.setRight(sibPlotRect.right());
            }
            break;
        }
    }
    _bookModel->setData(plotMathRectIdx,bbox);
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
        QString param = _sieListModel->data(idx).toString();
        _addParamToBook(param);
    }
}

QString TrickView::_paramUnit(const QString &param)
{
    QString unit;

    QStringList paramList = param.split('.');
    QDomElement rootElement = _sieDoc.documentElement();

    QString elType;
    for (int i = 0; i < paramList.size(); ++i ) {
        if ( i == 0 ) {
            QDomNodeList tlos = rootElement.elementsByTagName(
                                                            "top_level_object");
            QString simObject = paramList.at(i);
            for (int j = 0; j < tlos.size(); ++j) {
                QDomElement tlo = tlos.at(j).toElement();
                if ( tlo.attribute("name") == simObject ) {
                    elType = tlo.attribute("type");
                    break;
                }
            }
        } else {
            if ( _name2element.contains(elType) ) {
                QDomElement el = _name2element.value(elType);
                QDomNodeList members = el.elementsByTagName("member");
                for (int j = 0; j < members.size(); ++j) {
                    QDomElement member = members.at(j).toElement();
                    QString memberName = member.attribute("name");
                    if ( memberName == paramList.at(i) ) {
                        elType = member.attribute("type");
                        unit = member.attribute("units");
                        break;
                    }
                }
            } else {
                fprintf(stderr, "koviz [bad scoobs]: tv bad type=%s\n",
                        elType.toLatin1().constData());
                exit(-1);
            }
        }
    }

    return unit;
}

bool TrickView::_isParamDynAlloced(const QString &param)
{
    bool isDynAlloced = false;

    QStringList paramList = param.split('.');
    QDomElement rootElement = _sieDoc.documentElement();

    QString elType;
    for (int i = 0; i < paramList.size(); ++i ) {
        if ( i == 0 ) {
            QDomNodeList tlos = rootElement.elementsByTagName(
                                                            "top_level_object");
            QString simObject = paramList.at(i);
            for (int j = 0; j < tlos.size(); ++j) {
                QDomElement tlo = tlos.at(j).toElement();
                if ( tlo.attribute("name") == simObject ) {
                    elType = tlo.attribute("type");
                    break;
                }
            }
        } else {
            if ( _name2element.contains(elType) ) {
                QDomElement el = _name2element.value(elType);
                QDomNodeList members = el.elementsByTagName("member");
                for (int j = 0; j < members.size(); ++j) {
                    QDomElement member = members.at(j).toElement();
                    QString memberName = member.attribute("name");
                    if ( memberName == paramList.at(i) ) {
                        elType = member.attribute("type");
                        QDomNodeList dims = member.elementsByTagName(
                                                                   "dimension");
                        for (int k = 0; k < dims.size(); ++k) {
                            QDomElement dimEl = dims.at(k).toElement();
                            int dim = dimEl.text().toInt();
                            if ( dim == 0 ) {
                                isDynAlloced = true;
                                break;
                            }
                        }
                        break;
                    }
                }
            } else {
                fprintf(stderr, "koviz [bad scoobs]: tv bad type=%s\n",
                        elType.toLatin1().constData());
                exit(-1);
            }
        }
    }

    return isDynAlloced;
}


// Given undimensioned param name e.g. ball.state.out.position
// Return list of dimensioned params e.g.
//       ball.state.out.position[0]
//       ball.state.out.position[1]
QStringList TrickView::_expandParam(const QString &param)
{
    QStringList expandedParams;

    QStringList paramList = param.split('.');
    QDomElement rootElement = _sieDoc.documentElement();

    QString elType;
    for (int i = 0; i < paramList.size(); ++i ) {
        if ( i == 0 ) {
            QDomNodeList tlos = rootElement.elementsByTagName(
                                                            "top_level_object");
            QString simObject = paramList.at(i);
            for (int j = 0; j < tlos.size(); ++j) {
                QDomElement tlo = tlos.at(j).toElement();
                if ( tlo.attribute("name") == simObject ) {
                    elType = tlo.attribute("type");
                    expandedParams = __appendMember(expandedParams,tlo);
                    break;
                }
            }
        } else {
            if ( _name2element.contains(elType) ) {
                QDomElement el = _name2element.value(elType);
                QDomNodeList members = el.elementsByTagName("member");
                for (int j = 0; j < members.size(); ++j) {
                    QDomElement member = members.at(j).toElement();
                    QString memberName = member.attribute("name");
                    if ( memberName == paramList.at(i) ) {
                        expandedParams = __appendMember(expandedParams,member);
                        elType = member.attribute("type");
                        break;
                    }
                }
            } else {
                fprintf(stderr, "koviz [bad scoobs]: bad type=%s\n",
                        elType.toLatin1().constData());
                exit(-1);
            }
        }
    }

    return expandedParams;
}

// Given paramsIn list and member, e.g:
//
//     paramsIn:
//         simObj.state[0][0]
//         simObj.state[0][1]
//         simObj.state[1][0]
//         simObj.state[1][1]
//     member: position  (with xy dims)
//
// Return list with dimensioned member appended to paramsIn elements, e.g:
//
//     Return:
//         simObj.state[0][0].position[0]
//         simObj.state[0][0].position[1]
//         simObj.state[0][1].position[0]
//         simObj.state[0][1].position[1]
//         simObj.state[1][0].position[0]
//         simObj.state[1][0].position[1]
//         simObj.state[1][1].position[0]
//         simObj.state[1][1].position[1]
//
QStringList TrickView::__appendMember(QStringList &paramsIn,
                                      const QDomElement &member)
{
    QStringList params;

    QString memberName = member.attribute("name");
    QList<QList<int> > dims = _genDimensions(member);

    if ( paramsIn.isEmpty() ) {
        if ( dims.isEmpty() ) {
            params.append(memberName);
        } else {
            for (int i = 0; i < dims.size(); ++i ) {
                QString dimensionedMemberName = memberName;
                for (int j = 0; j < dims.at(i).size(); ++j) {
                    int d = dims.at(i).at(j);
                    QString bracketDim = QString("[%1]").arg(d);
                    dimensionedMemberName += bracketDim;
                }
                params.append(dimensionedMemberName);
            }
        }
    } else {
        foreach (QString paramIn, paramsIn ) {
            if ( dims.isEmpty() ) {
                params.append(paramIn + '.' + memberName);
            } else {
                for (int i = 0; i < dims.size(); ++i ) {
                    QString dimensionedMemberName = memberName;
                    for (int j = 0; j < dims.at(i).size(); ++j) {
                        int d = dims.at(i).at(j);
                        QString bracketDim = QString("[%1]").arg(d);
                        dimensionedMemberName += bracketDim;
                    }
                    params.append(paramIn + '.' + dimensionedMemberName);
                }
            }
        }
    }

    return params;
}

// Given tol/member element e.g.:
//
//    <member
//         name="dmT_CmdFbFa"
//         type="double"
//         io_attributes="15"
//         units="--">
//       <dimension>2</dimension>
//       <dimension>2</dimension>
//     </member>
//
// Return list of dimensions e.g.:
//      0 0
//      0 1
//      1 0
//      1 1
QList<QList<int> > TrickView::_genDimensions(const QDomElement& el)
{
    QList<QList<int> > dimensions;

    QList<int> dimList;
    QDomNodeList dims = el.elementsByTagName("dimension");
    for (int i = 0; i < dims.size(); ++i) {
        QDomElement dim = dims.at(i).toElement();
        dimList.append(dim.text().toInt());
    }

    if ( dimList.size() > 0 ) {
        QList<int> dim;
        __genDimensions(dimList,dim,dimensions);
    }

    return dimensions;
}

//
// Recursive helper routine for _genDimensions()
//
void TrickView::__genDimensions(const QList<int>& dimList,
                                QList<int>& dim,
                                QList<QList<int> >& dimensions)
{
    if ( dim.size() < dimList.size() ) {
        for (int i = 0; i < dimList.at(dim.size()); ++i) {
            QList<int> copyDim = dim;
            copyDim.append(i);
            __genDimensions(dimList,copyDim,dimensions);
        }
    } else {
        dimensions.append(dim);
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
    QModelIndex idx = _tvModel->index(start,0);
    QVariant v = _tvModel->data(idx);
    QString msg = QString("Time = %1").arg(v.toDouble());
    _setMessageLabel(msg);

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
    int nMatches = _sieListModel->setRegexp(rx);
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

void TrickView::_createTVModel(const QString& host, int port)
{
    QTcpSocket sieSocket;
    sieSocket.connectToHost(host,port);
    if (sieSocket.waitForConnected(500)) {
        sieSocket.write("trick.send_sie_resource()\n");
    }

    // Wait on Trick to send sie resource (takes ~50 seconds with GIVS!)
    QByteArray header;
    int nWaits = 0;
    int maxWaits = 99;  // JJ Watt!
    while ( 1 ) {
        QString msg = QString("Wait on trick.send_sie_resource (%1 of %2)").
                              arg(nWaits).arg(maxWaits);
        QMetaObject::invokeMethod(this, "_setMessageLabel",
                                  Qt::QueuedConnection,
                                  Q_ARG(QString, msg));
        bool isReady = sieSocket.waitForReadyRead(1000);
        if ( isReady ) {
            break;
        } else {
            if ( nWaits > maxWaits ) {
                _waitLabel->setText("Gave up on Trick SIE :(");
                return;
            }
            nWaits++;
            continue;
        }
    }

    // Read header with nbytes (top line in msg)
    header.append(sieSocket.readLine());
    QString s(header);
    QStringList fields = s.split('\t');
    s = fields.at(1);
    uint nbytes = s.toUInt();
    QString msg = QString("Transferring SIE! nbytes=%1").arg(nbytes);
    QMetaObject::invokeMethod(this, "_setMessageLabel",
                              Qt::QueuedConnection,
                              Q_ARG(QString, msg));

    // Read sie xml from Trick variable server
    QByteArray sieXML;
    while ( sieXML.size() < nbytes ) {
        bool isReady = sieSocket.waitForReadyRead();
        if ( !isReady ) {
            QString msg = QString("Failed loading SIE!");
            QMetaObject::invokeMethod(this, "_setMessageLabel",
                                  Qt::QueuedConnection,
                                  Q_ARG(QString, msg));
            return;
        }
        sieXML.append(sieSocket.readAll());
        QString msg = QString("Getting SIE! nbytes %1 of %2").
                             arg(nbytes).arg(sieXML.size());
        QMetaObject::invokeMethod(this, "_setMessageLabel",
                                  Qt::QueuedConnection,
                                  Q_ARG(QString, msg));
    }

    QString errMsg;
    if ( _sieDoc.setContent(sieXML,&errMsg) ) {
        QDomElement rootElement = _sieDoc.documentElement();

        // Cache off classes for speed
        QDomNodeList classElements = rootElement.elementsByTagName("class");
        for (int i = 0; i < classElements.size(); ++i) {
            QDomElement classElement = classElements.at(i).toElement();
            QString className = classElement.attribute("name");
            _name2element.insert(className,classElement);
        }

        int nObjects = 0;
        QDomNodeList tlos = rootElement.elementsByTagName("top_level_object");
        for (int i = 0; i < tlos.size(); ++i) {
            QDomElement tlo = tlos.at(i).toElement();
            if ( !tlo.attribute("name").startsWith("trick_") &&
                 !tlo.attribute("name").endsWith("integ_loop") ) {
                QList<QDomElement> path;
                path.append(tlo);
                QString msg = QString("Load %1 (%2 of %3)").
                                      arg(tlo.attribute("name")).
                                      arg(i).arg(tlos.size());
                QMetaObject::invokeMethod(this, "_setMessageLabel",
                                          Qt::QueuedConnection,
                                          Q_ARG(QString, msg));
                ++nObjects;
                _loadSieElement(tlo, path);
            }
        }
        QString msg = QString("Success! Finished loading %1 params from %2 objects!").
                              arg(_params.size()).arg(nObjects);
        QMetaObject::invokeMethod(this, "_setMessageLabel",
                              Qt::QueuedConnection,
                              Q_ARG(QString, msg));
    } else {
        QString msg = QString("Could not load sieXML document! Error=%1").
                              arg(errMsg);
        QMetaObject::invokeMethod(this, "_setMessageLabel",
                                  Qt::QueuedConnection, Q_ARG(QString, msg));
        return;
    }
}

void TrickView::_loadSieElement(const QDomElement &element,
                                QList<QDomElement> &path)
{
    QString elementType = element.attribute("type");
    QDomElement classMatch = _name2element.value(elementType);

    QDomNodeList memberElements = classMatch.elementsByTagName("member");
    for (int i = 0; i < memberElements.size(); ++i ) {

        QDomElement memberElement = memberElements.at(i).toElement();

        bool isMemberInPath = false;
        for (int j = 0; j < path.size(); ++j) {
            if (path.at(j).attribute("type")==memberElement.attribute("type")){
                // To avoid inf recursion don't add same member type to path
                // E.g. jeod_time.time_manager.dyn_time.time_manager.dyn_time...
                isMemberInPath = true;
                break;
            }
        }

        bool isPrimitive = true;
        QString memberType = memberElement.attribute("type");
        if ( _name2element.contains(memberType) ) {
            isPrimitive = false;
        }

        if ( isPrimitive || isMemberInPath ) {
            QString param;
            for (int j = 0; j < path.size(); ++j ) {
                param += path.at(j).attribute("name") + ".";
            }
            param += memberElement.attribute("name");
            _params.append(param);
        } else {
            QList<QDomElement> copypath(path);
            copypath.append(memberElement);
            _loadSieElement(memberElement,copypath);
        }
    }
}

void TrickView::_addParamToBook(const QString &param)
{
    QModelIndex currIdx = _bookSelectModel->currentIndex();
    QModelIndex pageIdx;
    QModelIndex plotIdx;
    if ( _bookModel->data(currIdx).toString() == "Plot" ) {
        plotIdx = currIdx;
        pageIdx = _bookModel->getIndex(plotIdx,"Page");
    }
    int i = 0;
    QString unit = _paramUnit(param);
    foreach ( QString p, _expandParam(param) ) {
        if ( !plotIdx.isValid() ) {
            pageIdx = _createPage();
            plotIdx = _addPlotToPage(pageIdx,p,unit);
        } else {
            Qt::KeyboardModifiers kmods = QApplication::keyboardModifiers();
            bool isAlt = (kmods & Qt::AltModifier);
            bool isCtl = (kmods & Qt::ControlModifier);
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
