#include "sie_listmodel.h"

SieListModel::SieListModel(const QString &host, int port, QObject *parent)
    : QAbstractListModel{parent},
      _fetchCount(0),
      _fetchChunkSize(10000)
{
    QFuture<void> ftr = QtConcurrent::run(this, &SieListModel::_createSIEModel,
                                          host,port);
    Q_UNUSED(ftr);

    connect(&_vsSocketParamSizes,SIGNAL(readyRead()),
            this,SLOT(_vsReadParamSizes()));
    _vsSocketParamSizes.connectToHost(host,port);
    if (!_vsSocketParamSizes.waitForConnected(1000)) {
        fprintf(stderr, "koviz [error]: "
                        "Could not connect to trick var server on"
                        "host=%s port=%d!\n",
                host.toLatin1().constData(),port);
        return;
    }
    QString msg = QString("trick.var_set_send_stdio(True)\n");
    _vsSocketParamSizes.write(msg.toUtf8());
}

int SieListModel::rowCount(const QModelIndex &parent) const
{
    if ( parent.isValid() ) {
        return 0;  // Flat list has no children
    } else {
        return _fetchCount;
    }
}

QVariant SieListModel::data(const QModelIndex &index, int role) const
{
    QVariant variant;

    if ( !index.isValid() || index.row() < 0 ) {
        return variant;
    }
    if ( index.row() >= _fetchCount ) {
        return variant;
    }

    if ( role == Qt::DisplayRole ) {
        if ( _rx.isEmpty() || _rx == ".*" ) {
            variant = _params.at(index.row());
        } else {
            variant = _filteredParams.at(index.row());
        }
    }

    return variant;
}

// This is for drag-n-drop.  A widget that accepts a drop needs to know
// what model the dropped data came from.
QMimeData *SieListModel::mimeData(const QModelIndexList &indexes) const
{
    QMimeData* mimedata = QAbstractItemModel::mimeData(indexes);
    QByteArray data("SieListModel");
    mimedata->setData("koviz-model-type",data);
    return mimedata;
}

int SieListModel::setRegexp(const QString &rx)
{
    beginResetModel();
    _rx = rx;
    _filteredParams.clear();
    QRegularExpression filter(rx);
    foreach (QString param, _params) {
        if ( filter.match(param).hasMatch() ) {
            _filteredParams.append(param);
        }
    }
    _fetchCount = 0;
    endResetModel();

    return _filteredParams.size();
}

bool SieListModel::canFetchMore(const QModelIndex &parent) const
{
    bool isCanFetchMore = false;

    if ( parent.isValid() ) {
        return false;
    }

    if ( _rx.isEmpty() || _rx == ".*" ) {
        if ( _fetchCount < _params.size() ) {
            isCanFetchMore = true;
        }
    } else {
        if ( _fetchCount < _filteredParams.size() ) {
            isCanFetchMore = true;
        }
    }

    return isCanFetchMore;
}

void SieListModel::fetchMore(const QModelIndex &parent)
{
    if (parent.isValid()) {
        return;
    }

    int remainder;
    if ( _rx.isEmpty() || _rx == ".*" ) {
        remainder = _params.size() - _fetchCount;
    } else {
        remainder = _filteredParams.size() - _fetchCount;
    }
    int nItemsToFetch = qMin(_fetchChunkSize, remainder);

    if ( nItemsToFetch > 0 ) {
        beginInsertRows(parent,_fetchCount,_fetchCount+nItemsToFetch-1);
        _fetchCount += nItemsToFetch;
        endInsertRows();
    }
}

Qt::ItemFlags SieListModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags defaultFlags = QAbstractListModel::flags(index);

    if (index.isValid())
        return Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled | defaultFlags;
    else
        return Qt::ItemIsDropEnabled | defaultFlags;
}

void SieListModel::_createSIEModel(const QString& host, int port)
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
        emit sendMessage(msg);
        bool isReady = sieSocket.waitForReadyRead(1000);
        if ( isReady ) {
            break;
        } else {
            if ( nWaits > maxWaits ) {
                emit sendMessage("Gave up on Trick SIE :(");
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
    emit sendMessage(msg);

    // Read sie xml from Trick variable server
    QByteArray sieXML;
    while ( sieXML.size() < nbytes ) {
        bool isReady = sieSocket.waitForReadyRead();
        if ( !isReady ) {
            QString msg = QString("Failed loading SIE!");
            emit sendMessage(msg);
            return;
        }
        sieXML.append(sieSocket.readAll());
        QString msg = QString("Getting SIE! nbytes %1 of %2").
                             arg(nbytes).arg(sieXML.size());
        emit sendMessage(msg);
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
                emit sendMessage(msg);
                ++nObjects;
                _loadSieElement(tlo, path);
            }
        }
        QString msg = QString("Success! Finished loading %1 params "
                              "from %2 objects!").
                              arg(_params.size()).arg(nObjects);
        emit sendMessage(msg);
    } else {
        QString msg = QString("Could not load sieXML document! Error=%1").
                              arg(errMsg);
        emit sendMessage(msg);
        return;
    }
}

void SieListModel::_loadSieElement(const QDomElement &element,
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

QString SieListModel::paramUnit(const QString &param)
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

// Returns the alloced size (num elements) of the parameter
int SieListModel::paramSize(const QString &param)
{
    int sz = -1;

    QEventLoop loop;

    // Use a QTimer for a possible timeout waiting on vs server
    QTimer timer;
    timer.setSingleShot(true);
    timer.start(2000);

    // If the event loop times out, break out of the event loop
    connect(&timer, SIGNAL(timeout()), &loop, SLOT(quit()));

    // If the socket is ready to read, break out of the event loop
    connect(&_vsSocketParamSizes,SIGNAL(readyRead()), &loop, SLOT(quit()));

    // Send the msg
    QString msg = QString("sys.stdout.write("
                          "str(trick.get_size(trick.get_address(\"%1[0]\")))"
                          ")\n").arg(param);
    _vsSocketParamSizes.write(msg.toUtf8());

    // Loop until socket read or timeout
    loop.exec();

    if ( timer.isActive() ) {
        // Message received before timer timeout
        while (_vsSocketParamSizes.bytesAvailable() > 0) {
            QByteArray line1 = _vsSocketParamSizes.readLine();
            Q_UNUSED(line1);
            QByteArray line2 = _vsSocketParamSizes.readLine();
            QString msg(line2);
            bool ok;
            sz = msg.toDouble(&ok);
            if ( !ok ) {
                sz = -1;
            }
        }
    } else {
        // Timeout occured.  No message from var server, so returning -1
        fprintf(stderr, "koviz [error]: No msg from var server!\n");
    }

    return sz;
}

// Given undimensioned param name e.g. ball.state.out.position
// Return list of dimensioned params e.g.
//       ball.state.out.position[0]
//       ball.state.out.position[1]
QStringList SieListModel::expandParam(const QString &param)
{
    QStringList params;
    _expandParam("",param,&params);
    return params;
}

void SieListModel::_expandParam(const QString &expandedParam,
                             const QString &param,
                             QStringList* params)
{
    if ( expandedParam.isEmpty() ) {
        QString e = param.split('.').at(0);
        _expandParam(e,param,params);
    } else if ( expandedParam.endsWith('.') ) {
        int i = expandedParam.count('.');
        QString ep = expandedParam + param.split('.').at(i);
        _expandParam(ep,param,params);
    } else {
        QString e = expandedParam.split('.').last();
        int eDimIdx = e.count('[');
        QString p = param.section('.',0,expandedParam.split('.').size()-1);
        QDomElement pEl = _domElement(p);
        QDomNodeList dims = pEl.elementsByTagName("dimension");
        int nDims = dims.size();
        if ( eDimIdx == nDims ) {
            if ( expandedParam.split('.').size() == param.split('.').size() ) {
                // Finished recursing, append fully expanded parameter
                params->append(expandedParam);
            } else {
                QString ep = expandedParam + ".";
                _expandParam(ep,param,params);
            }
        } else if ( eDimIdx < nDims ) {
            QDomElement dim = dims.at(eDimIdx).toElement();
            int sizeDim = dim.text().toInt();
            if ( sizeDim == 0 ) {
                // Dyn alloced
                sizeDim = paramSize(expandedParam);
            }
            for (int i = 0; i < sizeDim; ++i) {
                QString ep = expandedParam + QString("[%1]").arg(i);
                _expandParam(ep,param,params);
            }
        } else {
            // No way
        }
    }
}

QDomElement SieListModel::_domElement(const QString &param)
{
    QDomElement element;

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
                    element = tlo;
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
                        element = member;
                        break;
                    }
                }
            } else {
                fprintf(stderr, "koviz [bad scoobs]: sie bad type=%s\n",
                        elType.toLatin1().constData());
                exit(-1);
            }
        }
    }

    return element;
}
