/*
 * Snap! Crackle! Pop!
 */

#include <stdio.h>
#include <ctype.h>
#include <cmath>
#include <QDir>
#include <QFile>
#include <stdexcept>

#include "snap.h"
#include "versionnumber.h"

bool topThreadGreaterThan(const QPair<double,Thread*>& a,
                         const QPair<double,Thread*>& b)
{
    if ( a.second->frequency() > 0.000001 && b.second->frequency() > 0.000001 ) {
        return (a.first/a.second->frequency()) > (b.first/b.second->frequency());
    } else {
        return (a.first > b.first);
    }
}

QString Snap::_err_string;
QTextStream Snap::_err_stream(&Snap::_err_string);

Snap::Snap(const QString &irundir,
           const QStringList &timeNames,
           bool is_delay_load) :
    _rundir(irundir), _timeNames(timeNames),
    _is_realtime(false),
    _curr_sort_method(NoSort), _trickJobModel(0),_modelFrame(0),
    _num_overruns(0), _numFrames(0), _frame_avg(0.0),_frame_stddev(0),
    _threads(0),_simobjects(0),_progress(0)
{

    _create_table_summary();
    _create_table_spikes();
    _create_table_thread_summary();
    _create_table_top_jobs();
    _create_table_sim_objects();

    if ( ! is_delay_load ) {
        _load();
    }
}

void Snap::load()
{
    LoadThread t(this,15000,0,100); // TODO: estimate loading time instead
    t.start();                      //      of hard-coded 15sec
    t.wait();
    t.quit();
    emit finishedLoading();
}

void Snap::_load()
{
    _process_models();        // _jobs list created

    qSort(_jobs.begin(), _jobs.end(), jobAvgTimeGreaterThan);

    _curr_sort_method = SortByJobAvgTime;

    _threads = new Threads(_rundir,_jobs,_timeNames);

    _thread0 = 0 ;
    foreach ( Thread* thread, threads()->hash()->values() ) {
        if ( thread->threadId() == 0 ) {
            _thread0 = thread;
            break;
        }
    }
    if ( _thread0 == 0 ) {
        _err_stream << "koviz [error]: no main thread with id==0 found!!!";
        throw std::runtime_error(_err_string.toLatin1().constData());
    }

    _is_realtime  = _thread0->isRealTime();
    _num_overruns = _thread0->numOverruns();
    _numFrames    = _thread0->numFrames();
    _frame_avg    = _thread0->avgRunTime();
    _frame_stddev = _thread0->stdDeviation();
    _frames = _process_frames();

    _simobjects = new SimObjects(_jobs,frame_rate());

    _set_data_table_summary();
    _set_data_table_spikes();
    _set_data_table_thread_summary();
    _set_data_table_top_jobs();
    _set_data_table_sim_objects();
}

Snap::~Snap()
{
    foreach ( Job* job, _jobs ) {
        delete job;
    }
    foreach ( CurveModel* curve, _curves ) {
        delete curve;
    }

    if (_trickJobModel ) delete _trickJobModel;
    foreach ( DataModel* m, _userJobModels ) {
        delete m;
    }
    if (_modelFrame ) delete _modelFrame;

    foreach ( SnapTable* table, tables ) {
        delete table;
    }

    delete _threads;
    delete _simobjects;
}


void Snap::_create_table_summary()
{
    _table_summary = new SnapTable("Summary");
    tables.append(_table_summary);

    SnapTable* table = _table_summary;
    table->insertColumns(0,1);

    int r = 0 ;
    table->insertRows(r,1);
    table->setHeaderData(r,Qt::Vertical,QVariant("Run directory"));
    table->setData(table->index(r,0),QVariant(rundir()));
    r++;

    table->insertRows(r,1);
    table->setHeaderData(r,Qt::Vertical,QVariant("Start time"));
    table->setHeaderData(r,Qt::Vertical,QVariant("%.3lf"),Role::Format);
    r++;

    table->insertRows(r,1);
    table->setHeaderData(r,Qt::Vertical,QVariant("Stop time"));
    table->setHeaderData(r,Qt::Vertical,QVariant("%.3lf"),Role::Format);
    r++;

    table->insertRows(r,1);
    table->setHeaderData(r,Qt::Vertical,QVariant("Num jobs"));
    r++;

    table->insertRows(r,1);
    table->setHeaderData(r,Qt::Vertical,QVariant("Num frames"));
    r++;

    table->insertRows(r,1);
    table->setHeaderData(r,Qt::Vertical,QVariant("Num overruns"));
    r++;

    table->insertRows(r,1);
    table->setHeaderData(r,Qt::Vertical,QVariant("Percentage overruns"));
    table->setHeaderData(r,Qt::Vertical,QVariant("%.0lf%%"),Role::Format);
    r++;

    table->insertRows(r,1);
    table->setHeaderData(r,Qt::Vertical,QVariant("Frame rate(s)"));
    r++;

    table->insertRows(r,1);
    table->setHeaderData(r,Qt::Vertical,QVariant("Frame avg"));
    table->setHeaderData(r,Qt::Vertical,QVariant("%.6lf"),Role::Format);
    r++;

    table->insertRows(r,1);
    table->setHeaderData(r,Qt::Vertical,QVariant("Frame stddev"));
    table->setHeaderData(r,Qt::Vertical,QVariant("%.6lf"),Role::Format);
    r++;

    table->insertRows(r,1);
    table->setHeaderData(r,Qt::Vertical,QVariant("Num threads"));
    r++;

    table->insertRows(r,1);
    table->setHeaderData(r,Qt::Vertical,QVariant("Thread list"));
    r++;
}

void Snap::_set_data_table_summary()
{
    int r = 0 ;

    SnapTable* table = _table_summary;

    table->setData(table->index(r,0),QVariant(rundir())); r++;
    table->setData(table->index(r,0),QVariant(num_jobs())); r++;
    table->setData(table->index(r,0),QVariant(num_frames())); r++;
    table->setData(table->index(r,0),QVariant(num_overruns())); r++;
    table->setData(table->index(r,0),QVariant(percent_overruns())); r++;
    table->setData(table->index(r,0),QVariant(frame_rate())); r++;
    table->setData(table->index(r,0),QVariant(frame_avg())); r++;
    table->setData(table->index(r,0),QVariant(frame_stddev())); r++;
    table->setData(table->index(r,0),QVariant(num_threads())); r++;
    table->setData(table->index(r,0),QVariant(thread_listing())); r++;
}

//
// Top Ten Spikes
//
void Snap::_create_table_spikes()
{
    _table_spikes = new SnapTable("Spikes");
    tables.append(_table_spikes);
    SnapTable* table = _table_spikes;

    table->insertColumns(0,3);
    table->setHeaderData(0,Qt::Horizontal,QVariant("Time"));
    table->setHeaderData(1,Qt::Horizontal,QVariant("Frame"));
    table->setHeaderData(1,Qt::Horizontal,QVariant("%.6lf"),Role::Format);
    table->setHeaderData(2,Qt::Horizontal,QVariant("Job Load Index"));
    table->setHeaderData(2,Qt::Horizontal,QVariant("%.0lf"),Role::Format);
}

void Snap::_set_data_table_spikes()
{
    SnapTable* table = _table_spikes;

    int max_cnt = 10;
    int cnt = 0 ;
    foreach ( Frame frame, _frames ) {
        if ( ++cnt > max_cnt ) break;
        double tt = frame.timestamp();
        double ft = frame.frame_time();
        double jl = frame.jobloadindex();
        int row = table->rowCount();
        table->insertRows(row,1);
        table->setData(table->index(row,0),QVariant(tt));
        table->setData(table->index(row,1),QVariant(ft));
        table->setData(table->index(row,2),QVariant(jl));
    }
}

//
// Thread Summary
//
void Snap::_create_table_thread_summary()
{
    _table_thread_summary = new SnapTable("Threads");
    tables.append(_table_thread_summary);
    SnapTable* table = _table_thread_summary;

    table->insertColumns(0,8);
    table->setHeaderData(0,Qt::Horizontal,QVariant("ThreadID"));
    table->setHeaderData(1,Qt::Horizontal,QVariant("NumJobs"));
    table->setHeaderData(2,Qt::Horizontal,QVariant("Freq"));
    table->setHeaderData(2,Qt::Horizontal,QVariant("%.6lf"),Role::Format);
    table->setHeaderData(3,Qt::Horizontal,QVariant("NumOverruns"));
    table->setHeaderData(4,Qt::Horizontal,QVariant("ThreadAvg"));
    table->setHeaderData(4,Qt::Horizontal,QVariant("%.6lf"),Role::Format);
    table->setHeaderData(5,Qt::Horizontal,QVariant("AvgLoad%"));
    table->setHeaderData(5,Qt::Horizontal,QVariant("%.0lf"),Role::Format);
    table->setHeaderData(6,Qt::Horizontal,QVariant("ThreadMax"));
    table->setHeaderData(6,Qt::Horizontal,QVariant("%.6lf"),Role::Format);
    table->setHeaderData(7,Qt::Horizontal,QVariant("MaxLoad%"));
    table->setHeaderData(7,Qt::Horizontal,QVariant("%.0lf"),Role::Format);
}

void Snap::_set_data_table_thread_summary()
{
    SnapTable* table = _table_thread_summary;

    table->insertRows(0,num_threads());
    int row = 0 ;
    foreach ( Thread* thread, _threads->hash()->values() ) {

        table->setData(table->index(row,0),QVariant(thread->threadId()));
        table->setData(table->index(row,1),QVariant(thread->numJobs()));
        table->setData(table->index(row,2),QVariant(thread->frequency()));
        table->setData(table->index(row,3),QVariant(thread->numOverruns()));
        table->setData(table->index(row,4),QVariant(thread->avgRunTime()));
        table->setData(table->index(row,5),QVariant(thread->avgLoad()));
        table->setData(table->index(row,6),QVariant(thread->maxRunTime()));
        table->setData(table->index(row,7),QVariant(thread->maxLoad()));

        row++;
    }
}

//
// Job Time Avgs
//
void Snap::_create_table_top_jobs()
{
    _table_top_jobs  = new SnapTable("Top Jobs");
    SnapTable* table = _table_top_jobs;
    tables.append(_table_top_jobs);

    table->insertColumns(0,6);
    table->setHeaderData(0,Qt::Horizontal,QVariant("JobAvg"));
    table->setHeaderData(0,Qt::Horizontal,QVariant("%.6lf"),Role::Format);
    table->setHeaderData(1,Qt::Horizontal,QVariant("Thread"));
    table->setHeaderData(2,Qt::Horizontal,QVariant("ThreadAvgTime"));
    table->setHeaderData(2,Qt::Horizontal,QVariant("%.6lf"),Role::Format);
    table->setHeaderData(3,Qt::Horizontal,QVariant("JobFreq"));
    table->setHeaderData(3,Qt::Horizontal,QVariant("%.6lf"),Role::Format);
    table->setHeaderData(4,Qt::Horizontal,QVariant("JobName"));
    int align = (Qt::AlignLeft | Qt::AlignVCenter);
    table->setHeaderData(4,Qt::Horizontal,QVariant(align),Qt::TextAlignmentRole);
    table->setHeaderData(5,Qt::Horizontal,QVariant("JobId"));
    table->setHeaderData(5,Qt::Horizontal,QVariant(align),Qt::TextAlignmentRole);

}

void Snap::_set_data_table_top_jobs()
{
    SnapTable* table = _table_top_jobs;
    QList<Job*>* jobs = this->jobs(Snap::SortByJobAvgTime);
    int max_cnt = 50;
    int cnt = 0 ;
    foreach ( Job* job, *jobs ) {
        if ( ++cnt > max_cnt ) break;
        int row = table->rowCount();
        table->insertRows(row,1);
        table->setData(table->index(row,0),QVariant(job->avg_runtime()));
        table->setData(table->index(row,1),QVariant(job->thread_id()));
        table->setData(table->index(row,2),
                       QVariant(threads()->hash()->
                                value(job->thread_id())->avgRunTime()));
        table->setData(table->index(row,3),QVariant(job->freq()));
        table->setData(table->index(row,4),
                       QVariant(job->job_name().toLatin1().constData()));
        table->setData(table->index(row,5),
                       QVariant(job->job_id().toLatin1().constData()));
    }
}

void Snap::_create_table_sim_objects()
{
    _table_sim_objects = new SnapTable("Sim Objects");
    SnapTable* table = _table_sim_objects;
    tables.append(_table_sim_objects);

    table->insertColumns(0,6);
    table->setHeaderData(0,Qt::Horizontal,QVariant("SimObject"));
    table->setHeaderData(1,Qt::Horizontal,QVariant("AvgTime"));
    table->setHeaderData(1,Qt::Horizontal,QVariant("%.6lf"),Role::Format);
    table->setHeaderData(2,Qt::Horizontal,QVariant("NumJobs"));
    table->setHeaderData(3,Qt::Horizontal,QVariant("MinTime"));
    table->setHeaderData(4,Qt::Horizontal,QVariant("MaxTime"));
    table->setHeaderData(5,Qt::Horizontal,QVariant("Variance"));
}

void Snap::_set_data_table_sim_objects()
{
    SnapTable* table = _table_sim_objects;

    QList<SimObject> simobjects = _simobjects->list();
    int nrows = simobjects.size();
    if ( simobjects.size() > 100 ) {
        nrows = simobjects.size()/10;
    }

    int row = 0 ;
    foreach ( SimObject sobject, simobjects ) {
        if ( row > nrows ) break;
        table->insertRows(row,1);
        QVariant sname = sobject.name();
        QVariant njobs = sobject.jobs().size();
        table->setData(table->index(row,0),sname);
        table->setData(table->index(row,1),QVariant(sobject.avg_runtime()));
        table->setData(table->index(row,2),njobs);
        table->setData(table->index(row,3),QVariant(sobject.min_runtime()));
        table->setData(table->index(row,4),QVariant(sobject.max_runtime()));
        table->setData(table->index(row,5),QVariant(sobject.stddev_runtime()));
        row++;
    }
}

QList<Job *>* Snap::jobs(SortBy sort_method)
{
    if ( _curr_sort_method != sort_method ) {
        if ( sort_method == SortByJobAvgTime ) {
            qSort(_jobs.begin(), _jobs.end(), jobAvgTimeGreaterThan);
            _curr_sort_method = SortByJobAvgTime;
        } else if ( sort_method == SortByJobMaxTime ) {
            qSort(_jobs.begin(), _jobs.end(), jobMaxTimeGreaterThan);
            _curr_sort_method = SortByJobMaxTime;
        }
    }

    return &_jobs;
}

DataModel *Snap::_createModel( const QString &trk)
{
    DataModel* model;

    QFile file(trk);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        _err_stream << "koviz [error]: couldn't read/open file: " << trk << "\n";
        throw std::invalid_argument(_err_string.toLatin1().constData());
    } else {
        file.close();
    }

    //
    // If trk file is less than 50 bytes, it can't be legit
    //
    if ( file.size() < 50 ) {
        _err_stream << "koviz [error]: "
                    << "suspicious filesize of "
                    << file.size()
                    << " for file \""
                    << trk
                    << "\""
                    << "  - bailing like Rob Bailey!!!";
        throw std::invalid_argument(_err_string.toLatin1().constData());
    }

    try {
        model = DataModel::createDataModel(_timeNames,trk,trk);
    }
    catch (std::range_error &e) {
        _err_stream << e.what() << "\n\n";
        _err_stream << "koviz [error]: Snap::_createModel()\n";
        throw std::range_error(_err_string.toLatin1().constData());
    }


    return model;
}

void Snap::_process_models()
{
    _setLogFileNames();
    _trickJobModel = _createModel(_fileNameTrickJobs);

    // Trick 13 splits userjobs into separate files
    foreach ( QString userJob, _fileNamesUserJobs ) {
        DataModel* userJobModel =  _createModel(userJob);
        if ( userJobModel->rowCount() > 0 ) {
            // log*CX*.trk has no timing data (this happens in Trick 13)
            _userJobModels.append(userJobModel);
        } else {
            delete userJobModel;
        }
    }


    _process_jobs(_trickJobModel);
    foreach ( DataModel* userJobModel, _userJobModels ) {
        _process_jobs(userJobModel);
    }

}

double Snap::frame_rate() const
{
    if ( !_thread0 ) return 0;
    double rate = _thread0->frequency();
    return rate;
}

QString Snap::thread_listing() const
{
    QString listing;

    QString str;

    int nthreads = _threads->hash()->size();
    int ii = 0 ;
    int ltid = 0;
    foreach ( int tid, _threads->hash()->keys() ) {
        if ( ii != 0 && ii%16 == 0 ) {
            listing += QString("\n");
            listing += str.sprintf("%20s   "," ");
        }
        if ( ii > 0 && tid != ltid+1) {
            listing += QString("MISSING,");
        }
        listing += str.sprintf("%d", tid);
        if ( ii < nthreads-1 ) {
            listing += QString(",");
        }
        ltid = tid;
        ++ii;
    }

    return listing;
}

SnapTable* Snap::jobTableAtTime(double time)
{
    QString title = QString("Top Jobs At Time %1").arg(time);
    SnapTable* table = new SnapTable(title);

    int c = 0 ;
    table->insertColumns(c,6);
    table->setHeaderData(c,Qt::Horizontal,QVariant("Job Id")); c++; // c++ :)
    table->setHeaderData(c,Qt::Horizontal,QVariant("Job Name")); c++;
    table->setHeaderData(c,Qt::Horizontal,QVariant("Job Freq")); c++;
    table->setHeaderData(c,Qt::Horizontal,QVariant("Job Time")); c++;
    table->setHeaderData(c,Qt::Horizontal,QVariant("Thread Id")); c++;
    table->setHeaderData(c,Qt::Horizontal,QVariant("Thread Time")); c++;

    Frame frame(0,0,0,0); //bogus
    int tidx = _thread0->jobAtIndex(0)->curve()->indexAtTime(time);

    for ( int ii = 0; ii < _frames.size(); ++ii) {
        frame = _frames.at(ii);
        if ( frame.timeidx() == tidx ) {
            break;
        }
    }

    double ft = _thread0->runtime(time);

    int r = 0 ;
    int cnt = 0 ;
    double total = 0 ;
    for ( int ii = 0; ii < frame.topjobs()->length(); ++ii) {

        QPair<double,Job*> topjob = frame.topjobs()->at(ii);

        double rt =  topjob.first;
        Job* job = topjob.second;

        double delta = 1.0e-3;
        if ( rt < delta ) {
            break;
        }

        table->insertRows(r,1);
        table->setData(table->index(r,0),QVariant(job->job_id()));
        table->setData(table->index(r,1),QVariant(job->job_name()));
        table->setData(table->index(r,2),QVariant(job->freq()));
        table->setData(table->index(r,3),QVariant(rt));
        table->setData(table->index(r,4),QVariant(job->thread_id()));
        table->setData(table->index(r,5),
                       QVariant(threads()->hash()->value(job->thread_id())->
                                runtime(time)));
        r++;

        // limit printout of jobs
        if (job->job_name() != QString("trick_sys.sched.advance_sim_time")) {
            total += rt;
        }
        if ( total > 0.75*ft && ++cnt > 4 ) {
            break;
        }
    }

    return table;
}

double Snap::_calc_frame_avg()
{
    double sum = 0.0 ;
    foreach ( Frame frame, _frames ) {
        sum += frame.frame_time();
    }

    return sum/_frames.length() ;
}

double Snap::_calc_frame_stddev(double frameAvg)
{
    if ( frameAvg == 0.0 ) {
        return 0.0;
    }

    double sum = 0.0 ;
    foreach ( Frame frame, _frames ) {
        double frametime =  frame.frame_time();
        double vv = (frametime-frameAvg)*(frametime-frameAvg);
        sum += vv;
    }

    return sqrt(sum/_frames.length()) ;
}

bool Snap::_process_jobs(DataModel* model )
{
    bool ret = true;

    int nParams = model->columnCount();
    for ( int i = 1 ; i < nParams; ++i ) {
        CurveModel* curve = new CurveModel(model,0,i,i);
        _curves.append(curve);
        Job* job = new Job(curve);
        if ( job->isFrameTimerJob() ) {
            delete job;
            continue;
        }
        QString job_id = job->job_id();
        _id_to_job[job_id] = job;
        _jobs.append(job);
    }

    return ret;
}

QList<Frame> Snap::_process_frames()
{
    QList<Frame> frames;

    SnapTable* curve = _thread0->runtimeCurve();
    int rc = curve->rowCount();

    for ( int row = 0 ; row < rc ; ++row ) {
        QModelIndex tIdx = curve->index(row,0);
        QModelIndex ftIdx = curve->index(row,1);
        double t = curve->data(tIdx).toDouble();   // timestamp
        double ft = curve->data(ftIdx).toDouble(); // frame time
        Frame frame(&_jobs,row,t,ft);
        frames.append(frame);
    }

    qSort(frames.begin(), frames.end(), frameTimeGreaterThan);

    return frames;
}

SnapReport::SnapReport(Snap &snap) : _snap(snap)
{
}

QString SnapReport::report()
{
    QString rpt;
    QString str;

    int cnt = 0;
    int max_cnt = 10;

    /*
    if ( ! _snap.is_realtime() ) {
        rpt.append("Oh Snap!!! - The following sim run had realtime disabled!\n");
        rpt += str.sprintf("          %s\n",_snap.rundir().toLatin1().constData());
        rpt.append("Try again!\n");
        return rpt;
    }
    */

    QString divider("------------------------------------------------\n");
    QString endsection("\n\n");

    rpt.append(
"************************************************************************\n"
"*                            Koviz Results                             *\n"
"************************************************************************\n\n");

    rpt += str.sprintf("%20s = %s\n", "Run directory ",
                                     _snap.rundir().toLatin1().constData());
    QString yesNo("Yes");
    if ( !_snap.is_realtime() ) {
        yesNo = QString("No");
    }
    rpt += str.sprintf("%20s = %s\n", "Real-time ",yesNo.toLatin1().constData());
    rpt += str.sprintf("%20s = %d\n", "Num jobs", _snap.num_jobs());
    rpt += str.sprintf("%20s = %d\n", "Num frames",_snap.num_frames());
    rpt += str.sprintf("%20s = %d\n", "Num overruns",_snap.num_overruns());
    rpt += str.sprintf("%20s = %.2lf%%\n", "Percentage overruns",
                            _snap.percent_overruns());
    rpt += str.sprintf("%20s = %.9lf\n", "Frame rate", _snap.frame_rate());
    rpt += str.sprintf("%20s = %.6lf\n", "Frame avg",_snap.frame_avg());
    rpt += str.sprintf("%20s = %.6lf\n","Frame stddev",_snap.frame_stddev());
    rpt += str.sprintf("%20s = %d\n", "Num threads",_snap.num_threads());
    rpt += str.sprintf("%20s = %s\n","Thread list",
                                   _snap.thread_listing().toLatin1().constData());
    rpt += endsection;

    //
    // Spike Summary
    //
    const QList<Frame>* frames = _snap.frames();
    rpt += divider;
    rpt += QString("Top Spikes\n\n");
    rpt += str.sprintf("    %15s %15s %15s\n", "Time", "Spike", "JobLoadIndex%");
    cnt = 0 ;
    foreach ( Frame frame, *frames ) {
        if ( ++cnt > max_cnt ) break;
        double tt = frame.timestamp();
        double ft = frame.frame_time();
        rpt += str.sprintf("    %15.6lf %15.6lf %14.0lf%%\n",
                          tt ,ft, frame.jobloadindex());

    }
    rpt += endsection;

    //
    // Thread Summary
    //
    rpt += divider;
    rpt += QString("Thread Time Summary\n\n");
    rpt += str.sprintf("    %10s %10s %15s %15s %15s %15s %15s %15s\n",
            "Thread", "NumJobs", "Freq", "NumOverruns", "ThreadAvg", "AvgLoad%",
            "ThreadMax", "MaxLoad%");

    foreach ( Thread* thread, _snap.threads()->hash()->values() ) {

        rpt += str.sprintf("    %10d %10d %15.6lf %15d %15.6lf "
                          "%14.0lf%% %15.6lf %14.0lf%%\n",
                 thread->threadId(),thread->numJobs(),thread->frequency(),
                 thread->numOverruns(), thread->avgRunTime(),
                 thread->avgLoad(), thread->maxRunTime(), thread->maxLoad());
    }
    rpt += endsection;

    //
    // Top Jobs Per Thread
    //
    rpt += divider;
    rpt += QString("Top Jobs by Thread\n\n");
    rpt += str.sprintf("    %8s %8s %15s %15s %10s%%     %-50s\n",
           "Thread", "NumJobs", "ThreadAvg", "JobAvgTime", "Percent", "JobName");
    foreach ( Thread* thread, _snap.threads()->hash()->values() ) {

        rpt += str.sprintf("    %8d %8d %15.6lf ",
                   thread->threadId(), thread->numJobs(),thread->avgRunTime());

        if ( thread->avgRunTime() < 0.000005 && thread->numJobs() > 1 ) {
            rpt += str.sprintf("%15s  %10s     %-49s\n", "--","--","--");
            continue;
        }

        double sum = 0;
        for ( int ii = 0; ii < 5; ++ii) {
            Job* job = thread->jobAtIndex(ii) ;
            double load = thread->avgJobLoad(job);
            if ( job->job_name() == "real_time.rt_sync.rt_monitor" ) {
                sum += load;
            }
            if ( (load < 0.01 || (sum > 98.0 && ii > 0)) &&
                  thread->numJobs() > 1 ) {
                break;
            } else {
                if ( ii > 0 ) {
                    rpt += str.sprintf("    %8s %8s %15s ", "","","");
                }
                rpt += str.sprintf("%15.6lf ",thread->avgJobRuntime(job));
                if ( thread->avgRunTime() > 0.0000001 ) {
                    rpt += str.sprintf("%10.0lf%%", load);
                } else {
                    rpt += str.sprintf(" %10s", "--");
                }
                rpt += str.sprintf("     %-50s\n",
                                   job->job_name().toLatin1().constData());
            }
            if ( ii == thread->numJobs()-1 ) {
                break;
            }
        }
    }
    rpt += endsection;

    //
    // Job Time Avgs
    //
    rpt += divider;
    rpt += str.sprintf("Top Job Avg Times\n\n");
    rpt += str.sprintf("    %15s %6s %15s %15s    %-40s\n",
            "JobAvg", "Thread", "ThreadAvgTime", "JobFreq", "JobName");

    QList<Job*>* jobs = _snap.jobs(Snap::SortByJobAvgTime);
    cnt = 0 ;
    foreach ( Job* job, *jobs ) {

        if ( ++cnt > max_cnt ) break;

        rpt += str.sprintf("    %15.6lf %6d %15.6lf %15.6lf    %-40s\n",
                   job->avg_runtime(),
                   job->thread_id(),
                   _snap.threads()->hash()->value(job->thread_id())->avgRunTime(),
                   job->freq(),
                   job->job_name().toLatin1().constData() );

    }
    rpt += endsection;

    //
    // Job Time Maxes
    //
    rpt += divider;
    rpt += str.sprintf("Top Job Max Times\n\n");
    rpt += str.sprintf("    %15s %15s %6s %15s %15s    %-40s\n",
                   "JobMax", "SimTime","Thread", "ThreadTime",
                   "JobFreq","JobName");
    jobs = _snap.jobs(Snap::SortByJobMaxTime);
    cnt = 0 ;
    foreach ( Job* job, *jobs ) {

        if ( ++cnt > max_cnt ) break;

        rpt += str.sprintf("    %15.6lf %15.6lf %6d %15.6lf %15.6lf    %-40s\n",
                job->max_runtime(),
                job->max_timestamp(),
                job->thread_id(),
                _snap.threads()->hash()->value(job->thread_id())->
                                     runtime(job->max_timestamp()),
                job->freq(),
                job->job_name().toLatin1().constData() );

    }
    rpt += endsection;

    //
    // Top 10 Sim Objects
    //
    QList<SimObject> simobjects = _snap.simobjects();
    if ( simobjects.length() > 100 ) {
        max_cnt = simobjects.length()/10;
    } else if ( simobjects.length() < 10 ) {
        max_cnt = simobjects.length();
    }

    // Format string to take length of simobject into account
    QString format;
    int maxlen = 0 ;
    cnt = 0 ;
    foreach ( SimObject sobject, simobjects ) {
        if ( ++cnt > max_cnt ) break;
        int len = sobject.name().length();
        if ( len > maxlen ) maxlen = len;
    }

    rpt += divider;
    rpt += QString("Top Sim Object Avg Times\n\n");
    format.sprintf("    %%%ds %%15s %%10s\n",maxlen);
    rpt += str.sprintf(TXT(format),"SimObject","AvgTime", "NumJobs");
    format.sprintf("    %%%ds %%15.6lf %%10d\n",maxlen);
    cnt = 0 ;
    foreach ( SimObject sobject, simobjects ) {
        if ( ++cnt > max_cnt ) break;
        rpt += str.sprintf(TXT(format),TXT(sobject.name()),
                           sobject.avg_runtime(),
                           sobject.jobs().length());
    }
    max_cnt = 10;
    rpt += endsection;

    //
    // Spike to Job Correlation
    //
    rpt += divider;
    rpt += QString("Top Spikes Job Breakdown\n\n");
    cnt = 0 ;
    foreach ( Frame frame, *frames ) {

        if ( ++cnt > max_cnt ) break;

        double tt = frame.timestamp();
        double ft = frame.frame_time();

        rpt += str.sprintf("Spike at time %.4lf of %g\n", tt ,ft);
        rpt += str.sprintf( "    %6s %15s %15s %15s    %-50s\n",
                        "Thread",
                        "ThreadTime",
                        "JobTime",
                        "JobFreq",
                        "JobName");

        int cnt2 = 0 ;
        double total = 0 ;
        for ( int ii = 0; ii < frame.topjobs()->length(); ++ii) {

            QPair<double,Job*> topjob = frame.topjobs()->at(ii);

            double rt =  topjob.first;
            Job* job = topjob.second;

            double delta = 1.0e-3;
            if ( rt < delta ) {
                continue;
            }

            rpt += str.sprintf(
                    "    %6d %15.6lf %15.6lf %15.6lf    %-50s\n",
                    job->thread_id(),
                    _snap.threads()->hash()->
                          value(job->thread_id())->runtime(tt),
                    rt,
                    job->freq(),
                    job->job_name().toLatin1().constData()) ;


            // limit printout of jobs
            if (job->job_name() != QString("trick_sys.sched.advance_sim_time")) {
                total += rt;
            }
            if ( total > 0.75*ft && ++cnt2 > 4 ) {
                break;
            }

        }
        rpt += endsection;
    }

    //
    // Spike to Thread Correlation
    //
    rpt += divider;
    rpt += QString("Top Spike Thread Breakdown\n\n");
    cnt = 0 ;
    foreach ( Frame frame, *frames ) {

        if ( ++cnt > max_cnt ) break;

        double tt = frame.timestamp();
        double ft = frame.frame_time();

        QList<QPair<double, Thread*> > topthreads;
        foreach ( Thread* thread, _snap.threads()->hash()->values() ) {
            topthreads.append(qMakePair(thread->runtime(tt),thread));
        }
        qSort(topthreads.begin(),topthreads.end(),topThreadGreaterThan);

        rpt += str.sprintf("Spike at time %g of %g\n",tt,ft);
        rpt += str.sprintf("    %6s %15s %15s\n", "Thread", "ThreadFreq",
                                            "ThreadTime");
        for ( int ii = 0 ; ii < topthreads.length(); ++ii ) {
            QPair<double,Thread*> topthread = topthreads.at(ii);
            if ( topthread.first < 1.0e-3 ) {
                break;
            }
            rpt += str.sprintf("    %6d %15.6lf %15.6lf\n",
                               topthread.second->threadId(),
                               topthread.second->frequency(),
                               topthread.first);
        }
        rpt += endsection;
    }

    return rpt;

}


void Snap::_setLogFileNames()
{
    _fileNameTrickJobs = _rundir + "/log_trickjobs.trk";
    if ( !QFileInfo(_fileNameTrickJobs).exists() ) {
        _fileNameTrickJobs = _rundir + "/log_frame_trickjobs.trk";
    }

    if ( !QFileInfo(_fileNameTrickJobs).exists() ) {
        _fileNameTrickJobs = _rundir + "/log_snap_trickjobs.trk";
        if ( !QFileInfo(_fileNameTrickJobs).exists() ) {
            _err_stream << "koviz [error]: cannot find log_trickjobs.trk, "
                        << "log_frame_trickjobs.trk "
                        << "or log_snap_trickjobs.trk files in "
                        << "directory " << _rundir;
            throw std::invalid_argument(_err_string.toLatin1().constData());
        }
    }

    // Trick 13 splits userjobs into separate files
    QDir dir(_rundir);
    QStringList filter;
    filter << "*userjobs*.trk";
    dir.setNameFilters(filter);
    QStringList userJobs = dir.entryList(QDir::Files);
    foreach ( QString userJob, userJobs ) {
        QString path = _rundir + "/" + userJob;
        _fileNamesUserJobs << path;
    }
    if ( _fileNamesUserJobs.isEmpty() ) {
        _err_stream << "koviz [error]: no *userjob*.trk files found in "
                    << "directory " << _rundir;
        throw std::invalid_argument(_err_string.toLatin1().constData());
    }

    _fileNameLogFrame = _rundir + "/log_frame.trk";
    if ( !QFileInfo(_fileNameLogFrame).exists() ) {
        _fileNameLogFrame = _rundir + "/log_snap_frame.trk";
        if ( !QFileInfo(_fileNameLogFrame).exists() ) {
            _err_stream << "koviz [error]: cannot find log_frame.trk or "
                        << "log_snap_frame.trk files in "
                        << "directory " << _rundir;
            throw std::invalid_argument(_err_string.toLatin1().constData());
        }
    }
}
