#include "datamodel_tv.h"

TVModel::TVModel(const QString& host, int port, QObject *parent) :
    DataModel(QStringList(), "", parent),
    _host(host),_port(port),
    _timeCol(0), _iteratorTimeIndex(0)
{
    _iteratorTimeIndex = new TVModelIterator(0,this,
                                              _timeCol,_timeCol,_timeCol);
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
    return &_params.at(col).param;
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
    if ( ! pidx.isValid() ) {
        return _params.at(0).values.size();
    } else {
        return 0;
    }
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
    return val;
}
