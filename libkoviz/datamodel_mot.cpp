#include "datamodel_mot.h"

QString MotModel::_err_string;
QTextStream MotModel::_err_stream(&MotModel::_err_string);

MotModel::MotModel(const QStringList& timeNames,
                   const QString& motfile,
                   QObject *parent) :
    DataModel(timeNames, motfile, parent),
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

    QStringList items = line.split(',',QString::SkipEmptyParts);
    int col = 0;
    foreach ( QString item, items ) {
        QString name;
        QString unit;
        if ( item.contains('{') && item.contains('}') ) {
            // Unit is between curlies
            int i = item.indexOf('{');
            int j = item.indexOf('}');
            if ( i < j ) {
                unit = item.mid(i+1,j-i-1);
                if ( !Unit::isUnit(unit) ) {
                    fprintf(stderr,"koviz [warning]: Unsupported "
                                   "unit=\"%s\" in file=\"%s\"\n",
                                   unit.toLatin1().constData(),
                                   _motfile.toLatin1().constData());
                    unit = "--";
                }
            }
            // Assume parameter name precedes unit
            if ( i > 0 ) {
                name = item.left(i-1).trimmed();
            }
        } else {
            name = item.trimmed();
            unit = "--";
        }

        Parameter* param = new Parameter;
        param->setName(name);
        param->setUnit(unit);

        _col2param.insert(col,param);
        _paramName2col.insert(name,col);

        ++col;
    }
    _ncols = col;

    // Make sure time param exists in model and set time column
    bool isFoundTime = false;
    foreach (QString timeName, _timeNames) {
        if ( _paramName2col.contains(timeName)) {
            _timeCol = _paramName2col.value(timeName) ;
            isFoundTime = true;
            break;
        }
    }
    if ( ! isFoundTime ) {
        _err_stream << "koviz [error]: couldn't find time param \""
                    << _timeNames.join("=") << "\" in file=" << _motfile
                    << ".  Try setting -timeName on commandline option.";
        throw std::runtime_error(_err_string.toLatin1().constData());
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

    // Read in data
    in.seek(0);
    in.readLine(); // mot header line
    int i = 0;
    while ( !in.atEnd() ) {
        QString line = in.readLine();
        int from = 0;
        while (1) {
            int j = line.indexOf(',',from);
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
    return _paramName2col.value(paramName,-1);
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
                return ( it->at(high)->t() > time ) ? high-1 : high;
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
        QStringList vals = s.split(":");
        if (vals.length() == 3 ) {
            // Try converting to a utc timestamp
            val = 3600.0*vals.at(0).toDouble(&ok);
            if ( ok ) {
                val += 60.0*vals.at(1).toDouble(&ok);
                if ( ok ) {
                    val += vals.at(2).toDouble(&ok);
                }
            }
        }
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
