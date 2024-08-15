#include "snaptable.h"
#include <stdio.h>

SnapTable::SnapTable(const QString& tableName, QObject *parent) :
    QAbstractTableModel(parent),
    _orientation(Qt::Horizontal),
    _tableName(tableName)
{
}

SnapTable::~SnapTable()
{
    for ( _idata =_data.begin(); _idata !=_data.end(); ++_idata) {
        delete (*_idata);
    }
    foreach ( QVariant* var, _col_headers ) { delete var; }
    foreach ( QVariant* var, _row_headers ) { delete var; }
    foreach ( Role* role, _col_roles ) { delete role; }
    foreach ( Role* role, _row_roles ) { delete role; }
}

QModelIndex SnapTable::parent(const QModelIndex &index) const
{
    if ( !index.isValid() ) {
        return index; // get rid of compiler warning of unused param
    } else {
        QModelIndex rootidx = QModelIndex();
        return rootidx;
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

    if ( ! pidx.isValid() && !_data.empty() ) {
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
            val = _data.at(col)->at(row);
            if ( !prole->format.isEmpty() && val.type() == QVariant::Double ) {
                QString str;
                double d = val.toDouble();
                val = str.asprintf(prole->format.toLatin1().constData(),d);
                bool r = val.convert(QVariant::Double); // for sorting by double
                if ( r == false ) {
                    // formatted val might not be a double e.g. 48%
                    val = str.asprintf(prole->format.toLatin1().constData(),d);
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

    if ( role == Qt::EditRole || role == Role::EditNoEmitDataChange ) {
        (*_data.at(col))[row] = value;
        if ( role != Role::EditNoEmitDataChange ) {
            emit dataChanged(idx,idx);
        }
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

    int rc = rowCount(pidx);
    if ( rc == 0 || row < 0 ) {
        row = 0 ;
    } else if ( row >= rc ) {
        row = rc;
    }

    beginInsertRows(pidx,row,row+count-1);

    for ( _idata =_data.begin(); _idata !=_data.end(); ++_idata) {
        vector<QVariant>* col = (*_idata);
        col->insert(col->begin()+row,count,QVariant(0.0));
    }

    // TODO: insert roles/headers above after changing them to vectors
    if ( _hasRowRoles() ) {
        for ( int ii = 0; ii < count; ++ii) {
            int cc = columnCount();
            for ( int jj = 0; jj < cc; ++jj) {
                QVariant* row_header = new QVariant(QString(""));
                _row_headers.insert(row,row_header);

                Role* role = _createRowRole();
                _row_roles.insert(row,role);
            }
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

    for ( _idata =_data.begin(); _idata !=_data.end(); ++_idata) {
        vector<QVariant>* col = (*_idata);
        col->erase(col->begin()+row,col->begin()+row+count);
    }

    // TODO: make stuff vectors! and do like above
    if ( _hasRowRoles() ) {
        for ( int cc = 0; cc < columnCount(pidx); ++cc) {
            int row_start = row+count-1;
            if ( row_start >= rowCount(pidx) ) {
                row_start = rowCount(pidx)-1;
            }
            for ( int rr = row_start; rr >= row; --rr) {
                delete _row_headers.at(rr);
                _row_headers.removeAt(rr);

                delete _row_roles.at(rr);
                _row_roles.removeAt(rr);
            }
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

    int cc = columnCount(pidx);
    if ( cc == 0 || column < 0 ) {
        column = 0 ;
    } else if ( column >= cc ) {
        column = cc;
    }

    int nrows = rowCount(pidx) ;

    beginInsertColumns(pidx,column,column+count-1);

    for ( int ii = column; ii < column+count; ++ii) {
        vector<QVariant>* col = new vector<QVariant>(nrows);
        _data.insert(_data.begin()+column,col);

        _col_headers.insert(ii,new QVariant);
        Role* role = _createColumnRole();
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

    for ( _idata = _data.begin()+column+count-1; _idata >=_data.begin()+column;
          --_idata) {
        delete (*_idata);
    }
    _data.erase(_data.begin()+column,_data.begin()+column+count);

    for ( int ii = column+count-1; ii >= column; --ii) {
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

        ret = prole->value(role);
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

    if ( role == Qt::EditRole || role == Role::EditNoEmitDataChange) {
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

        ret = prole->setValue(role,val);
    }

    if ( ret && role != Role::EditNoEmitDataChange ) {
        emit headerDataChanged(orientation,sect,sect);
    }

    return ret;
}

Role *SnapTable::_createColumnRole()
{
    Role* role = new Role();
    return role;
}

Role *SnapTable::_createRowRole()
{
    Role* role = new Role();
    return role;
}
