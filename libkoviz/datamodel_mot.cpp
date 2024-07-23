#include "datamodel_mot.h"

QString MotModel::_err_string;
QTextStream MotModel::_err_stream(&MotModel::_err_string);

MotModel::MotModel(const QStringList& timeNames,
                   const QString &runPath,
                   const QString& motfile,
                   QObject *parent) :
    DataModel(timeNames, runPath, motfile, parent),
    _timeNames(timeNames),_motfile(motfile),
    _nrows(0), _ncols(0),_iteratorTimeIndex(0),
    _data(0)
{
    _init();
}

void MotModel::_init()
{
    QFile file(_motfile);

    if (!file.open(QIODevice::ReadOnly)) {
        _err_stream << "koviz [error]: could not open "
                    << _motfile << "\n";
        throw std::runtime_error(_err_string.toLatin1().constData());
    }
    QTextStream in(&file);
    in.setCodec("UTF-8");

    // Header - skip until line with variables
    bool isEndHeader = false;
    QString line;
    while ( 1 ) {
        line = in.readLine();
        if ( line.isNull() ) {
            break;
        }
        if ( line.contains("endheader") ) {
            isEndHeader = true;
            break;
        }
    }
    if ( !isEndHeader ) {
        fprintf(stderr, "koviz [error]: \"endheader\" not found in file=%s\n",
                _motfile.toLatin1().constData());
        exit(-1);
    }


    // Read in variables
    line = in.readLine();
    if ( line.isNull() ) {
        // No param list!
        fprintf(stderr, "koviz [error]: malformed *.mot file=%s\n",
                _motfile.toLatin1().constData());
        exit(-1);
    }
    QStringList items = line.split('\t',QString::SkipEmptyParts);
    int col = 0;
    foreach ( QString item, items ) {
        QString name = item.trimmed();
        QString unit = "--";
        if ( name == "time" ) {
            unit = "s";
        }

        Parameter* param = new Parameter;
        param->setName(name);
        param->setUnit(unit);

        _col2param.insert(col,param);
        _paramName2col.insert(name,col);

        ++col;
    }
    _ncols = col;

    if ( _ncols == 0 ) {
        fprintf(stderr, "koviz [error]: no params in *.mot file=%s\n",
                         _motfile.toLatin1().constData());
        exit(-1);
    }

    // Time param should be column 0
    if ( _col2param.value(0)->name() == "time" ) {
        _timeCol = 0;
    } else {
        fprintf(stderr, "koviz [error]: \"time\" param not found in "
                        "file=%s\n",  _motfile.toLatin1().constData());
        exit(-1);
    }

    _iteratorTimeIndex = new MotModelIterator(0,this,
                                              _timeCol,_timeCol,_timeCol);

    // Get number of data rows in mot file
    while ( !in.atEnd() ) {
        in.readLine();
        ++_nrows;
    }

    // Allocate to hold *all* parsed data
    _data = (double*)malloc(_nrows*_ncols*sizeof(double));

    // Skip header
    in.seek(0);
    while ( 1 ) {
        line = in.readLine();
        if ( line.isNull() ) {
            break;
        }
        if ( line.contains("endheader") ) {
            break;
        }
    }
    in.readLine();

    // Read in data
    int i = 0;
    while ( !in.atEnd() ) {
        QString line = in.readLine();
        int from = 0;
        while (1) {
            int j = line.indexOf('\t',from);
            if ( j < 0 ) {
                _data[i] = _convert(line.mid(from,line.size()-from));
                ++i;
                break;
            }
            _data[i] = _convert(line.mid(from,j-from));
            from = j+1;
            ++i;
        }
    }

    file.close();
}

void MotModel::map()
{
}

void MotModel::unmap()
{
}

int MotModel::paramColumn(const QString &paramName) const
{
    int col = _paramName2col.value(paramName,-1);
    if ( paramName == "sys.exec.out.time" ) {
        col = _timeCol;
    }
    return col;
}

ModelIterator *MotModel::begin(int tcol, int xcol, int ycol) const
{
    return new MotModelIterator(0,this,tcol,xcol,ycol);
}

MotModel::~MotModel()
{
    foreach ( Parameter* param, _col2param.values() ) {
        delete param;
    }
    if ( _data ) {
        free(_data);
        _data = 0;
    }

    if ( _iteratorTimeIndex ) {
        delete _iteratorTimeIndex;
        _iteratorTimeIndex = 0;
    }
}

const Parameter* MotModel::param(int col) const
{
    return _col2param.value(col);
}

int MotModel::indexAtTime(double time)
{
    return _idxAtTimeBinarySearch(_iteratorTimeIndex,0,rowCount()-1,time);
}

int MotModel::_idxAtTimeBinarySearch (MotModelIterator* it,
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

double MotModel::_convert(const QString &s)
{
    double val = 0.0;

    bool ok;
    val = s.toDouble(&ok);
    if ( !ok ) {
        fprintf(stderr, "koviz [error]: mot file has bad value=%s\n",
                s.toLatin1().constData());
        exit(-1);
    }

    return val;
}

int MotModel::rowCount(const QModelIndex &pidx) const
{
    if ( ! pidx.isValid() ) {
        return _nrows;
    } else {
        return 0;
    }
}

int MotModel::columnCount(const QModelIndex &pidx) const
{
    if ( ! pidx.isValid() ) {
        return _ncols;
    } else {
        return 0;
    }
}


QVariant MotModel::data(const QModelIndex &idx, int role) const
{
    Q_UNUSED(role);
    QVariant val;

    if ( idx.isValid() && _data ) {
        int row = idx.row();
        int col = idx.column();
        val = _data[row*_ncols+col];
    }

    return val;
}

bool MotModel::isValid(const QString &motFile)
{
    QFile file(motFile);
    if (!file.open(QIODevice::ReadOnly)) {
        return false;
    }
    QTextStream in(&file);
    in.setCodec("UTF-8");

    // Header - check for "endheader" line
    bool isEndHeader = false;
    QString line;
    while ( 1 ) {
        line = in.readLine();
        if ( line.isNull() ) {
            break;
        }
        if ( line.contains("endheader") ) {
            isEndHeader = true;
            break;
        }
    }
    if ( !isEndHeader ) {
        file.close();
        return false;
    }

    // Parameter list
    line = in.readLine();
    if ( line.isNull() ) {
        // No param list!
        file.close();
        return false;
    }
    QStringList items = line.split('\t',QString::SkipEmptyParts);
    if ( items.isEmpty() ) {
        file.close();
        return false;
    }

    if ( items.at(0) != "time" ) {
        file.close();
        return false;
    }

    return true;
}
