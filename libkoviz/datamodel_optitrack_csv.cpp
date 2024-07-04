#include "datamodel_optitrack_csv.h"

QString OptiTrackCsvModel::_err_string;
QTextStream OptiTrackCsvModel::_err_stream(&OptiTrackCsvModel::_err_string);

OptiTrackCsvModel::OptiTrackCsvModel(const QStringList& timeNames,
                   const QString &runPath,
                   const QString& csvFile,
                   QObject *parent) :
    DataModel(timeNames, runPath, csvFile, parent),
    _timeNames(timeNames),_csvFile(csvFile),
    _nrows(0), _ncols(0),
    _timeCol(0),  // Time is created and placed in first column
    _iteratorTimeIndex(0),
    _data(0)
{
    _init();
}

void OptiTrackCsvModel::_init()
{
    QFile file(_csvFile);

    if (!file.open(QIODevice::ReadOnly)) {
        _err_stream << "koviz [error]: could not open "
                    << _csvFile << "\n";
        throw std::runtime_error(_err_string.toLatin1().constData());
    }
    QTextStream in(&file);
    in.setCodec("UTF-8");

    // Read header
    bool isFrameRate = false;
    bool isNumFrames = false;
    bool isEndHeader = false;
    QString line;
    while ( 1 ) {
        line = in.readLine();
        if ( line.startsWith("Capture Frame Rate") ) {
            QStringList fields = line.split(',');
            _frameRate = fields.at(1).toDouble();
            isFrameRate = true;
        }
        if ( line.startsWith("Total Exported Frames") ) {
            QStringList fields = line.split(',');
            _nrows = fields.at(1).toDouble();
            isNumFrames = true;
        }
        if ( line.startsWith("MocapFrame") ) {
            QStringList fields = line.split(',');
            _ncols = fields.size()+1; // +1 for sys.exec.out.time

            // Create sys.exec.out.time (DeviceFrame/CaptureFrameRate)
            int col = _timeCol; // time column is 0
            Parameter* param = new Parameter;
            param->setName("sys.exec.out.time");
            param->setUnit("s");
            _col2param.insert(col,param);
            _paramName2col.insert(param->name(),col);
            ++col;

            foreach ( QString field, fields ) {
                QString name = field.trimmed();
                QString unit = "--";

                Parameter* param = new Parameter;
                param->setName(name);
                param->setUnit(unit);

                _col2param.insert(col,param);
                _paramName2col.insert(name,col);
                ++col;
            }
            isEndHeader = true;
            break;
        }

    }
    if ( !isFrameRate ) {
        fprintf(stderr, "koviz [error]: \"Capture Frame Rate\" "
                        "not found in file=%s\n",
                        _csvFile.toLatin1().constData());
        exit(-1);
    }
    if ( !isNumFrames ) {
        fprintf(stderr, "koviz [error]: \"Total Exported Frames\" "
                        "not found in file=%s\n",
                        _csvFile.toLatin1().constData());
        exit(-1);
    }
    if ( !isEndHeader ) {
        fprintf(stderr, "koviz [error]: MocapFrame not found in file=%s\n",
                _csvFile.toLatin1().constData());
        exit(-1);
    }

    _iteratorTimeIndex = new OptiTrackCsvModelIterator(0,this,
                                              _timeCol,_timeCol,_timeCol);

    // Allocate to hold *all* parsed data
    // Time is calculated so not in _data (reason for _ncols-1)
    _data = (double*)malloc(_nrows*(_ncols-1)*sizeof(double));

    // Read in data
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

void OptiTrackCsvModel::map()
{
}

void OptiTrackCsvModel::unmap()
{
}

int OptiTrackCsvModel::paramColumn(const QString &paramName) const
{
    return _paramName2col.value(paramName,-1);
}

ModelIterator *OptiTrackCsvModel::begin(int tcol, int xcol, int ycol) const
{
    return new OptiTrackCsvModelIterator(0,this,tcol,xcol,ycol);
}

OptiTrackCsvModel::~OptiTrackCsvModel()
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

const Parameter* OptiTrackCsvModel::param(int col) const
{
    return _col2param.value(col);
}

int OptiTrackCsvModel::indexAtTime(double time)
{
    return _idxAtTimeBinarySearch(_iteratorTimeIndex,0,rowCount()-1,time);
}

int OptiTrackCsvModel::_idxAtTimeBinarySearch (OptiTrackCsvModelIterator* it,
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

double OptiTrackCsvModel::_convert(const QString &s)
{
    double val = 0.0;

    bool ok;
    val = s.toDouble(&ok);
    if ( !ok ) {
        fprintf(stderr, "koviz [error]: Optitrack csv file has bad value=%s\n",
                s.toLatin1().constData());
        exit(-1);
    }

    return val;
}

int OptiTrackCsvModel::rowCount(const QModelIndex &pidx) const
{
    if ( ! pidx.isValid() ) {
        return _nrows;
    } else {
        return 0;
    }
}

int OptiTrackCsvModel::columnCount(const QModelIndex &pidx) const
{
    if ( ! pidx.isValid() ) {
        return _ncols;
    } else {
        return 0;
    }
}


QVariant OptiTrackCsvModel::data(const QModelIndex &idx, int role) const
{
    Q_UNUSED(role);
    QVariant val;

    if ( idx.isValid() && _data ) {
        int row = idx.row();
        int col = idx.column();
        if ( col == 0 ) {
            // time = DeviceFrame/(CaptureFrameRate)
            val = _data[row*(_ncols-1)+2]/_frameRate;
        } else {
            val = _data[row*(_ncols-1)+(col-1)];
        }
    }

    return val;
}

bool OptiTrackCsvModel::isValid(const QString &fileName)
{
    bool isOptiTrack = false;
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        return false;
    }
    QTextStream in(&file);
    in.setCodec("UTF-8");
    QString line0 = in.readLine();
    QString line1 = in.readLine();
    if ( line0.startsWith("Format Version") &&
         line1.startsWith("Take Name") ) {
        isOptiTrack = true;
    }
    file.close();
    return isOptiTrack;
}
