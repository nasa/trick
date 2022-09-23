
#ifndef TRICKHDF5_HH
#define TRICKHDF5_HH

#include <stdio.h>
#include <vector>
#include "DataStream.hh"
#include "hdf5.h"
#include "H5PTpublic.h"

class TrickHDF5 : public DataStream {

    public:
		TrickHDF5(char * file, char * param, char *time) ;
        ~TrickHDF5();

        int get(double * time , double * value ) ;
        int peek(double * time , double * value ) ;

        void begin() ;
        int end() ;
        int step() ;

    private:
        hid_t           file;
        hid_t           root_group;
        hid_t           time_dataset, parameter_dataset;
        H5T_class_t     parameter_class;
        H5T_sign_t      parameter_sign;
        H5T_order_t     parameter_order;
        size_t          parameter_size;
        hsize_t         num_packets;
        hsize_t         packet_index;

} ;

int HDF5LocateParam( const char * file_name , const char * param_name ) ;
int HDF5LocateParam( string file_name , string param_name ) ;

#endif
