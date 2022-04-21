#include "datamodel_trick.h"
#include <QStringList>
#include <stdio.h>
#include <stdexcept>
#include <unistd.h>

QString TrickModel::_err_string;
QTextStream TrickModel::_err_stream(&TrickModel::_err_string);

TrickModel::TrickModel(const QStringList& timeNames,
                       const QString& trkfile, QObject *parent) :
    DataModel(timeNames, trkfile, parent),
    _timeNames(timeNames),_trkfile(trkfile),
    _nrows(0), _row_size(0), _ncols(0), _timeCol(0),_pos_beg_data(0),
    _mem(0), _data(0), _fd(-1), _file(_trkfile),_iteratorTimeIndex(0)
{
    _load_trick_header();
    map();
}

bool TrickModel::_load_trick_header()
{
    bool ret = true;

    if (!_file.open(QIODevice::ReadOnly)) {
        _err_stream << "koviz [error]: could not open "
                    << _trkfile << "\n";
        throw std::runtime_error(_err_string.toLatin1().constData());
    }
    QDataStream in(&_file);

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
        _err_stream << "koviz [error]: unrecognized file or Trick version: "
                    << _trkfile << "\n";
        throw std::runtime_error(_err_string.toLatin1().constData());
    }

    in.readRawData(data,1) ; // -
    in.readRawData(data,1) ; // L or B (endian)
    if ( data[0] == 'L' ) {
        in.setByteOrder(QDataStream::LittleEndian);
    } else {
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
        TrickParameter* p = _col2param.value(cc);
        _paramtypes.push_back(p->type());
    }
    if ( _row_size == 0 ) {
        _err_stream << "koviz [error]: trk file \""
                    << _file.fileName() << "\" is corrupt!\n";
        throw std::runtime_error(_err_string.toLatin1().constData());
    }

    // Sanity check. Bytes remaining should be a multiple of the record size
    qint64 nbytes = _file.bytesAvailable();
    if ( nbytes % _row_size != 0 ) {
        _err_stream << "koviz [error]: trk file \""
                    << _file.fileName() << "\" is corrupt!\n";
        throw std::runtime_error(_err_string.toLatin1().constData());
    }

    // Make sure time param exists in model and set time column
    bool isFoundTime = false;
    foreach (QString timeName, _timeNames) {
        if ( _param2column.contains(timeName)) {
            _timeCol = _param2column.value(timeName) ;
            isFoundTime = true;
            break;
        }
    }
    if ( ! isFoundTime ) {
        _err_stream << "koviz [error]: couldn't find time param \""
                    << _timeNames.join("=") << "\" in trkfile=" << _trkfile
                    << ".  Try setting -timeName on commandline option.";
        throw std::runtime_error(_err_string.toLatin1().constData());
    }

    // Save address of begin location of data for map()
    _pos_beg_data = _file.pos();

    // Calculate number of timestamped records in file
    _nrows = nbytes/(qint64)_row_size;

    _file.close();

    return ret;
}

// Returns byte size of parameter
qint32 TrickModel::_load_binary_param(QDataStream& in, int col)
{
    TrickParameter* param  = new TrickParameter;

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

    if (!_file.open(QIODevice::ReadOnly)) {
        _err_stream << "koviz [error]: could not open "
                    << _file.fileName() << "\n";
        throw std::runtime_error(_err_string.toLatin1().constData());
    }

    _mem = (ptrdiff_t) _file.map(0,_file.size());

    if ( _mem == 0 ) {
        _err_stream << "koviz [error]: TrickModel couldn't allocate memory for : "
                    << _file.fileName() << "\n";
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
        _file.unmap((uchar*)_mem);
        _file.close();
        _data = 0 ;
    }
    if ( _iteratorTimeIndex ) {
        delete _iteratorTimeIndex;
        _iteratorTimeIndex = 0;
    }
}

ModelIterator *TrickModel::begin(int tcol, int xcol, int ycol) const
{
    return new TrickModelIterator(0,this,tcol,xcol,ycol);
}

TrickModel::~TrickModel()
{
    unmap();
    foreach ( Parameter* param, _col2param.values() ) {
        delete param;
    }
}

const Parameter* TrickModel::param(int col) const
{
    return _col2param.value(col);
}

int TrickModel::indexAtTime(double time)
{
    return _idxAtTimeBinarySearch(_iteratorTimeIndex,0,rowCount()-1,time);
}

void TrickModel::writeTrkHeader(QDataStream &out,
                                const QList<TrickParameter>& params)
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
    foreach ( TrickParameter p, params ) {
        TrickModel::_write_binary_param(out,p);
    }
}

void TrickModel::_write_binary_param(QDataStream& out, const TrickParameter& p)
{
    // Write name, unit and type info
    TrickModel::_write_binary_qstring(out, p.name());
    TrickModel::_write_binary_qstring(out, p.unit());
    qint32 t = (qint32)p.type(); out << t;
    qint32 s = (qint32)p.size(); out << s;
}


void TrickModel::_write_binary_qstring(QDataStream& out, const QString &str)
{
    qint32 size_str = (qint32) str.size();
    out << size_str;
    out.writeRawData(str.toLatin1().constData(),str.size());
}

int TrickModel::_idxAtTimeBinarySearch (TrickModelIterator* it,
                                       int low, int high, double time)
{
        if (high <= 0 ) {
                return 0;
        }
        if (low >= high) {
                // Time not found, choose closest near high
                double t1 = it->at(high-1)->t();
                double t2 = it->at(high)->t();
                double t3 = t2;
                it = it->at(high+1);
                if ( !it->isDone() ) {
                    t3 = it->at(high+1)->t();
                }

                int i;
                if ( qAbs(time-t1) < qAbs(time-t2) ) {
                    if ( qAbs(time-t1) < qAbs(time-t3) ) {
                        i = high-1;
                    } else {
                        if ( !it->isDone() ) {
                            i = high+1;
                        } else {
                            i = high;
                        }
                    }
                } else {
                    if ( qAbs(time-t2) < qAbs(time-t3) ) {
                        i = high;
                    } else {
                        if ( !it->isDone() ) {
                            i = high+1;
                        } else {
                            i = high;
                        }
                    }
                }
                return i;
        } else {
                int mid = (low + high)/2;
                if (time == it->at(mid)->t()) {
                        return mid;
                } else if ( time < it->at(mid)->t() ) {
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
