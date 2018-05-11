#ifndef SESSION_H
#define SESSION_H

#include <QString>
#include <QStringList>
#include <QFile>
#include <QFileInfo>
#include <QTextStream>

class Session
{
public:
    Session(const QString& sessionFileName);

    QStringList runs() const;
    QStringList dps() const;
    QString device() const;
    QString presentation() const;
    double timeMatchTolerance() const;
    double frequency() const;

private:
    Session();

    QStringList _runs;
    QStringList _dps;
    QString _device;
    QString _presentation;
    double _timeMatchTolerance;
    double _frequency;
};

#endif // SESSION_H
