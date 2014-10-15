#ifndef VERSIONNUMBER_H
#define VERSIONNUMBER_H

#include <QString>
#include <QStringList>
#include <QTextStream>
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
    VersionNumber(const QString& s);

    VersionNumber(int major, int minor, int patch, int revision) :
        _major(major), _minor(minor), _patch(patch),
        _revision(revision) {}

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
    VersionNumber();
    int _major;
    int _minor;
    int _patch;
    int _revision;
    static QString _err_string;
    static QTextStream _err_stream;
};

#endif // VERSIONNUMBER_H
