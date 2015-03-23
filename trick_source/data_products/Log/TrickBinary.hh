
#ifndef TRICKBINARY_HH
#define TRICKBINARY_HH

#include <stdio.h>
#include "DataStream.hh"

class TrickBinary : public DataStream {

       public:
               TrickBinary(char * file, char * param ) ;
               ~TrickBinary() ;

               int get(double * time , double * value ) ;
               int peek(double * time , double * value ) ;

               void begin() ;
               int end() ;
               int step() ;

       private:
               FILE *fp_ ;
               int swap_ ;
               int time_size_ ;
               int num_params_ ;
               int record_offset_ ;
               int record_size_ ;
               char * record_ ;
               int type_ ;
               int size_ ;

               int data_offset_ ;

} ;

int TrickBinaryLocateParam( const char * file_name , const char * param_name ) ;
char** TrickBinaryGetVariableNames(const char* file_name) ;
int    TrickBinaryGetNumVariables(const char* file_name) ;
int    TrickBinaryReadByteOrder( FILE* fp ) ;
char** TrickBinaryGetVariableUnits(const char* file_name) ;

#endif
