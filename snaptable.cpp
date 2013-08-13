#include "snaptable.h"

SnapTable::SnapTable(const QString& tableName, QObject *parent) :
    QAbstractTableModel(parent),
    _orientation(Qt::Horizontal),
    _tableName(tableName)
{
}

SnapTable::~SnapTable()
{
    for ( int ii = 0; ii < _data.size(); ++ii) {
        delete _col_headers.at(ii);
        delete _col_roles.at(ii);
        QList<QVariant*>* col = _data.at(ii);
        for ( int jj = 0; jj < col->size(); ++jj) {
            delete col->at(jj);
            delete _row_headers.at(jj);
            delete _row_roles.at(jj);
        }
        delete col;
    }

}


QModelIndex SnapTable::index(int row, int column,
                                  const QModelIndex &pidx) const
{
    QModelIndex idx = QModelIndex();

    if ( !pidx.isValid() && row < rowCount() && column < columnCount() ) {
        idx = createIndex(row,column);
    }

    return idx;
}

int SnapTable::rowCount(const QModelIndex &pidx) const
{
    int nrows = 0 ;

    if ( ! pidx.isValid() && !_data.isEmpty() ) {
        // root
        nrows = _data.at(0)->size();
    }

    return nrows;
}

int SnapTable::columnCount(const QModelIndex &pidx) const
{
    int ncols = 0 ;

    if ( ! pidx.isValid() ) {
        ncols = _data.size();
    }

    return ncols;
}

QVariant SnapTable::data(const QModelIndex &idx, int role) const
{
    QVariant val;

    if ( idx.isValid() ) {
        int row = idx.row();
        int col = idx.column();

        Role* prole;
        if (_orientation == Qt::Horizontal) {
            prole = _col_roles.at(idx.column());
        } else {
            prole = _row_roles.at(idx.row());
        }

        if ( role == Qt::DisplayRole ) {
            QVariant* dptr = _data.at(col)->at(row);
            val = *dptr;
            if ( val.type() == QVariant::Double ) {
                if ( prole->format.size() > 0 ) {
                    QString str;
                    double d = val.toDouble();
                    val = str.sprintf(prole->format.toAscii().constData(),d);
                }
            }
        } else if ( role == Qt::TextAlignmentRole ) {
            val = prole->alignment;
        }
    }

    return val;
}

// Sets the role data for the item at index to value.
// Returns true if successful; otherwise returns false.
// The dataChanged() signal should be emitted if the data was successfully set.
bool SnapTable::setData(const QModelIndex &idx, const QVariant &value, int role)
{
    bool ret = true;

    int row = idx.row();
    int col = idx.column();

    if ( role == Qt::EditRole ) {
        QVariant* dptr = _data.at(col)->at(row);
        *dptr = value;
        dataChanged(idx,idx);
    } else {
        ret = false;
    }

    return ret;
}

bool SnapTable::insertRows(int row, int count, const QModelIndex &pidx)
{
    bool ret = true;

    if ( pidx.isValid() ) {
        return false;
    }

    if ( row == 0 ) {
        beginInsertRows(pidx,0,count-1);
    } else {
        beginInsertRows(pidx,row+1,row+count);
    }

    for ( int ii = 0; ii < count; ++ii) {
        for ( int jj = 0; jj < columnCount(); ++jj) {
            QList<QVariant*>* col = _data.at(jj);
            QVariant* val = new QVariant(QString(""));
            col->insert(row,val);
            QVariant* row_header = new QVariant(QString(""));
            _row_headers.insert(row,row_header);

            Role* role = new Role;
            role->format = QString("");
            role->alignment = (Qt::AlignRight | Qt::AlignVCenter);
            _row_roles.insert(row,role);
        }
    }

    endInsertRows();


    return ret;
}

bool SnapTable::removeRows(int row, int count, const QModelIndex &pidx)
{
    bool ret = true;

    if ( count <= 0 ) {
        return false;
    }

    if ( pidx.isValid() ) {
        return false;
    }

    if ( row > rowCount()-1 ) {
        row = rowCount()-1 ;
    }

    beginRemoveRows(pidx,row,row+count-1);

    for ( int cc = 0; cc < columnCount(pidx); ++cc) {
        QList<QVariant*>* col = _data[cc];
        int row_start = row+count-1;
        if ( row_start >= rowCount(pidx) ) {
            row_start = rowCount(pidx)-1;
        }
        for ( int rr = row_start; rr >= row; --rr) {
            QVariant* row_val = col->at(rr);
            delete row_val;
            col->removeAt(rr);

            delete _row_headers.at(rr);
            _row_headers.removeAt(rr);

            delete _row_roles.at(rr);
            _row_roles.removeAt(rr);
        }
    }

    endRemoveRows();

    return ret;
}

// Inserts count new columns into the model before the given column.
// The items in each new column will be children of the item represented
// by the parent model index.
//
// If column is 0, the columns are prepended to any existing columns.
// If column is columnCount(), the columns are appended to any existing columns.
// If parent has no children, a single row with count columns is inserted.
// Returns true if the columns were successfully inserted; otherwise false.
//
bool SnapTable::insertColumns(int column, int count,
                                   const QModelIndex &pidx)
{
    bool ret = true;

    if ( count <= 0 ) {
        return false;
    }

    if ( pidx.isValid() ) {
        return false;
    }

    if ( column > columnCount() ) {
        column = columnCount() ;
    }

    if ( column == 0 ) {
        beginInsertColumns(pidx,0,count-1);
    } else {
        beginInsertColumns(pidx,column+1,column+count);
    }


    int nrows = rowCount(pidx) ;

    for ( int ii = column; ii < column+count; ++ii) {
        QList<QVariant*>* col = new QList<QVariant*>;
        for ( int jj = 0; jj < nrows; ++jj) {
            col->append(new QVariant);
        }
        _data.insert(ii,col);
        _col_headers.insert(ii,new QVariant);
        Role* role = new Role;
        role->format = QString("");
        role->alignment = (Qt::AlignRight | Qt::AlignVCenter);
        _col_roles.insert(ii,role);
    }

    endInsertColumns();

    return ret;
}

bool SnapTable::removeColumns(int column, int count, const QModelIndex &pidx)
{
    bool ret = true;

    if ( count <= 0 ) {
        return false;
    }

    if ( pidx.isValid() ) {
        return false;
    }

    if ( column > columnCount()-1 ) {
        column = columnCount()-1 ;
    }

    beginRemoveColumns(pidx,column,column+count-1);

    int nrows = rowCount(pidx) ;
    for ( int ii = column+count-1; ii >= column; --ii) {
        QList<QVariant*>* col = _data[ii];
        for ( int jj = 0; jj < nrows; ++jj) {
            delete col->at(jj);
        }
        delete col;
        _data.removeAt(ii);

        delete _col_headers.at(ii);
        _col_headers.removeAt(ii);

        delete _col_roles.at(ii);
        _col_roles.removeAt(ii);
    }

    endRemoveColumns();

    return ret;
}

QVariant SnapTable::headerData(int sect, Qt::Orientation orientation,
                              int role) const
{
    if ( sect < 0 ) return QVariant();

    if ( orientation == Qt::Horizontal && sect >= columnCount()) {
        return QVariant();
    }
    if ( orientation == Qt::Vertical && sect >= rowCount()) {
        return QVariant();
    }

    QVariant ret;

    if ( role == Qt::DisplayRole ) {
        if ( orientation == Qt::Horizontal ) {
            ret = *_col_headers.at(sect);
        } else {
            ret = *_row_headers.at(sect);
        }
    } else {

        Role* prole;

        if (orientation == Qt::Horizontal) {
            prole = _col_roles.at(sect);
        } else {
            prole = _row_roles.at(sect);
        }

        switch (role)
        {
        case Format: ret = QVariant(prole->format); break;
        case Qt::TextAlignmentRole:
                     ret = QVariant(prole->alignment); break;
        default: break;
        };
    }

    return ret;
}

bool SnapTable::setHeaderData(int sect, Qt::Orientation orientation,
                                   const QVariant &val, int role)
{
    if ( sect < 0 ) return false;

    if ( orientation == Qt::Horizontal && sect >= columnCount()) {
        return false;
    }
    if ( orientation == Qt::Vertical && sect >= rowCount()) {
        return false;
    }

    bool ret = false;

    if ( role == Qt::EditRole ) {
        ret = true;
        if ( orientation == Qt::Horizontal ) {
            _col_headers.at(sect)->setValue(val);
        } else {
            _row_headers.at(sect)->setValue(val);
        }
    } else {

        Role* prole;
        if (orientation == Qt::Horizontal) {
            prole = _col_roles.at(sect);
        } else {
            prole = _row_roles.at(sect);
        }

        _orientation = orientation;  // for now, last setHeader call sets orient

        switch (role)
        {
        case Format: prole->format = val.toString(); ret = true; break;
        case Qt::TextAlignmentRole:
                     prole->alignment = val.toInt(); ret = true; break;
        default:
            break;
        };
    }

    if ( ret ) {
        emit headerDataChanged(orientation,sect,sect);
    }

    return ret;
}
