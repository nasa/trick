#include "thread.h"

#include <cmath>
#include <QtCore/qmath.h>

QString Thread::_err_string;
QTextStream Thread::_err_stream(&Thread::_err_string);

static bool intLessThan(int a, int b)
{
    return a < b;
}

Thread::Thread(const QString &runDir, double startTime, double stopTime) :
    _runDir(runDir),_startTime(startTime),_stopTime(stopTime),
    _threadId(-1), _sJobExecThreadInfo(runDir),
    _avg_runtime(0),_avg_load(0), _tidx_max_runtime(0),
    _max_runtime(0), _max_load(0),_stdev(0),_freq(0.0),
    _num_overruns(0),_runtimeCurve(0),_frameModel(0),_frameModelIsRealTime(false)

{
}

Thread::~Thread()
{
    if ( _runtimeCurve ) {
        delete _runtimeCurve;
    }
}

void Thread::addJob(Job* job)
{
    if ( !_jobs.isEmpty() && _threadId != job->thread_id() ) {
        _err_stream << "snap [bad scoobies]: Thread::addJob() called with "
                    << "job with threadId that doesn't match other jobs. "
                    << "Conflicing jobs are:\n    "  << _jobs.at(0)->job_name()
                    << "\nand\n    " << job->job_name() ;
        throw std::runtime_error(_err_string.toAscii().constData());
    }

    if ( _jobs.isEmpty() ) {
        _threadId = job->thread_id();
        _sJobExecThreadInfo.setThreadId(_threadId);
    }

    _jobs.append(job);
}

void Thread::_do_stats()
{
    if ( _jobs.size() == 0 ) {
        return;
    }

    qSort(_jobs.begin(),_jobs.end(),jobAvgTimeGreaterThan);

    _freq = _calcFrequency();
    if ( _freq == 0 ) {
        return; // Nothing to calculate, so return
    }

    if ( _threadId == 0 && _freq == 0.0 ) {
        QString msg;
        msg += "snap [error]: couldn't find job";
        msg += " trick_sys.sched.advance_sim_time.";
        msg += " Cannot determine thread0's frequency.";
        throw std::runtime_error(msg.toAscii().constData());
    }


    //
    // If thread0, use log_frame.trk
    //
    if ( _threadId == 0 ) {
        _frameModelSet(); // _frameModel & _isFrameModelRealTime set
        _frameModelCalcIsRealTime();
    }

    int frameCnt = _calcNumFrames();

    //
    // Create table with frameCnt number of rows
    //
    QString tableName = QString("Thread %1 Runtime").arg(_threadId);
    _runtimeCurve = new SnapTable(tableName);
    _runtimeCurve->insertColumns(0,2);
    _runtimeCurve->setHeaderData(0,Qt::Horizontal,QString("sys.exec.out.time"));
    _runtimeCurve->setHeaderData(1,Qt::Horizontal,QString("ThreadRunTime"));
    _runtimeCurve->insertRows(0,frameCnt);

    double sum_time = 0.0;
    double sum_squares = 0 ;
    int rowCount = 0 ;
    if ( _threadId == 0 && _frameModelIsRealTime ) {

        TrickCurveModel* timeToSyncWithAMFChildrenCurve = 0;
        foreach ( Job* job, _jobs ) {
            if ( job->job_name().contains("advance_sim_time") ) {
                timeToSyncWithAMFChildrenCurve = job->curve();
                break;
            }
        }
        if ( timeToSyncWithAMFChildrenCurve == 0 ) {
            _err_stream << "snap [bad scoobies]: cannot find advance_sim_time "
                        <<   " parameter for thread0 frame calculation."
                        << "  Trick may have changed the name.";
            throw std::runtime_error(_err_string.toAscii().constData());
        }
        TrickModelIterator iamf = timeToSyncWithAMFChildrenCurve->begin();

        TrickModelIterator it = _frameModel->begin(0,
                                                   _frameModelRunTimeCol,
                                                   _frameModelOverrunTimeCol);
        const TrickModelIterator e = _frameModel->end(0,
                                                   _frameModelRunTimeCol,
                                                   _frameModelOverrunTimeCol);
        _num_overruns = 0;
        _max_runtime = 0.0;
        int tidx = 0 ;
        while (it != e) {

            // If overrun time is above 0, tally an overrun.
            double ov = it.y()/1000000.0;
            if ( ov > 0.0 ) _num_overruns++;

            // Snap calculates frame time (ft) by excluding executive
            // time waiting to sync with wall clock but includes
            // the sync with amf children
            int amfIdx  = timeToSyncWithAMFChildrenCurve->indexAtTime(it.t());
            double timeToSyncWithAMFChildren = iamf[amfIdx].y()/1000000.0;
            double frameSchedTime = it.x()/1000000.0;
            double ft = frameSchedTime + timeToSyncWithAMFChildren;


            if ( ft < 0 ) ft = 0.0;
            if ( ft > _max_runtime ) {
                _max_runtime = ft;
                _tidx_max_runtime = tidx;
            }
            _frameidx2runtime[tidx] = ft;
            _jobtimestamp2frameidx[it.t()] = tidx;
            QModelIndex timeIdx = _runtimeCurve->index(rowCount,0);
            QModelIndex valIdx = _runtimeCurve->index(rowCount,1);
            ++rowCount;
            _runtimeCurve->setData(timeIdx,it.t());
            _runtimeCurve->setData(valIdx,ft);
            sum_time += ft*1000000.0;
            sum_squares += ft*ft;

            ++it;
            ++tidx;
        }

        QModelIndex idx0 = _runtimeCurve->index(0,0);
        _startTime = _runtimeCurve->data(idx0).toDouble();
        int rc = _runtimeCurve->rowCount();
        QModelIndex idxLast = _runtimeCurve->index(rc-1,0);
        _stopTime = _runtimeCurve->data(idxLast).toDouble();

    } else {
        //
        // Calc frame runtimes (sum of jobs runtimes per frame), num overruns etc.
        //
        Job* job0 = _jobs.at(0);
        TrickCurveModel* curve = job0->curve();
        TrickModelIterator it = curve->begin();
        const TrickModelIterator e = curve->end();
        double frame_time = 0.0;
        _max_runtime = 0.0;
        int frameidx = 0 ;
        int tidx = 0 ;
        TrickModelIterator it2;

        it = curve->begin();
        _startTime = it.t();
        double tnext = it.t() + _freq;
        double epsilon = 1.0e-6;

        _num_overruns = 0;
        while (it != e) {

            double frameTimeStamp = it.t();
            QList<double> jobTimeStampsAcrossFrame;
            while ( it != e && it.t()+epsilon < tnext ) {

                jobTimeStampsAcrossFrame.append(it.t());

                foreach ( Job* job, _jobs ) {

                    if ( job->job_name().startsWith("frame_userjobs_C") ) {
                        // For Trick 13
                        // Do not use child frame scheduling time for frame
                        // time sum.  Snap reports the sum of the userjobs,
                        // not the frame scheduling time since the frame
                        // scheduling time includes executive overhead
                        // (e.g. the frame logging itself).
                        continue;
                    }

                    it2 = job->curve()->begin();
                    double ft = it2[tidx].x();
                    if ( ft < 0 ) {
                        ft = 0.0;
                    }
                    frame_time += ft;
                }
                ++tidx;
                ++it;

                if ( _freq < epsilon ) {
                    break;
                }
            }

            double ft = frame_time/1000000.0;

            if ( ft > _freq ) {
                _num_overruns++;
            }

            if ( ft > _max_runtime ) {
                _max_runtime = ft;
                _tidx_max_runtime = frameidx;
            }

            _frameidx2runtime[frameidx] = ft;
            foreach ( double t, jobTimeStampsAcrossFrame ) {
                _jobtimestamp2frameidx[t] = frameidx;
            }

            QModelIndex timeIdx = _runtimeCurve->index(rowCount,0);
            QModelIndex valIdx = _runtimeCurve->index(rowCount,1);
            ++rowCount;
            _runtimeCurve->setData(timeIdx,frameTimeStamp);
            _runtimeCurve->setData(valIdx,ft);
            sum_time += frame_time;
            sum_squares += ft*ft;
            frame_time = 0.0;
            frameidx++;
            tnext += _freq;
        }

        if ( rowCount > 0 ) {
            _stopTime = it[rowCount-1].t();
        }
    }

    double ss = sum_squares;
    double s = sum_time;
    double n = (double)rowCount;

    _avg_runtime = (s/n)/1000000.0;
    _stdev       = qSqrt(ss/n - (s*s/(n*n))*(1.0e-12));

    if ( _freq > 0.0000001 ) {
        _avg_load = 100.0*_avg_runtime/_freq;
        _max_load = 100.0*_max_runtime/_freq;
    }
}

//
// Guess the thread freq by:
//
//   If thread0:
//              freq = freq of job trick_sys.sched.advance_sim_time
//   If threads1-N:
//              If Trick 13:
//                    Use S_job_execution
//              Else
//                    Guess that the thread freq is the same freq of the job
//                    with max cycle time
//
double Thread::_calcFrequency()
{
    double freq;

    if ( _sJobExecThreadInfo.hasInfo() && _threadId > 0 ) {
        _freq = _sJobExecThreadInfo.frequency();
        return _freq;
    }

    freq = -1.0e20;
    foreach ( Job* job, _jobs ) {
        if ( _threadId == 0 ) {
            if ( job->job_name() == "trick_sys.sched.advance_sim_time" ) {
                freq = job->freq();
                break;
            }
        } else {
            if ( job->freq() < 0.000001 ) continue;
            if ( job->freq() > freq ) {
                freq = job->freq();
            }
        }
    }
    if (freq == -1.0e20) {
        freq = 0.0;
    }

    return freq;
}

void Thread::_frameModelSet()
{
    _frameModel = 0;

    QString fileNameLogFrame = _runDir + "/log_frame.trk";
    if ( !QFileInfo(fileNameLogFrame).exists() ) {
        fileNameLogFrame = _runDir + "/log_snap_frame.trk";
        if ( !QFileInfo(fileNameLogFrame).exists() ) {
            _err_stream << "snap [error]: cannot find log_frame.trk or "
                        << "log_snap_frame.trk files in directory "
                        << _runDir;
            throw std::invalid_argument(_err_string.toAscii().constData());
        }
    }
    try {
        QString trk(fileNameLogFrame);
        _frameModel = new TrickModel(trk,trk,_startTime,_stopTime);
    }
    catch (std::range_error &e) {
        _err_stream << e.what() << "\n\n";
        _err_stream << "snap [error]: -start or -stop opts have bad vals\n";
        throw std::range_error(_err_string.toAscii().constData());
    }

    int nFrames = _frameModel->rowCount();
    if ( nFrames == 0 ) {
        _err_stream << "snap [error]: file \"" << _frameModel->tableName()
                    << "\" has no points";
        throw std::invalid_argument(_err_string.toAscii().constData());
    }

    int frameTimeCol = -1;
    int overrunTimeCol = -1;
    int nParams = _frameModel->columnCount();
    for ( int i = 0 ; i < nParams; ++i ) {

        QString param = _frameModel->headerData(i,Qt::Horizontal).toString();

        if ( param ==  Frame::frame_time_name ) {
            frameTimeCol = i;
        } else if ( param == Frame::overrun_time_name ) {
            overrunTimeCol = i;
        }
        if ( frameTimeCol > 0 && overrunTimeCol > 0 ) {
            break;
        }
    }
    if ( frameTimeCol < 0 || overrunTimeCol < 0 ) {

        QString param  = ( frameTimeCol  < 0 ) ?
                    Frame::frame_time_name : Frame::overrun_time_name ;
        // Shouldn't happen unless trick renames that param
        _err_stream << "snap [error]: Couldn't find parameter "
                        << param
                        << " in file \""
                        << _frameModel->tableName()
                        << "\"";
            throw std::invalid_argument(_err_string.toAscii().constData());
    }

    _frameModelRunTimeCol = frameTimeCol;
    _frameModelOverrunTimeCol = overrunTimeCol;


}

int Thread::_calcNumFrames()
{
    int frameCnt = 0;

    if ( _threadId == 0 && _frameModelIsRealTime == true ) {
        frameCnt = _frameModel->rowCount();
    } else {
        Job* job0 = _jobs.at(0);
        TrickCurveModel* curve = job0->curve();
        TrickModelIterator it = curve->begin();
        const TrickModelIterator e = curve->end();
        if ( _freq < 0.000001 ) {
            frameCnt = curve->rowCount();
        } else {
            double tnext = it.t() + _freq;
            frameCnt = 1;
            while (it != e) {
                if (it.t()+1.0e-6 > tnext) {
                    tnext += _freq;
                    ++frameCnt ;
                }
                ++it;
            }
        }
    }

    return frameCnt;
}

void Thread::_frameModelCalcIsRealTime()
{
    _frameModelIsRealTime = false;

    if ( _frameModel == 0 ) return;

    TrickModelIterator it = _frameModel->begin(0,
                                               _frameModelRunTimeCol,
                                               _frameModelOverrunTimeCol);
    const TrickModelIterator e = _frameModel->end(0,
                                               _frameModelRunTimeCol,
                                               _frameModelOverrunTimeCol);
    _num_overruns = 0;
    while (it != e) {
        double ft = it.x();
        if ( ft > 0 ) {
            _frameModelIsRealTime = true;
            break;
        }
        ++it;
    }
}

double Thread::runtime(int tidx) const
{
    double rt = -1.0;

    for ( int ii = tidx; ii >= 0 ; --ii) {
        if ( _frameidx2runtime.contains(ii) ) {
            rt = _frameidx2runtime[ii];
            break;
        }
    }

    return rt;
}

double Thread::runtime(double timestamp) const
{
    int frameidx = _jobtimestamp2frameidx.value(timestamp);
    double rt = _frameidx2runtime.value(frameidx);
    return rt;
}

int Thread::numFrames() const
{
    return _frameidx2runtime.keys().size();
}

double Thread::avgJobRuntime(Job *job) const
{
    double rt = 0.0;
    int nFrames = numFrames();
    if ( nFrames > 0 ) {
        rt = job->avg_runtime()*job->npoints()/nFrames;
    }
    return rt;
}

//
// Returns a percent average load of job on thread
// A load of 0.0 can mean it's negligible
// If there's only one job on a thread, load is 100% if there's any load
//
double Thread::avgJobLoad(Job *job) const
{
    double load = 0.0;

    if ( _avg_runtime > 0.000001 ) {
        if ( _jobs.length() == 1 ) {
            // Fix round off error.
            // If the job has an average above zero
            // and the thread has a single job
            // this job took 100%, so force it to 100.0
            load = 100.0;
        } else {
            load = 100.0*avgJobRuntime(job)/_avg_runtime;
        }
    }

    return load;

}

Threads::Threads(const QString &runDir, const QList<Job*>& jobs,
                 double startTime, double stopTime) :
    _jobs(jobs),_startTime(startTime),_stopTime(stopTime)
{
    foreach ( Job* job, _jobs ) {

        int tid = job->thread_id();
        if ( ! _ids.contains(tid) ) {
            _ids.append(tid);
            Thread* thread = new Thread(runDir,startTime,stopTime);
            _threads.insert(tid,thread);
        }

        Thread* thread = _threads.value(tid);
        thread->addJob(job);
    }

    qSort(_ids.begin(),_ids.end(),intLessThan);

    bool isRealTime = false;
    foreach ( Thread* thread, _threads.values() ) {
        thread->_do_stats();
        if ( thread->threadId() == 0 && thread->isRealTime()) {
            isRealTime = true;
        }
    }

    // If thread0 is realtime, set the other threads to realtime too
    if ( isRealTime ) {
        foreach ( Thread* thread, _threads.values() ) {
            thread->_frameModelIsRealTime = true;
        }
    }

}

Threads::~Threads()
{
    foreach ( Thread* thread, _threads.values() ) {
        delete thread;
    }
}
