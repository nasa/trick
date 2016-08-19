#ifndef DPTREEWIDGET_H
#define DPTREEWIDGET_H

#include <QGridLayout>
#include <QLineEdit>
#include <QTreeView>
#include <QFileSystemModel>
#include <QFileInfo>
#include <QHash>
#include "dp.h"
#include "dpfilterproxymodel.h"
#include "libsnap/montemodel.h"
#include "libsnap/plotbookmodel.h"
#include "libsnap/utils.h"

// Data Products TreeView with Search Box

class DPTreeWidget : public QWidget
{
    Q_OBJECT
public:
    explicit DPTreeWidget(const QString& timeName,
                          const QString& dpDirName,
                          const QStringList& dpFiles,
                          QStandardItemModel* dpVarsModel,
                          MonteModel* monteModel,
                          PlotBookModel* bookModel,
                          QItemSelectionModel*  bookSelectModel,
                          QWidget *parent = 0);
    ~DPTreeWidget();
    
signals:
    
public slots:

private:
    QString _timeName;
    QString _dpDirName;
    QStringList _dpFiles;
    QStandardItemModel* _dpVarsModel;
    QDir* _dir;
    MonteModel* _monteModel;
    PlotBookModel* _bookModel;
    QItemSelectionModel*  _bookSelectModel;
    QGridLayout* _gridLayout ;
    QLineEdit* _searchBox;
    QTreeView* _dpTreeView ;
    DPFilterProxyModel* _dpFilterModel;
    QFileSystemModel* _dpModel ;
    QModelIndex _dpModelRootIdx;

    void _setupModel();
    void _createDP(const QString& dpfile);
    void _createDPPages(const QString& dpfile);
    void _createDPTables(const QString& dpfile);
    QStandardItem* _addChild(QStandardItem* parentItem,
                   const QString& childTitle,
                   const QVariant &childValue=QVariant());
    void _addCurve(QStandardItem* curvesItem, DPCurve* dpcurve,
                   MonteModel* monteModel, int runId, int curveId,
                   const QList<QColor>& colors);
    bool _isDP(const QString& fp);
    QString _descrPlotTitle(DPPlot* plot);

    static QString _err_string;
    static QTextStream _err_stream;

private slots:
    void _searchBoxTextChanged(const QString &rx);
     void _dpTreeViewClicked(const QModelIndex& idx);
     void _dpTreeViewCurrentChanged(const QModelIndex &currIdx,
                                    const QModelIndex &prevIdx);

};

#endif // DPTREEVIEW_H
