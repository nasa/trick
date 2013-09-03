#ifndef SNAP_MAINWINDOW_H
#define SNAP_MAINWINDOW_H

#ifdef SNAPGUI

#include <QApplication>
#include <QMainWindow>
#include <QMenu>
#include <QMenuBar>
#include <QGridLayout>
#include <QTableView>
#include <QAction>
#include <QProgressBar>
#include <QList>

#include "snap.h"
#include "snapplot.h"
#include "trickdatamodel.h"

#include "timeit_linux.h"

class StartUpThread : public QThread
{
  public:
    StartUpThread(Snap* snap, QObject* parent=0) : QThread(parent), _snap(snap) {}

    void run()
    {
        TimeItLinux t; t.start();
        _snap->load();
        t.snap("snaploadtime=");
    }

  private:
    Snap* _snap;
};

class LoadTrickBinaryThread : public QThread
{
  public:
    LoadTrickBinaryThread(TrickDataModel* m,
                   const QString& logname, const QString& rundir,
                   QObject* parent=0) :
        QThread(parent),
        _m(m),
        _logname(logname),
        _rundir(rundir)
    {}

    void run()
    {
        _m->load_binary_trk(_logname,_rundir);
    }

  private:
    TrickDataModel* _m;
    QString _logname;
    QString _rundir;
};

class SnapWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit SnapWindow(const QString& rundir,
                       double start,double stop,
                       QWidget *parent = 0);
    ~SnapWindow();

private:
    Snap* _snap;
    StartUpThread* _startup_thread;
    QProgressBar* _bar;
    QGridLayout* _layout;
    QGridLayout* _left_lay ;

    void createMenu();
    QMenuBar* _menuBar;
    QMenu *_fileMenu;
    QAction *_exitAction;

    LoadTrickBinaryThread* _trickloader ;

    QList<QTableView*> _tvs;
    QTableView *_create_table_view(SnapTable* model);
    SnapTable* _curr_job_table;
    QTableView* _curr_job_tv;

    TrickDataModel* _frames;
    TrickDataModel* _userjobs;
    TrickDataModel* _trickjobs;
    SnapTable* _model_threads;
    QList<TrickDataModel*> _trick_models;

    SnapPlot* _plot_frame;
    SnapPlot* _plot_jobs ;

    int _spike_tab_idx;

signals:
    
private slots:
    void _tab_clicked(int idx);
    void _update_spikejob_plot(const QModelIndex& idx);
    void _update_topjob_plot(const QModelIndex& idx);
    void _update_thread_plot(const QModelIndex& idx);
    void __update_job_plot(const QModelIndex& idx);
    void _update_plot_jobs_xrange(const QCPRange& range);
    void _update_plot_frame_xrange(const QCPRange& range);
    void _update_job_table(const QModelIndex& idx);
    void _finishedLoading();
    void _trkFinished();

};

#endif // SNAPGUI

#endif // MAINWINDOW_H

