#include "datamodel_tv.h"

TVModel::TVModel(const QString& host, int port, QObject *parent) :
    DataModel(QStringList(), "", parent),
    _host(host),_port(port),
    _timeCol(0), _iteratorTimeIndex(0)
{
    _iteratorTimeIndex = new TVModelIterator(0,this,
                                              _timeCol,_timeCol,_timeCol);
    _init();
}

void TVModel::map()
{
}

void TVModel::unmap()
{
}

ModelIterator *TVModel::begin(int tcol, int xcol, int ycol) const
{
    return new TVModelIterator(0,this,tcol,xcol,ycol);
}

TVModel::~TVModel()
{
}

const Parameter* TVModel::param(int col) const
{
    return &_params.at(col);
}

int TVModel::paramColumn(const QString &paramName) const
{
    int col = 0;
    foreach ( TVParam param, _params ) {
        if ( param.name() == paramName ) {
            break;
        }
        ++col;
    }
    return col;
}

int TVModel::indexAtTime(double time)
{
    return _idxAtTimeBinarySearch(_iteratorTimeIndex,0,rowCount()-1,time);
}


int TVModel::_idxAtTimeBinarySearch (TVModelIterator* it,
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

int TVModel::rowCount(const QModelIndex &pidx) const
{
    int nRows = 0;

    if ( ! pidx.isValid() ) {
        // nRows is max param.values.size()
        foreach ( TVParam param, _params ) {
            if ( param.values.size() > nRows ) {
                nRows = param.values.size();
            }
        }
    }

    return nRows;
}

int TVModel::columnCount(const QModelIndex &pidx) const
{
    if ( ! pidx.isValid() ) {
        return _params.size();
    } else {
        return 0;
    }
}

QVariant TVModel::data(const QModelIndex &idx, int role) const
{
    QVariant val;

    int nValuesMissed = _params.at(idx.column()).nValuesMissed ;
    int nValues = _params.at(idx.column()).values.size();
    if ( idx.row() >= nValuesMissed && idx.row() < nValuesMissed+nValues ) {
        val = _params.at(idx.column()).values.at(idx.row()-nValuesMissed);
    }

    return val;
}

// Only appending supported
bool TVModel::insertRows(int row, int count, const QModelIndex &parent)
{
    bool isInsertedRows = false;

    if ( ! parent.isValid() && count > 0 ) {
        int nRows = rowCount(parent);
        if ( row == nRows ) {
            // Append rows
            beginInsertRows(parent,row,row+count-1);
            QList<QVariant> emptyValues;
            for ( int i = 0; i < count; ++i ) {
                QVariant emptyValue;
                emptyValues.append(emptyValue);
            }
            foreach ( TVParam param, _params ) {
                param.values.append(emptyValues);
            }
            isInsertedRows = true;
            endInsertRows();
        } else {
            fprintf(stderr, "koviz [bad scoobs]: TVModel::insertRows "
                            "append not supported!\n");
            exit(-1);
        }
    }

    return isInsertedRows;
}

void TVModel::addParam(const QString &paramName, const QString& unit)
{
    TVParam param(paramName,unit);

    QString msg = QString("trick.var_add(\"%1\")\n").arg(paramName);
    _vsSocket.write(msg.toUtf8());
}

void TVModel::_init()
{
    connect(&_vsSocket,SIGNAL(readyRead()), this,SLOT(_vsRead()));

    _vsSocket.connectToHost(_host,_port);
    if (!_vsSocket.waitForConnected(1000)) {
        fprintf(stderr, "koviz [error]: "
                        "Could not connect to trick var server on"
                        "host=%s port=%d!\n",
                _host.toLatin1().constData(),_port);
        return;
    }

    addParam("time", "s");
    addParam("ball.state.output.position[0]", "m");
    addParam("ball.state.output.position[1]", "m");
}

void TVModel::_vsRead()
{
    QByteArray bytes = _vsSocket.readLine();
    QString msg(bytes);
    QStringList fields = msg.split('\t');
    fprintf(stderr, "read=%s", msg.toLatin1().constData());
}
