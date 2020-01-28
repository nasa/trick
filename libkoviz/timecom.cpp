#include "timecom.h"

TimeCom::TimeCom(QObject* parent) : QObject(parent)
{
    socket = new QTcpSocket();
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
    currentTime = QString("t=%1").arg(liveTime);
    return sendCom2Bvis(currentTime);
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

int TimeCom::resendCurTime2Bvis()
{
    return sendCom2Bvis(currentTime);
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
