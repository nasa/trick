#include "datamodel_csv.h"

QString CsvModel::_err_string;
QTextStream CsvModel::_err_stream(&CsvModel::_err_string);

CsvModel::CsvModel(const QStringList& timeNames,
                   const QString &runPath,
                   const QString& csvfile,
                   QObject *parent) :
    DataModel(timeNames, runPath, csvfile, parent),
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
    in.setCodec("UTF-8");

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
                    fprintf(stderr,"koviz [warning]: Unsupported "
                                   "unit=\"%s\" in file=\"%s\"\n",
                                   unit.toLatin1().constData(),
                                   _csvfile.toLatin1().constData());
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
                    << _timeNames.join("=") << "\" in file=" << _csvfile
                    << ".  Try setting -timeName on commandline option.";
        throw std::runtime_error(_err_string.toLatin1().constData());
    }

    _iteratorTimeIndex = new CsvModelIterator(0,this,
                                              _timeCol,_timeCol,_timeCol);

#ifdef __linux
    TimeItLinux timer;
    timer.start();
#endif

    // Get number of data rows in csv file
    while ( !in.atEnd() ) {
        in.readLine();
        ++_nrows;
    }

    // Allocate to hold *all* parsed data
    _data = (double*)malloc(_nrows*_ncols*sizeof(double));

    // Begin Progress Dialog
    QString msg("Loading ");
    msg += QFileInfo(fileName()).fileName();
    msg += "...";
    QProgressDialog progress(msg, "Abort", 0, _nrows-1, 0);
    progress.setWindowModality(Qt::WindowModal);
    int row = 0;

    // Read in data
    in.seek(0);
    in.readLine(); // csv header line
    int i = 0;
    while ( !in.atEnd() ) {
        if (progress.wasCanceled()) {
             break;
        }
        if ( row % 10000 == 0 ) {
            progress.setValue(row);
        }
        ++row;

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

#ifdef __linux
        if ( row % 10000 == 0 ) {
            int secs = qRound(timer.stop()/1000000.0);
            div_t d = div(secs,60);
            QString msg = QString("Loaded %1 of %2 lines "
                                  "(%3 min %4 sec)")
                    .arg(row).arg(_nrows).arg(d.quot).arg(d.rem);
            progress.setLabelText(msg);
        }
#endif
    }

    // End Progress Dialog
    progress.setValue(_nrows-1);

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

const Parameter* CsvModel::param(int col) const
{
    return _col2param.value(col);
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

double CsvModel::_convert(const QString &s)
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
    if ( !ok ) {
        // If single char, convert to unicode value
        if ( s.size() == 1 ) {
            val = s.at(0).unicode();
        }
    }

    return val;
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
