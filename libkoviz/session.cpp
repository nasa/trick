#include "session.h"

Session::Session() :
    _device("terminal"),
    _presentation("compare"),
    _timeMatchTolerance(1.0e-6)
{
    _colors << "" << "" << ""
            << "" << "" << "" << "";
}

Session::Session(const QString &sessionFileName) :
    _device("terminal"),
    _presentation("compare"),
    _timeMatchTolerance(1.0e-6)
{
    _colors << "" << "" << ""
            << "" << "" << "" << "";

    QFile file(sessionFileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        fprintf(stderr, "koviz [error]: Cannot read session file %s!\n",
                sessionFileName.toLatin1().constData());
        exit(-1);
    }

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();
        if ( line.contains("RUN:",Qt::CaseInsensitive) ) {
            int i = line.indexOf("RUN:",0,Qt::CaseInsensitive);
            QString run = line.mid(i+4).trimmed();
            run = run.remove("\"");
            if ( run.isEmpty() ) {
                fprintf(stderr,"koviz [error]: empty run specification in "
                               "session file %s.\n",
                               sessionFileName.toLatin1().constData());
                exit(-1);
            }
            _runs << run.split(" ",QString::SkipEmptyParts).at(0);

        } else if ( line.contains("PRODUCT:",Qt::CaseInsensitive) ) {
            int i = line.indexOf("PRODUCT:",0,Qt::CaseInsensitive);
            QString dp = line.mid(i+8).trimmed();
            if ( dp.startsWith("\"") ) {
                dp = dp.mid(1);
            }
            if ( dp.endsWith("\"") ) {
                dp.chop(1);
            }
            _dps << dp;
        } else if ( line.contains("DEVICE:",Qt::CaseInsensitive) ) {
            int i = line.indexOf("DEVICE:",0,Qt::CaseInsensitive);
            _device = line.mid(i+7).trimmed();
            _device = _device.remove("\"");
            if ( _device.isEmpty() ) {
                fprintf(stderr,"koviz [error]: empty device specification in "
                               "session file %s.\n",
                               sessionFileName.toLatin1().constData());
                exit(-1);
            }
            QStringList list = _device.split(" ",QString::SkipEmptyParts);
            if ( !QString::compare(list.at(0),"FILE",Qt::CaseInsensitive) ) {
                _device = list.at(1); // device is filename
                if ( _device == "terminal") { // in case filename=="terminal"
                    _device = "terminal.pdf";
                }
            } else {
                if ( !_device.compare("terminal",Qt::CaseInsensitive) ) {
                    _device = "terminal";
                } else {
                    // device is considered to be a filename
                }
            }
        } else if ( line.contains("PRESENTATION:",Qt::CaseInsensitive) ) {
            int i = line.indexOf("PRESENTATION:",0,Qt::CaseInsensitive);
            _presentation = line.mid(i+13).trimmed();
            if ( _presentation.startsWith("\"") ) {
                _presentation = _presentation.mid(1);
            }
            if ( _presentation.endsWith("\"") ) {
                _presentation.chop(1);
            }
            if ( _presentation != "compare" &&
                 _presentation != "error" &&
                 _presentation != "error+compare") {
                fprintf(stderr,"koviz [error]: session file has presentation "
                               "set to \"%s\".  For now, koviz only "
                               "supports \"compare\", \"error\" and "
                               "\"error+compare\"\n",
                               _presentation.toLatin1().constData());
                exit(-1);
            }

        } else if (line.contains("TIME_MATCH_TOLERANCE:",Qt::CaseInsensitive)) {
            int i = line.indexOf("TIME_MATCH_TOLERANCE:",0,Qt::CaseInsensitive);
            bool ok;
            _timeMatchTolerance = line.mid(i+21).trimmed().toDouble(&ok);
            if ( !ok ) {
                fprintf(stderr,"koviz [error]: time match tolerance in session"
                               "file %s is corrupt.\n",
                        sessionFileName.toLatin1().constData());
                exit(-1);
            }

        } else if ( line.contains("FREQUENCY:",Qt::CaseInsensitive) ) {
            int i = line.indexOf("FREQUENCY:",0,Qt::CaseInsensitive);
            bool ok;
            _frequency = line.mid(i+10).trimmed().toDouble(&ok);
            if ( !ok ) {
                fprintf(stderr,"koviz [error]: frequency spec in session"
                               "file %s is corrupt.\n",
                        sessionFileName.toLatin1().constData());
                exit(-1);
            }
        } else if ( line.contains(QRegExp("[Cc][1-7]:")) ) {
            int i = line.indexOf(QRegExp("[Cc][1-7]:"),0);
            int colorId = QString(line.at(i+1)).toInt(); // 1-7
            QString color = line.mid(i+3).trimmed();
            if ( color.startsWith("\"") ) {
                color = color.mid(1);
            }
            if ( color.endsWith("\"") ) {
                color.chop(1);
            }
            _colors.replace(colorId-1,color);
        }
    }

    file.close();
}

QStringList Session::runs() const
{
    return _runs;
}

QStringList Session::dps() const
{
    return _dps;
}

QString Session::device() const
{
    return _device;
}

QString Session::presentation() const
{
    return _presentation;
}

double Session::timeMatchTolerance() const
{
    return _timeMatchTolerance;
}

double Session::frequency() const
{
    return _frequency;
}
