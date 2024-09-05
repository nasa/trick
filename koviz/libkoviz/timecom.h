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
#include <QStringList>
#include <stdlib.h>

class TimeCom : public QObject
{
    Q_OBJECT
public:
    explicit TimeCom(const QString& host, int port, QObject *parent=0);
    ~TimeCom();

    void sendRun2Bvis(const QString &iRunDir);
    void sendTime2Bvis(double liveTime);
    void sendList2Bvis(const QStringList& list);

signals:
    void timechangedByBvis(double time);

private slots:
     void _timeComRead();

private:
    QTcpSocket *socket;
    QString _host;
    int _port;

    int _connect2Bvis();
    void _sendMsg2Bvis(const QString& msg);
    QStringList _msgQueue;
};

#endif // TIMECOM_H
