#include <QApplication>

#include <stdexcept>
#include <cstdlib>          // for rand() and RAND_MAX def

#include <QtTest/QtTest>
#include <QDir>
#include "libsnaprt/snap.h"
#include "trickdatamodel.h"

#define TEST_NON_RM2000
#define TEST_RM2000_RPT
#define TEST_RM2000_BENCHMARK

QString testRunsDir()
{
    QString dirName = QDir::currentPath() + "/../testsnap/runs/";
    QDir dir(dirName);
    if ( ! dir.exists() ) {
        QString msg = "snap [error]: can't find directory \"" + dirName + QString("\".  Aborting test!\n");
        throw std::runtime_error(msg.toAscii().constData());
    }
    return dirName;
}

class TestSnap: public QObject
{
    Q_OBJECT

public:
    TestSnap();

private:

    bool _is_generate_rundata;

    TrickDataModel _log_frame;
    TrickDataModel _log_trickjobs;
    TrickDataModel _log_userjobs;

    QString _run(const QString& runname); //  return test_dir/runs/RUN_$runname
    void _touch(const QString& fname); // make an empty file

    bool _write_logs(// writes log_frame/userjobs/trickjobs.trk/header
            const QString& rundir);

    void _create_log_userjobs(TrickDataModel &model, double start, double stop,
                                    double frame_rate, const QList<Job> &jobs);
    void _create_log_frame(TrickDataModel &model,
                              double start, double stop, double freq);
    void _create_log_trickjobs(TrickDataModel &model,
                               double start, double stop, double freq);
    void _add_param(TrickDataModel &model,
                    int col,
                    const QString &pname,
                    const QString &type,
                    const QString& unit);

private slots:
    void initTestCase();

#ifdef TEST_NON_RM2000
    void empty_run1();
    void empty_run2();
    void is_realtime0();
    void is_realtime1();
    void rundir();
    void range1();
    void range2();
    void range3();
    void range4();
    void range5();
    void range6();
    void range7();
    void range8();
    void range9();
    void range10();
    void range11();
    void range12();
    void range13();
    void range14();
    void range15();
    void num_jobs1();
    void num_jobs2();
    void num_frames1();
    void num_frames2();
    void num_overruns1();
    void num_overruns2();
    void num_overruns3();
    void num_overruns4();
    void num_overruns5();
    void num_overruns6();
    void num_overruns7();
    void frame_rate1();
    void frame_rate2();
    void frame_avg1();
    void frame_avg2();
    void frame_stddev1();
    void jobid();
    void job_num();
    void job_name();
    void job_sobject();
    void job_threadid();
    void job_class();
    void job_npoints1();
    void job_npoints2();
    void job_npoints3();
    void gettimebyidx1();
    void job_timestamps1();
    void job_timestamps2();
    void job_runtime1();
    void job_avgtime();
    void job_max();
    void job_stddev();
    void job_freq();
    void thread1();
#ifdef TEST_RM2000_RPT
    void run_rm2000();
#endif
#ifdef TEST_RM2000_BENCHMARK
    void benchmark_rm2000();
#endif
#endif
    void cleanupTestCase() {}
};

TestSnap::TestSnap()
{
}

QString TestSnap::_run(const QString& runname)
{
    QDir dir;

    bool ok;

    QString rundir = testRunsDir() + QString("RUN_") + runname;


    if ( ! dir.exists(rundir) ) {
        ok = dir.mkdir(rundir);
        if ( !ok ) {
            QString msg = QString("snap [error]: couldn't mkdir(\"") + rundir +
                          QString("\")\n");
            throw std::runtime_error(msg.toAscii().constData());
        }
    }

    return rundir;
}

void TestSnap::_touch(const QString &fname)
{
    QFile file(fname);
    if ( !file.exists() ) {
        file.open(QIODevice::WriteOnly);
        file.write(""); // write zero bytes
        file.close();
    }
}

bool TestSnap::_write_logs(const QString &rundir)
{
    if (! _is_generate_rundata ) {
        return false;
    }

    _log_frame.write_log_header("log_frame",rundir);
    _log_frame.write_binary_trk("log_frame",rundir);

    _log_userjobs.write_log_header("log_userjobs",rundir);
    _log_userjobs.write_binary_trk("log_userjobs",rundir);

    _log_trickjobs.write_log_header("log_trickjobs",rundir);
    _log_trickjobs.write_binary_trk("log_trickjobs",rundir);

    return true;
}

void TestSnap::_create_log_userjobs(TrickDataModel &model,
                                    double start,
                                    double stop,
                                    double frame_rate,
                                    const QList<Job>& jobs)
{
    int ncols = model.columnCount();
    if ( ncols > 0 ) {
        model.removeColumns(0,ncols);
    }

    model.insertColumns(0,1);
    model.setHeaderData(
        0,Qt::Horizontal,
        QVariant(QString("sys.exec.out.time")),
        Param::Name);
    model.insertColumns(1,jobs.size());

    int ii = 1;
    foreach ( Job job, jobs ) {
        QString jobname = job.log_name();
        model.setHeaderData(
                    ii,Qt::Horizontal,
                    QVariant(jobname), Param::Name);
        model.setHeaderData(
                    ii,Qt::Horizontal,QVariant(QString("s")),
                    Param::Unit);
        model.setHeaderData(
                    ii,Qt::Horizontal,QVariant(QString("double")),
                    Param::Type);
        ii++;
    }

    // Set timestamp and "runtime" for job
    double tt = start;
    int rc = 0 ;
    while ( tt <= stop+1.0e-9 ) {
        model.insertRows(rc,1);
        for ( int ii = 0; ii < jobs.size()+1; ++ii) {
            QModelIndex idx = model.index(rc,ii);
            if ( ii == 0 ) {
                model.setData(idx,QVariant(tt));
            } else {
                Job job = jobs.at(ii-1);
                double jobfreq = job.freq();
                int mod = (int)(jobfreq/frame_rate);
                if ( rc%mod == 0 ) {
                    // e.g. if frame_rate=0.1 and jobfreq=1.0, then
                    //      mod=10... so every 10th timestamp will have a
                    //      "runtime" > 0... making it a 1.0 job cycle
                    model.setData(idx,QVariant(1.0+1000000.0*qAbs(sin(tt))));
                }
            }
        }
        tt += frame_rate;
        rc++;
    }
}

void TestSnap::_create_log_frame(
                                    TrickDataModel &model,
                                    double start,
                                    double stop,
                                    double freq)
{
    _add_param(model,
               0,
               QString("sys.exec.out.time"),
               QString("double"),
               QString("s"));

    _add_param(model,
               1,
               QString("real_time.rt_sync.frame_sched_time"),
               QString("double"),
               QString("--"));

    _add_param(model,
               2,
               QString("real_time.rt_sync.frame_overrun_time"),
               QString("double"),
               QString("--"));

    _add_param(model,
               3,
               QString("JOB_data_record_group.userjobs.89.01(end_of_frame)"),
               QString("double"),
               QString("s"));

    _add_param(model,
               4,
               QString("JOB_data_record_group.trickjobs.90.01(end_of_frame)"),
               QString("double"),
               QString("s"));


    // Timestamp+4params
    const int nparams = 5;
    double tt = start;
    QModelIndex idx[nparams];
    int rc = 0 ;
    while ( tt <= stop+1.0e-9 ) {
        model.insertRows(rc,1);
        for ( int ii = 0; ii < nparams; ++ii) {
            idx[ii] = model.index(rc,ii);
            if ( ii == 0 ) {
                model.setData(idx[ii],QVariant(tt));
            } else {
                model.setData(idx[ii],QVariant(1.0+sin(tt)));
            }
        }
        tt += freq;
        rc++;
    }

}

void TestSnap::_create_log_trickjobs(TrickDataModel &model,
                                     double start, double stop, double freq)
{
    _add_param(model,
               0,
               QString("sys.exec.out.time"),
               QString("double"),
               QString("s"));


    _add_param(model,
               1,
               QString("JOB_trick_sys.sched.advance_sim_time.23.18(atime_1.000)"),
               QString("double"),
               QString("s"));

    _add_param(model,
               2,
               QString("JOB_trick_ip.ip.checkpoint.27.04(checkpoint)"),
               QString("double"),
               QString("s"));


    _add_param(model,
               3,
               QString("JOB_data_record_group.userjobs.89.01(end_of_frame)"),
               QString("double"),
               QString("s"));

    _add_param(model,
               4,
               QString("JOB_data_record.drd.signal_thread.29.03(end_of_frame)"),
               QString("double"),
               QString("s"));

    _add_param(model,
               5,
               QString("JOB_trick_ip.ip.process_event.27.02(automatic_1.000)"),
               QString("double"),
               QString("s"));

    _add_param(model,
               6,
               QString("JOB_real_time.rt_sync.rt_monitor.30.10(end_of_frame)"),
               QString("double"),
               QString("s"));

    // Timestamp+5params
    const int nparams = 7;
    double tt = start;
    QModelIndex idx[nparams];
    int rc = 0 ;
    while ( tt <= stop+1.0e-9 ) {
        model.insertRows(rc,1);
        for ( int ii = 0; ii < nparams; ++ii) {
            idx[ii] = model.index(rc,ii);
            switch (ii)
            {
            case 0:  model.setData(idx[ii],QVariant(tt)); break;
            default: model.setData(idx[ii],QVariant(100.0+100000.0*qAbs(sin(tt))));
                     break;
            };
        }
        tt += freq;
        rc++;
    }
}


void TestSnap::_add_param(TrickDataModel &model, int col,
                          const QString &pname, const QString &type,
                          const QString &unit)
{
    model.insertColumns(col,1);
    model.setHeaderData(
            col,Qt::Horizontal,
                QVariant(pname), Param::Name);
    model.setHeaderData(
            col,Qt::Horizontal,QVariant(unit), Param::Unit);
    model.setHeaderData(
            col,Qt::Horizontal,QVariant(type), Param::Type);
}

void TestSnap::initTestCase()
{
    _is_generate_rundata = true;   // turn this to false once happy with
                                   // RUN data that is generated by this test

    _create_log_frame(_log_frame,0,100,0.1);
    _create_log_trickjobs(_log_trickjobs,0,100,0.1);

    QList<Job> jobs;
    int njobs = 5;
    for ( int ii = 1; ii < njobs+1; ++ii) {
        QString jname = QString("JOB_s1.x.a_C1.2500.%1(scheduled_1.000)").arg(ii);
        Job job(jname);
        jobs.append(job);
    }
    _create_log_userjobs(_log_userjobs,0,100,0.1,jobs);
}

#ifdef TEST_NON_RM2000
void TestSnap::empty_run1()
{
    QString rundir = _run("empty1");
    QString fname = rundir +  QString("/log_frame.trk");

    _touch(fname);

    try {
        Snap snap(rundir);
    } catch (std::invalid_argument &e) {
        return;         // Should throw this exception
    }

    QCOMPARE(false,true);  // if exception fails this fails
}

void TestSnap::empty_run2()
{
    QString rundir = _run("empty2");
    QString fname0 = rundir +  QString("/log_frame.trk");
    QString fname1 = rundir +  QString("/log_trickjobs.trk");
    _touch(fname0);
    _touch(fname1);

    try {
        Snap snap(rundir);
    } catch (std::invalid_argument &e) {
        return;         // Should throw this exception
    }

    QCOMPARE(false,true);  // if exception fails this fails
}

// Test is_realtime() when run is non realtime
void TestSnap::is_realtime0()
{
    QString rundir = _run("0");

    for ( int row = 0; row < _log_frame.rowCount(); ++row) {
        for ( int col = 1; col < _log_frame.columnCount(); ++col) {
            QModelIndex idx = _log_frame.index(row,col);
            _log_frame.setData(idx,QVariant(0.0));
        }
    }
    _write_logs(rundir);

    Snap snap(rundir);
    bool expected = snap.is_realtime();
    QCOMPARE(expected,false);
}

// Test is_realtime() when run is realtime
void TestSnap::is_realtime1()
{
    QString rundir = _run("1");

    // col=1 if any point is non-zero for this param it's realtime
    QModelIndex idx = _log_frame.index(500,1);
    _log_frame.setData(idx,QVariant(100.0));

    // Set an overrun time too
    idx = _log_frame.index(500,2);
    _log_frame.setData(idx,QVariant(200.0));

    _write_logs(rundir);

    Snap snap(rundir);
    bool expected = snap.is_realtime();
    QCOMPARE(expected,true);
}

void TestSnap::rundir()
{
    QString rundir = _run("1");
    Snap snap(rundir);
    QCOMPARE(snap.rundir(),rundir);
}

void TestSnap::range1()
{
    Snap snap(_run("1"));         // default 1.0
    QCOMPARE(snap.start(),1.0);
}

void TestSnap::range2()
{
    Snap snap(_run("1"));
    QCOMPARE(snap.start(),1.0);
}

void TestSnap::range3()
{
    try {
        Snap snap(_run("1"),101.0);  // exceed timestamp again
    } catch (std::range_error &e) {
        return;         // Should throw this exception
    }

    QCOMPARE(false,true);  // if exception fails this fails
}

void TestSnap::range4()
{
    Snap snap(_run("1"),0.0);  // zero-th timestamp
    QCOMPARE(snap.start(),0.0);
}

void TestSnap::range5()
{
    Snap snap(_run("1"),100.0);  // last timestamp
    QCOMPARE(snap.start(),100.0);
}

void TestSnap::range6()
{
    Snap snap(_run("1"),-100.0);  // negative timestamp
    QCOMPARE(snap.start(),0.0);
}

void TestSnap::range7()
{
    Snap snap(_run("1"),-100.0,200.0);  // big stop
    QCOMPARE(snap.start(),0.0);
    QCOMPARE(snap.stop(),100.0);
}

void TestSnap::range8()
{
    Snap snap(_run("1"),10.0,100.0);  // last stop
    QCOMPARE(snap.start(),10.0);
    QCOMPARE(snap.stop(),100.0);
}

void TestSnap::range9()
{
    Snap snap(_run("1"),49.0,50.0);  // mid stop
    QCOMPARE(snap.start(),49.0);
    QCOMPARE(snap.stop(),50.0);
}

void TestSnap::range10()
{
    Snap snap(_run("1"),49.0,49.0);  // start/stop on same point
    QCOMPARE(snap.start(),49.0);
    QCOMPARE(snap.stop(),49.0);
    QCOMPARE(snap.num_frames(),1);
}

void TestSnap::range11()
{
    Snap snap(_run("1"),40.001,40.001);  // start/stop on same point off cycle
    QCOMPARE(snap.start(),40.1);
    QCOMPARE(snap.stop(),40.1);
    QCOMPARE(snap.num_frames(),1);
}

void TestSnap::range12()
{
    Snap snap(_run("1"),39.999,39.999);  // start/stop on same point off cycle
    QCOMPARE(snap.start(),40.0);
    QCOMPARE(snap.stop(),40.0);
    QCOMPARE(snap.num_frames(),1);
}

void TestSnap::range13()
{
    Snap snap(_run("1"),99.999,99.999);  // start/stop on same point off cycle
                                      // on end bound point
    QCOMPARE(snap.start(),100.0);
    QCOMPARE(snap.stop(),100.0);
    QCOMPARE(snap.num_frames(),1);
}

void TestSnap::range14()
{
    try {
        Snap snap(_run("1"),40.0,30.00);  // stop > start
    } catch (std::range_error &e) {
        return;         // Should throw this exception
    }

    QCOMPARE(false,true);  // if exception fails this fails
}

void TestSnap::range15()
{
    Snap snap(_run("1"),100.0,100.0001);  // start/stop on same point off cycle
                                    // on end bound point
    QCOMPARE(snap.start(),100.0);
    QCOMPARE(snap.stop(),100.0);
    QCOMPARE(snap.num_frames(),1);
}

void TestSnap::num_jobs1()
{
    Snap snap(_run("0"));
    QCOMPARE(snap.num_jobs(),11);
}

void TestSnap::num_jobs2()
{
    Snap snap(_run("1"));
    QCOMPARE(snap.num_jobs(),11);
}

void TestSnap::num_frames1()
{
    Snap snap(_run("1"),0,100);
    QCOMPARE(snap.num_frames(),10*100+1);
}

void TestSnap::num_frames2()
{
    Snap snap(_run("1"),50);
    QCOMPARE(snap.num_frames(),10*50+1);
}

void TestSnap::num_overruns1()
{
    Snap snap(_run("1"));
    QCOMPARE(snap.num_overruns(),1);
}

void TestSnap::num_overruns2()
{
    Snap snap(_run("1"),0,50);
    QCOMPARE(snap.num_overruns(),1);
}

void TestSnap::num_overruns3()
{
    Snap snap(_run("1"),0,49.1);
    QCOMPARE(snap.num_overruns(),0);
}

void TestSnap::num_overruns4()
{
    Snap snap(_run("1"),50);
    QCOMPARE(snap.num_overruns(),1);
}

void TestSnap::num_overruns5()
{
    Snap snap(_run("1"),50.1,53);
    QCOMPARE(snap.num_overruns(),0);
}

void TestSnap::num_overruns6()
{
    Snap snap(_run("0"));
    QCOMPARE(snap.num_overruns(),0);
}

void TestSnap::num_overruns7()
{
    Snap snap(_run("1"),49.99,49.99);
    QCOMPARE(snap.num_overruns(),1);
}

void TestSnap::frame_rate1()
{
    Snap snap(_run("1"));
    QCOMPARE(snap.frame_rate(),QString("0.1"));
}

void TestSnap::frame_rate2()
{
    QString rundir = _run("2");

    _log_frame.insertRows(1,1);
    QModelIndex idx = _log_frame.index(1,0);
    _log_frame.setData(idx,QVariant(0.05));
    _write_logs(rundir);

    Snap snap(rundir,0);
    QCOMPARE(snap.num_frames(),10*100+1+1); // extra frame at 0.05
    QCOMPARE(snap.frame_rate(),QString("0.05,0.1"));
}

void TestSnap::frame_avg1()
{
    QString rundir = _run("3");

    _log_frame.removeRows(1,1);

    int cnt = 0 ;
    double sum = 0 ;
    double rt = 700000.0; // num microseconds as a double
    for ( int ii = 0; ii < _log_frame.rowCount(); ++ii) {
        if ( ii%10 == 0 ) {
            QModelIndex idx = _log_frame.index(ii,1);
            _log_frame.setData(idx,QVariant(rt));
            sum += rt;
        }
        cnt++;
    }
    double avg = sum/(double)cnt/1000000.0;
    _write_logs(rundir);

    Snap snap(rundir,0);

    // sanity to see if extra row added in frame_rate2() is gone
    QCOMPARE(snap.num_frames(),10*100+1);
    QCOMPARE(snap.frame_rate(),QString("0.1"));

    QCOMPARE(snap.frame_avg(),avg);
}

void TestSnap::frame_avg2()
{
    QString rundir = _run("4");

    int cnt = 0 ;
    double sum = 0 ;
    double rt; // rand num microseconds as a double
    for ( int ii = 0; ii < _log_frame.rowCount(); ++ii) {
        QModelIndex idx = _log_frame.index(ii,1);
        rt = 1000000.0*(qrand()/(double)RAND_MAX);
        _log_frame.setData(idx,QVariant(rt));
        sum += rt;
        cnt++;
    }
    double avg = sum/(double)cnt/1000000.0;
    _write_logs(rundir);

    Snap snap(rundir,0);

    QCOMPARE(snap.frame_avg(),avg);
}

void TestSnap::frame_stddev1()
{
    QString rundir = _run("4");

    double sum = 0.0 ;
    double rt;
    for ( int ii = 0; ii < _log_frame.rowCount(); ++ii) {
        QModelIndex idx = _log_frame.index(ii,1);
        rt = _log_frame.data(idx).toDouble();
        sum += rt;
    }
    double cnt = (double)_log_frame.rowCount();
    double avg = sum/cnt;

    // Take the square root of the avg
    // of the squared differences of the values
    // from their avg value
    double sum_sqr_diffs = 0.0;
    for ( int ii = 0; ii < _log_frame.rowCount(); ++ii) {
        QModelIndex idx = _log_frame.index(ii,1);
        rt = _log_frame.data(idx).toDouble();
        sum_sqr_diffs += (avg-rt)*(avg-rt);
    }
    double stddev = qSqrt(sum_sqr_diffs/cnt)/1000000.0;

    Snap snap(rundir,0);

    QCOMPARE(snap.frame_stddev(),stddev);
}

void TestSnap::jobid()
{
    QString rundir = _run("4");
    Snap snap(rundir,0);

    QCOMPARE(snap.jobs()->at(0)->log_name(),
             QString("JOB_trick_ip.ip.process_event.27.02(automatic_1.000)"));
}

void TestSnap::job_num()
{
    QString rundir = _run("4");
    Snap snap(rundir,0);

    QCOMPARE(snap.jobs()->at(0)->job_num(),QString("27.02"));
}

void TestSnap::job_name()
{
    QString rundir = _run("4");
    Snap snap(rundir);

    QCOMPARE(snap.jobs()->at(1)->job_name(),QString("s1.x.a"));
}

void TestSnap::job_sobject()
{
    QString rundir = _run("4");
    Snap snap(rundir);

    QCOMPARE(snap.jobs()->at(1)->sim_object_name(), QString("s1"));
}

void TestSnap::job_threadid()
{
    QString rundir = _run("4");
    Snap snap(rundir);
    QCOMPARE(snap.jobs()->at(1)->thread_id(),1);
}

void TestSnap::job_class()
{
    QString rundir = _run("4");
    Snap snap(rundir);
    QCOMPARE(snap.jobs()->at(1)->job_class(),QString("scheduled"));
}

void TestSnap::job_npoints1()
{
    QString rundir = _run("4");
    Snap snap(rundir);
    QCOMPARE(snap.jobs()->at(1)->npoints(),snap.num_frames());
}

void TestSnap::job_npoints2()
{
    QString rundir = _run("4");
    Snap snap(rundir,10.01,49.999);
    QCOMPARE(snap.jobs()->at(1)->npoints(),snap.num_frames());
}

void TestSnap::job_npoints3()
{
    QString rundir = _run("4");
    Snap snap(rundir,9.48,50.123);
    QCOMPARE(snap.jobs()->at(1)->npoints(),snap.num_frames());
}

void TestSnap::gettimebyidx1()
{
    QString rundir = _run("4");
    Snap snap(rundir,0,1.0e20);
    TrickCurveModel* curve = snap.jobs()->at(0)->curve();
    QCOMPARE(curve->indexAtTime(89.7),897); // kacc
}

void TestSnap::job_timestamps1()
{
    QString rundir = _run("4");
    Snap snap(rundir);
    QCOMPARE(snap.jobs()->at(0)->curve()->begin().t(),1.0);
}

void TestSnap::job_timestamps2()
{
    QString rundir = _run("4");
    Snap snap(rundir,9.48,50.123);
    TrickModelIterator it = snap.jobs()->at(1)->curve()->begin();
    int npoints = snap.jobs()->at(1)->curve()->rowCount();
    QCOMPARE(it[npoints-1].t(),50.1);
}

void TestSnap::job_runtime1()
{
    QString rundir = _run("4");
    Snap snap(rundir);
    TrickCurveModel* curve = snap.jobs()->at(0)->curve();
    int tidx = curve->indexAtTime(89.0);
    TrickModelIterator it = curve->begin();
    QCOMPARE(it[tidx].x(),1.0+1000000*qAbs(sin(89.0)));
}

void TestSnap::job_avgtime()
{
    QString rundir = _run("5");

    for ( int cc = 1; cc < _log_userjobs.columnCount(); ++cc) {
        for ( int rr = 0; rr < _log_userjobs.rowCount(); ++rr) {
            QModelIndex idx = _log_userjobs.index(rr,cc);
            double val = 100000.0*(double)cc;
            _log_userjobs.setData(idx,QVariant(val));
        }
    }
    _write_logs(rundir);

    Snap snap(rundir,0);

    //
    // avg is 0.1*column (reverse order to see if sort works)
    //
    QList<Job*>* jobs = snap.jobs(Snap::SortByJobAvgTime);
    for ( int ii = 0; ii < jobs->size(); ++ii) {
        Job* job = jobs->at(ii);
        if ( job->sim_object_name() != QString("s1") ) continue ;
        QCOMPARE(job->avg_runtime(),0.1*(double)(5-ii));
    }

    return;
}

void TestSnap::job_max()
{
    QString rundir = _run("6");

    // Put max at 10 seconds for job in column 1, 20 seconds column 2 etc.
    for ( int cc = 1; cc < _log_userjobs.columnCount(); ++cc) {
        double sf = (double)qrand();// make scale random num between 0.0-0.5
        sf = 0.5*sf/(double)RAND_MAX;
        for ( int rr = 0; rr < _log_userjobs.rowCount(); ++rr) {
            QModelIndex idx = _log_userjobs.index(rr,0);
            double tt = _log_userjobs.data(idx).toDouble();
            idx = _log_userjobs.index(rr,cc);
            double val = sf*1000000.0*qAbs(sin(tt));
            _log_userjobs.setData(idx,QVariant(val));
        }
    }
    for ( int cc = 1; cc < _log_userjobs.columnCount(); ++cc) {
        QModelIndex idx = _log_userjobs.index(cc*100,cc); //10sec,20sec...
        double val = cc*1000000.0; // make max 1.0,2.0,3.0... etc.
        _log_userjobs.setData(idx,QVariant(val));
    }

    _write_logs(rundir);

    Snap snap(rundir,0);

    //
    // max is 5.0,4.0,3.0,2.0,1.0 seconds at 50,40,30,20,10 seconds respectively
    //
    QList<Job*>* jobs = snap.jobs();
    for ( int ii = 0; ii < jobs->size(); ++ii) {
        Job* job = jobs->at(ii);
        if ( job->job_num() == "2500.1" ) {
            QCOMPARE(job->max_runtime(),1.0);
            QCOMPARE(job->max_timestamp(),10.0*1.0);
        }
        if ( job->job_num() == "2500.5" ) {
            QCOMPARE(job->max_runtime(),5.0);
            QCOMPARE(job->max_timestamp(),10.0*5.0);
        }
    }

    return;
}

void TestSnap::job_stddev()
{
    QString rundir = _run("6");

    const int job_num = 5;

    // Check stddev for job5
    double rc = (double)_log_userjobs.rowCount();
    double sum = 0.0 ;
    for ( int rr = 0; rr < _log_userjobs.rowCount(); ++rr) {
        QModelIndex idx = _log_userjobs.index(rr,job_num);
        double val = _log_userjobs.data(idx).toDouble()/1000000.0;
        sum += val;
    }
    double avg = sum/rc;

    sum = 0.0;
    for ( int rr = 0; rr < _log_userjobs.rowCount(); ++rr) {
        QModelIndex idx = _log_userjobs.index(rr,job_num);
        double val = _log_userjobs.data(idx).toDouble()/1000000.0;
        sum += (avg-val)*(avg-val);
    }
    double stddev = qSqrt(sum/rc);

    Snap snap(rundir,0); // start must be at 0 to match calc above

    QList<Job*>* jobs = snap.jobs();
    for ( int ii = 0; ii < jobs->size(); ++ii) {
        Job* job = jobs->at(ii);
        if ( job->job_num() == "2500.5" ) {
            if ( qAbs(1.0 + job->stddev_runtime() - stddev) < 1.0 + 1.0e-6 ) {
                stddev = job->stddev_runtime();
            }
            QCOMPARE(job->stddev_runtime(),stddev);
        }
    }

    return;
}

// TODO
void TestSnap::job_freq()
{
}

void TestSnap::thread1()
{
    QString rundir = _run("7");

    QList<Job> jobs;
    Job j1("JOB_s1.x.a_C1.1001.0(scheduled_1.000)");
    Job j2("JOB_s1.x.b_C1.1000.5(scheduled_0.500)");
    Job j3("JOB_s1.x.c_C1.1000.2(scheduled_0.200)");
    Job j4("JOB_s1.x.d_C1.1000.1(scheduled_0.100)");
    Job j5("JOB_s2.x.e_C2.2000.1(scheduled_0.100)");
    Job j6("JOB_s3.x.f_C3.3000.1(scheduled_0.200)");
    Job j7("JOB_s3.x.g_C3.3000.1(scheduled_0.200)");
    jobs << j1 << j2 << j3 << j4 << j5 << j6 << j7;

    double frame_rate = 0.1;
    double start = 0;
    double stop = 100.0;

    _create_log_userjobs(_log_userjobs,start,stop,frame_rate,jobs);

    // Alter runtimes for C1-C3 to be a constant of 0.01 seconds
    double tt = start;
    int rc = 0 ;
    while ( tt <= stop+1.0e-9 ) {
        for ( int ii = 1; ii < jobs.size()+1; ++ii) {
            QModelIndex idx = _log_userjobs.index(rc,ii);
            Job job = jobs.at(ii-1);
            double jobfreq = job.freq();
            int mod = (int)(jobfreq/frame_rate);
            if ( rc%mod == 0 ) {
                // Runtime set to 0.01 seconds (10 thousand microseconds)
                _log_userjobs.setData(idx,QVariant(10000.0));
            }
        }
        tt += frame_rate;
        rc++;
    }

    _write_logs(rundir);

    Snap snap(rundir,start,stop);

    QCOMPARE(snap.num_threads(),4);

    QList<Thread> threads = snap.threads()->list();
    int tid = 0;
    foreach ( Thread thread, threads ) {

        // Thread Id
        QCOMPARE(thread.thread_id,tid);

        // Freq
        if ( tid == 0 ) {
            QCOMPARE(thread.freq,0.1);
        } else if ( tid == 1 ) {
            QCOMPARE(thread.freq,1.0);
        } else if ( tid == 2 ) {
            QCOMPARE(thread.freq,0.1);
        } else if ( tid == 3 ) {
            QCOMPARE(thread.freq,0.2);
        }

        // Num Jobs on thread
        if ( tid == 0 ) {
            QCOMPARE(thread.jobs.size(),6);
        } else if ( tid == 1 ) {
            QCOMPARE(thread.jobs.size(),4);
        } else if ( tid == 2 ) {
            QCOMPARE(thread.jobs.size(),1);
        } else if ( tid == 3 ) {
            QCOMPARE(thread.jobs.size(),2);
        }

        // Num frames
        if ( tid == 0 ) {
            QCOMPARE(thread.nframes(),1000);
        } else if ( tid == 1 ) {
            QCOMPARE(thread.nframes(),100);
        } else if ( tid == 2 ) {
            QCOMPARE(thread.nframes(),1000);
        } else if ( tid == 3 ) {
            QCOMPARE(thread.nframes(),500);
        }

        // Runtime
        double sum = 0.0;
        foreach ( Job* job, thread.jobs ) {
            double rt = (0.01)*(thread.freq)/job->freq();
            sum += rt;
        }
        // Duplicate lines because failure prints line number
        if ( tid == 1 ) {
            QCOMPARE(thread.runtime(45.0),sum);
        } else if ( tid == 2 ) {
            QCOMPARE(thread.runtime(45.0),sum);
        } else if ( tid == 3 ) {
            QCOMPARE(thread.runtime(45.0),sum);
        }

        tid++;
    }
}
#endif

#ifdef TEST_RM2000_RPT
//
// If this fails, remember that the expected result has "vetter"
// hard coded in the run directory (TODO: fix this!)
//
void TestSnap::run_rm2000()
{
    QString rundir = testRunsDir() + "/RUN_rm2000";
    Snap snap(rundir);
    SnapReport rpt(snap);
    QString actual_rpt = rpt.report();
    int i = actual_rpt.indexOf("Run directory");
    i = actual_rpt.indexOf('=',i);
    int j = actual_rpt.indexOf("/RUN_rm2000",i);
    actual_rpt = actual_rpt.remove(i+2,j-i-1);
    if ( i < 0 || j < 0 ) {
        QFAIL("Snap: [error] run_rm2000 rpt doesn't have correct "
                       "\"Run directory = .../RUN_rm2000\" line.\n");
    }

    QString fname = testRunsDir() + "/RUN_rm2000/expected-rm2000.rpt";
    QFile file(fname);

    if (!file.open(QIODevice::ReadOnly)) {
        fprintf(stderr,"Snap: [error] could not open %s\n",
                fname.toAscii().constData());
        exit(-1);
    }
    QString expected_rpt = file.readAll();

    QCOMPARE(actual_rpt,expected_rpt);
}
#endif

#ifdef TEST_RM2000_BENCHMARK
void TestSnap::benchmark_rm2000()
{
    QBENCHMARK_ONCE {
        QString rundir = testRunsDir() + "/RUN_rm2000";
        Snap snap(rundir);
    }
}
#endif


QTEST_APPLESS_MAIN(TestSnap);

#if 0
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QTEST_DISABLE_KEYPAD_NAVIGATION
    TestSnap tc;
    int ret = QTest::qExec(&tc, 0, argv);
    app.exit(ret);
    if ( true && ret == 0 ) {
        //app.exec();
    }

    return ret;
}
#endif

#include "testsnap.moc"
