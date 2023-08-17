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
#include <float.h>
#include "bookmodel.h"
#include "sie_listmodel.h"
#include "datamodel_tv.h"

class TrickView : public QWidget
{
    Q_OBJECT
public:
    explicit TrickView(PlotBookModel* bookModel,
                       QItemSelectionModel*  bookSelectModel,
                       QWidget *parent = 0);
    ~TrickView();

signals:
    
public slots:

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

    QItemSelectionModel* _varsSelectModel;
    SieListModel* _sieListModel;
    TVModel* _tvModel;

    void _createPage(const QString& yName);


private slots:
     void _tvSearchBoxTextChanged(const QString& rx);
     void _tvSearchBoxReturnPressed();
     void _tvSelectionChanged(const QItemSelection& currVarSelection,
                              const QItemSelection& prevVarSelection);
     void _setWaitLabel(const QString& msg);
};

#endif // TRICKVIEW_H
