#ifndef BOUNDEDTRICKBINARYRIVER_H
#define BOUNDEDTRICKBINARYRIVER_H

#include <QMap>
#include "TrickBinaryRiver.hh"

#include <stdexcept>
#include <string>
using namespace std;

class BoundedTrickBinaryRiver : public TrickBinaryRiver {
  public:
     BoundedTrickBinaryRiver(char* filename, double start, double stop);

     int start() { return _start ; }
     int stop() { return _stop ; }

     int getNumPoints() { return _npoints ; }
     double* getTimeStamps() { return _timestamps ; }
     double* getVals(const char* param) {
         if ( _npoints > 0 ) {
             return _vals.value(param) ;
         } else {
             return 0;
         }
     }

  private:
     BoundedTrickBinaryRiver(
                    char* file_name,
                    vector<Transformation>& transforms=empty_transforms) :
         TrickBinaryRiver(file_name,transforms){}

     double _start;
     double _stop;
     int _npoints;
     double* _timestamps;
     QMap<const char*,double*> _vals;
};

#endif // BOUNDEDTRICKBINARYRIVER_H
