#ifndef TRICKVARSORTFILTERPROXYMODEL_H
#define TRICKVARSORTFILTERPROXYMODEL_H

#include <QSortFilterProxyModel>
#include <QList>
#include <QVariant>

class TrickVarSortFilterProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    explicit TrickVarSortFilterProxyModel(QObject *parent = nullptr);

protected:
    virtual bool lessThan(const QModelIndex &left,
                          const QModelIndex &right) const;

private:
    QList<QVariant> _nameAndIndices(const QString& s) const;
};
#endif // TRICKVARSORTFILTERPROXYMODEL_H
