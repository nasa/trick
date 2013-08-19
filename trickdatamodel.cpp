#include "trickdatamodel.h"

#include <stdlib.h>

const int TrickDataModel::TRICK_DOUBLE = 11;
const int TrickDataModel::TRICK_LONG_LONG = 14;
const int TrickDataModel::TRICK_UNSIGNED_LONG_LONG = 15;


TrickDataModel::TrickDataModel(TrickVersion version,
                               Endianness endianness) :
        QAbstractItemModel(),
        _trick_version(version),
        _endianess(endianness)
{
}

TrickDataModel::~TrickDataModel()
{
    for ( _idata =_data.begin(); _idata !=_data.end(); ++_idata) {
        delete (*_idata);
    }
    foreach ( Param* param, _params ) {
        delete param;
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

    if ( ! pidx.isValid() && !_data.empty() ) {
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
            val = _data.at(col)->at(row);
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
        (*_data.at(col))[row] = value;
        emit dataChanged(idx,idx);
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

    if ( columnCount() == 0 ) {
        fprintf(stderr,"snap [error]: attempting to insert rows into a table"
                 " with no columns.  Use insertColumns() before insertRows()\n");
        return false;
    }

    beginInsertRows(pidx,row,row+count-1);

    for ( _idata =_data.begin(); _idata !=_data.end(); ++_idata) {
        vector<QVariant>* col = (*_idata);
        col->insert(col->begin()+row,count,QVariant(0.0));
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

    for ( _idata =_data.begin(); _idata !=_data.end(); ++_idata) {
        vector<QVariant>* col = (*_idata);
        col->erase(col->begin()+row,col->begin()+row+count);
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

    int nrows = rowCount(pidx) ;

    for ( int ii = column; ii < column+count; ++ii) {

        vector<QVariant>* col = new vector<QVariant>(nrows);
        _data.insert(_data.begin()+column,col);

        // Meta data for "roles"
        Param* param = new Param;
        param->name = QString("Parameter%1").arg(ii);
        param->unit = "--";
        param->type = TRICK_DOUBLE;
        if ( ii == 0 ) {
            param->name = "sys.exec.out.time";
            param->unit = "s";
            param->type = TRICK_DOUBLE;
        }
        _params.insert(ii,param);
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

    for ( _idata = _data.begin()+column+count-1; _idata >=_data.begin()+column;
          --_idata) {
        delete (*_idata);
    }
    _data.erase(_data.begin()+column,_data.begin()+column+count);

    for ( int ii = column+count-1; ii >= column; --ii) {
        delete _params.at(ii);
        _params.removeAt(ii);
    }

    endRemoveColumns();

    return ret;
}

QVariant TrickDataModel::headerData(int sect,
                                    Qt::Orientation orientation, int role) const
{
    if ( sect < 0 ) return QVariant();

    if ( orientation == Qt::Horizontal && sect >= columnCount()) {
        return QVariant();
    }
    if ( orientation == Qt::Vertical && sect >= rowCount()) {
        return QVariant();
    }

    QVariant ret;


    if ( role < Qt::UserRole ) {
        ret = QAbstractItemModel::headerData(sect,orientation,role);
    } else {
        if (orientation == Qt::Horizontal) {
            Param* param = _params.at(sect);
            switch (role)
            {
            case ParamName: ret = QVariant(param->name); break;
            case ParamUnit: ret = QVariant(param->unit); break;
            case ParamType: ret = QVariant(param->type); break;
            case ParamSize: ret = QVariant(param->size); break;
            default:
                break;
            };
        }
    }

    return ret;
}

bool TrickDataModel::setHeaderData(int sect, Qt::Orientation orientation,
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

    if ( role < Qt::UserRole ) {
        // TODO: Need to implement this since the
        // QAbstractItemModel::setHeaderData() is a no-op and always
        // returns false
        ret = QAbstractItemModel::setHeaderData(sect,orientation,val,role);
    } else {
        if (orientation == Qt::Horizontal) {
            Param* param = _params[sect];
            if ( val.type() == QVariant::String ) {
                switch (role)
                {
                case ParamName: param->name = val.toString(); ret = true; break;
                case ParamUnit: param->unit = val.toString(); ret = true; break;
                default:
                    break;
                };
            }
            if ( val.type() == QVariant::Int ) {
                switch (role)
                {
                case ParamSize: param->size = val.toInt(); ret = true; break;
                case ParamType: param->type = val.toInt(); ret = true; break;
                default:
                    break;
                }
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
        out <<  headerData(col,Qt::Horizontal,ParamUnit).toString();
        out << "\t";
        out <<  headerData(col,Qt::Horizontal,ParamName).toString();
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
    QString systime = headerData(0,Qt::Horizontal,ParamName).toString();
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

bool TrickDataModel::load_binary_trk(const QString &log_name,
                                      const QString& rundir)
{
    bool ret = true;

    QString fname = QString("%1.trk").arg(log_name);
    fname = rundir + "/" + fname;

    QFile file(fname);

    if (!file.open(QIODevice::ReadOnly)) {
        fprintf(stderr,"Snap: [error] could not open %s\n",
                fname.toAscii().constData());
        return false;
    }
    QDataStream in(&file);

    if ( _endianess == LittleEndian ) {
        in.setByteOrder(QDataStream::LittleEndian);
    } else {
        in.setByteOrder(QDataStream::BigEndian);
    }


    //
    // Trick-version-endian (10 bytes)
    //
    char data[1024];
    in.readRawData(data,6); // Trick-
    in.readRawData(data,2); // version
    if ( data[0] == '1' && data[1] == '0' ) {
        _trick_version = TrickVersion10;
    } else {
        _trick_version = TrickVersion07;
    }
    in.readRawData(data,1) ; // -
    in.readRawData(data,1) ; // L or B (endian)
    if ( data[0] == 'L' ) {
        _endianess = LittleEndian;
    } else {
        _endianess = BigEndian;
    }

    //
    // Number of parameters (4 byte integer)
    //
    qint32 nparams;
    in >> nparams;

    //
    // Read param header info (name,unit,type,bytesize)
    //
    int record_size = 0 ;
    int col = columnCount();
    insertColumns(col,nparams);
    for ( int cc = 0; cc < nparams; ++cc) {
        _load_binary_param(in,col+cc);
        record_size += headerData(col+cc,Qt::Horizontal,ParamSize).toInt();
    }

    // Sanity check.  The bytes remaining should be a multiple of the record size
    qint64 nbytes = file.bytesAvailable();
    if ( nbytes % record_size != 0 ) {
        // TODO: throw runtime exception
        fprintf(stderr,"snap [error]: trk file %s is corrupt!\n",
                file.fileName().toAscii().constData());
        exit(-1);
    }

    // Calc number of data records in trk file
    qint64 num_records = nbytes/(qint64)record_size;

    // Allocate rows
    if ( rowCount() > 0 ) {
        // TODO: throw runtime exception
        fprintf(stderr,"snap [bad scoobies]: row count should be zero!\n");
        exit(-1);
    }
    insertRows(0,num_records);

    //
    // Read param data
    //
    int paramtype;
    vector<int> paramtypes;
    int ncols = columnCount();

    for ( int row = 0; row < rowCount(); ++row) {

        for ( int col = 0; col < ncols; ++col) {

            if ( row == 0 ) {
                paramtype = headerData(col,Qt::Horizontal,ParamType).toInt();
                paramtypes.push_back(paramtype);
            } else {
                paramtype = paramtypes.at(col);
            }

            switch (paramtype) {

            case TRICK_DOUBLE : {
                double val;
                in >> val;
                (*_data.at(col))[row] = val;
                break;
            }

            case TRICK_UNSIGNED_LONG_LONG : {
                unsigned long long val;
                in >> val;
                (*_data.at(col))[row] = val;
                break;
            }

            case TRICK_LONG_LONG : {
                long long val;
                in >> val;
                (*_data.at(col))[row] = val;
                break;
            }

            default :
            {
                fprintf(stderr,"snap [error]: can't handle trick type \"%d\"\n",
                        paramtype);
                exit(-1);
            }

            }
        }
    }
    file.close();

    QModelIndex idx1 = index(0,0);
    QModelIndex idx2 = index(rowCount(),ncols);
    emit dataChanged(idx1,idx2);

    return ret;
}

void TrickDataModel::_write_binary_param(QDataStream& out, int col)
{
    // Param name
    QString param =  headerData(col,Qt::Horizontal,ParamName).toString();
    _write_binary_qstring(param,out);

    // Param unit
    QString unit =  headerData(col,Qt::Horizontal,ParamUnit).toString();
    _write_binary_qstring(unit,out);

    // Param type
    TrickType tricktype = _tricktype(col);
    out << tricktype.id;
    out << tricktype.size;
}

// Returns byte size of parameter
void TrickDataModel::_load_binary_param(QDataStream& in, int col)
{
    qint32 sz;

    // Param name
    in >> sz;
    char* param_name = new char[sz+1];
    in.readRawData(param_name,sz);
    param_name[sz] = '\0';
    setHeaderData(col,Qt::Horizontal,param_name,ParamName);
    delete[] param_name;

    // Param unit
    in >> sz;
    char* param_unit = new char[sz+1];
    in.readRawData(param_unit,sz);
    param_unit[sz] = '\0';
    setHeaderData(col,Qt::Horizontal,param_unit,ParamUnit);
    delete[] param_unit;

    // Param type
    qint32 param_type;
    in >> param_type;
    setHeaderData(col,Qt::Horizontal,param_type,ParamType);

    // Param bytesize
    in >> sz;
    setHeaderData(col,Qt::Horizontal,sz,ParamSize);
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
