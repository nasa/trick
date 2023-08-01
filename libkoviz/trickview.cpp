#include "trickview.h"

#ifdef __linux
#include "timeit_linux.h"
#endif

TrickView::TrickView(PlotBookModel *bookModel,
                     QWidget *parent) :
    QWidget(parent),
    _bookModel(bookModel)
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
    _listView->setDragEnabled(false);
    _listView->setModel(_sieListModel);

    _gridLayout->addWidget(_listView,2,0);
    _listView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    //_listView->setSelectionModel(_tvSelectModel);
    _listView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    _listView->setFocusPolicy(Qt::ClickFocus);
    /*
    connect(_varsSelectModel,
         SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
         this,
         SLOT(_tvSelectModelSelectionChanged(QItemSelection,QItemSelection)));
         */

    //QFuture<void> future = QtConcurrent::run(this, &TrickView::loadDatabase);
    QString host("localhost");
    QFuture<void> future = QtConcurrent::run(this,&TrickView::_createTVModel,
                                             host,17100);
}

TrickView::~TrickView()
{
}


void TrickView::_tvSelectModelSelectionChanged(
                                const QItemSelection &currVarSelection,
                                const QItemSelection &prevVarSelection)
{
    Q_UNUSED(prevVarSelection); // TODO: handle deselection (prevSelection)
}

void TrickView::_tvSearchBoxTextChanged(const QString &rx)
{
    //_varsFilterModel->setFilterRegExp(rx);
}

void TrickView::_setWaitLabel(const QString &msg)
{
    _waitLabel->setText(msg);
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
    _setWaitLabel(msg);
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
        QMetaObject::invokeMethod(this, "_setWaitLabel",
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
    QMetaObject::invokeMethod(this, "_setWaitLabel",
                              Qt::QueuedConnection,
                              Q_ARG(QString, msg));

    // Read sie xml from Trick variable server
    QByteArray sieXML;
    while ( sieXML.size() < nbytes ) {
        bool isReady = sieSocket.waitForReadyRead();
        if ( !isReady ) {
            QString msg = QString("Failed loading SIE!");
            QMetaObject::invokeMethod(this, "_setWaitLabel",
                                  Qt::QueuedConnection,
                                  Q_ARG(QString, msg));
            return;
        }
        sieXML.append(sieSocket.readAll());
        QString msg = QString("Getting SIE! nbytes %1 of %2").
                             arg(nbytes).arg(sieXML.size());
        QMetaObject::invokeMethod(this, "_setWaitLabel",
                                  Qt::QueuedConnection,
                                  Q_ARG(QString, msg));
    }

    QDomDocument doc;
    QString errMsg;
    if ( doc.setContent(sieXML,&errMsg) ) {
        QDomElement rootElement = doc.documentElement();

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
                QMetaObject::invokeMethod(this, "_setWaitLabel",
                                          Qt::QueuedConnection,
                                          Q_ARG(QString, msg));
                ++nObjects;
                _loadSieElement(tlo, path);
            }
        }
        QString msg = QString("Success! Finished loading %1 params from %2 objects!").
                              arg(_params.size()).arg(nObjects);
        QMetaObject::invokeMethod(this, "_setWaitLabel",
                              Qt::QueuedConnection,
                              Q_ARG(QString, msg));
    } else {
        QString msg = QString("Could not load sieXML document! Error=%1").
                              arg(errMsg);
        QMetaObject::invokeMethod(this, "_setWaitLabel",
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
