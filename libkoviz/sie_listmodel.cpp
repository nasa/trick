#include "sie_listmodel.h"

SieListModel::SieListModel(const QString &host, int port, QObject *parent)
    : QAbstractListModel{parent},
      _host(host),
      _port(port),
      _fetchCount(0),
      _fetchChunkSize(10000),
      _isModelResetting(false)
{
    QFuture<void> ftr = QtConcurrent::run(this, &SieListModel::_createSIEModel,
                                          host,port);
    Q_UNUSED(ftr);

    _connectToSim();
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

    if ( !index.isValid() || index.row() < 0 || _isModelResetting ) {
        return variant;
    }
    if ( index.row() >= _fetchCount ) {
        return variant;
    }

    _mutex.lock();
    if ( role == Qt::DisplayRole ) {
        if ( _rx.isEmpty() || _rx == ".*" ) {
            variant = _params.at(index.row());
        } else {
            variant = _filteredParams.at(index.row());
        }
    }
    _mutex.unlock();

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
    _isModelResetting = true;
    beginResetModel();
    _rx = rx;
    _mutex.lock();
    _filteredParams.clear();
    QRegularExpression filter(rx);
    QStringList paramMatches;
    foreach (QString param, _params) {
        if ( filter.match(param).hasMatch() ) {
            paramMatches.append(param);
        }
    }
    _mutex.unlock();
    const int nMemoryFilter = 256;
    if ( paramMatches.size() <= nMemoryFilter ) {
        // If param list is reasonably short, filter further for params in
        // Trick managed memory
        QProgressDialog progress("Filtering for Trick memory managed params",
                                 "Abort", 0, paramMatches.size()-1, 0);
        progress.setWindowModality(Qt::WindowModal);
        progress.setMinimumDuration(500);
        int i = 0;
        QStringList memFilteredParamMatches;
        foreach (QString param, paramMatches) {
            if ( expandParam(param).size() > 0 ) {
                memFilteredParamMatches.append(param);
            }
            progress.setValue(i++);
            QString msg = QString("Found %1 Trick managed params.  "
                                  "Searching %2 of %3")
                                   .arg(memFilteredParamMatches.size())
                                   .arg(i).arg(paramMatches.size());
            progress.setLabelText(msg);
            if (progress.wasCanceled()) {
                break;
            }
        }
        _mutex.lock();
        _filteredParams = memFilteredParamMatches;
        _mutex.unlock();
        progress.setValue(paramMatches.size()-1);
    } else {
        // Do not filter for params in Trick managed memory
        // since it takes a long time
        _mutex.lock();
        _filteredParams = paramMatches;
        _mutex.unlock();
    }
    _fetchCount = 0;
    endResetModel();
    _isModelResetting = false;

    return _filteredParams.size();
}

bool SieListModel::canFetchMore(const QModelIndex &parent) const
{
    bool isCanFetchMore = false;

    if ( parent.isValid() ) {
        return false;
    }

    _mutex.lock();
    if ( _rx.isEmpty() || _rx == ".*" ) {
        if ( _fetchCount < _params.size() ) {
            isCanFetchMore = true;
        }
    } else {
        if ( _fetchCount < _filteredParams.size() ) {
            isCanFetchMore = true;
        }
    }
    _mutex.unlock();

    return isCanFetchMore;
}

void SieListModel::fetchMore(const QModelIndex &parent)
{
    if (parent.isValid()) {
        return;
    }

    _mutex.lock();
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
    _mutex.unlock();
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
    // This loads _sieDoc with the Trick SIE xml database
    bool isSIE = _createSieDocument();

    if ( isSIE ) {
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
        emit modelLoaded();
    } else {
        QString msg = QString("Could not load sieXML document!");
        emit sendMessage(msg);
        return;
    }
}

// Load _sieDoc with the Trick SIE xml database
bool SieListModel::_createSieDocument()
{
    bool isSIE = false;

    int ntries = 5;
    int i = 0;
    while (1) {
        isSIE = __createSieDocument();
        if ( isSIE ) {
            // All good
            break;
        } else if ( qApp->closingDown() ) {
            break;
        } else if ( ++i > ntries ) {
            // Giving up
            QString msg = QString("Giving up on Trick SIE :(");
            emit sendMessage(msg);
            break;
        } else {
            // Sleep before trying again
            QString msg = QString("Wait 5 seconds and try SIE again "
                                  "(%1 of %2)").arg(i).arg(ntries);
            emit sendMessage(msg);
            QThread::sleep(2);
        }
    }

    return isSIE;
}

bool SieListModel::__createSieDocument()
{
    bool isSIE = false;

    _sieDoc = QDomDocument();

    QTcpSocket sieSocket;

    int nWaits = 0;
    while ( 1 ) {
        sieSocket.connectToHost(_host,_port);
        sieSocket.waitForConnected(1000);
        if (sieSocket.state() == QAbstractSocket::ConnectedState){
            break;
        } else if ( qApp->closingDown() ) {
            return false;
        } else {
            QString msg = QString("Wait on Trick server (%1)").arg(++nWaits);
            emit sendMessage(msg);
            QThread::sleep(1);
        }
    }
    QString msg = QString("Connected to Trick server");
    emit sendMessage(msg);
    QThread::sleep(1);

    // Tell the Trick server to send the SIE database
    sieSocket.write("trick.send_sie_resource()\n");

    // Wait on Trick to send sie database (which can be large!)
    QByteArray header;
    nWaits = 0;
    int maxWaits = 10;
    while ( 1 ) {
        QString msg = QString("Wait on trick.send_sie_resource (%1 of %2)").
                              arg(nWaits).arg(maxWaits);
        emit sendMessage(msg);
        bool isReady = sieSocket.waitForReadyRead(1000);
        if ( isReady ) {
            break;
        } else if ( qApp->closingDown() ) {
            return false;
        } else {
            if ( nWaits >= maxWaits ) {
                emit sendMessage("Gave up on Trick SIE :(");
                return false;
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
    msg = QString("Transferring SIE! nbytes=%1").arg(nbytes);
    emit sendMessage(msg);

    // Read sie xml from Trick variable server
    QByteArray sieXML;
    while ( sieXML.size() < nbytes ) {
        bool isReady = sieSocket.waitForReadyRead();
        if ( !isReady ) {
            QString msg = QString("Failed loading SIE!");
            emit sendMessage(msg);
            return false;
        }
        sieXML.append(sieSocket.readAll());
        QString msg = QString("Getting SIE! nbytes %1 of %2").
                             arg(nbytes).arg(sieXML.size());
        emit sendMessage(msg);
    }

    QString errMsg;
    isSIE = _sieDoc.setContent(sieXML,&errMsg) ;
    if ( !isSIE ) {
        QString msg = QString("Could not load sieXML document! Error=%1").
                              arg(errMsg);
        emit sendMessage(msg);
    }

    return isSIE;
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
            _mutex.lock();
            _params.append(param);
            _mutex.unlock();
        } else {
            QList<QDomElement> copypath(path);
            copypath.append(memberElement);
            _loadSieElement(memberElement,copypath);
        }
    }
}

QString SieListModel::paramUnit(const QString &paramIn)
{
    QString unit;

    if ( paramIn == "time" || paramIn == "sys.exec.out.time" ) {
        return "s";
    }

    QString param = paramIn;
    QRegularExpression rgx("\\[\\d+\\]");
    param.replace(rgx,""); // remove bracketed dimensions

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
    connect(_vsSocketParamSizes,SIGNAL(readyRead()), &loop, SLOT(quit()));

    // Send the msg
    QString msg = QString("sys.stdout.write("
                          "str(trick.get_size(trick.get_address(\"%1[0]\")))"
                          ")\n").arg(param);
    _vsSocketParamSizes->write(msg.toUtf8());

    // Loop until socket read or timeout
    loop.exec();

    if ( timer.isActive() ) {
        // Message received before timer timeout
        while (_vsSocketParamSizes->bytesAvailable() > 0) {
            QByteArray line1 = _vsSocketParamSizes->readLine();
            Q_UNUSED(line1);
            QByteArray line2 = _vsSocketParamSizes->readLine();
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

// This checks if paramIn is in the model SIE,
// but does not check if it is actually allocated
// and exists in memory.  This checks if the
// parameter is in the SIE db.
//
// To check whether or not the param exists
// and is allocated in memory, one would need
// not only ask if the param exists, but also
// ask if expandParam(param).size() > 0
//
bool SieListModel::isParamExists(const QString &paramIn)
{
    bool isExists = false;

    _mutex.lock();
    QString param = paramIn;
    QRegularExpression rgx("\\[\\d+\\]");
    param.replace(rgx,"");
    if ( paramIn == "time" ) {
        isExists = true;
    } else if ( _params.contains(param) ) {
        isExists = true;
    }
    _mutex.unlock();

    return isExists;
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

void SieListModel::_socketDisconnect()
{
    _isModelResetting = true;
    beginResetModel();
    _mutex.lock();
    _params.clear();
    _filteredParams.clear();
    _mutex.unlock();

    _rx.clear();
    _fetchCount = 0;
    _sieDoc.clear();
    _name2element.clear();
    endResetModel();
    _isModelResetting = false;

    QString msg = QString("Lost sim on host=%1 port=%2\n").
                          arg(_host).arg(_port);
    sendMessage(msg);

    _vsSocketParamSizes->flush();
    _vsSocketParamSizes->abort();
    _vsSocketParamSizes->close();
    _vsSocketParamSizes->deleteLater();

    QEventLoop loop;
    connect(this, SIGNAL(modelLoaded()), &loop, SLOT(quit()));
    QFuture<void> ftr = QtConcurrent::run(this, &SieListModel::_createSIEModel,
                                          _host,_port);
    Q_UNUSED(ftr);
    loop.exec();

    _connectToSim();  // reconnect to sim
}

void SieListModel::_connectToSim()
{
    QTimer timer;
    timer.setInterval(2000);
    QEventLoop loop;

    _vsSocketParamSizes = new QTcpSocket(this);

    connect(_vsSocketParamSizes, SIGNAL(disconnected()),
            this, SLOT(_socketDisconnect()));

    // If the socket is ready to read or times out, break out of the event loop
    connect(_vsSocketParamSizes,SIGNAL(connected()), &loop, SLOT(quit()));
    connect(&timer, SIGNAL(timeout()), &loop, SLOT(quit()));

    int i = 0;
    while (1) {

        timer.start();
        _vsSocketParamSizes->connectToHost(_host, _port);
        int ret = loop.exec();
        if ( ret < 0 ) {
            return;
        }
        timer.stop();

        if ( _vsSocketParamSizes->state() == QAbstractSocket::ConnectedState ) {
            QString msg = QString("Connected to sim on host=%1 port=%2").
                           arg(_host).arg(_port);
            sendMessage(msg);
            break;
        } else {
            QString msg = QString("(%1) Attempting to connect to sim on "
                                  "host=%2 port=%3")
                                   .arg(i++).arg(_host).arg(_port);
            sendMessage(msg);
        }
    }

    QString msg = QString("trick.var_set_send_stdio(True)\n");
    _vsSocketParamSizes->write(msg.toUtf8());
}
