#ifndef MONTE_WINDOW_H
#define MONTE_WINDOW_H

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
#include "plotpage.h"
#include "trickmodel.h"
#include "timelineloader.h"
#include "timelineplot.h"
#include "monte.h"
#include "dp.h"

#include "timeit_linux.h"

class MonteWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MonteWindow(QWidget *parent = 0);
    ~MonteWindow();

private:
    QGridLayout* _layout;
    QGridLayout* _left_lay ;

    void createMenu();
    QMenuBar* _menuBar;
    QMenu *_fileMenu;
    QAction *_exitAction;
    PlotPage* _plot_monte ;
    TimeItLinux _timer;

signals:
};

#endif // SNAPGUI

#endif // MAINWINDOW_H

