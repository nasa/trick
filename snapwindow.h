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
    //QGroupBox *horizontalGroupBox;
    //QGroupBox *gridGroupBox;
    //QGroupBox *formGroupBox;
    //QTextEdit *smallEditor;
    //QTextEdit *bigEditor;
    //QLabel *labels[NumGridRows];
    //QLineEdit *lineEdits[NumGridRows];
    //QPushButton *buttons[NumButtons];
    //QDialogButtonBox *buttonBox;

    QMenu *_fileMenu;
    QAction *_exitAction;

    void _zoom_to_fit(QCustomPlot* plot,
                      double xmin, double xmax, double ymin, double ymax);

signals:
    
public slots:
    
};

#endif // SNAPGUI

#endif // MAINWINDOW_H

