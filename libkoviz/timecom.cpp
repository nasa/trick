#include "timecom.h"
TimeCom::TimeCom( const QString& host,int port,QObject* parent) :
    QObject(parent),
    _host(host),
    _port(port)
{
    socket = new QTcpSocket();
    connect(socket,SIGNAL(readyRead()),
            this,SLOT(_timeComRead()));
    _connect2Bvis();
}

TimeCom::~TimeCom()
{
    socket->close();
    delete socket;
}

int TimeCom::_connect2Bvis()
{
    if (socket->state() == QTcpSocket::ConnectedState ) {
        return 0;
    }
    if (socket->state() == QTcpSocket::ConnectingState ) {
        return 1;
    }
    socket->connectToHost(_host,_port);
    if (socket->state() == QTcpSocket::ConnectedState) {
        return 0;
    } else {
        return 1;
    }
}

void TimeCom::_sendMsg2Bvis(const QString &msg)
{
    if ( _connect2Bvis() != 0 ) {
        if ( !msg.startsWith("t=") ) {
            // Throw away time msgs, but queue other msgs for sending later
            _msgQueue.append(msg);
        }
        return;
    }

    if (socket->state() == QTcpSocket::ConnectedState) {
        // Send queued msgs that occurred while connecting
        foreach (QString qmsg, _msgQueue) {
            qint64 bytesWritten = socket->write(qmsg.toLocal8Bit().data());
            if ( bytesWritten < 0 ) {
                fprintf(stderr, "koviz [error]: bvis socket write failed!!!\n");
            }
        }
        _msgQueue.clear();

        qint64 bytesWritten = socket->write(msg.toLocal8Bit().data());
        if ( bytesWritten < 0 ) {
            fprintf(stderr, "koviz [error]: bvis socket write failed!!!\n");
        }
    }
}

void TimeCom::sendTime2Bvis(double liveTime)
{
#if QT_VERSION >= 0x050000
    Qt::ApplicationState state = qApp->applicationState();
    if ( state & Qt::ApplicationActive) {
        QString msg = QString("t=%1,").arg(liveTime);
        _sendMsg2Bvis(msg);
    }
#else
    fprintf(stderr, "koviz [error]: Bvis needs qt5 or greater!\n");
    exit(-1);
#endif
}

void TimeCom::_timeComRead()
{
    QByteArray bytes = socket->readLine();

    // Read all packets, set bytes to last packet read
    while (1) {
        QByteArray line = socket->readLine();
        if ( line.size() <= 0 ) {
            break;
        } else {
            bytes = line;
        }
    }

    QString msg(bytes);
    QStringList fields = msg.split('=');
    if ( fields.at(0) == "TIME" ) {
        double time = fields.at(1).toDouble();
        emit timechangedByBvis(time);
    }
}

void TimeCom::sendRun2Bvis(const QString& iRunDir)
{
    QDir rdir(iRunDir);

    QString runDir = iRunDir;

    // If a run subdir contains *.motcsv, send run subdir
    QStringList filter;
    filter << "*.motcsv";
    QStringList dirs = rdir.entryList(QDir::Dirs);
    foreach ( QString dir, dirs ) {
        QString fdir = iRunDir + "/" + dir;
        QDir subdir(fdir);
        if ( ! subdir.entryList(filter,QDir::Files).isEmpty() ) {
            runDir = subdir.absolutePath();
            break;
        }
    }
    QString msg = QString("run=%1,").arg(runDir);
    _sendMsg2Bvis(msg);
}

void TimeCom::sendList2Bvis(const QStringList &list)
{
    QString msg;
    foreach ( QString el, list ) {
        msg += el;
        msg += ',';
    }

    _sendMsg2Bvis(msg);
}
