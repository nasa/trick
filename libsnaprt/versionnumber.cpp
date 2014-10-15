#include "versionnumber.h"

QString VersionNumber::_err_string;
QTextStream VersionNumber::_err_stream(&VersionNumber::_err_string);

VersionNumber::VersionNumber() :
    _isValid(false),_major(0),_minor(0),_patch(0),_revision(0)
{
}

VersionNumber::VersionNumber(int major, int minor, int patch, int revision) :
        _isValid(false), _major(major), _minor(minor), _patch(patch),
        _revision(revision)
{
    if ( major >= 0 && minor >= 0 && patch >= -1 && revision >= 0 ) {
        _isValid = true;
    }
}

VersionNumber::VersionNumber(const QString &s) :
    _isValid(false), _major(0),_minor(0),_patch(0),_revision(0)
{
    if ( s.isEmpty() ) {
        return;
    }

    QString sNum;
    for ( int i = 0 ; i < s.length(); ++i ) {
        if ( s.at(i).isDigit() ) {
            sNum = s.mid(i);
            break;
        }
    }

    QStringList list = sNum.split('.');

    bool ok = true;
    if ( list.length() == 1 ) {
        _major = list.at(0).toInt(&ok);
    } else if ( list.length() == 2 ) {
        _major = list.at(0).toInt(&ok);
        if ( ok ) {
            _minor = list.at(1).toInt(&ok);
        }
    } else if ( list.length() == 3 ) {
        _major = list.at(0).toInt(&ok);
        if ( ok ) {
            _minor = list.at(1).toInt(&ok);
        }
        if ( ok ) {
            // Handle "13.4.0-543" "13.4.dev-1" cases
            QString s3 = list.at(2);
            QStringList l3 = s3.split('-');
            if ( l3.at(0) == "dev" ) {
                _patch = -1;
            } else {
                _patch = l3.at(0).toInt(&ok);
            }
            if ( ok && l3.length() == 2 ) {
                _revision = l3.at(1).toInt(&ok);
            }
        }
    } else {
        ok = false;
    }

    if ( !ok) {
        _err_stream << "snap [error]: "
                    << " version number error found with the string \""
                    << s << "\"";
        throw std::runtime_error(_err_string.toAscii().constData());
    }

    _isValid = true;
}

QString VersionNumber::toString()
{

    QString patchStr("dev");
    if ( _patch >= 0 ) {
        patchStr = QString("%0").arg(_patch);
    }

    return QString("%0.%1.%2-%3").arg(_major).arg(_minor).
                                 arg(patchStr).arg(_revision);
}

bool VersionNumber::operator== (const VersionNumber& o) const
{
    return (major() == o.major() &&
            minor() == o.minor() &&
            patch() == o.patch() &&
            revision() == o.revision());
}

bool VersionNumber::operator!= (const VersionNumber& o) const
{
    return (!(*this == o) );
}

bool VersionNumber::operator< (const VersionNumber& o) const
{
    return (major() < o.major() ||
           (major() == o.major() && (minor() <  o.minor() ||
           (minor() == o.minor() && (patch() < o.patch() ||
           (patch() == o.patch() &&  revision() < o.revision()))))));
}

bool VersionNumber::operator<= (const VersionNumber& o) const
{
    return ( *this < o || *this == o ) ;
}

bool VersionNumber::operator> (const VersionNumber& o) const
{
    return ( !(*this < o || *this == o) ) ;
}

bool VersionNumber::operator>= (const VersionNumber& o) const
{
    return ( (*this > o || *this == o) ) ;
}
