#include "snapwindow.h"
#include <unistd.h>   // used for sleep()

#include <QDockWidget>
#include <QTabWidget>
#include <QTableView>
#include <QHeaderView>
#include <QFrame>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QSplitter>
#include <QSortFilterProxyModel>
#include <QFileInfo>

SnapWindow::SnapWindow(const QString& rundir,
                       double start,double stop,
                       QWidget *parent) :
    QMainWindow(parent),
    _curr_job_table(0),
    _curr_job_tv(0)
{
    setWindowTitle(tr("Snap!"));

    createMenu();

    _snap = new Snap(rundir,start,stop,true);
    _startup_thread = new StartUpThread(_snap);
    qApp->connect(qApp,SIGNAL(aboutToQuit()), _startup_thread,SLOT(quit()));
    qApp->connect(_startup_thread,SIGNAL(finished()), this,SLOT(_postLoad()));
    _startup_thread->start();

    // TODO: An UGLY workaround for the slim chance that the start up thread
    //       doesn't get it's job done in time.  I really want to fix this.
    //       To induce the problem, comment out this sleep(1) and put
    //       a sleep(2) in snapwindow.h at top of run() method like so
    //
    //       void run()
    //       {
    //           sleep(2);
    //           TimeItLinux t; t.start();
    //           _snap->load();
    //           t.snap("snaploadtime=");
    //       }
    sleep(1);

    // Central Widget
    QSplitter* s = new QSplitter;
    setCentralWidget(s);

    QFrame* frame = new QFrame;
    frame->setObjectName(QString::fromUtf8("frame"));
    frame->setFrameShape(QFrame::StyledPanel);
    //frame->setFrameShadow(QFrame::Sunken);
    frame->setLineWidth(1);
    frame->setMidLineWidth(0);
    s->addWidget(frame);

    _left_lay = new QGridLayout(frame);
    _left_lay->setSpacing(0);
    _left_lay->setContentsMargins(12, 12, 12, 12);
    _left_lay->setObjectName(QString::fromUtf8("verticalLayout"));

    //
    // Left side panel
    //
    QTabWidget* tab = new QTabWidget(frame);
    _left_lay->addWidget(tab,0,0,1,1);

    for ( int ii = 0; ii < _snap->tables.size(); ++ii) {

        QTableView* tv = _create_table_view(_snap->tables.at(ii));
        _tvs.append(tv);
        QString title = _snap->tables.at(ii)->tableName();
        tab->addTab(tv,title);

        if ( title == "Top Jobs" ) {
            tv->hideColumn(5);
            connect(tv->selectionModel(),
                    SIGNAL(currentChanged(QModelIndex, QModelIndex)),
                    this,
                    SLOT(_update_topjob_plot(QModelIndex)));
        } else if ( title == "Spikes" ) {
            _spike_tab_idx = ii;
            connect(tab,SIGNAL(currentChanged(int)),
                    this,SLOT(_tab_clicked(int)));
            connect(tv->selectionModel(),
                    SIGNAL(currentChanged(QModelIndex, QModelIndex)),
                    this,
                    SLOT(_update_job_table(QModelIndex)));
        } else if ( title == "Summary" ) {
            tv->setSortingEnabled(false);
        } else if ( title == "Threads" ) {
            connect(tv->selectionModel(),
                    SIGNAL(currentChanged(QModelIndex, QModelIndex)),
                    this,
                    SLOT(_update_thread_plot(QModelIndex)));
        }
    }


    // Left side bottom - progress bar for loading
    QProgressBar* bar = new QProgressBar;
    _bar = bar;
    bar->setMinimum(0);
    bar->setMaximum(100);
    QString fmt("Loading: ");
    fmt += _snap->rundir();
    bar->setFormat(fmt);
    bar->setTextVisible(true);
    _left_lay->addWidget(bar,1,0,1,1);
    connect(_snap, SIGNAL(progressChanged(int)),
            bar, SLOT(setValue(int)));
    connect(_snap, SIGNAL(finishedLoading()),
            this, SLOT(_finishedLoading()));
    _left_lay->addWidget(bar,2,0,1,1);


    //
    // Right side panel
    //
    QSizePolicy sizePolicy(QSizePolicy::MinimumExpanding,
                           QSizePolicy::MinimumExpanding);
    QFrame* f2 = new QFrame;
    f2->setSizePolicy(sizePolicy);
    s->addWidget(f2);

    QGridLayout* lay2 = new QGridLayout(f2);
    lay2->setSpacing(0);
    lay2->setContentsMargins(0, 0, 0, 0);
    lay2->setObjectName(QString::fromUtf8("layout2"));

    QString fname = _snap->fileNameLogFrame();
    _model_frames = new TrickModel("sys.exec.out.time", fname) ;
    _plot_frame = new Plot(f2);

    lay2->addWidget(_plot_frame,0,0,1,1);

    TrickCurveModel* frame_curve = new TrickCurveModel(_model_frames,0,0,1,
                                                       "Frame Scheduled Time",
                                                       1.0,     // x scale
                                                       1.0e-6); // y scale

    TrickCurve* curve = _plot_frame->axisRect()->addCurve(frame_curve);
    Q_UNUSED(curve);
    _plot_frame->axisRect()->axis(QCPAxis::atBottom)->
            setLabel("Time (s)");
    _plot_frame->axisRect()->axis(QCPAxis::atLeft)->
            setLabel("Frame Scheduled Time (s)");
    _plot_frame->axisRect()->zoomToFit();
    _plot_frame->replot();

    //
    // timeline test (development!)
    //
#ifdef TIMELINE
    _plot_frame->hide();
    QString csv("log_timeline.csv");
    csv = rundir + "/" + csv;
    TimeLinePlot* plot_timeline = new TimeLinePlot(csv);
    lay2->addWidget(plot_timeline,0,0,1,1);
#endif

    _timer.start();
    QStringList fnames = _snap->fileNamesUserJobs();
    foreach ( QString fname, fnames ) {
        TrickModel* m = new TrickModel("sys.exec.out.time",fname,fname);
        _models_userjobs.append(m);
        _trick_models.append(m);
    }
    _timer.snap("load_userjobs=");
    _plot_jobs = new Plot(f2);

    lay2->addWidget(_plot_jobs,1,0,1,1);
    _plot_jobs->axisRect()->axis(QCPAxis::atBottom)->setLabel("Time (s)");
    _plot_jobs->axisRect()->axis(QCPAxis::atLeft)->setLabel("Job Time (s)");
    connect(_plot_frame->axisRect()->axis(QCPAxis::atBottom),
            SIGNAL(rangeChanged(QCPRange)),
            this,
            SLOT(_update_plot_jobs_xrange(QCPRange)));
    connect(_plot_jobs->axisRect()->axis(QCPAxis::atBottom),
            SIGNAL(rangeChanged(QCPRange)),
            this,SLOT(_update_plot_frame_xrange(QCPRange)));


    fname = _snap->fileNameTrickJobs();

    _model_trickjobs = new TrickModel("sys.exec.out.time", fname);
    _trick_models.append(_model_trickjobs);


    //
    // Resize main window
    //
    resize(1600,800);
    frame->setMaximumWidth(800);

    //
    // Hack to resize notebook of tables to correct size
    //
#if 0
    int w = 0;
    int ncols = _snap->tables.at(3)->columnCount();
    for ( int ii = 0; ii < ncols; ++ii) {
        w += _tvs.at(3)->columnWidth(ii);  // Job Culprits
    }
    w += frame->contentsMargins().left();
    w += frame->contentsMargins().right();
    int margins = frame->frameRect().width() - frame->childrenRect().width();
    w += margins;
    frame->setMaximumWidth(w+8);
#endif

}

SnapWindow::~SnapWindow()
{
    _startup_thread->quit();
    delete _startup_thread;

    delete _model_frames;
    foreach ( TrickModel* m, _models_userjobs ) {
        delete m;
    }
    delete _model_trickjobs;

    foreach (TrickCurveModel* cm, _curve_models ) {
        delete cm;
    }

    delete _snap;
}


void SnapWindow::createMenu()
{
    _menuBar = new QMenuBar;
    _fileMenu = new QMenu(tr("&File"), this);
    _exitAction = _fileMenu->addAction(tr("E&xit"));
    _menuBar->addMenu(_fileMenu);
    connect(_exitAction, SIGNAL(triggered()),this, SLOT(close()));
    setMenuWidget(_menuBar);
}

void SnapWindow::_update_spikejob_plot(const QModelIndex &idx)
{
    QModelIndex jidx = idx.model()->index(idx.row(),0);
    __update_job_plot(jidx);
}

void SnapWindow::_update_topjob_plot(const QModelIndex &idx)
{
    QModelIndex jidx = idx.model()->index(idx.row(),5);
    __update_job_plot(jidx);
}

void SnapWindow::__update_job_plot(const QModelIndex &idx)
{
    QString jobId = idx.model()->data(idx).toString();


    Job* job = 0;
    foreach (Job* jobObj, *_snap->jobs() ) {
        if ( jobObj->job_id() == jobId ) {
            job = jobObj;
            break;
        }
    }

    foreach ( TrickModel* model, _trick_models ) {

        for ( int ii = 0; ii < model->columnCount(); ++ii) {

            QString paramJobID = model->param(ii).name();

            if ( paramJobID == jobId ) {

                while ( _plot_jobs->axisRect()->curveCount() > 0 ) {
                    _plot_jobs->axisRect()->removeCurve(0);
                }

                TrickCurveModel* cm  = new TrickCurveModel(model,0,0,ii,
                                                           paramJobID,
                                                           1.0,    // x scale
                                                           1.0e-6); // y scale
                // Y-axis just says "Job Time (s)"
                _curve_models.append(cm);
                TrickCurve* curve = _plot_jobs->axisRect()->addCurve(cm);
                _plot_jobs->axisRect()->axis(QCPAxis::atLeft)->
                        setLabel("Job Time (s)");

                // Put flatline curve of job freq on plot
                if ( job ) {
                    _freqTimes.clear();
                    _freqVals.clear();
                    bool ok;
                    QCPRange timeRange = curve->xRange(ok);
                    _freqTimes.append(timeRange.lower);
                    _freqTimes.append(timeRange.upper);
                    _freqVals.append(job->freq());
                    _freqVals.append(job->freq());
                    TrickCurve* c = _plot_jobs->axisRect()->
                                    addCurve(&_freqTimes,&_freqVals);
                    Q_UNUSED(c);
                }

                //
                // Put name of job at top right of jobs plot
                //
                QCPPlotTitle *title = new QCPPlotTitle(_plot_jobs);
                QString plotTitle = paramJobID.remove("JOB_");
                title->setText(plotTitle);
                title->setFont(QFont("sans", 10, QFont::Normal));
                QCPLayoutInset* inset = _plot_jobs->axisRect()->insetLayout();
                if ( inset->elementCount() > 0 ) {
                    QCPLayoutElement* el = inset->takeAt(0);
                    delete el;
                }
                inset->addElement(title,Qt::AlignTop | Qt::AlignRight);

                break;
            }
        }
    }

    QCPRange rangeFrame = _plot_frame->
                                 axisRect()->axis(QCPAxis::atBottom)->range();

    if ( rangeFrame.size() > 0 ) {
        _plot_jobs->axisRect()->zoomToFit(rangeFrame);
        _plot_jobs->replot();
    }
}

void SnapWindow::_update_plot_jobs_xrange(const QCPRange &xrange)
{
    _plot_jobs->axisRect()->zoomToFit(xrange);
    _plot_jobs->replot();
}

void SnapWindow::_update_plot_frame_xrange(const QCPRange &xrange)
{
    _plot_frame->axisRect()->zoomToFit(xrange);
    _plot_frame->replot();
}

#if 0
    //
    // Add all thread's jobs runtime curves to plot
    // This is a hack and will leak memory... just saving it
    QString threadTrk = QString("log_frame_userjobs_C%1.trk").arg(col);
    foreach ( TrickModel* m, _models_userjobs ) {
        if ( m->tableName().endsWith(threadTrk) ) {
            for ( int c = 1 ; c < m->columnCount(); ++c ) {
                TrickCurveModel* curve = new TrickCurveModel(m,0,0,c,
                                     m->param(c).name());
                _plot_jobs->axisRect()->addCurve(curve);
            }
            break;
        }
    }
#endif
void SnapWindow::_update_thread_plot(const QModelIndex &idx)
{
    QModelIndex thread_idx = idx.model()->index(idx.row(),0);
    int tid = idx.model()->data(thread_idx).toInt();
    QString thread_name = QString("Thread_%1").arg(tid);
    Thread* thread = _snap->threads()->hash()->value(tid);
    SnapTable* model_thread = thread->runtimeCurve();

    while ( _plot_jobs->axisRect()->curveCount() > 0 ) {
        _plot_jobs->axisRect()->removeCurve(0);
    }

    //
    // Add thread runtime curve to plot
    //
    QCPLayoutInset* inset = _plot_jobs->axisRect()->insetLayout();
    if ( inset->elementCount() > 0 ) {
        QCPLayoutElement* el = inset->takeAt(0);
        delete el;
    }
    _times.clear();
    _vals.clear();
    if ( model_thread ) {
        for ( int i = 0; i < model_thread->rowCount(); ++i) {
            QModelIndex tIdx = model_thread->index(i,0);
            QModelIndex vIdx = model_thread->index(i,1);
            _times.append(model_thread->data(tIdx).toDouble());
            _vals.append(model_thread->data(vIdx).toDouble());
        }

        _plot_jobs->axisRect()->addCurve(&_times,&_vals);
    }

    //
    // Add flatline curve of thread freq (cycle time) to plot
    //
    _freqTimes.clear();
    _freqVals.clear();
    QModelIndex tBeginIdx = model_thread->index(0,0);
    QModelIndex tEndIdx = model_thread->index(model_thread->rowCount()-1,0);
    double tBegin = model_thread->data(tBeginIdx).toDouble();
    double tEnd = model_thread->data(tEndIdx).toDouble();
    _freqTimes.append(tBegin);
    _freqTimes.append(tEnd);
    _freqVals.append(thread->frequency());
    _freqVals.append(thread->frequency());
    _plot_jobs->axisRect()->addCurve(&_freqTimes,&_freqVals);

    QString yLabel = QString("Thread %1 Run Time (s)").arg(tid);
    _plot_jobs->axisRect()->axis(QCPAxis::atLeft)->setLabel(yLabel);
    QCPRange range = _plot_frame->axisRect()->axis(QCPAxis::atBottom)->range();
    _plot_jobs->axisRect()->zoomToFit(range);
    _plot_jobs->replot();
}

void SnapWindow::_update_job_table(const QModelIndex &idx)
{
    QModelIndex time_idx = idx.model()->index(idx.row(),0);
    double time = idx.model()->data(time_idx).toDouble();

    if ( _curr_job_table ) {
        delete _curr_job_table;
        _curr_job_table = _snap->jobTableAtTime(time);
        _curr_job_tv->setModel(_curr_job_table);
    } else {
        _curr_job_table = _snap->jobTableAtTime(time);
        _curr_job_tv = _create_table_view(_curr_job_table);
        _left_lay->addWidget(_curr_job_tv,1,0,1,1);
    }

    _curr_job_tv->hideColumn(0);
    _curr_job_tv->setSortingEnabled(true);
    _curr_job_tv->sortByColumn(1,Qt::AscendingOrder);

    static QSortFilterProxyModel *proxyModel = 0;
    if ( proxyModel ) {
        delete proxyModel;
    }
    proxyModel = new QSortFilterProxyModel(this);
    proxyModel->setSourceModel(_curr_job_table);
    _curr_job_tv->setModel(proxyModel);

    // Zoom in around spike
#ifndef TIMELINE
    _plot_frame->axisRect()->zoomToFit(QCPRange(time-5.0,time+5.0));
    _plot_frame->replot();
#else
    _plot_frame->zoomToFit(QCPRange(time-0.01,time+0.01));
    _plot_frame->replot();
#endif

    connect(_curr_job_tv->selectionModel(),
            SIGNAL(currentChanged(QModelIndex, QModelIndex)),
            this,
            SLOT(_update_spikejob_plot(QModelIndex)));

    // Programmatically select top job in spike
    // so it will automatically plot
    QItemSelectionModel* sm = _curr_job_tv->selectionModel();
    QAbstractItemModel* m = _curr_job_tv->model();
    QModelIndex sidx = m->index(0,0);
    sm->select(sidx,
               QItemSelectionModel::ClearAndSelect|QItemSelectionModel::Rows);
    _curr_job_tv->setCurrentIndex(sidx);

}

QTableView* SnapWindow::_create_table_view(SnapTable *model)
{
    QTableView* tv = new QTableView();
    tv->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tv->setSelectionMode(QAbstractItemView::SingleSelection);
    tv->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    tv->setShowGrid(true);
    tv->setCurrentIndex(QModelIndex());
    tv->horizontalHeader()->setStretchLastSection(false);
    tv->horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);
    tv->setSelectionBehavior(QAbstractItemView::SelectRows);
    tv->setTextElideMode(Qt::ElideMiddle);

    tv->setSortingEnabled(true);
    QSortFilterProxyModel* proxyModel = new QSortFilterProxyModel(this);
    proxyModel->setSourceModel(model);
    tv->setModel(proxyModel);

    if ( model->orientation() == Qt::Horizontal ) {
        tv->verticalHeader()->hide();
    }

    return tv;
}

void SnapWindow::_tab_clicked(int idx)
{
    if ( _curr_job_tv ) {
        if ( idx == _spike_tab_idx ) {
            _curr_job_tv->show();
        } else {
            _curr_job_tv->hide();
        }
    }
}

void SnapWindow::_finishedLoading()
{
    _bar->hide();
}

void SnapWindow::_postLoad()
{
    // I put this hook in thinking I might need it later
}

