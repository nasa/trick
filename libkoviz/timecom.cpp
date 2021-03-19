#include "timecom.h"

TimeCom::TimeCom(QObject* parent) : 
    QObject(parent),
    currentTime("t=0"),
    firstRunSent(true),
    BvisPort(64052),
    TimeComPort(BvisPort + 1)
{
    socket = new QTcpSocket();
    connect(socket,SIGNAL(readyRead()),
            this,SLOT(_timeComRead()));
    currentRun = QString("");
    connect2Bvis();
}

TimeCom::~TimeCom()
{
    socket->close();
    delete socket;
}

int TimeCom::connect2Bvis()
{
    const QString localhost = "127.0.0.1";
    socket->connectToHost(localhost,BvisPort);
    if (socket->waitForConnected(500)) {
        printf("Connected to bvis!\n");
    } else {
        // timed out
        return 1;
    }
    return 0;
}

int TimeCom::sendCom2Bvis(QString com)
{
    if (firstRunSent == false) {
        com = QString("run=%1").arg(currentRun);
    }

    if (socket->state() != QTcpSocket::ConnectedState) {
        if(connect2Bvis() != 0) {
            return 1;
        }
    }

    qint64 bytesWritten = socket->write(
                (const char *) com.toLocal8Bit().data());

    if (firstRunSent == false && bytesWritten > 0)
        firstRunSent = true;

    return 0;
}

int TimeCom::sendTime2Bvis(double liveTime)
{
#if QT_VERSION >= 0x050000
    Qt::ApplicationState state = qApp->applicationState();
    if ( state & Qt::ApplicationActive) {
        currentTime = QString("t=%1").arg(liveTime);
        return sendCom2Bvis(currentTime);
    } else {
        return 0;
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

int TimeCom::sendRun2Bvis(QString iRunDir)
{
    QDir rdir(iRunDir);

    // Locate dir that contains *.motcsv
    QString runDir;
    QString cmd;
    QStringList filter;
    filter << "*.motcsv";
    QStringList motcsvs = rdir.entryList(filter,QDir::Files);
    if ( !motcsvs.isEmpty() ) {
        runDir = iRunDir;
    } else {
        // See if subdir under iRunDir has *.motcsv (normal case)
        QStringList dirs = rdir.entryList(QDir::Dirs);
        foreach ( QString dir, dirs ) {
            QString fdir = iRunDir + "/" + dir;
            QDir subdir(fdir);
            if ( ! subdir.entryList(filter,QDir::Files).isEmpty() ) {
                runDir = subdir.absolutePath();
                break;
            }
        }
    }
    cmd = QString("run=%1").arg(runDir);
    int ret = sendCom2Bvis(cmd);
    if (ret == 0) {
        currentRun = runDir;
    }
    return ret;
}

int TimeCom::sendFirstRun(QString firstRun)
{
    currentRun = firstRun;
    if (0 == sendCom2Bvis())
    {
        firstRunSent = true;
        return 0;
    }
    return 1;
}
