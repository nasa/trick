#ifndef DPTREEWIDGET_H
#define DPTREEWIDGET_H

#include <QGridLayout>
#include <QLineEdit>
#include <QTreeView>
#include <QFileSystemModel>
#include <QFileInfo>
#include "dp.h"
#include "dpfilterproxymodel.h"
#include "libsnapdata/montemodel.h"
#include "libqplot/plotbookmodel.h"

// Data Products TreeView with Search Box

class DPTreeWidget : public QWidget
{
    Q_OBJECT
public:
    explicit DPTreeWidget(const QString& dpDirName,
                          const QStringList& dpFiles,
                          QStandardItemModel* dpVarsModel,
                          MonteModel* monteModel,
                          PlotBookModel* plotModel,
                          QItemSelectionModel*  plotSelectModel,
                          QWidget *parent = 0);
    ~DPTreeWidget();
    
signals:
    
public slots:

private:
    QString _dpDirName;
    QStringList _dpFiles;
    QStandardItemModel* _dpVarsModel;
    QDir* _dir;
    MonteModel* _monteModel;
    PlotBookModel* _plotModel;
    QItemSelectionModel*  _plotSelectModel;
    QGridLayout* _gridLayout ;
    QLineEdit* _searchBox;
    QTreeView* _dpTreeView ;
    DPFilterProxyModel* _dpFilterModel;
    QFileSystemModel* _dpModel ;
    QModelIndex _dpModelRootIdx;

    void _setupModel();
    void _createDPPages(const QString& dpfile);
    bool _isDP(const QString& fp);
    QString _descrPlotTitle(DPPlot* plot);

private slots:
    void _searchBoxTextChanged(const QString &rx);
     void _dpTreeViewClicked(const QModelIndex& idx);
     void _dpTreeViewCurrentChanged(const QModelIndex &currIdx,
                                           const QModelIndex &prevIdx);

};

#endif // DPTREEVIEW_H
