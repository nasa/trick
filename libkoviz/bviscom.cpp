#include "bviscom.h"

BvisCom::BvisCom(QWidget *parent) : QWidget(parent)
{
    socket = new QTcpSocket();
    currentRun = QString("");
    printf("Created BvisCom instance!\n");
    connect2Bvis();
}

BvisCom::~BvisCom()
{
    socket->close();
    delete socket;
    printf("Closed and deleted Socket\n");
}




int BvisCom::connect2Bvis()
{
    const QString localhost = "127.0.0.1";
    const int BvisPort = 64052;
    socket->connectToHost(localhost,BvisPort);
    if (socket->waitForConnected(500))
    {
        printf("Connected!\n");
    }
    else
    {
//        printf("TimedOut\n");
        return 1;
    }
//    socket->close();
    return 0;
}

int BvisCom::sendCom2Bvis(QString com)
{
    if (firstRunSent == false)
        com = QString("run=%1").arg(currentRun);

    if (socket->state() != QTcpSocket::ConnectedState)
    {
        if(connect2Bvis() != 0)
        {
            return 1;
        }
    }

    qint64 bytesWritten = socket->write(
                (const char *) com.toLocal8Bit().data());
    printf("\nSending to Bvis: %s\nBytes written: %d\n\n",
           (const char *) com.toLocal8Bit().data(), bytesWritten);

    if (firstRunSent == false && bytesWritten > 0)
        firstRunSent = true;

    return 0;
}

int BvisCom::sendTime2Bvis(double liveTime)
{
//    sendCom2Bvis();
    currentTime = QString("t=%1").arg(liveTime);
//    printf("Com sent: %s\n",currentTime.toLatin1().data());
//    sendCom2Bvis(QString("t=%1").arg(liveTime));
    return sendCom2Bvis(currentTime);
}

int BvisCom::sendRun2Bvis(QString runDir)
{
//    if (QString::compare(runDir,currentRun) != 0)
//        return 0;

//    sendCom2Bvis();
//    QThread::msleep(10);
//    printf("Com sent: %s\n",QString("run=%1").arg(runDir).toLatin1().data());
    int ret = sendCom2Bvis(QString("run=%1").arg(runDir));
    if (ret == 0)
        currentRun = runDir;
//    QThread::msleep(2000);
//    sendCom2Bvis(currentTime);
    return ret;
}

int BvisCom::resendCurTime2Bvis()
{
    return sendCom2Bvis(currentTime);
}

int BvisCom::sendFirstRun(QString firstRun)
{
    currentRun = firstRun;
    printf("");
    if (0 == sendCom2Bvis())
    {
        firstRunSent = true;
        return 0;
    }
    return 1;
}
