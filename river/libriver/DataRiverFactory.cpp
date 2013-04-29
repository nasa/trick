#include "DataRiverFactory.hh"

#include <stdio.h>
#ifndef WIN32
#include <strings.h>
#endif

#include "TrickBinaryRiver.hh"
#include "Trick04River.hh"
#include "TrickVSRiver.hh"
#include "HDF5River.hh"
#include "CsvRiver.hh"
#include "../libtvs/vsclientconnector.hh"
#include "../libutils/exception.hh"

#include <iostream>
using std::cerr;
using std::endl;

DataRiver* get_data_river( VSClientConnector* connector, double rate,
                           int numnames, char** names,
                           vector<Transformation>& transforms,
                           void (*callback)(VSMessage& msg)
                         )
{
    if (!connector || (!names && transforms.size() == 0) || !callback) return NULL;
    try {
        return new TrickVSRiver(connector, rate, numnames, 
                                names, transforms, callback) ;
    } catch (Exception& e) {
        throw e;
    }
}

DataRiver* get_data_river( char* path,
                           vector<Transformation>& transforms 
                         )
{
        string strpath(path);
        DataRiver* river = 0 ;
        unsigned int pathlen = strpath.length();

        if ( strpath.rfind(".trk", pathlen-4) != string::npos) {
                river = new TrickBinaryRiver( path, transforms ) ;
        } else if ( strpath.rfind(".000", pathlen-4) != string::npos) {
                river = new Trick04River( path, transforms ) ;
        } else if ( strpath.rfind(".csv", pathlen-4) != string::npos) {
                river = new CsvRiver(path, transforms) ;
        } else if ( strpath.rfind(".h5", pathlen-3) != string::npos) {
                river = new HDF5River(path, transforms) ;
        }
        #if 0
        else if  ( !strcmp( &(path[idx]) , ".csv")) {
                river = new CsvRiver(path) ;
        } else if ( !strcmp( &(path[idx]) , ".fix")) {
                river = new FixedAsciiRiver(path);
        } else if ( !strcmp( &(path[idx]) , ".mat")) {
                river = new MatLabRiver(path) ; 
        } else if ( !strcmp( &(path[idx]) , ".oca")) {
                river = new OctaveAsciiRiver(path); 
        } else if ( !strcmp( &(path[idx]) , ".ocb")) {
                river = new OctaveBinaryRiver(path) ;
        }
        #endif

        return river ;
}
