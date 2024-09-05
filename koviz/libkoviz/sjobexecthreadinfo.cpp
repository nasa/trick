#include "sjobexecthreadinfo.h"

QString SJobExecThreadInfo::_err_string;
QTextStream SJobExecThreadInfo::_err_stream(&SJobExecThreadInfo::_err_string);

SJobExecThreadInfo::SJobExecThreadInfo(const QString &runDir, int threadId) :
    _runDir(runDir),
    _hasInfo(false),
    _threadId(threadId),
    _kind(QString()),
    _freq(0.0)
{
    _sJobExecutionFileName = _runDir + "/S_job_execution";
    _trickVersion = TrickVersion(_runDir).versionNumber();
    _calcThreadInfo();
}

void SJobExecThreadInfo::setThreadId(int threadId)
{
    _threadId = threadId;
    _calcThreadInfo();
}


void SJobExecThreadInfo::_calcThreadInfo()
{
    if ( _threadId < 0 ) {
        return;
    }

    QFile file(_sJobExecutionFileName);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text )) {
        return;
    }

    QTextStream in(&file);
    QString line = in.readLine();
    if ( line != "Thread information" ) {
        return;
    }

    _hasInfo = true;

    bool isFinishedReading = false;

    QRegExp typeRgx;
    QRegExp cpuNumRgx;
    QString threadName;
    if ( _trickVersion >= VersionNumber("13.4.dev-0") ) {
        typeRgx = QRegExp(" *process_type = *");
        cpuNumRgx = QRegExp("\\s*cpus\\s*=\\s*");
        threadName = QString("Trick::Threads");
    } else {
        typeRgx = QRegExp(" *Type = *");
        cpuNumRgx = QRegExp("\\s*rt_cpu_number\\s*=\\s*");
        threadName = QString("Thread %1").arg(_threadId);
    }

    int currThread = -1;

    //
    // Reading top part of S_job_execution
    // Thread _threadID
    while (1) {
        QString line = in.readLine();
        if (line.isNull()) {
            _err_stream << "koviz [bad scoobies]: "
                        << "parse error.  Couldn't find line "
                        << threadName
                        << " at top of file "
                        << _sJobExecutionFileName ;
            throw std::runtime_error(
                        _err_string.toLatin1().constData());
        }

        if ( _trickVersion >= VersionNumber("13.4.dev-0") ) {
            if(line.contains(threadName)) {
                ++currThread;
                if(currThread == _threadId) {
                    break;
                }
            }
        } else {
            if ( line == threadName ) { // Thread _threadId e.g. "Thread 9"
                break;
            }
        }
    }

    // Read thread info
    while (1) {

        QString line = in.readLine();
        if (line.isNull()) { break; }

        if ( line.contains(typeRgx) ) {

            //
            // Type = foo
            //
            line = line.remove(typeRgx);
            if ( line == "asynchronous" ) {
                _kind = "Asynchronous";
                _freq = 1.0e20;
            } else if ( line == "scheduled" ) {
                _kind = "Scheduled";
                _freq = 0.0;  // may be set in next code block (advance sim time)
            } else if ( line.startsWith("asynchronous must finish") ) {
                _kind = "AMF";
                line = line.remove(QRegExp(".*= "));
                bool ok = true;
                _freq = line.toDouble(&ok);
                if ( !ok ) {
                    _err_stream << "koviz [bad scoobies]: "
                                << "parse error.  Couldn't determine "
                                << "frequency for AMF thread " << _threadId
                                << " in file "
                                << _sJobExecutionFileName ;
                    throw std::runtime_error(
                                _err_string.toLatin1().constData());
                }

            }

        } else if ( line.contains(cpuNumRgx) ) {

            //
            // rt_cpu_number = # (or cpus)
            line = line.remove(cpuNumRgx);
            bool ok = true;
            int cpuNum = line.toInt(&ok);
            Q_UNUSED(cpuNum);
            if ( ok || line == "unassigned" || line == "none assigned" ) {
                _rtCPUNumber = line;
            } else {
                _err_stream << "koviz [bad scoobies]: "
                            << "parse error.  Couldn't determine "
                            << "rt_cpu_number for thread " << _threadId
                            << " in file "
                            << _sJobExecutionFileName << ". "
                            << "Token is \"" << line << "\"";
                throw std::runtime_error(
                            _err_string.toLatin1().constData());
            }

            isFinishedReading = true;
        }

        if ( isFinishedReading ) {
            //
            // Quit reading
            //
            break;
        }
    }

    //
    // Get Main thread frame time (thread0:trick_sys.sched.advance_sim_time )
    //
    // TODO: I don't think Trick reports the freq of advance_sim_time
    //       correctly in S_job_execution because the koviz curve
    //       of advance_sim_time is not (at least in RUN_orlando) the
    //       S_job_execution freq... but I'm keeping this
    if ( _threadId == 0 ) {

        while (1) {
            QString line = in.readLine();
            if (line.isNull()) { break; }

            if ( line.contains("trick_sys.sched.advance_sim_time") ) {
                QStringList fields = line.split('|');
                bool ok = true;
                _freq = fields.at(5).trimmed().toDouble(&ok);
                if ( !ok ) {
                    _err_stream << "koviz [bad scoobies]: "
                                << "parse error.  Couldn't determine "
                                << "freq for thread " << _threadId
                                << " in file "
                                << _sJobExecutionFileName << ". "
                                << "Token is \"" << line << "\"."
                                << "Search for \"advance_sim_time\"";
                    throw std::runtime_error(
                                _err_string.toLatin1().constData());
                }
                break;
            }
        }
        if ( _freq == 0.0 ) {
            _err_stream << "koviz [bad scoobies]: "
                        << "parse error.  Couldn't determine "
                        << "freq for thread " << _threadId
                        << " in file "
                        << _sJobExecutionFileName << ". "
                        << "Could not find \"advance_sim_time\"";
            throw std::runtime_error(
                        _err_string.toLatin1().constData());
        }
    }

    file.close();
}
