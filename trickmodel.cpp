#include "trickmodel.h"
#include <stdio.h>

const int TrickModel::TRICK_DOUBLE             = DEF_TRICK_DOUBLE;
const int TrickModel::TRICK_LONG_LONG          = DEF_TRICK_LONG_LONG;
const int TrickModel::TRICK_UNSIGNED_LONG_LONG = DEF_TRICK_UNSIGNED_LONG_LONG;

TrickModel::TrickModel(const QString& trkfile,
                     const QString& tableName, QObject *parent) :
    SnapTable(tableName,parent),
    _trkfile(trkfile),
    _tableName(tableName)
{
    _fd = open(trkfile.toAscii().constData(), O_RDONLY);
    fstat(_fd, &_fstat);

    _mem = (char*)mmap(NULL,_fstat.st_size,PROT_READ,MAP_SHARED,_fd,0);

    if (_mem == MAP_FAILED) {
        // TODO: throw error
    }

    _load_trick_header();
}

bool TrickModel::_load_trick_header()
{
    bool ret = true;

    QFile file(_trkfile);

    if (!file.open(QIODevice::ReadOnly)) {
        fprintf(stderr,"Snap: [error] could not open %s\n",
                _trkfile.toAscii().constData());
        return false;
    }
    QDataStream in(&file);

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
        in.setByteOrder(QDataStream::LittleEndian);
    } else {
        _endianess = BigEndian;
        in.setByteOrder(QDataStream::BigEndian);
    }

    //
    // Number of parameters (4 byte integer)
    //
    in >> _ncols;

    //
    // Read param header info (name,unit,type,bytesize)
    //
    _row_size = 0 ;
    for ( int cc = 0; cc < _ncols; ++cc) {
        _col_headers.insert(cc,new QVariant);
        Role* role = _createColumnRole();
        _col_roles.insert(cc,role);
        _col2offset[cc] = _row_size;
        _row_size += _load_binary_param(in,cc);
        int paramtype = headerData(cc,Qt::Horizontal,Param::Type).toInt();
        _paramtypes.push_back(paramtype);
    }

    // Save address of start location of data in _data
    int pos_beg_data = file.pos();
    _data = &_mem[pos_beg_data];

    // Sanity check.  The bytes remaining should be a multiple of the record size
    qint64 nbytes = file.bytesAvailable();
    if ( nbytes % _row_size != 0 ) {
        // TODO: throw runtime exception
        fprintf(stderr,"snap [error]: trk file %s is corrupt!\n",
                file.fileName().toAscii().constData());
        exit(-1);
    }

    // Calc number of data records in trk file
    _nrows = nbytes/(qint64)_row_size;

    file.close();

    return ret;
}

// Returns byte size of parameter
qint32 TrickModel::_load_binary_param(QDataStream& in, int col)
{
    qint32 sz;

    // Param name
    in >> sz;
    char* param_name = new char[sz+1];
    in.readRawData(param_name,sz);
    param_name[sz] = '\0';
    setHeaderData(col,Qt::Horizontal,param_name,Param::Name);
    setHeaderData(col,Qt::Horizontal,param_name,Qt::EditRole);
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

    return sz;
}

TrickModel::~TrickModel()
{
    munmap(_mem, _fstat.st_size);
    close(_fd);
}

TrickModelIterator TrickModel::begin(int tcol, int xcol, int ycol) const
{
    return TrickModelIterator(0,this,tcol,xcol,ycol);
}

TrickModelIterator TrickModel::end(int tcol, int xcol, int ycol) const
{
    return TrickModelIterator(this->rowCount(),this,tcol,xcol,ycol);
}


int TrickModel::rowCount(const QModelIndex &pidx) const
{
    if ( ! pidx.isValid() ) {
        return _nrows;
    } else {
        return 0;
    }
}

int TrickModel::columnCount(const QModelIndex &pidx) const
{
    if ( ! pidx.isValid() ) {
        return _ncols;
    } else {
        return 0;
    }
}


// TODO: byteswap like
//      dp = (double *)(address) ;
//      value = _swap ? trick_byteswap_double(*dp) : *dp ;
QVariant TrickModel::data(const QModelIndex &idx, int role) const
{
    QVariant val;

    if ( idx.isValid() ) {
        int row = idx.row();
        int col = idx.column();

        if ( role == Qt::DisplayRole ) {
            qint64 _pos_data = row*_row_size + _col2offset.value(col);
            const char* addr = &(_data[_pos_data]);
            int paramtype =  _paramtypes.at(col);
            val = _toDouble(addr,paramtype);
        }
    }

    return val;
}

// mmap is read only for now
bool TrickModel::setData(const QModelIndex &idx, const QVariant &value, int role)
{
    Q_UNUSED(idx);
    Q_UNUSED(value);
    Q_UNUSED(role);
    return false;
}

// mmap is read only for now
bool TrickModel::insertRows(int row, int count, const QModelIndex &pidx)
{
    Q_UNUSED(row);
    Q_UNUSED(count);
    Q_UNUSED(pidx);
    return false;
}

// mmap is read only for now
bool TrickModel::removeRows(int row, int count, const QModelIndex &pidx)
{
    Q_UNUSED(row);
    Q_UNUSED(count);
    Q_UNUSED(pidx);
    return false;
}

// mmap is read only for now
bool TrickModel::insertColumns(int column, int count,
                                   const QModelIndex &pidx)
{
    Q_UNUSED(column);
    Q_UNUSED(count);
    Q_UNUSED(pidx);
    return false;
}

// mmap is read only for now
bool TrickModel::removeColumns(int column, int count, const QModelIndex &pidx)
{
    Q_UNUSED(column);
    Q_UNUSED(count);
    Q_UNUSED(pidx);
    return false;
}

Role* TrickModel::_createColumnRole()
{
    Param* param = new Param;
    return param;
}

