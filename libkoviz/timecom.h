#ifndef BVISCOM_H
#define BVISCOM_H

#include <QWidget>
#include <QTcpSocket>
#include <QString>
#include <QThread>

class TimeCom : public QWidget
{
    Q_OBJECT
public:
    explicit TimeCom(QWidget *parent = 0);
    ~TimeCom();

    int connect2Bvis();
    int sendCom2Bvis(QString com = "t=8resetcom");
    int sendRun2Bvis(QString runDir);
    int resendCurTime2Bvis();
    int sendFirstRun(QString firstRun);

private slots:
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
