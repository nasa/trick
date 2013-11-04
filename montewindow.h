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

    MonteModel* _monteModel;
    void _createMontePages(const QString& dpfile);
    bool _isDP(const QString& fp);
    bool _isRUN(const QString& fp);
    bool _isMONTE(const QString& fp);

    QString _descrPlotTitle(DPPlot* plot);

private slots:
     void _slotDirTreeClicked(const QModelIndex& idx);
     void _selectVarChanged(const QItemSelection& currSelection,
                              const QItemSelection& prevSelection);
     void _varsSearchBoxTextChanged(const QString& rx);
     void _dpSearchBoxTextChanged(const QString& rx);
signals:
};

#endif // SNAPGUI

#endif // MAINWINDOW_H

