#ifndef _SNAPD_H_
#define _SNAPD_H_

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

class SnapDiff : public QMainWindow
{
    Q_OBJECT

public:
    explicit SnapDiff(const QString& run1,
                      const QString& run2,
                         QWidget *parent = 0);
    ~SnapDiff();

private:
    QString _run1;
    QString _run2;
    Runs *_runs1;
    Runs *_runs2;
    Runs *_runs;
    QStringList _commonParams;

    QGridLayout* _layout;
    QGridLayout* _left_lay ;

    void createMenu();
    QMenuBar* _menuBar;
    QMenu *_fileMenu;
    QAction *_pdfAction;
    QAction *_exitAction;
    TimeItLinux _timer;

    QTabWidget* _nbDPVars;

    QStandardItemModel* _createVarsModel(const QStringList &params);
    QStandardItemModel* _varsModel;
    VarsWidget* _varsWidget;
    void _updateVarSelection(const QModelIndex& pageIdx);

    DPTreeWidget* _dpTreeWidget ;
    void _updateDPSelection(const QModelIndex& pageIdx);

    QStandardItemModel* _plotModel;
    QTreeView* _plotTreeView ;
    PlotBookView* _plotBookView;
    QItemSelectionModel* _plotSelectModel;

    MonteModel* _monteModel;
    bool _isRUN(const QString& fp);
    bool _isMONTE(const QString& fp);
    bool _isCurveIdx(const QModelIndex &idx) const;

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

