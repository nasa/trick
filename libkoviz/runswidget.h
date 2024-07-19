#ifndef RUNSWIDGET_H
#define RUNSWIDGET_H

#include <QWidget>
#include <QStandardItemModel>
#include <QGridLayout>
#include <QLineEdit>
#include <QListView>
#include <QFileInfo>
#include <QDir>
#include <QTreeView>
#include <QFileSystemModel>
#include <QSortFilterProxyModel>
#include <QRegExp>

#include "bookmodel.h"
#include "monteinputsview.h"
#include "runs.h"

//class RunsWidgetFilterProxyModel;  // fwd declaration, see class below

class RunsWidget : public QWidget
{
    Q_OBJECT
public:
    explicit RunsWidget(Runs* runs,
                        PlotBookModel* bookModel,
                        MonteInputsView* monteInputsView,
                        QWidget *parent = 0);
    ~RunsWidget();

signals:
    
public slots:

private:
    QStandardItemModel* _runsModel;
    Runs* _runs;
    PlotBookModel* _bookModel;
    MonteInputsView* _monteInputsView;
    QGridLayout* _gridLayout ;
    QLineEdit* _searchBox;
    QListView* _listView ;
    QTreeView* _fileTreeView;
    QFileSystemModel* _fileModel;
    QSortFilterProxyModel* _filterModel;

    QStandardItemModel* _createVarsModel(Runs* runs);


private slots:
     void _runsSearchBoxReturnPressed();
};

class RunsWidgetFilterProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    explicit RunsWidgetFilterProxyModel(Runs* runs, QObject *parent = nullptr) :
        _runs(runs),
        QSortFilterProxyModel(parent) {}

    /*
    void setFilterRegExp(const QRegExp &regExp) {
        _regExp = regExp;
        invalidateFilter();
    }
    */

protected:
    bool filterAcceptsRow(int row,const QModelIndex &srcIdx) const;

private:
    QRegExp _regExp;
    Runs* _runs;

    bool _isDirAccept(const QString &path,
                        const QRegularExpression &rx,
                        int depth) const;
};


#endif // RUNSWIDGET_H
