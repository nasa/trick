#ifndef DPFILTERPROXYMODEL_H
#define DPFILTERPROXYMODEL_H

#include <QSortFilterProxyModel>
#include <QFileSystemModel>
#include <QRegExp>
#include <QString>
#include <QHash>

#include "libsnapdata/montemodel.h"
#include "libsnapdata/timeit_linux.h"
#include "libqplot/dp.h"

class DPFilterProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    explicit DPFilterProxyModel( MonteModel* monteModel,
                                 QObject *parent = 0);


protected:
    bool filterAcceptsRow(int row, const QModelIndex &pidx) const;
    bool filterAcceptsColumn(int col, const QModelIndex &pidx) const;
signals:
    
public slots:

private:
    MonteModel* _monteModel;
    QHash<QString,int> _modelParams;
    static QHash<QString,bool> _acceptedDPFileCache; // static to get around const

    bool _isAccept(const QModelIndex& idx,
                   QFileSystemModel* m,
                   const QRegExp& rx) const;


    
};

#endif // DPFILTERPROXYMODEL_H
