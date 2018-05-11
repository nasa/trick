#ifndef SESSION_H
#define SESSION_H

#include <QString>
#include <QStringList>
#include <QFile>
#include <QFileInfo>
#include <QTextStream>
#include <QRegExp>

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
    QString color1() const { return _colors.at(0); }
    QString color2() const { return _colors.at(1); }
    QString color3() const { return _colors.at(2); }
    QString color4() const { return _colors.at(3); }
    QString color5() const { return _colors.at(4); }
    QString color6() const { return _colors.at(5); }
    QString color7() const { return _colors.at(6); }

private:
    Session();

    QStringList _runs;
    QStringList _dps;
    QString _device;
    QString _presentation;
    double _timeMatchTolerance;
    double _frequency;
    QStringList _colors;
};

#endif // SESSION_H
