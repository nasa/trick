#include "boundedtrickbinaryriver.h"

#include <QString>

BoundedTrickBinaryRiver::BoundedTrickBinaryRiver(
        char *filename,  double start, double stop) :
    TrickBinaryRiver(filename),
    _start(start),
    _stop(stop),
    _npoints(0),
    _timestamps(0)
{
    // start time greater than first timestamp?
    QString qs = QString("BoundedTrickBinaryRiver [range error]: "
                         "start=%1 stop=%2 filename=\"%3\"\n").
            arg(start).arg(stop).arg(filename);
    if ( start > stop ) {
        throw std::range_error(qs.toAscii().constData());
    }

    int np = TrickBinaryRiver::getNumPoints();
    double *ts = TrickBinaryRiver::getTimeStamps();
    bool is_first = true;
    for ( int ii = 0; ii < np; ++ii) {
        double tt = ts[ii];
        if ( tt < start - 0.000001 ) {
            continue;
        }
        if ( tt > stop + 0.000001 && is_first == false ) {
            break;
        }
        _stop = tt;

        if ( is_first ) {
            _timestamps = &(ts[ii]);
            std::vector<LOG_PARAM> params = TrickBinaryRiver::getParamList() ;
            for ( uint jj = 0; jj < params.size(); ++jj) {
                LOG_PARAM param = params[jj];
                double* vals = TrickBinaryRiver::getVals(
                            const_cast<char*>(param.data.name.c_str()));
                _vals[const_cast<char*>(param.data.name.c_str())] =  &(vals[ii]);

            }
            is_first = false ;
        }
        _npoints++;
    }

    if ( _timestamps == 0 ) {
        throw std::range_error(qs.toAscii().constData());
    }
}
