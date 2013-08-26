#ifndef SNAP_MAINWINDOW_H
#define SNAP_MAINWINDOW_H

#ifdef SNAPGUI

#include <QMainWindow>
#include <QMenu>
#include <QMenuBar>
#include <QGridLayout>
#include <QTableView>
#include <QAction>
#include <QProgressBar>

#include "snap.h"
#include "snapplot.h"
#include "trickdatamodel.h"

class SnapWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit SnapWindow(Snap *snap, QWidget *parent = 0);
    ~SnapWindow();

private:
    Snap* _snap;
    QProgressBar* _bar;
    QGridLayout* _layout;
    QGridLayout* _left_lay ;

    void createMenu();
    QMenuBar* _menuBar;
    QMenu *_fileMenu;
    QAction *_exitAction;

    QList<QTableView*> _tvs;
    QTableView *_create_table_view(QAbstractItemModel* model,
                                   Qt::Orientation orientation);

    TrickDataModel* _frames;
    TrickDataModel* _userjobs;
    TrickDataModel* _trickjobs;

    SnapPlot* _plot_jobs ;

signals:
    
private slots:
    void _update_job_plot(const QModelIndex& idx);
    void _finishedLoading();
    void _trkFinished();

};

#endif // SNAPGUI

#endif // MAINWINDOW_H

