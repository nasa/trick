
#ifndef MATLAB_HH
#define MATLAB_HH

#include <stdio.h>
#include "DataStream.hh"

class MatLab : public DataStream {

       public:
               MatLab(char * file, char * param , char * time ) ;

               int get(double * time , double * value ) ;
               int peek(double * time , double * value ) ;

               void begin() ;
               int end() ;
               int step() ;

       private:
               FILE *fp_ ;
               char swap_ ;

               struct MatLabVar {
                       char size_ ;
                       char type_ ;
                       int bytes_ ;
                       char num_dims_ ;
                       int * dims_ ;
                       int field_num_ ;
                       int data_offset_ ;
               } ;

               struct MatLabVar y_value_ ;
               struct MatLabVar time_value_ ;

               double getvalue(MatLabVar *) ;

               int curr_row_ ;

} ;

int MatLabLocateParam( char *file_name , char * param_name , char * time_name ) ;

#endif
