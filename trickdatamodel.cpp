#include "trickdatamodel.h"

#include <stdlib.h>

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

bool TrickDataModel::removeRows(int row, int count, const QModelIndex &pidx)
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
        QString tricktype("double");
        if ( ii == 0 ) {
            name = "sys.exec.out.time";
            unit = "s";
            tricktype = "double";
        }
        _name.insert(ii,name);
        _unit.insert(ii,unit);
        _type.insert(ii,tricktype);
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

bool TrickDataModel::removeColumns(int column, int count, const QModelIndex &pidx)
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
    }

    endRemoveColumns();

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
            switch (role)
            {
            case TrickRoleParamName: ret = QVariant(_name.at(sect)); break;
            case      TrickRoleUnit: ret = QVariant(_unit.at(sect)); break;
            case      TrickRoleType: ret = QVariant(_type.at(sect)); break;
            default:                                                break;
            };
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
            if ( val.type() == QVariant::String ) {
                switch (role)
                {
                case TrickRoleParamName:
                    _name.replace(sect,val.toString()); ret = true; break;
                case TrickRoleUnit:
                    _unit.replace(sect,val.toString()); ret = true; break;
                case TrickRoleType:
                    _type.replace(sect,val.toString()); ret = true; break;

                default:                                           break;
                };
            }
        }
    }

    return ret;
}

bool TrickDataModel::write_log_header(const QString &log_name,
                                      const QString& rundir)
{
    bool ret = true;

    QString fname = QString("%1.header").arg(log_name);
    fname = rundir + "/" + fname;

    QFile file(fname);
    if (!file.open(QIODevice::WriteOnly)) {
        fprintf(stderr,"Snap: [error] could not open %s\n",
                fname.toAscii().constData());
        return false;
    }
    QTextStream out(&file);

    out << log_name << " byte_order is " ;
    if ( _endianess == LittleEndian ) {
        out << "little_endian\n";
    } else {
        out << "big_endian\n";
    }

    //
    // Write log header param info
    //
    for ( int col = 0; col < columnCount(); ++col) {
        out << log_name ;
        out << "\t";
        out << _tricktype(col).name;
        out << "\t";
        out <<  headerData(col,Qt::Horizontal,TrickRoleUnit).toString();
        out << "\t";
        out <<  headerData(col,Qt::Horizontal,TrickRoleParamName).toString();
        out << "\n";

    }

    file.close();
    return ret;
}

bool TrickDataModel::write_binary_trk(const QString &log_name,
                                      const QString& rundir)
{
    bool ret = true;

    QString fname = QString("%1.trk").arg(log_name);
    fname = rundir + "/" + fname;

    QFile file(fname);

    if (!file.open(QIODevice::WriteOnly)) {
        fprintf(stderr,"Snap: [error] could not open %s\n",
                fname.toAscii().constData());
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
        _write_binary_param(out,ii);
    }

    //
    // Write actual param data
    //
    for ( int ii = 0; ii < rowCount(); ++ii) {
        for ( int jj = 0; jj < columnCount(); ++jj) {
            _write_binary_param_data(out,ii,jj);
        }
    }


    file.close();
    return ret;
}

void TrickDataModel::_write_binary_param(QDataStream& out, int col)
{
    // Param name
    QString param =  headerData(col,Qt::Horizontal,TrickRoleParamName).toString();
    _write_binary_qstring(param,out);

    // Param unit
    QString unit =  headerData(col,Qt::Horizontal,TrickRoleUnit).toString();
    _write_binary_qstring(unit,out);

    // Param type
    TrickType tricktype = _tricktype(col);
    out << tricktype.id;
    out << tricktype.size;
}

void TrickDataModel::_write_binary_param_data(QDataStream& out, int row, int col)
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

void TrickDataModel::_write_binary_qstring(const QString &str, QDataStream& out)
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

// Param type
// Determine by checking type of first QVariant type
// Return Pair<TrickType,Size>
//
// I'm keeping this even though the header data has it
// it's a different way to get the trick type other than
// the coded table headerdata
TrickType TrickDataModel::_tricktype(int col)
{
    TrickType tricktype;

    if ( rowCount() == 0 ) {
        fprintf(stderr,"snap [error] : can't write trk param since"
                                      "table has no rows/data\n");
        exit(-1);
    }

    QVariant val0 = data(index(0,col));

    if ( val0.type() == QVariant::Double ) {
        tricktype.id = 10;
        tricktype.size = sizeof(double);
        tricktype.name = "double";
    } else {
        // Need to add new types
        fprintf(stderr,"snap [error]: need to add trick type for \"%s\"\n",
                val0.typeName());
        exit(-1);
    }

    return tricktype;
}
