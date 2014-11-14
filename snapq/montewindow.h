#ifndef RUNS_WINDOW_H
#define RUNS_WINDOW_H

#include <QApplication>
#include <QMainWindow>
#include <QMenu>
#include <QMenuBar>
#include <QLineEdit>
#include <QGridLayout>
#include <QList>
#include <QStandardItemModel>
#include <QFileSystemModel>
#include <QItemSelectionModel>
#include <QSortFilterProxyModel>
#include <QRegExp>
#include <QTreeView>
#include <QTableView>
#include <QListView>
#include <QFileInfo>
#include <QTabWidget>
#include <QStringList>
#include <QFileDialog>

#include "libsnapdata/runs.h"
#include "libqplot/dp.h"
#include "libqplot/plotbookview.h"
#include "libqplot/dptreewidget.h"
#include "libqplot/varswidget.h"

#include "libsnapdata/timeit_linux.h"

class MonteWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MonteWindow(const QString& montedir,
                         QWidget *parent = 0);
    ~MonteWindow();

private:
    QString _montedir;
    Runs* _runs;
    QGridLayout* _layout;
    QGridLayout* _left_lay ;

    void createMenu();
    QMenuBar* _menuBar;
    QMenu *_fileMenu;
    QAction *_pdfAction;
    QAction *_exitAction;
    TimeItLinux _timer;

    QTabWidget* _nbDPVars;

    QStandardItemModel* _createVarsModel(MonteModel* mm);
    QStandardItemModel* _varsModel;
    VarsWidget* _varsWidget;
    void _updateVarSelection(const QModelIndex& pageIdx);

    DPTreeWidget* _dpTreeWidget;
    void _updateDPSelection(const QModelIndex& pageIdx);

    QStandardItemModel* _plotModel;
    QTreeView* _plotTreeView ;
    PlotBookView* _plotBookView;
    QItemSelectionModel* _plotSelectModel;

    MonteModel* _monteModel;
    bool _isRUN(const QString& fp);
    bool _isMONTE(const QString& fp);

    int currSelectedRun();

    bool _isCurveIdx(const QModelIndex& idx) const;

private slots:
     void _plotSelectModelSelectionChanged(const QItemSelection& currSel,
                                          const QItemSelection& prevSel);
     void _plotSelectModelCurrentChanged(const QModelIndex& currIdx,
                                        const QModelIndex& prevIdx);
     void _plotModelRowsAboutToBeRemoved(const QModelIndex& pidx,
                                         int start, int end);
     void _savePdf();
};

#endif // MAINWINDOW_H

