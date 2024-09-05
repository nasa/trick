#include "versionnumber.h"

QString VersionNumber::_err_string;
QTextStream VersionNumber::_err_stream(&VersionNumber::_err_string);
QString TrickVersion::_err_string;
QTextStream TrickVersion::_err_stream(&TrickVersion::_err_string);

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
            // Handle "13.4.0-543","13.4.dev-1" and "19.8.0-beta" cases
            QString s3 = list.at(2);
            QStringList l3 = s3.split('-');
            _patch = l3.at(0).toInt(&ok);
            if ( !ok ) {
                // E.g. if 19.8.dev-40, then patch string is "dev"
                _patch = -1;
                _patchString = l3.at(0);
                ok = true;
            }
            if ( l3.length() == 2 ) {
                _revision = l3.at(1).toInt(&ok);
                if ( !ok ) {
                    // E.g. if 19.8.0-beta, then revision string is "beta"
                    _revision = -1;
                    _revisionString = l3.at(1);
                    ok = true;
                }
            }
        }
    } else {
        ok = false;
    }

    if ( !ok) {
        _err_stream << "koviz [error]: "
                    << " version number error found with the string \""
                    << s << "\"";
        throw std::runtime_error(_err_string.toLatin1().constData());
    }

    _isValid = true;
}

QString VersionNumber::toString()
{

    QString patchStr(_patchString);
    if ( _patch >= 0 ) {
        patchStr = QString("%1").arg(_patch);
    }
    QString revisionStr(_revisionString);
    if ( _revision >= 0 ) {
        revisionStr = QString("%1").arg(_revision);
    }

    return QString("%0.%1.%2-%3").arg(_major).arg(_minor).
                                                 arg(patchStr).arg(revisionStr);
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


TrickVersion::TrickVersion()
{
}

TrickVersion::TrickVersion(const QString &runDir)
{
    _versionNumber = _calcTrickVersion(runDir);
}

// Uses S_run_summary if present
//
// If there is no S_run_summary, there is a hack to send 13.3 or 13.4
// based on S_job_execution
//
// TODO: make this robust if no S_run_summary present
//
VersionNumber TrickVersion::_calcTrickVersion(const QString& runDir) const
{
    VersionNumber v;

    QString sRunSummary = runDir + "/S_run_summary";

    QFile file(sRunSummary);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text )) {
        QTextStream in(&file);
        while (1) {
            QString line = in.readLine();
            if ( line.startsWith("Trick version") ) {
                v = VersionNumber(line);
                break;
            }
        }

        file.close();

    } else {
        // S_run_summary DNE, so use S_job_execution and send back a hack
        QString sJobExecutionFileName = runDir + "/S_job_execution";
        QFile sjob(sJobExecutionFileName);
        if (sjob.open(QIODevice::ReadOnly | QIODevice::Text )) {
            QTextStream in(&file);
            QString line = in.readLine();
            if ( line == "Thread information" ) {
                line = in.readLine(); // hack
                if ( line.startsWith("Trick::Threads") ) {
                    v = VersionNumber("13.4.0-0");
                } else {
                    // This is just a way to say Pre 13.4
                    // It could very well be trick 10.0 (TODO)
                    v = VersionNumber("13.0.0-0");
                }
            }
        }
    }

    return v;
}
