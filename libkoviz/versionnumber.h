#ifndef VERSIONNUMBER_H
#define VERSIONNUMBER_H

#include <QString>
#include <QStringList>
#include <QTextStream>
#include <QFile>
#include <stdexcept>

// Version = 2.1.0.4
//        2 == major
//        1 == minor
//        0 == patchLevel
//        4 == revision
//
// Examples
// Trick-13.4.dev-1
// Trick-13.4.0
// Trick-13.4.0
// Trick-13.1.0-3104

class VersionNumber
{
public:
    VersionNumber(); // invalid version
    VersionNumber(const QString& s);
    VersionNumber(int major, int minor, int patch, int revision);

    bool isValid() { return _isValid; }

    QString toString();

    int major() const { return _major; }
    int minor() const { return _minor; }
    int patch() const { return _patch; }
    int revision() const { return _revision; }

    bool operator==(const VersionNumber& o) const;
    bool operator!=(const VersionNumber& o) const;
    bool operator<(const VersionNumber& o) const;
    bool operator<=(const VersionNumber& o) const;
    bool operator>(const VersionNumber& o) const;
    bool operator>=(const VersionNumber& o) const;

private:
    bool _isValid;
    int _major;
    int _minor;
    int _patch;
    int _revision;
    QString _patchString;
    QString _revisionString;
    static QString _err_string;
    static QTextStream _err_stream;
};

class TrickVersion
{
public:
    TrickVersion(); // invalid version
    TrickVersion(const QString& runDir);

    VersionNumber versionNumber() { return _versionNumber; }

private:
    VersionNumber _versionNumber;
    VersionNumber _calcTrickVersion(const QString& runDir) const;
    static QString _err_string;
    static QTextStream _err_stream;
};

#endif // VERSIONNUMBER_H
