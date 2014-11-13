#ifndef DPTREEWIDGET_H
#define DPTREEWIDGET_H

#include <QFrame>
#include <QGridLayout>
#include <QLineEdit>
#include <QTreeView>
#include <QFileSystemModel>
#include <QFileInfo>
#include "dpfilterproxymodel.h"
#include "dp.h"
#include "libsnapdata/montemodel.h"

// Data Products TreeView with Search Box

class DPTreeWidget : public QWidget
{
    Q_OBJECT
public:
    explicit DPTreeWidget(const QString& dirName,
                          MonteModel* monteModel,
                          QStandardItemModel* plotModel,
                          QItemSelectionModel*  _plotSelectModel,
                          QWidget *parent = 0);
    ~DPTreeWidget();
    
signals:
    
public slots:

private:
    QString _dirName;
    QDir* _dir;
    MonteModel* _monteModel;
    QStandardItemModel* _plotModel;
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
