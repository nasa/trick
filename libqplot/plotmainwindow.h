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

#include "libsnapdata/monte.h"
#include "libqplot/dp.h"
#include "libqplot/plotbookview.h"
#include "libqplot/plotbookmodel.h"
#include "libqplot/dptreewidget.h"
#include "libqplot/varswidget.h"
#include "libqplot/monteinputsview.h"

#include "libsnapdata/timeit_linux.h"

class PlotMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit PlotMainWindow( const QString& dpDir,
                             const QStringList& titles,
                             MonteModel* monteModel,
                             QStandardItemModel* varsModel,
                             QStandardItemModel* monteInputsModel=0,
                             QWidget *parent = 0);

    explicit PlotMainWindow( const QStringList& dpFiles,
                             const QStringList& titles,
                             MonteModel* monteModel,
                             QStandardItemModel* varsModel,
                             QStandardItemModel* monteInputsModel=0,
                             QWidget *parent = 0);

     void savePdf(const QString& fname);

    ~PlotMainWindow();

private:
    const QString& _dpDir;
    const QStringList& _dpFiles;
    MonteModel* _monteModel;
    QStandardItemModel* _varsModel;
    QStandardItemModel* _monteInputsModel;
    MonteInputsView* _monteInputsView;
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
    void _updateVarSelection(const QModelIndex& pageIdx);

    DPTreeWidget* _dpTreeWidget;
    void _updateDPSelection(const QModelIndex& pageIdx);

    PlotBookModel* _plotModel;
    //QTreeView* _plotTreeView ;
    PlotBookView* _plotBookView;
    QItemSelectionModel* _plotSelectModel;

    bool _isRUN(const QString& fp);
    bool _isMONTE(const QString& fp);

    void _selectCurrentRunOnPageItem(QStandardItem* pageItem);

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

#endif // PLOTMAINWINDOW_H
