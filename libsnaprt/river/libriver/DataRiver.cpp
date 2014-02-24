#include "DataRiver.hh"
#include <iostream>
#include <string>
using std::string;
using std::cerr;
using std::endl;

DataRiver::~DataRiver() 
{
}

// Given a time, return index of array of timestamps 
// which correspond to timestamp
// If no matching timestamp for time,
// return index of timestamp most immediate (prior) 
// to time 
int DataRiver::getIndexAtTime( double* time ) 
{
        return _idxAtTimeBinarySearch( getTimeStamps(), 
                                       0, (getNumPoints() - 1), time );
}


int DataRiver::_idxAtTimeBinarySearch (double* timestamps, 
                                       int low, int high, double *time)
{
        if (high <= 0 ) { 
                return 0; 
        }
        if (low >= high) {
                return ( timestamps[high] > *time ) ? high-1 : high;
        } else {
                int mid = (low + high)/2;
                if (*time == timestamps[mid]) { 
                        return mid; 
                } else if ( *time < timestamps[mid] ) {
                        return _idxAtTimeBinarySearch(timestamps, 
                                                      low, mid-1, time);
                } else {
                        return _idxAtTimeBinarySearch(timestamps, 
                                                      mid+1, high, time);
                }
        }
}

void DataRiver::getOneShotValue(const string& name, double& value, string& units)
{
    // Assigning a null pointer to a string& causes a SEGFAULT,
    // so we have to check.
    const char* char_unit = getUnit(name.c_str());
    if (char_unit) {
        units = char_unit;
        value = getVals(name.c_str())[0];
    } else {
        units = "";
        value = 0.0;
    }
}

double DataRiver::getStartTime()
{
    int numpoints = getNumPoints();
    if (numpoints) {
        return (getTimeStamps())[0];
    } else {
        return 0;
    }
}

double DataRiver::getStopTime()
{
    int numpoints = getNumPoints();
    if (numpoints) {
        return (getTimeStamps())[numpoints-1];
    } else {
        return 0;
    }
}
