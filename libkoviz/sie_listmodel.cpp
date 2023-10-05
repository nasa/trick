#include "sie_listmodel.h"

SieListModel::SieListModel(QObject *parent)
    : QAbstractListModel{parent},
      _fetchCount(0),
      _fetchChunkSize(10000)
{
}

int SieListModel::rowCount(const QModelIndex &parent) const
{
    if ( parent.isValid() ) {
        return 0;  // Flat list has no children
    } else {
        return _fetchCount;
    }
}

QVariant SieListModel::data(const QModelIndex &index, int role) const
{
    QVariant variant;

    if ( !index.isValid() || index.row() < 0 ) {
        return variant;
    }
    if ( index.row() >= _fetchCount ) {
        return variant;
    }

    if ( role == Qt::DisplayRole ) {
        if ( _rx.isEmpty() || _rx == ".*" ) {
            variant = _params->at(index.row());
        } else {
            variant = _filteredParams.at(index.row());
        }
    }

    return variant;
}

// This is for drag-n-drop.  A widget that accepts a drop needs to know
// what model the dropped data came from.
QMimeData *SieListModel::mimeData(const QModelIndexList &indexes) const
{
    QMimeData* mimedata = QAbstractItemModel::mimeData(indexes);
    QByteArray data("SieListModel");
    mimedata->setData("koviz-model-type",data);
    return mimedata;
}

void SieListModel::setParams(const QStringList* params)
{
    _params = params;
}

int SieListModel::setRegexp(const QString &rx)
{
    beginResetModel();
    _rx = rx;
    _filteredParams.clear();
    QRegularExpression filter(rx);
    foreach (QString param, *_params) {
        if ( filter.match(param).hasMatch() ) {
            _filteredParams.append(param);
        }
    }
    _fetchCount = 0;
    endResetModel();

    return _filteredParams.size();
}

bool SieListModel::canFetchMore(const QModelIndex &parent) const
{
    bool isCanFetchMore = false;

    if ( parent.isValid() ) {
        return false;
    }

    if ( _rx.isEmpty() || _rx == ".*" ) {
        if ( _fetchCount < _params->size() ) {
            isCanFetchMore = true;
        }
    } else {
        if ( _fetchCount < _filteredParams.size() ) {
            isCanFetchMore = true;
        }
    }

    return isCanFetchMore;
}

void SieListModel::fetchMore(const QModelIndex &parent)
{
    if (parent.isValid()) {
        return;
    }

    int remainder;
    if ( _rx.isEmpty() || _rx == ".*" ) {
        remainder = _params->size() - _fetchCount;
    } else {
        remainder = _filteredParams.size() - _fetchCount;
    }
    int nItemsToFetch = qMin(_fetchChunkSize, remainder);

    if ( nItemsToFetch > 0 ) {
        beginInsertRows(parent,_fetchCount,_fetchCount+nItemsToFetch-1);
        _fetchCount += nItemsToFetch;
        endInsertRows();
    }
}

Qt::ItemFlags SieListModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags defaultFlags = QAbstractListModel::flags(index);

    if (index.isValid())
        return Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled | defaultFlags;
    else
        return Qt::ItemIsDropEnabled | defaultFlags;
}
