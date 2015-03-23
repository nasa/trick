
#ifndef CSV_HH
#define CSV_HH

#include <stdio.h>
#include "DataStream.hh"

class Csv : public DataStream {

       public:
               Csv(char * file, char * param ) ;

               int get(double * time , double * value ) ;
               int peek(double * time , double * value ) ;

               void begin() ;
               int end() ;
               int step() ;

       private:
               FILE *fp_ ;
               int field_num_ ;
               int data_offset_ ;
               char * line_ ;

} ;

int CsvLocateParam( char * file_name , char * param_name ) ;

#endif
