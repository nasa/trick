#include "trickvarsortfilterproxymodel.h"

TrickVarSortFilterProxyModel::TrickVarSortFilterProxyModel(QObject *parent)
    : QSortFilterProxyModel{parent}
{
}

bool TrickVarSortFilterProxyModel::lessThan(const QModelIndex &left,
                                            const QModelIndex &right) const
{
    QString ll = sourceModel()->data(left).toString();
    QString rr = sourceModel()->data(right).toString();

    QStringList lls = ll.split('.');
    QStringList rrs = rr.split('.');

    int min = qMin(lls.size(),rrs.size());
    for (int i = 0; i < min; ++i) {
        QList<QVariant> lv = _nameAndIndices(lls.at(i));
        QList<QVariant> rv = _nameAndIndices(rrs.at(i));
        int n = qMin(lv.size(),rv.size());
        for (int j = 0; j < n; ++j) {
            if ( j == 0 ) { // Name
                QString lname = lv.at(0).toString();
                QString rname = rv.at(0).toString();
                if ( lname != rname ) {
                    int c = QString::compare(lname,rname);
                    if ( c < 0 ) {
                        return true;
                    } else if ( c > 0 ) {
                        return false;
                    }
                }
            } else { // Indices
                bool lok;
                bool rok;
                int lidx = lv.at(j).toInt(&lok);
                int ridx = rv.at(j).toInt(&rok);
                if ( lok && rok ) {
                    if ( lidx < ridx ) {
                        return true;
                    } else if ( lidx > ridx ) {
                        return false;
                    }
                } else {
                    // Indice is not an int (shouldn't happen)
                    int c =  QString::compare(lv.at(j).toString(),
                                              rv.at(j).toString()) ;
                    if ( c < 0 ) {
                        return true;
                    } else if ( c > 0 ) {
                        return false;
                    }
                }
            }
        }
    }

    int c = QString::compare(ll,rr);
    if ( c < 0 ) {
        return true;
    } else {
        return false;
    }
}

// s=fred[3][4][5]
// returns {fred,3,4,5}
// This expects proper form e.g. not "fred[0]wilma[1][[[[2]barney"
QList<QVariant> TrickVarSortFilterProxyModel::_nameAndIndices(
                                                         const QString &s) const
{
    QList<QVariant> list;

    int i,j;

    i = s.indexOf('[');
    if ( i >= 0 ) {
        list.append(s.left(i));  // Appends name
        i = 0;
        while ( 1 ) {
            i = s.indexOf('[', i);
            if ( i < 0 ) {
                break;
            }
            j = s.indexOf(']', i);
            if ( j < 0 ) {
                break;
            }
            list.append(s.mid(i+1,j-i-1));  // Appends index
            i = j;
        }
    } else {
        list.append(s);
    }

    return list;
}
