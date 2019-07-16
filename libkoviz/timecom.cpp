#include "timecom.h"

TimeCom::TimeCom(QWidget *parent) : QWidget(parent)
{
    socket = new QTcpSocket();
    currentRun = QString("");
    printf("Created BvisCom instance!\n");
//    connect(socket,SIGNAL(readyRead()),this,SLOT(readTime4Koviz()));
    connect2Bvis();
}

TimeCom::~TimeCom()
{
    socket->close();
    delete socket;
    printf("Closed and deleted Socket\n");
}




int TimeCom::connect2Bvis()
{
    const QString localhost = "127.0.0.1";
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

int TimeCom::sendCom2Bvis(QString com)
{
    /* I believe I had it like this because sending a run command immediately
     * followed by a time stamp command seems to make them both be ignored by
     * bvis
    // overwritting the com so that the run command is successfully sent
    // sending a time stamp command at the same time as a run command seems to
    // make them both fail.*/
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

/*
    if (socket->state() != QTcpSocket::ConnectedState)
    {
        if(connect2Bvis() != 0)
        {
            return 1;
        }
    }

    if (firstRunSent == false)
    {
        QString firstRunCom = QString("run=%1").arg(currentRun);

        qint64 bytesWritten = socket->write(
                    (const char *) firstRunCom.toLocal8Bit().data());
        printf("\nSending to Bvis: %s\nBytes written: %d\n\n",
               (const char *) firstRunCom.toLocal8Bit().data(), bytesWritten);

        if (firstRunSent == false && bytesWritten > 0)
            firstRunSent = true;

    }

    qint64 bytesWritten = socket->write(
                (const char *) com.toLocal8Bit().data());
    printf("\nSending to Bvis: %s\nBytes written: %d\n\n",
           (const char *) com.toLocal8Bit().data(), bytesWritten);


    return 0; */
}

int TimeCom::sendTime2Bvis(double liveTime)
{
//    sendCom2Bvis();
    currentTime = QString("t=%1").arg(liveTime);
//    printf("Com sent: %s\n",currentTime.toLatin1().data());
//    sendCom2Bvis(QString("t=%1").arg(liveTime));
    return sendCom2Bvis(currentTime);
}

int TimeCom::sendRun2Bvis(QString runDir)
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

int TimeCom::resendCurTime2Bvis()
{
    return sendCom2Bvis(currentTime);
}

int TimeCom::sendFirstRun(QString firstRun)
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

//void TimeCom::readTime4Koviz()
//{
//    printf(socket->readAll());
//    printf("\n");
//}
