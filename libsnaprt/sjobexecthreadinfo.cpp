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

    QString threadName = QString("Thread %1").arg(_threadId);

    //
    // Reading top part of S_job_execution
    // Thread _threadID
    //     Type = foo
    //     rt_cpu_number = #
    while (1) {

        QString line = in.readLine();
        if (line.isNull()) { break; }

        if ( line == threadName ) { // Thread _threadId e.g. "Thread 9"

            //
            // Type = foo
            //
            line = in.readLine();
            if (line.isNull()) { break; }
            line = line.remove(QRegExp(" *Type = *"));
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

            //
            // rt_cpu_number = #
            line = in.readLine();
            if (line.isNull()) { break; }
            line = line.remove(QRegExp("\\s*rt_cpu_number\\s*=\\s*"));
            bool ok = true;
            int cpuNum = line.toInt(&ok);
            Q_UNUSED(cpuNum);
            if ( ok || line == "unassigned" ) {
                _rtCPUNumber = line;
            } else {
                _err_stream << "snap [bad scoobies]: "
                            << "parse error.  Couldn't determine "
                            << "rtCPUNum for thread " << _threadId
                            << " in file "
                            << _sJobExecutionFileName << ". "
                            << "Token is \"" << line << "\"";
                throw std::runtime_error(
                            _err_string.toAscii().constData());
            }

            //
            // Quit reading
            //
            break;
        }
    }

    //
    // Get Main thread frame time (thread0:trick_sys.sched.advance_sim_time )
    //
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

#if 0
// Guess frequency (cycle time) of thread
//
// If Trick 13, use S_job_execution if possible
// Else Use this
//
// Guess the thread freq by:
//   thread0:
//              It's the same as sim frame time which
//              can be determined by the job:
//                       trick_sys.sched.advance_sim_time
//   threads1-N:
//              Guess that the thread freq is the same freq of the job
//              with max cycle time
double Thread::_calcFrequency()
{
    double f;

    f = -1.0e20;
    foreach ( Job* job, _jobs ) {
        if ( _threadId == 0 ) {
            if ( job->job_name() == "trick_sys.sched.advance_sim_time" ) {
                f = job->freq();
                break;
            }
        } else {
            if ( job->freq() < 0.000001 ) continue;
            if ( job->freq() > f ) {
                f = job->freq();
            }
        }
    }
    if (f == -1.0e20) {
        f = 0.0;
    }

    return f;
}
#endif
