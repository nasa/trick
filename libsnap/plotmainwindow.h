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
#include <QTreeView>

#include "libsnap/monte.h"
#include "libsnap/dp.h"
#include "libsnap/bookmodel.h"
#include "libsnap/dptreewidget.h"
#include "libsnap/varswidget.h"
#include "libsnap/monteinputsview.h"
#include "libsnap/rangeinput.h"
#include "libsnap/bookview.h"

class PlotMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit PlotMainWindow( const QString& timeName,
                             double startTime,
                             double stopTime,
                             const QString& presentation,
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
    QString _timeName;
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

    QTabWidget* _nbDPVars;
    VarsWidget* _varsWidget;
    QFrame* _dpFrame ;
    DPTreeWidget* _dpTreeWidget;

    PlotBookModel* _bookModel;
    QTreeView* _plotTreeView ;
    BookView* _bookView;

    bool _isRUN(const QString& fp);
    bool _isMONTE(const QString& fp);

private slots:
     void _nbCurrentChanged(int i);
     void _plotModelRowsAboutToBeRemoved(const QModelIndex& pidx,
                                         int start, int end);
     void _savePdf();

     void _startTimeChanged(double startTime);
     void _stopTimeChanged(double stopTime);
     void _monteInputsHeaderViewClicked(int section);
     void _monteInputsViewCurrentChanged(const QModelIndex& currIdx,
                                         const QModelIndex& prevIdx);
     void _bookViewCurrentChanged(const QModelIndex& currIdx,
                                  const QModelIndex& prevIdx);
     void _bookModelRowsInserted(const QModelIndex& pidx, int start, int end);
};

#endif // PLOTMAINWINDOW_H
