
#ifndef DATASTREAM_HH
#define DATASTREAM_HH

#include <iostream>
using namespace std;

class DataStream {

       public:

               DataStream();
               virtual ~DataStream();

               virtual int get(double * timeStamp , double * paramValue) = 0 ;
               virtual int peek(double * timeStamp , double * paramValue) = 0 ;

               int getValueAtTime(double timeStamp, double *paramValue ) ;

               virtual string getFileName() ;
               virtual string getUnit() ;
               virtual string getTimeUnit() ;

               virtual void begin() = 0 ;
               virtual int end() = 0 ;
               virtual int step() = 0 ;

       protected:
               string fileName_ ;
               string unitStr_ ;
               string unitTimeStr_ ;
} ;

#endif
