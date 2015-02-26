// The delta stream returns the difference between
// two streams designated with the <var:run> syntax

#ifndef _DELTA_H_
#define _DELTA_H_

#include <stdio.h>
#include "DataStream.hh"
#include "DataStreamGroup.hh"

class Delta : public DataStream {

   public:

       Delta(const char * deltaStatement, const char* timeName) ;
       ~Delta() ;

       int get(double * time , double * value ) ;
       int peek(double * time , double * value ) ;

       void begin() ;
       int end() ;
       int step() ;

   private:

       DataStream* dataStream1_ ;
       DataStream* dataStream2_ ;
       DataStreamGroup dsg_ ;
} ;

#endif
