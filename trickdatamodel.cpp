#include "trickdatamodel.h"

#include <stdlib.h>
#include <QBitArray>

TrickDataModel::TrickDataModel(TrickVersion version,
                               Endianness endianness) :
        QAbstractItemModel(),
        _trick_version(version),
        _endianess(endianness)
{
}

TrickDataModel::~TrickDataModel()
{
    for ( int ii = 0; ii < _data.size(); ++ii) {
        QList<QVariant*>* col = _data.at(ii);
        for ( int jj = 0; jj < col->size(); ++jj) {
            QVariant* val = col->at(jj);
            delete val;
        }
        delete col;
    }

}


QModelIndex TrickDataModel::index(int row, int column,
                                  const QModelIndex &pidx) const
{
    QModelIndex idx = QModelIndex();

    if ( !pidx.isValid() && row < rowCount() && column < columnCount() ) {
        idx = createIndex(row,column);
    }

    return idx;
}

QModelIndex TrickDataModel::parent(const QModelIndex &index) const
{
    if ( !index.isValid() ) {
        return index; // get rid of compiler warning of unused param
    } else {
        QModelIndex rootidx = QModelIndex();
        return rootidx;
    }
}

int TrickDataModel::rowCount(const QModelIndex &pidx) const
{
    int nrows = 0 ;

    if ( ! pidx.isValid() && !_data.isEmpty() ) {
        // root
        nrows = _data.at(0)->size();
    }

    return nrows;
}

int TrickDataModel::columnCount(const QModelIndex &pidx) const
{
    int ncols = 0 ;

    if ( ! pidx.isValid() ) {
        ncols = _data.size();
    }

    return ncols;
}

QVariant TrickDataModel::data(const QModelIndex &idx, int role) const
{
    QVariant val;

    if ( idx.isValid() ) {
        int row = idx.row();
        int col = idx.column();

        if ( role == Qt::DisplayRole ) {
            QVariant* dptr = _data.at(col)->at(row);
            val = *dptr;
        }
    }

    return val;
}

// Sets the role data for the item at index to value.
// Returns true if successful; otherwise returns false.
// The dataChanged() signal should be emitted if the data was successfully set.
bool TrickDataModel::setData(const QModelIndex &idx,
                             const QVariant &value, int role)
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

bool TrickDataModel::insertRows(int row, int count, const QModelIndex &pidx)
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
            QVariant* val = new QVariant(0.0);
            col->insert(row,val);
        }
    }

    endInsertRows();


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
bool TrickDataModel::insertColumns(int column, int count,
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

    //
    // Meta data for "roles" :(
    //
    for ( int ii = column; ii < column+count; ++ii) {
        QString name = QString("Parameter%1").arg(ii);
        QString unit("--");
        if ( ii == 0 ) {
            name = "sys.exec.out.time";
            unit = "s";
        }
        _name.insert(ii,name);
        _unit.insert(ii,unit);
    }

    int nrows = rowCount(pidx) ;

    for ( int ii = 0; ii < count; ++ii) {
        QList<QVariant*>* col = new QList<QVariant*>;
        for ( int jj = 0; jj < nrows; ++jj) {
            QVariant* val = new QVariant;
            col->append(val);
        }
        _data.insert(column,col);
    }

    endInsertColumns();

    return ret;
}

QVariant TrickDataModel::headerData(int sect,
                                    Qt::Orientation orientation, int role) const
{
    QVariant ret;

    if ( role < Qt::UserRole ) {
        ret = QAbstractItemModel::headerData(sect,orientation,role);
    } else {
        if (orientation == Qt::Horizontal && sect >= 0 && sect < columnCount()) {
            if ( role == TrickRoleParamName ) {
                ret = QVariant(_name.at(sect));
            } else if ( role == TrickRoleUnit ) {
                ret = QVariant(_unit.at(sect));
            }
        }
    }

    return ret;
}

bool TrickDataModel::setHeaderData(int sect, Qt::Orientation orientation,
                                   const QVariant &val, int role)
{
    bool ret = false;

    if ( role < Qt::UserRole ) {
        ret = QAbstractItemModel::setHeaderData(sect,orientation,val,role);
    } else {
        if (orientation == Qt::Horizontal && sect >= 0 && sect < columnCount()) {
            if (role == TrickRoleParamName && val.type() == QVariant::String ) {
                _name.replace(sect,val.toString());
                ret = true;
            } else if (role == TrickRoleUnit && val.type() == QVariant::String) {
                _unit.replace(sect,val.toString());
                ret = true;
            }
        }
    }

    return ret;
}

bool TrickDataModel::write(const QString &trk)
{
    bool ret = true;

    QFile file(trk);

    if (!file.open(QIODevice::WriteOnly)) {
        fprintf(stderr,"Snap: [error] could not open %s\n",
                trk.toAscii().constData());
        return false;
    }
    QDataStream out(&file);

    if ( _endianess == LittleEndian ) {
        out.setByteOrder(QDataStream::LittleEndian);
    } else {
        out.setByteOrder(QDataStream::BigEndian);
    }


    //
    // Trick-version-endian (10 bytes)
    //
    out.writeRawData("Trick-",6) ;
    if ( _trick_version == TrickVersion07 ) {
        out.writeRawData("07",2) ;
    } else {
        fprintf(stderr,"snap [error]: unsupported trick version\n");
        file.close();
        return false;
    }
    out.writeRawData("-",1) ;
    if ( _endianess == LittleEndian ) {
        out.writeRawData("L",1) ;
    } else {
        out.writeRawData("B",1) ;
    }

    //
    // Num params recorded (4 bytes - int)
    //
    qint32 nparams = (qint32)columnCount();
    out << nparams;

    //
    // Sanity check for "sys.exec.out.time"
    //
    QString systime = headerData(0,Qt::Horizontal,TrickRoleParamName).toString();
    if ( systime != "sys.exec.out.time" ) {
        fprintf(stderr,"snap [error]: holy cow, %s changed to %s\n",
                "sys.exec.out.time", systime.toAscii().constData());
        file.close();
        exit(-1);
    }

    //
    // Write trick header param info
    //
    for ( int ii = 0; ii < columnCount(); ++ii) {
        _write_param_header(out,ii);
    }

    //
    // Write actual param data
    //
    for ( int ii = 0; ii < rowCount(); ++ii) {
        for ( int jj = 0; jj < columnCount(); ++jj) {
            _write_param_data(out,ii,jj);
        }
    }


    file.close();
    return ret;
}

void TrickDataModel::_write_param_header(QDataStream& out, int col)
{

    // Param name
    QString param =  headerData(col,Qt::Horizontal,TrickRoleParamName).toString();
    _write_qstring(param,out);

    // Param unit
    QString unit =  headerData(col,Qt::Horizontal,TrickRoleUnit).toString();
    _write_qstring(unit,out);

    // Param type
    // Determine by checking type of first QVariant type
    if ( rowCount() == 0 ) {
        fprintf(stderr,"snap [error] : can't write trk param since"
                                      "table has no rows/data\n");
        exit(-1);
    }
    QVariant val0 = data(index(0,col));
    QPair<qint32,qint32> tricktype = _tricktype(val0);
    out << tricktype.first;
    out << tricktype.second;
}

void TrickDataModel::_write_param_data(QDataStream& out, int row, int col)
{
    QVariant val = data(index(row,col));

    switch ( val.type() ) {

    case QVariant::Double :     out << val.toDouble();     break;
    case QVariant::Int :        out << val.toInt();        break;
    case QVariant::ULongLong :  out << val.toULongLong();  break;

    default :
        fprintf(stderr,"snap [error]: can't handle type \"%s\"\n",
                val.typeName());
        exit(-1);
    }


}

void TrickDataModel::_write_qstring(const QString &str, QDataStream& out)
{
    qint32 size_str = (qint32) str.size();
    out << size_str;
    out.writeRawData(str.toAscii().constData(),str.size());
}

// MemoryManager/include/parameter_types.h
#if 0
    typedef enum {
        TRICK_VOID               =   0, /* No type */
        TRICK_CHARACTER          =   1, /* (char) */
        TRICK_UNSIGNED_CHARACTER =   2, /* (unsigned char) */
        TRICK_STRING             =   3, /* (char *) */
        TRICK_SHORT              =   4, /* (short) */
        TRICK_UNSIGNED_SHORT     =   5, /* (unsigned short) */
        TRICK_INTEGER            =   6, /* (int) */
        TRICK_UNSIGNED_INTEGER   =   7, /* (unsigned int) */
        TRICK_LONG               =   8, /* (long) */
        TRICK_UNSIGNED_LONG      =   9, /* (unsigned long) */
        TRICK_FLOAT              =  10, /* (float) */
        TRICK_DOUBLE             =  11, /* (double) */
        TRICK_BITFIELD           =  12, /* (signed int : 1) */
        TRICK_UNSIGNED_BITFIELD  =  13, /* (unsigned int : 1) */
        TRICK_LONG_LONG          =  14, /* (long long) */
        TRICK_UNSIGNED_LONG_LONG =  15, /* (long long) */
        TRICK_FILE_PTR           =  16, /* (file *) */
        TRICK_BOOLEAN            =  17, /* (C++ boolean) */
        TRICK_WCHAR              =  18, /* (wchar_t) */
        TRICK_WSTRING            =  19, /* (wchar_t *) */
        TRICK_VOID_PTR           =  20, /* an arbitrary address */
        TRICK_ENUMERATED         =  21, /* User defined type (enumeration) */
        TRICK_STRUCTURED         =  22, /* User defined type (struct/class) */
        TRICK_USER_DEFINED_TYPE  =  23, /* User defined type (where type details are as yet unknown) */
        TRICK_NUMBER_OF_TYPES
    } TRICK_TYPE ;

#endif
QPair<qint32, qint32> TrickDataModel::_tricktype(const QVariant &val)
{
    QPair<qint32,qint32> tricktype_size;

    if ( val.type() == QVariant::Double ) {
        tricktype_size.first = 10;
        tricktype_size.second = sizeof(double);
    } else {
        // Need to add new types
        fprintf(stderr,"snap [error]: need to add trick type for \"%s\"\n",
                val.typeName());
        exit(-1);
    }

    return tricktype_size;
}
