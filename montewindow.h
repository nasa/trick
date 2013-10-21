#ifndef MONTE_WINDOW_H
#define MONTE_WINDOW_H

#ifdef SNAPGUI

#include <QApplication>
#include <QMainWindow>
#include <QMenu>
#include <QMenuBar>
#include <QGridLayout>
#include <QList>
#include <QFileSystemModel>
#include <QItemSelectionModel>
#include <QTreeView>
#include <QFileInfo>
#include <QTabWidget>

#include "monte.h"
#include "plotpage.h"
#include "dp.h"

#include "timeit_linux.h"

class MonteWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MonteWindow(const QString& montedir, QWidget *parent = 0);
    ~MonteWindow();

private:
    QString _montedir;
    QGridLayout* _layout;
    QGridLayout* _left_lay ;

    void createMenu();
    QMenuBar* _menuBar;
    QMenu *_fileMenu;
    QAction *_exitAction;
    PlotPage* _plot_monte ;
    TimeItLinux _timer;
    QFileSystemModel* _treemodel ;
    QItemSelectionModel* _selectModel;
    QTreeView* _treeview ;
    QTabWidget* _nb;
    MonteModel* _monteModel;
    void _createMontePages(const QString& dpfile, const QString& datadir);
    bool _isDP(const QString& fp);
    bool _isRUN(const QString& fp);
    bool _isMONTE(const QString& fp);

private slots:
     void _slotDirTreeClicked(const QModelIndex& idx);

signals:
};

#endif // SNAPGUI

#endif // MAINWINDOW_H

