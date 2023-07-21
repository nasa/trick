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
    connect(_searchBox,SIGNAL(textChanged(QString)),
            this,SLOT(_tvSearchBoxTextChanged(QString)));
    _gridLayout->addWidget(_searchBox,0,0);

    _tvModel = _createTVModel("localhost", 17100);

    // Vars list view
    _listView = new QListView(parent);
    _listView->setDragEnabled(false);
    _listView->setModel(_tvModel);
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
    //_tvFilterModel->setFilterRegExp(rx);
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
        QDomNodeList tlos = rootElement.elementsByTagName("top_level_object");
        for (int i = 0; i < tlos.size(); ++i) {
            QDomElement tlo = tlos.at(i).toElement();
            if ( !tlo.attribute("name").startsWith("trick_") &&
                 !tlo.attribute("name").endsWith("integ_loop") ) {
                _loadSieElement(tlo, tlo.attribute("name"));
            }
        }
    } else {
        fprintf(stderr, "TODO: popup err=%s\n", errMsg.toLatin1().constData());
        delete tvModel;
        return 0;
    }


    QStringList params;
    params.append("ball.state.out.position[0]");
    params.append("ball.state.out.position[1]");
    params.append("ball.state.out.velocity[0]");
    params.append("ball.state.out.velocity[1]");

    QStandardItem *rootItem = tvModel->invisibleRootItem();
    foreach (QString param, params) {
        QStandardItem *varItem = new QStandardItem(param);
        rootItem->appendRow(varItem);
    }

    return tvModel;
}

void TrickView::_loadSieElement(const QDomElement &element, const QString &path)
{
    QString elementType = element.attribute("type");
    QDomElement root = element.ownerDocument().documentElement();
    QDomNodeList classElements = root.elementsByTagName("class");
    QDomElement classMatch;
    for (int i = 0; i < classElements.size(); ++i) {
        QDomElement classElement = classElements.at(i).toElement();
        if ( elementType == classElement.attribute("name") ) {
            classMatch = classElement;
            break;
        }
    }

    QDomNodeList memberElements = classMatch.elementsByTagName("member");
    for (int i = 0; i < memberElements.size(); ++i ) {
        QDomElement classElement;
        QDomElement memberElement = memberElements.at(i).toElement();
        QString memberType = memberElement.attribute("type");
        bool isPrimitive = true;
        for (int i = 0; i < classElements.size(); ++i) {
            classElement = classElements.at(i).toElement();
            QString className = classElement.attribute("name");
            if ( className == memberType ) {
                isPrimitive = false;
                break;
            }
        }

        QString memberPath = path + '.' + memberElement.attribute("name");
        if ( isPrimitive ||
             memberElement.attribute("type") == classElement.attribute("name")){
            fprintf(stderr, "path=%s\n", memberPath.toLatin1().constData());
        } else {
            _loadSieElement(memberElement,memberPath);
        }
    }
}
