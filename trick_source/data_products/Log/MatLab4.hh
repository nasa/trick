
#ifndef MATLAB4_HH
#define MATLAB4_HH

#include <stdio.h>
#include "DataStream.hh"

class MatLab4 : public DataStream {

       public:
               MatLab4(char * file, char * param , char * time ) ;

               int get(double * time , double * value ) ;
               int peek(double * time , double * value ) ;

               void begin() ;
               int end() ;
               int step() ;

       private:

               // a little routine to do the getting to avoind code duplication

               FILE *fp_ ;
               char swap_ ;

               struct MatLab4Var {
                       int type_ ;
                       int size_ ;
                       int rows_ ;
                       int columns_ ;
                       int field_num_ ;
                       int imaginary_ ;
                       int data_offset_ ;
               } ;

               struct MatLab4Var y_value_ ;
               struct MatLab4Var time_value_ ;

               double getvalue(MatLab4Var *) ;

               int curr_row_ ;
} ;

int MatLab4LocateParam( char *file_name , char * param_name , char * time_name ) ;

#endif
