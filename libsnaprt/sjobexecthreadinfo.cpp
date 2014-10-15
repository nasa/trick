#include "sjobexecthreadinfo.h"

#include <QDebug>


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
    _trickVersion = _calcTrickVersion();
    _calcThreadInfo();
}

void SJobExecThreadInfo::setThreadId(int threadId)
{
    _threadId = threadId;
    _calcThreadInfo();
}

// Uses S_run_summary if present
// If there is no S_run_summary, hack on s_job_exec file
VersionNumber SJobExecThreadInfo::_calcTrickVersion() const
{
    VersionNumber v;

    QString sRunSummary = _runDir + "/S_run_summary";
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
        QFile sjob(_sJobExecutionFileName);

        if (sjob.open(QIODevice::ReadOnly | QIODevice::Text )) {
            QTextStream in(&file);
            QString line = in.readLine();
            if ( line == "Thread information" ) {
                line = in.readLine(); // hack
                if ( line.startsWith("Trick::Threads") ) {
                    v = VersionNumber("13.4.0-0");
                } else {
                    v = VersionNumber("13.0.0-0");
                }
            }
        }
    }

    return v;
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
        threadName = QString("Trick::Threads (Child_%1)").arg(_threadId);
    } else {
        typeRgx = QRegExp(" *Type = *");
        cpuNumRgx = QRegExp("\\s*rt_cpu_number\\s*=\\s*");
        threadName = QString("Thread %1").arg(_threadId);
    }

    //
    // Reading top part of S_job_execution
    // Thread _threadID
    while (1) {
        QString line = in.readLine();
        if (line.isNull()) {
            _err_stream << "snap [bad scoobies]: "
                        << "parse error.  Couldn't find line "
                        << threadName
                        << " at top of file "
                        << _sJobExecutionFileName ;
            throw std::runtime_error(
                        _err_string.toAscii().constData());
        }

        if ( line == threadName ) { // Thread _threadId e.g. "Thread 9"
            break;
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
                    _err_stream << "snap [bad scoobies]: "
                                << "parse error.  Couldn't determine "
                                << "frequency for AMF thread " << _threadId
                                << " in file "
                                << _sJobExecutionFileName ;
                    throw std::runtime_error(
                                _err_string.toAscii().constData());
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
                _err_stream << "snap [bad scoobies]: "
                            << "parse error.  Couldn't determine "
                            << "rt_cpu_number for thread " << _threadId
                            << " in file "
                            << _sJobExecutionFileName << ". "
                            << "Token is \"" << line << "\"";
                throw std::runtime_error(
                            _err_string.toAscii().constData());
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
    //       correctly in S_job_execution because the snapq curve
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
                    _err_stream << "snap [bad scoobies]: "
                                << "parse error.  Couldn't determine "
                                << "freq for thread " << _threadId
                                << " in file "
                                << _sJobExecutionFileName << ". "
                                << "Token is \"" << line << "\"."
                                << "Search for \"advance_sim_time\"";
                    throw std::runtime_error(
                                _err_string.toAscii().constData());
                }
                break;
            }
        }
        if ( _freq == 0.0 ) {
            _err_stream << "snap [bad scoobies]: "
                        << "parse error.  Couldn't determine "
                        << "freq for thread " << _threadId
                        << " in file "
                        << _sJobExecutionFileName << ". "
                        << "Could not find \"advance_sim_time\"";
            throw std::runtime_error(
                        _err_string.toAscii().constData());
        }
    }

    file.close();
}
