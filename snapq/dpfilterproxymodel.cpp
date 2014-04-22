#include "dpfilterproxymodel.h"
#include <QDebug>

DPFilterProxyModel::DPFilterProxyModel(QObject *parent) :
    QSortFilterProxyModel(parent)
{
}

bool DPFilterProxyModel::filterAcceptsRow(int row,
                                          const QModelIndex &pidx) const
{
    if ( ! pidx.isValid() ) {
        return false;
    }

    bool isAccept = false;
    QRegExp rx(filterRegExp());
    QFileSystemModel* m = (QFileSystemModel*) sourceModel();
    int ncols = m->columnCount(pidx);
    for ( int col = 0; col < ncols ; ++col) {
        QModelIndex idx = m->index(row,col,pidx);
        if ( _isAccept(idx,m,rx) ) {
             isAccept = true;
            break;
        }
    }

    return isAccept;
}

bool DPFilterProxyModel::filterAcceptsColumn(int col,
                                             const QModelIndex &pidx) const
{
    if ( !pidx.isValid() ) {
        return false;
    }

    if ( col > 0 ) {
        return true;
    }

    bool isAccept = false;
    QRegExp rx(filterRegExp());
    QFileSystemModel* m = (QFileSystemModel*) sourceModel();
    int nrows = m->rowCount(pidx);
    for ( int row = 0; row < nrows ; ++row) {
        QModelIndex idx = m->index(row,col,pidx);
        if ( _isAccept(idx,m,rx) ) {
             isAccept = true;
            break;
        }
    }

    return isAccept;
}

bool DPFilterProxyModel::_isAccept(const QModelIndex &idx,
                                   QFileSystemModel *m, const QRegExp &rx) const
{
    bool isAccept = false ;

    if ( m->isDir(idx) ) {
        isAccept = true;
    } else {
        QString path = m->filePath(idx);
        if ( path.contains(rx) && (m->fileInfo(idx).suffix() == "xml" ||
             ((m->fileInfo(idx).fileName().startsWith("DP_") &&
              m->fileInfo(idx).isFile()))) ) {
            isAccept = true;
        }
    }

    return isAccept;
}
