#include "snapwindow.h"

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
    _startup_thread->start();

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
        if ( _snap->tables.at(ii)->tableName() == "Thread Runtimes" ) {
            _model_threads = _snap->tables.at(ii);
            continue;
        }
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
    _model_frames = new TrickModel(fname) ;
    _plot_frame = new SnapPlot(f2);

    lay2->addWidget(_plot_frame,0,0,1,1);

    TrickCurveModel* frame_curve = new TrickCurveModel(_model_frames,0,0,1,
                                                       "Frame Scheduled Time",
                                                       1.0e-6);

    _plot_frame->addCurve(frame_curve);
    _plot_frame->xAxis->setLabel("Time (s)");
    _plot_frame->yAxis->setLabel("Frame Scheduled Time (s)");
    _plot_frame->zoomToFit();

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
        TrickModel* m = new TrickModel(fname);
        _models_userjobs.append(m);
        _trick_models.append(m);
    }
    _timer.snap("load_userjobs=");
    _plot_jobs = new SnapPlot(f2);
    lay2->addWidget(_plot_jobs,1,0,1,1);
    _plot_jobs->xAxis->setLabel("Time (s)");
    _plot_jobs->yAxis->setLabel("Job Time (s)");
    connect(_plot_frame->xAxis,SIGNAL(rangeChanged(QCPRange)),
            this,SLOT(_update_plot_jobs_xrange(QCPRange)));
    connect(_plot_jobs->xAxis,SIGNAL(rangeChanged(QCPRange)),
            this,SLOT(_update_plot_frame_xrange(QCPRange)));


    fname = _snap->fileNameTrickJobs();

    _model_trickjobs = new TrickModel(fname);
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
    QString jobname = idx.model()->data(idx).toString();

    foreach ( TrickModel* model, _trick_models ) {
        for ( int ii = 0; ii < model->columnCount(); ++ii) {
            QString name = model->headerData
                    (ii,Qt::Horizontal,Param::Name).toString();
            if ( name == jobname ) {
                if ( _plot_jobs->curveCount() > 0 ) {
                    _plot_jobs->removeCurve(0);
                }

                TrickCurveModel* cm  = new TrickCurveModel(model,0,0,ii,
                                                           name,1.0e-6);
                _curve_models.append(cm);
                _plot_jobs->addCurve(cm);
                _plot_jobs->yAxis->setLabel("Job Time (s)");
            }
        }
    }

    _plot_jobs->zoomToFit(_plot_frame->xAxis->range());
    _plot_jobs->replot();
}

void SnapWindow::_update_plot_jobs_xrange(const QCPRange &xrange)
{
    _plot_jobs->zoomToFit(xrange);
}

void SnapWindow::_update_plot_frame_xrange(const QCPRange &xrange)
{
    _plot_frame->zoomToFit(xrange);
}

void SnapWindow::_update_thread_plot(const QModelIndex &idx)
{
    QModelIndex thread_idx = idx.model()->index(idx.row(),0);
    int tid = idx.model()->data(thread_idx).toInt();
    QString thread_name = QString("Thread_%1").arg(tid);
    int col = -1 ;
    for ( int c = 1; c < _model_threads->columnCount(); ++c) {
        QString name = _model_threads->headerData(c,Qt::Horizontal).toString();
        if ( name == thread_name ) {
            col = c ;
            break;
        }
    }
    if ( col < 0 ) {
        qDebug() << "snap [bad scoobies]: this shouldn't happen";
        qDebug() << "     probably means thread runtime headerData() has changed";
    }

    if ( _plot_jobs->curveCount() > 0 ) {
        _plot_jobs->removeCurve(0);
    }
    // TODO:!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    // thread is missing plot since using TrickModel and not SnapTable.
    // Need to put iterator in SnapTable so TrickCurve can use both
    // models the same way for getting data.
    // Also, need valueScalefactor in addCurve (see below)
    // and the valueScalefactor has to be in the iterator

    //_plot_jobs->addCurve(_model_threads,0,0,col,1.0e-6);
    _plot_jobs->yAxis->setLabel("Thread Time (s)");
    _plot_jobs->zoomToFit(_plot_frame->xAxis->range());
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
    _plot_frame->zoomToFit(QCPRange(time-5.0,time+5.0));
#else
    _plot_frame->zoomToFit(QCPRange(time-0.01,time+0.01));
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

