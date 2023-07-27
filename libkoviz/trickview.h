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
#include "bookmodel.h"

class TrickView : public QWidget
{
    Q_OBJECT
public:
    explicit TrickView(PlotBookModel* bookModel,
                       QWidget *parent = 0);
    ~TrickView();

signals:
    
public slots:

private:
    QStandardItemModel* _tvModel;
    PlotBookModel* _bookModel;
    QGridLayout* _gridLayout ;
    QLineEdit* _searchBox;
    QListView* _listView ;

    QStandardItemModel* _createTVModel(const QString& host, int port);
    void _loadSieElement(const QDomElement& element,
                         QList<QDomElement> &path);
    QHash<QString,QDomElement> _name2element;
    QStringList _params;

    QSortFilterProxyModel* _varsFilterModel;
    QItemSelectionModel* _varsSelectModel;

private slots:
     void _tvSearchBoxTextChanged(const QString& rx);
     void _tvSearchBoxReturnPressed();
     void _tvSelectModelSelectionChanged(const QItemSelection& currVarSelection,
                                        const QItemSelection& prevVarSelection);

     void _sieRead();
};

#endif // TRICKVIEW_H
