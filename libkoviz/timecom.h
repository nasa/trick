#ifndef BVISCOM_H
#define BVISCOM_H

#include <QObject>
#include <QTcpSocket>
#include <QString>
#include <QThread>
#include <QDir>
#include <QFileInfo>
#include <QByteArray>
#include <QApplication>
#include <stdlib.h>

class TimeCom : public QObject
{
    Q_OBJECT
public:
    explicit TimeCom(QObject *parent=0);
    ~TimeCom();

    int connect2Bvis();
    int sendCom2Bvis(QString com = "t=8resetcom");
    int sendRun2Bvis(QString iRunDir);
    int resendCurTime2Bvis();
    int sendFirstRun(QString firstRun);
    int sendTime2Bvis(double liveTime);

signals:
    void timechangedByBvis(double time);

private slots:
     void _timeComRead();

private:
    QTcpSocket *socket;
    QString currentRun;
    QString currentTime;
    bool firstRunSent;
    const int BvisPort;
    const int TimeComPort;
};

#endif // TIMECOM_H
