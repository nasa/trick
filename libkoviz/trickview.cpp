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
    _searchBox = new QLineEdit(parent);
    //connect(_searchBox,SIGNAL(textChanged(QString)),
    //        this,SLOT(_tvSearchBoxTextChanged(QString)));
    connect(_searchBox,SIGNAL(returnPressed()),
            this,SLOT(_tvSearchBoxReturnPressed()));
    _gridLayout->addWidget(_searchBox,0,0);

    _tvModel = _createTVModel("localhost", 17100);
    //_tvModel = _createTVModel("localhost", 44479);

    // Setup models
    _varsFilterModel = new QSortFilterProxyModel;
    _varsFilterModel->setDynamicSortFilter(true);
    _varsFilterModel->setSourceModel(_tvModel);
    QRegExp rx(QString(".*"));
    _varsFilterModel->setFilterRegExp(rx);
    _varsFilterModel->setFilterKeyColumn(0);
    _varsSelectModel = new QItemSelectionModel(_varsFilterModel);

    // Vars list view
    _listView = new QListView(parent);
    _listView->setDragEnabled(false);
    _listView->setModel(_varsFilterModel);
    _gridLayout->addWidget(_listView,1,0);
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

void TrickView::_sieRead()
{

}

void TrickView::_tvSearchBoxTextChanged(const QString &rx)
{
    _varsFilterModel->setFilterRegExp(rx);
}

void TrickView::_tvSearchBoxReturnPressed()
{
    QString rx = _searchBox->text();
    _varsFilterModel->setFilterRegExp(rx);
}

QStandardItemModel* TrickView::_createTVModel(const QString& host, int port)
{
    QStandardItemModel* tvModel = new QStandardItemModel(0,1);

    QTcpSocket sieSocket;
    sieSocket.connectToHost(host,port);
    if (sieSocket.waitForConnected(500)) {
        sieSocket.write("trick.send_sie_resource()\n");
    }

    // Read number of bytes from header (top line in msg)
    QByteArray header;
    bool isReady = sieSocket.waitForReadyRead(90000);
    if ( !isReady ) {
        delete tvModel;
        fprintf(stderr, "TODO: Make me a popup!\n");
        return 0;
    }
    header.append(sieSocket.readLine());
    QString s(header);
    QStringList fields = s.split('\t');
    s = fields.at(1);
    uint nbytes = s.toUInt();

    fprintf(stderr, "nbytes=%d\n", nbytes);

    // Read sie xml from Trick variable server
    QByteArray sieXML;
    while ( sieXML.size() < nbytes ) {
        isReady = sieSocket.waitForReadyRead();
        if ( !isReady ) {
            delete tvModel;
            fprintf(stderr, "TODO: Make meeeee a popup!\n");
            return 0;
        }
        sieXML.append(sieSocket.readAll());
        fprintf(stderr, "read %d bytes\n", sieXML.size());
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

        QDomNodeList tlos = rootElement.elementsByTagName("top_level_object");
        for (int i = 0; i < tlos.size(); ++i) {
            QDomElement tlo = tlos.at(i).toElement();
            if ( !tlo.attribute("name").startsWith("trick_") &&
                 !tlo.attribute("name").endsWith("integ_loop") ) {
                QList<QDomElement> path;
                path.append(tlo);
                fprintf(stderr, "Load top_level_object=%s\n",
                        tlo.attribute("name").toLatin1().constData());
                _loadSieElement(tlo, path);
            }
        }
    } else {
        fprintf(stderr, "TODO: popup err=%s\n", errMsg.toLatin1().constData());
        delete tvModel;
        return 0;
    }

    QStandardItem *rootItem = tvModel->invisibleRootItem();
    foreach (QString param, _params) {
        QStandardItem *varItem = new QStandardItem(param);
        rootItem->appendRow(varItem);
    }

    return tvModel;
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
