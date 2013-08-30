#include "trickdatamodel.h"

#include <stdlib.h>

const int TrickDataModel::TRICK_DOUBLE = 11;
const int TrickDataModel::TRICK_LONG_LONG = 14;
const int TrickDataModel::TRICK_UNSIGNED_LONG_LONG = 15;


TrickDataModel::TrickDataModel(TrickVersion version,
                               Endianness endianness) :
    SnapTable(),
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

Role* TrickDataModel::_createColumnRole()
{
    Param* param = new Param;
    return param;
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
        out <<  headerData(col,Qt::Horizontal,Param::Unit).toString();
        out << "\t";
        out <<  headerData(col,Qt::Horizontal,Param::Name).toString();
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
    QString systime = headerData(0,Qt::Horizontal,Param::Name).toString();
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
        record_size += headerData(col+cc,Qt::Horizontal,Param::Size).toInt();
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
                paramtype = headerData(col,Qt::Horizontal,Param::Type).toInt();
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
    QString param =  headerData(col,Qt::Horizontal,Param::Name).toString();
    _write_binary_qstring(param,out);

    // Param unit
    QString unit =  headerData(col,Qt::Horizontal,Param::Unit).toString();
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
    setHeaderData(col,Qt::Horizontal,param_name,Param::Name);
    delete[] param_name;

    // Param unit
    in >> sz;
    char* param_unit = new char[sz+1];
    in.readRawData(param_unit,sz);
    param_unit[sz] = '\0';
    setHeaderData(col,Qt::Horizontal,param_unit,Param::Unit);
    delete[] param_unit;

    // Param type
    qint32 param_type;
    in >> param_type;
    setHeaderData(col,Qt::Horizontal,param_type,Param::Type);

    // Param bytesize
    in >> sz;
    setHeaderData(col,Qt::Horizontal,sz,Param::Size);
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
