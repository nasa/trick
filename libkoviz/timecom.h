#ifndef BVISCOM_H
#define BVISCOM_H

#include <QObject>
#include <QTcpSocket>
#include <QString>
#include <QThread>
#include <QDir>
#include <QFileInfo>

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

private:
    QTcpSocket *socket;
    QString currentRun;
    QString currentTime = QString("t=0");
    bool firstRunSent = true;
    const int BvisPort = 64052;
    const int TimeComPort = BvisPort + 1;
};

#endif // TIMECOM_H
