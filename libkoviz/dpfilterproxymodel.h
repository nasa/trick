#ifndef DPFILTERPROXYMODEL_H
#define DPFILTERPROXYMODEL_H

#include <QSortFilterProxyModel>
#include <QFileSystemModel>
#include <QRegExp>
#include <QString>
#include <QHash>

#include "dp.h"
#include "sie_listmodel.h"

class DPFilterProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    explicit DPFilterProxyModel( const QStringList& params,
                                 SieListModel* sieModel,
                                 QObject *parent = 0);

protected:
    bool filterAcceptsRow(int row, const QModelIndex &pidx) const;
    bool filterAcceptsColumn(int col, const QModelIndex &pidx) const;
signals:
    
public slots:

private:
    QHash<QString,int> _modelParams;
    SieListModel* _sieModel;
    static QHash<QString,bool> _acceptedDPFileCache; // static to get around const

    bool _isAccept(const QModelIndex& idx,
                   QFileSystemModel* m,
                   const QRegExp& rx) const;


    
};

#endif // DPFILTERPROXYMODEL_H
