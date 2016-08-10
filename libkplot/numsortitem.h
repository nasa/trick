#ifndef NUMSORTITEM_H
#define NUMSORTITEM_H

#include <QStandardItem>

class NumSortItem : public QStandardItem
{
public:
    NumSortItem(const QString& text) : QStandardItem(text) {}

    inline virtual int userType() const
    {
        return QVariant::Double;
    }

    bool operator<(const QStandardItem &other) const
    {
        const int role = model() ? model()->sortRole() : Qt::DisplayRole;
        const QVariant l = data(role), r = other.data(role);
        return l.toDouble() < r.toDouble();
    }
};

#endif // NUMSORTITEM_H
