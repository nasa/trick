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
#include <float.h>
#include "bookmodel.h"
#include "sie_listmodel.h"
#include "datamodel_tv.h"

class TrickView : public QWidget
{
    Q_OBJECT
public:
    explicit TrickView(const QString& trickhost,
                       int trickport,
                       PlotBookModel* bookModel,
                       QItemSelectionModel*  bookSelectModel,
                       QWidget *parent = 0);
    ~TrickView();

    void setDragEnabled(bool isEnabled);

signals:
    
public slots:
    void slotDropEvent(QDropEvent* event,const QModelIndex& idx);

private:
    PlotBookModel* _bookModel;
    QItemSelectionModel*  _bookSelectModel;
    QGridLayout* _gridLayout ;
    QLabel* _waitLabel;
    QLineEdit* _searchBox;
    QListView* _listView ;

    void _createTVModel(const QString& host, int port);
    void _loadSieElement(const QDomElement& element,
                         QList<QDomElement> &path);
    QHash<QString,QDomElement> _name2element;
    QStringList _params;

    QDomDocument _sieDoc;
    QItemSelectionModel* _varsSelectModel;
    SieListModel* _sieListModel;
    TVModel* _tvModel;

    QModelIndex _createPage();
    QModelIndex _addPlotToPage(const QModelIndex& pageIdx,
                               const QString& yName,
                               const QString& yUnit);
    QModelIndex _addCurveToPlot(const QModelIndex& plotIdx,
                                const QString& yName,
                                const QString& yUnit);
    void _changeXOnPlot(const QString& xName, const QString& xUnit,
                        const QModelIndex& xAxisLabelIdx);
    QString _paramUnit(const QString &param);
    QStringList _expandParam(const QString& param);
    QStringList __appendMember(QStringList& paramsIn,
                               const QDomElement&  member);
    QList<QList<int> > _genDimensions(const QDomElement& el);
    void __genDimensions(const QList<int>& dimList,
                         QList<int> &dim,
                         QList<QList<int> >& dimensions);


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
