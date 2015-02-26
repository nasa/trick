
/*
 * $Log: TrickBinary.hh,v $
 * Revision 9.2  2008-07-22 09:45:59-05  wwoodard
 * G..4-782  Create TrickBinaryGetVariableUnits().
 *
 * Revision 7.2  2008-07-17 12:39:50-05  wwoodard
 * G..4-782  Create TrickBinaryGetVariableUnits().
 *
 * Revision 7.1  2006-06-22 14:11:56-05  lin
 * Bump version number for 07
 *
 * Revision 1.5  2005-09-16 11:19:57-05  vetter
 * Add Access Method For Variable Names In Stream Classes
 *
 * Revision 1.4  2005-05-26 15:02:02-05  lin
 * speed up reading of data in TrickBinary
 *
 */

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
