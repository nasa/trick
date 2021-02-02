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
#include <QHash>
#include <QStandardItemModel>
#include <QFileInfo>
#include <QStringList>
#include <QFileDialog>
#include <QMessageBox>
#include <QLineEdit>
#include <QSizePolicy>
#include <QTreeView>
#include <QFile>
#include <QTextStream>
#include <QDate>
#include <QSettings>
#include <QProcess>
#include <QTcpSocket>
#include <QStatusBar>

#include "monte.h"
#include "dp.h"
#include "bookmodel.h"
#include "dptreewidget.h"
#include "varswidget.h"
#include "monteinputsview.h"
#include "timeinput.h"
#include "bookview.h"
#include "runs.h"
#include "timecom.h"
#include "videowindow.h"

class PlotMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit PlotMainWindow( const QString& trickhost,
                             uint trickport,
                             double trickoffset,
                             const QString& videoFileName,
                             double videoOffset,
                             const QString& excludePattern,
                             const QString& filterPattern,
                             const QString& scripts,
                             bool isDebug,
                             bool isPlotAllVars,
                             const QStringList& timeNames,
                             double startTime,
                             double stopTime,
                             double timeMatchTolerance,
                             double frequency,
                             const QHash<QString,QVariant>& shifts,//rundir->val
                             const QString& presentation,
                             const QString& dpDir,
                             const QStringList& dpFiles,
                             const QStringList& titles,
                             const QStringList& legends,
                             const QStringList& colors,
                             const QStringList& linestyles,
                             const QStringList& symbolstyles,
                             const QStringList& groups,
                             const QString& orient,
                             bool isLegend,
                             const QString& foreground,
                             const QString& background,
                             bool isShowTables,
                             bool isShowPageTitle,
                             const QString &isShowPlotLegend,
                             QStringList unitOverrides,
                             QString map,
                             QString mapFile,
                             Runs* runs,
                             QStandardItemModel* varsModel,
                             QStandardItemModel* monteInputsModel=0,
                             QWidget *parent = 0);

     void savePdf(const QString& fname);

    ~PlotMainWindow();

protected:
     virtual void closeEvent(QCloseEvent *event);


private:
    const QString& _trickhost;
    uint _trickport;
    double _trickoffset;
    QString _videoFileName;
    double _videoOffset;
    QString _excludePattern;
    QString _filterPattern;
    QString _scripts;
    bool _isDebug;
    QStringList _timeNames;
    QString _presentation;
    QString _dpDir;
    QStringList _dpFiles;
    QStringList _titles;
    QString _foreground;
    QString _background;
    bool _isShowTables;
    bool _isShowPageTitle;
    QString _isShowPlotLegend;
    QStringList _unitOverrides;
    QString _map;
    QString _mapFile;
    Runs* _runs;
    QStandardItemModel* _varsModel;
    QStandardItemModel* _monteInputsModel;
    MonteInputsView* _monteInputsView;
    QHeaderView* _monteInputsHeaderView;
    TimeInput* _timeInput;

    QGridLayout* _layout;
    QGridLayout* _left_lay ;

    void createMenu();
    QMenuBar* _menuBar;
    QMenu *_fileMenu;
    QMenu *_optsMenu;
    QMenu *_scriptsMenu;
    QAction *_dpAction;
    QAction *_pdfAction;
    QAction *_sessionAction;
    QAction *_openVideoAction;
    QAction *_exitAction;
    QAction *_showLiveCoordAction;
    QAction *_clearPlotsAction;
    QAction *_clearTablesAction;
    QAction *_plotAllVarsAction;

    QTabWidget* _nbDPVars;
    VarsWidget* _varsWidget;
    QFrame* _dpFrame ;
    DPTreeWidget* _dpTreeWidget;

    PlotBookModel* _bookModel;
    QTreeView* _plotTreeView ;
    BookView* _bookView;

    QStatusBar* _statusBar;

    bool _isRUN(const QString& fp);
    bool _isMONTE(const QString& fp);

    void _readSettings();
    void _writeSettings();
    void _readMainWindowSettings();
    void _readVideoWindowSettings();

    QModelIndex _currCurveIdx();

    TimeCom* bviscom;

    VideoWindow* vidView;
    QTcpSocket* _vsSocket ;

    void _openVideoFile(const QString& fname);


private slots:
     void _nbCurrentChanged(int i);
     void _bookModelRowsAboutToBeRemoved(const QModelIndex& pidx,
                                         int start, int end);
     void _saveDP();
     void _savePdf();
     void _saveSession();
     void _openVideo();
     void _toggleShowLiveCoord();
     void _clearPlots();
     void _clearTables();
     void _launchScript(QAction *action);
     void _plotAllVars();

     void _startTimeChanged(double startTime);
     void _liveTimeChanged(double liveTime);
     void _stopTimeChanged(double stopTime);
     void _liveTimeNext();
     void _liveTimePrev();
     void _monteInputsHeaderViewClicked(int section);
     void _monteInputsViewCurrentChanged(const QModelIndex& currIdx,
                                         const QModelIndex& prevIdx);
     void _bookViewCurrentChanged(const QModelIndex& currIdx,
                                  const QModelIndex& prevIdx);
     void _bookModelRowsInserted(const QModelIndex& pidx, int start, int end);
     void _bookModelDataChanged(const QModelIndex &topLeft,
                                const QModelIndex &bottomRight,
                                const QVector<int> &roles = QVector<int>());

     void setTimeFromVideo(double time);
     void setTimeFromBvis(double time);
     void _scriptError(QProcess::ProcessError error);
     void _vsRead();
};

#endif // PLOTMAINWINDOW_H
