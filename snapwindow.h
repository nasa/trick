#ifndef SNAP_MAINWINDOW_H
#define SNAP_MAINWINDOW_H

#ifdef SNAPGUI

#include <QMainWindow>
#include <QMenu>
#include <QMenuBar>
#include <QGridLayout>
#include <QTableView>
#include <QAction>

#include "snap.h"
#include "qplot/qcustomplot.h"

class SnapWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit SnapWindow(Snap *snap, QWidget *parent = 0);
    QTableView* tableView;

private:
    Snap* _snap;
    QGridLayout* _layout;

    void createMenu();
    QMenuBar* _menuBar;
    QMenu *_fileMenu;
    QAction *_exitAction;

    QList<QTableView*> _tvs;
    QTableView *_create_table_view(QAbstractItemModel* model,
                                   Qt::Orientation orientation);
    void _zoom_to_fit(QCustomPlot* plot,
                      double xmin, double xmax, double ymin, double ymax);

signals:
    
public slots:
    
};

#endif // SNAPGUI

#endif // MAINWINDOW_H

