
#ifndef DATASTREAMFACTORY_HH
#define DATASTREAMFACTORY_HH

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>

#include "DataStream.hh"
#include "Csv.hh"
//#include "FixedAscii.hh"
//#include "OctaveAscii.hh"
//#include "OctaveBinary.hh"
#include "TrickBinary.hh"
//#include "TrickBinary04.hh"
#include "MatLab.hh"
#include "MatLab4.hh"
#include "ExternalProgram.hh"
#include "Delta.hh"
#ifdef HDF5
#include "TrickHDF5.hh"
#endif

int getVariableNames( const char* pathToData,
                      int* numVariableNames, char*** variableNames );

class DataStreamFactory {

   public:
        DataStreamFactory() {}
        ~DataStreamFactory() ;
        DataStream* create(const char* path, const char* paramName ,
                           const char* timeName );
        DataStream* create( const char* sharedLibName,
                            int nInputStreams, DataStream** istreams,
                            int nOutputStreams, int outputIdx) ;
        DataStream* create(const char* machine, const unsigned short port ,
                           const char* paramName );

} ;

#endif
