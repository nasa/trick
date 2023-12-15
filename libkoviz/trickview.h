#ifndef TRICKVIEW_H
#define TRICKVIEW_H

#include <QWidget>
#include <QStandardItemModel>
#include <QGridLayout>
#include <QLineEdit>
#include <QListView>
#include <QTcpSocket>
#include <QDomDocument>
#include <QHash>
#include <QItemSelectionModel>
#include <QSortFilterProxyModel>
#include <QFuture>
#include <QtConcurrent/QtConcurrent>
#include <QLabel>
#include <QMetaObject>
#include <QApplication>
#include <QDropEvent>
#include <QMessageBox>
#include <float.h>
#include "bookmodel.h"
#include "sie_listmodel.h"
#include "datamodel_tv.h"

class TrickView : public QWidget
{
    Q_OBJECT
public:
    explicit TrickView(SieListModel* sieModel,
                       TVModel* tvModel,
                       PlotBookModel* bookModel,
                       QItemSelectionModel*  bookSelectModel,
                       QWidget *parent = 0);
    ~TrickView();

    void setDragEnabled(bool isEnabled);

signals:
    
public slots:
    void slotDropEvent(QDropEvent* event,const QModelIndex& idx);

private:
    SieListModel* _sieModel;
    TVModel* _tvModel;
    PlotBookModel* _bookModel;
    QItemSelectionModel*  _bookSelectModel;
    QGridLayout* _gridLayout ;
    QLabel* _waitLabel;
    QLineEdit* _searchBox;
    QListView* _listView ;

    QItemSelectionModel* _varsSelectModel;

    int _addParamToBook(const QString& param);
    QModelIndex _createPage();
    QModelIndex _addPlotToPage(const QModelIndex& pageIdx,
                               const QString& yName,
                               const QString& yUnit);
    QModelIndex _addCurveToPlot(const QModelIndex& plotIdx,
                                const QString& yName,
                                const QString& yUnit);
    void _changeXOnPlot(const QString& xName, const QString& xUnit,
                        const QModelIndex& xAxisLabelIdx);

private slots:
     void _tvSearchBoxTextChanged(const QString& rx);
     void _tvSearchBoxReturnPressed();
     void _tvSelectionChanged(const QItemSelection& currVarSelection,
                              const QItemSelection& prevVarSelection);
     void _setMessageLabel(const QString& msg);
     void _tvModelRowAppended(const QModelIndex& parent, int start, int end);
     void _bookModelRowsAboutToBeRemoved(const QModelIndex& parent,
                                         int first, int last);
};

#endif // TRICKVIEW_H
