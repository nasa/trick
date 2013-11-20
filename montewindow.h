#ifndef MONTE_WINDOW_H
#define MONTE_WINDOW_H

#ifdef SNAPGUI

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

#include "monte.h"
#include "plotpage.h"
#include "dp.h"
#include "plotbookview.h"
#include "dpfilterproxymodel.h"

#include "timeit_linux.h"

class MonteWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MonteWindow(const QString& montedir, QWidget *parent = 0);
    ~MonteWindow();

private:
    QString _montedir;
    Monte* _monte;
    QGridLayout* _layout;
    QGridLayout* _left_lay ;

    void createMenu();
    QMenuBar* _menuBar;
    QMenu *_fileMenu;
    QAction *_exitAction;
    PlotPage* _plot_monte ;
    TimeItLinux _timer;

    QTabWidget* _nbDPVars;

    QStandardItemModel* _createVarsModel(MonteModel* mm);
    QStandardItemModel* _varsModel;
    QSortFilterProxyModel* _varsFilterModel;
    QItemSelectionModel* _varsSelectModel;
    QLineEdit* _varsSearchBox;
    QListView* _varsListView ;

    QFileSystemModel* _dpModel ;
    DPFilterProxyModel* _dpFilterModel;
    QTreeView* _dpTreeView ;
    QLineEdit* _dpSearchBox;

    QStandardItemModel* _plotModel;
    QTreeView* _plotTreeView ;
    PlotBookView* _plotBookView;
    QItemSelectionModel* _plotSelectModel;
    int _currQPIdx;

    MonteModel* _monteModel;
    void _createMontePages(const QString& dpfile);
    bool _isDP(const QString& fp);
    bool _isRUN(const QString& fp);
    bool _isMONTE(const QString& fp);

    QStandardItemModel* _monteInputsModel; // from monte_runs file
    QTableView* _monteInputsView ;
    QItemSelectionModel* _monteInputsSelectModel;

    QString _descrPlotTitle(DPPlot* plot);

    QStandardItem* _createQPItem();
    void _addPlotOfVarQPItem(QStandardItem* qpItem,
                          const QModelIndex &varIdx);
    QModelIndex _findPageWithCurve(const QString& curveName);

private slots:
     void _dpTreeViewClicked(const QModelIndex& idx);
     void _dpSearchBoxTextChanged(const QString& rx);
     void _varsSelectModelSelectionChanged(
                              const QItemSelection& currVarSelection,
                              const QItemSelection& prevVarSelection);
     void _varsSearchBoxTextChanged(const QString& rx);
     void _monteInputsSelectModelCurrentChanged(const QModelIndex& curr,
                                     const QModelIndex& prev);
     void _monteInputsViewHeaderSectionClicked(int section);
     void _plotSelectModelSelectionChanged(const QItemSelection& currSel,
                                          const QItemSelection& prevSel);
signals:
};

#endif // SNAPGUI

#endif // MAINWINDOW_H

