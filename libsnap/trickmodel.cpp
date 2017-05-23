#include "trickmodel.h"
#include <QStringList>
#include <stdio.h>
#include <stdexcept>
#include <unistd.h>

QString TrickModel::_err_string;
QTextStream TrickModel::_err_stream(&TrickModel::_err_string);

TrickModel::TrickModel(const QString& timeName,
                       const QString& trkfile,
                       const QString& tableName,
                       double startTime, double stopTime, QObject *parent) :
    QAbstractTableModel(parent),
    _timeName(timeName),
    _trkfile(trkfile),
    _tableName(tableName),
    _startTime(startTime),
    _stopTime(stopTime),
    _nrows(0), _row_size(0), _ncols(0), _timeCol(0),_pos_beg_data(0),
    _mem(0), _data(0), _fd(-1), _iteratorTimeIndex(0)
{
    _load_trick_header();
    map();
}

bool TrickModel::_load_trick_header()
{
    bool ret = true;

    QFile file(_trkfile);

    if (!file.open(QIODevice::ReadOnly)) {
        _err_stream << "snap [error]: could not open "
                    << _trkfile << "\n";
        throw std::runtime_error(_err_string.toLatin1().constData());
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
    } else if ( data[0] == '0' && data[1] == '7' ) {
        _trick_version = TrickVersion07;
    } else {
        _err_stream << "snap [error]: unrecognized file or Trick version: "
                    << _trkfile << "\n";
        throw std::runtime_error(_err_string.toLatin1().constData());
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
        _col2offset[cc] = _row_size;
        _row_size += _load_binary_param(in,cc);
        Parameter* p = _col2param.value(cc);
        _paramtypes.push_back(p->type());
    }

    // Sanity check.  The bytes remaining should be a multiple of the record size
    qint64 nbytes = file.bytesAvailable();
    if ( nbytes % _row_size != 0 ) {
        _err_stream << "snap [error]: trk file \""
                    << file.fileName() << "\" is corrupt!\n";
        throw std::runtime_error(_err_string.toLatin1().constData());
    }

    int maxRows = nbytes/_row_size;

    // Make sure time param exists in model and set time column
    bool isFoundTime = false;
    foreach (QString timeName, _timeName.split("=")) {
        if ( _param2column.contains(timeName)) {
            _timeCol = _param2column.value(timeName) ;
            isFoundTime = true;
            break;
        }
    }
    if ( ! isFoundTime ) {
        _err_stream << "snap [error]: couldn't find time param \""
                    << _timeName << "\" in trkfile=" << _trkfile
                    << ".  Try setting -timeName on commandline option.";
        throw std::runtime_error(_err_string.toLatin1().constData());
    }

    // Save address of begin location of data for map()
    // Also, calc number of rows (timestamped records between start & stop time)
    _pos_beg_data = file.pos();
    _nrows = nbytes/(qint64)_row_size;
    if  (_startTime != 0.0 || _stopTime != 1.0e20 ) {
        bool isStart = false;
        double inputStopTime = _stopTime;
        _nrows = 0 ;
        for ( int i = 0 ; i < maxRows ; ++i ) {
            double timeStamp;
            in >> timeStamp;
            in.skipRawData(_row_size-sizeof(double));
            if ( !isStart && timeStamp >= _startTime-1.0e-9 ) {
                _pos_beg_data = file.pos()-_row_size;
                _startTime = timeStamp;
                isStart = true;
            }
            if ( isStart && timeStamp <= inputStopTime+1.0e-9 ) {
                _nrows++;
                _stopTime = timeStamp;
            } else if ( timeStamp > _stopTime ) {
                if ( isStart && _nrows == 0 ) {
                    _stopTime = timeStamp;
                    _nrows++; // odd case!
                }
                break;
            }
        }

        if ( !isStart && maxRows > 0 ) {
            _err_stream << "snap [error]: startTime of "
                        << _startTime
                        << " specified by user "
                        << "exceeded all timestamps in non-empty file:\n    "
                        << _trkfile;
            throw std::range_error(_err_string.toLatin1().constData());
        }
    }


    file.close();

    return ret;
}

// Returns byte size of parameter
qint32 TrickModel::_load_binary_param(QDataStream& in, int col)
{
    Parameter* param  = new Parameter();

    qint32 sz;

    // Param name
    in >> sz;
    char* param_name = new char[sz+1];
    in.readRawData(param_name,sz);
    param_name[sz] = '\0';
    param->setName(param_name);
    _param2column.insert(QString(param_name),col);
    delete[] param_name;

    // Param unit
    in >> sz;
    char* param_unit = new char[sz+1];
    in.readRawData(param_unit,sz);
    param_unit[sz] = '\0';
    param->setUnit(param_unit);
    delete[] param_unit;

    // Param type
    qint32 param_type;
    in >> param_type;
    param->setType(param_type);

    // Param bytesize
    in >> sz;
    param->setSize(sz);

    // Store param in hash
    _col2param.insert(col,param);

    return sz;
}

void TrickModel::map()
{
    if ( _data ) return; // already mapped

    _fd = open(_trkfile.toLatin1().constData(), O_RDONLY);
    if ( _fd < 0 ) {
        _err_stream << "snap [error]: TrickModel could not open "
                    << _trkfile << "\n";
        throw std::runtime_error(_err_string.toLatin1().constData());
    }
    fstat(_fd, &_fstat);

    _mem = (ptrdiff_t)mmap(NULL,_fstat.st_size,PROT_READ,MAP_SHARED,_fd,0);

    if ( (void*)_mem == MAP_FAILED ) {
        _err_stream << "snap [error]: TrickModel couldn't allocate memory for : "
                    << _trkfile << "\n";
        throw std::runtime_error(_err_string.toLatin1().constData());
    }

    _data = _mem + _pos_beg_data;

    if ( _iteratorTimeIndex ) {
        delete _iteratorTimeIndex;
    }
    _iteratorTimeIndex = new TrickModelIterator(0,this,
                                                _timeCol,_timeCol,_timeCol);
}

void TrickModel::unmap()
{
    if ( _data ) {
        munmap((void*)_mem, _fstat.st_size);
        close(_fd);
        _data = 0 ;
    }
    if ( _iteratorTimeIndex ) {
        delete _iteratorTimeIndex;
        _iteratorTimeIndex = 0;
    }
}

TrickModel::~TrickModel()
{
    unmap();
    foreach ( Parameter* param, _col2param.values() ) {
        delete param;
    }
}

Parameter TrickModel::param(int col) const
{
    return *(_col2param.value(col));
}

TrickModelIterator TrickModel::begin(int tcol, int xcol, int ycol) const
{
    return TrickModelIterator(0,this,tcol,xcol,ycol);
}

TrickModelIterator TrickModel::end(int tcol, int xcol, int ycol) const
{
    return TrickModelIterator(this->rowCount(),this,tcol,xcol,ycol);
}

int TrickModel::indexAtTime(double time)
{
    return _idxAtTimeBinarySearch(*_iteratorTimeIndex,0,rowCount()-1,time);
}

void TrickModel::writeTrkHeader(QDataStream &out, const QList<Param>& params)
{
    // Make it little endian
    out.setByteOrder(QDataStream::LittleEndian);

    //
    // Trick-version-endian (10 bytes)
    //
    out.writeRawData("Trick-",6) ;
    out.writeRawData("07",2) ;
    out.writeRawData("-",1) ;
    out.writeRawData("L",1) ; // little endian

    //
    // Num params recorded (4 bytes - int)
    //
    qint32 nparams = (qint32)(params.size());
    out << nparams;

    //
    // Write trick header param info
    //
    foreach ( Param p, params ) {
        TrickModel::_write_binary_param(out,p);
    }
}

void TrickModel::_write_binary_param(QDataStream& out, const Param& p)
{
    // Write name, unit and type info
    TrickModel::_write_binary_qstring(out, p.name);
    TrickModel::_write_binary_qstring(out, p.unit);
    qint32 t = (qint32)p.type; out << t;
    qint32 s = (qint32)p.size; out << s;
}


void TrickModel::_write_binary_qstring(QDataStream& out, const QString &str)
{
    qint32 size_str = (qint32) str.size();
    out << size_str;
    out.writeRawData(str.toLatin1().constData(),str.size());
}

int TrickModel::_idxAtTimeBinarySearch (TrickModelIterator& it,
                                       int low, int high, double time)
{
        if (high <= 0 ) {
                return 0;
        }
        if (low >= high) {
                return ( it[high].t() > time ) ? high-1 : high;
        } else {
                int mid = (low + high)/2;
                if (time == it[mid].t()) {
                        return mid;
                } else if ( time < it[mid].t() ) {
                        return _idxAtTimeBinarySearch(it,
                                                      low, mid-1, time);
                } else {
                        return _idxAtTimeBinarySearch(it,
                                                      mid+1, high, time);
                }
        }
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
            ptrdiff_t addr = _data+_pos_data;
            int paramtype =  _paramtypes.at(col);
            val = _toDouble(addr,paramtype);
        }
    }

    return val;
}
