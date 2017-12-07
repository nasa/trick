#include "csvmodel.h"
#include "timeit_linux.h"

QString CsvModel::_err_string;
QTextStream CsvModel::_err_stream(&CsvModel::_err_string);

CsvModel::CsvModel(const QStringList& timeNames,
                   const QString& csvfile,
                   QObject *parent) :
    DataModel(timeNames, csvfile, parent),
    _timeNames(timeNames),_csvfile(csvfile),
    _nrows(0), _ncols(0),_iteratorTimeIndex(0),
    _data(0)
{
    _init();
}

void CsvModel::_init()
{
    QFile file(_csvfile);

    if (!file.open(QIODevice::ReadOnly)) {
        _err_stream << "koviz [error]: could not open "
                    << _csvfile << "\n";
        throw std::runtime_error(_err_string.toLatin1().constData());
    }
    QTextStream in(&file);

    QString line0 = in.readLine();
    QStringList items = line0.split(',',QString::SkipEmptyParts);
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
                    fprintf(stderr,"koviz [error]: Cannot handle "
                                   "unit=\"%s\" in file=\"%s\"\n",
                                   unit.toLatin1().constData(),
                                   _csvfile.toLatin1().constData());
                    exit(-1);
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
                    << _timeNames.join("=") << "\" in file=" << _csvfile
                    << ".  Try setting -timeName on commandline option.";
        throw std::runtime_error(_err_string.toLatin1().constData());
    }

    _iteratorTimeIndex = new CsvModelIterator(0,this,
                                              _timeCol,_timeCol,_timeCol);

    // Get number of data rows in csv file
    while ( !in.atEnd() ) {
        in.readLine();
        ++_nrows;
    }

    // Allocate to hold *all* parsed data
    _data = (double*)malloc(_nrows*_ncols*sizeof(double));

    // Read in data
    in.seek(0);
    in.readLine(); // csv header line
    int i = 0;
    while ( !in.atEnd() ) {
        QString line = in.readLine();
        int from = 0;
        while (1) {
            int j = line.indexOf(',',from);
            if ( j < 0 ) {
                _data[i] = line.mid(from,line.size()-from).toDouble();
                ++i;
                break;
            }
            _data[i] = line.mid(from,j-from).toDouble();
            from = j+1;
            ++i;
        }
    }

    file.close();
}

void CsvModel::map()
{
}

void CsvModel::unmap()
{
}

int CsvModel::paramColumn(const QString &paramName) const
{
    return _paramName2col.value(paramName,-1);
}

ModelIterator *CsvModel::begin(int tcol, int xcol, int ycol) const
{
    return new CsvModelIterator(0,this,tcol,xcol,ycol);
}

CsvModel::~CsvModel()
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

Parameter CsvModel::param(int col) const
{
    return *(_col2param.value(col));
}

int CsvModel::indexAtTime(double time)
{
    return _idxAtTimeBinarySearch(_iteratorTimeIndex,0,rowCount()-1,time);
}

int CsvModel::_idxAtTimeBinarySearch (CsvModelIterator* it,
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

int CsvModel::rowCount(const QModelIndex &pidx) const
{
    if ( ! pidx.isValid() ) {
        return _nrows;
    } else {
        return 0;
    }
}

int CsvModel::columnCount(const QModelIndex &pidx) const
{
    if ( ! pidx.isValid() ) {
        return _ncols;
    } else {
        return 0;
    }
}


QVariant CsvModel::data(const QModelIndex &idx, int role) const
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
