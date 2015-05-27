#ifndef PLOTMAINWINDOW_H
#define PLOTMAINWINDOW_H

#include <QMainWindow>
#include <QMenu>
#include <QMenuBar>
#include <QFrame>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QSplitter>
#include <QList>
#include <QStandardItemModel>
#include <QFileInfo>
#include <QStringList>
#include <QFileDialog>
#include <QLineEdit>
#include <QSizePolicy>

#include "libsnapdata/monte.h"
#include "libqplot/dp.h"
#include "libqplot/plotbookview.h"
#include "libqplot/plotbookmodel.h"
#include "libqplot/dptreewidget.h"
#include "libqplot/varswidget.h"
#include "libqplot/monteinputsview.h"
#include "libqplot/rangeinput.h"

#include "libsnapdata/timeit_linux.h"

class PlotMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit PlotMainWindow( const QString& presentation,
                             const QString& dpDir,
                             const QStringList& dpFiles,
                             const QStringList& titles,
                             MonteModel* monteModel,
                             QStandardItemModel* varsModel,
                             QStandardItemModel* monteInputsModel=0,
                             QWidget *parent = 0);

     void savePdf(const QString& fname);

    ~PlotMainWindow();

private:
    QString _presentation;
    QString _dpDir;
    QStringList _dpFiles;
    MonteModel* _monteModel;
    QStandardItemModel* _varsModel;
    QStandardItemModel* _monteInputsModel;
    MonteInputsView* _monteInputsView;
    QHeaderView* _monteInputsHeaderView;

    QGridLayout* _layout;
    QGridLayout* _left_lay ;

    void createMenu();
    QMenuBar* _menuBar;
    QMenu *_fileMenu;
    QAction *_pdfAction;
    QAction *_exitAction;
    TimeItLinux _timer;

    QTabWidget* _nbDPVars;
    VarsWidget* _varsWidget;
    QFrame* _dpFrame ;
    DPTreeWidget* _dpTreeWidget;

    PlotBookModel* _plotModel;
    //QTreeView* _plotTreeView ;
    PlotBookView* _plotBookView;
    QItemSelectionModel* _plotSelectModel;

    bool _isRUN(const QString& fp);
    bool _isMONTE(const QString& fp);

    bool _isCurveIdx(const QModelIndex& idx) const;

private slots:
     void _nbCurrentChanged(int i);
     void _plotSelectModelSelectionChanged(const QItemSelection& currSel,
                                          const QItemSelection& prevSel);
     void _plotModelRowsAboutToBeRemoved(const QModelIndex& pidx,
                                         int start, int end);
     void _savePdf();

     void _startTimeChanged(double startTime);
     void _stopTimeChanged(double stopTime);
     void _monteInputsHeaderViewClicked(int section);
};

#endif // PLOTMAINWINDOW_H
