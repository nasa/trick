#include "TrickBinaryRiver.hh"
#include "trick_byte_order.h"
#include "trick_byteswap.h"
#include "TrickParamTypes07.hh"
#include "TrickParamTypes10.hh"
#include "../libutils/exception.hh"

#include <float.h>
#include <cstdio>
#include <math.h>
#include <iostream>
using std::cerr;
using std::endl;
using std::string;
using std::vector;
using std::map;

#ifdef USE_DOUG_H
#include "doug.h"
static DSS_DOUG* doug = 0 ;
#endif

TrickBinaryRiver::TrickBinaryRiver(char * file_name,
                                   vector<Transformation>& transforms) :
                                           _transforms(transforms),
                                           _param_types(0)
{
    static const TrickParamTypes07 Trick07Type(0);
    static const TrickParamTypes10 Trick10Type(0);
    const int file_type_len = 10 ;
    char file_type[file_type_len + 1] ;
    int my_byte_order ;
    int len ;
    int ii ; 

    _time_size   = 0 ;
    _num_recs    = 0 ;
    _timestamps  = 0 ; 
    _num_params  = 0 ;
    _record      = 0 ;

    LOG_PARAM_INFO* param_info ;
    TrickParam*     param_data ;

    _fileName = file_name ;

    try {
        if ((_fp = fopen(file_name , "rb")) != 0 ) {
            memset(file_type, 0 , 10 ) ;
            fread(file_type , file_type_len , 1 , _fp ) ;
            file_type[file_type_len] = '\0' ;

            if ( !strncmp( file_type , "Trick-" , 6 ) ) {

                TRICK_GET_BYTE_ORDER(my_byte_order) ;
                switch ( file_type[file_type_len - 1] ) {
                    case 'L':
                            _swap = ( my_byte_order == TRICK_LITTLE_ENDIAN ) ? 0 : 1 ;
                            break ;
                    case 'B':
                            _swap = ( my_byte_order == TRICK_BIG_ENDIAN ) ? 0 : 1 ;
                            break ;
                }

                // num_params
                fread(&_num_params , 4 , 1 , _fp ) ;
                if ( _swap ) { 
                    _num_params = trick_byteswap_int(_num_params) ; 
                }
                _list_params.resize(_num_params);

                _record_size = 0 ;
                for ( ii = 0  ; ii < _num_params ; ii++ ) {
                    char units[256];
                    char name[256];

                    // Shorthand
                    param_info  = &(_list_params[ii].info);
                    param_data  = &(_list_params[ii].data);

                    // name length
                    fread(&len , 4 , 1 , _fp ) ;
                    if ( _swap ) { 
                            len = trick_byteswap_int(len) ; 
                    }

                    // name
                    fread(name , len , 1 , _fp ) ;
                    // use assign here rather than "=" because name is
                    // not NULL-terminated
                    param_data->name.assign(name, len);

                    // units length
                    fread(&len , 4 , 1 , _fp ) ;
                    if ( _swap ) { 
                        len = trick_byteswap_int(len) ; 
                    }

                    // units
                    fread(units , len , 1 , _fp ) ;
                    // use assign here rather than "=" because units is
                    // not NULL-terminated
                    param_data->units.assign(units, len);

                    // type of param
                    fread(&param_info->type , 4 , 1 , _fp ) ;
                    if ( _swap ) { 
                        param_info->type = trick_byteswap_int(param_info->type) ; 
                    }

                    // size of param
                    fread(&(param_info->size) , 4 , 1 , _fp ) ;
                    if ( _swap ) { 
                        param_info->size = trick_byteswap_int(param_info->size) ; 
                    }

                    // Set time information, and determine type info
                    if ( param_data->name == "sys.exec.out.time" ) {
                        if (param_info->type == Trick07Type.doubleType()) {
                            //cerr << "Found Trick 07 .trk" << endl;
                            _param_types = new TrickParamTypes07(_swap);
                        } else if (param_info->type == Trick10Type.doubleType()) {
                            //cerr << "Found Trick 10 .trk" << endl;
                            _param_types = new TrickParamTypes10(_swap);
                        } else {
                            cerr << "Unable to determine type -- guessing Trick10" << endl;
                            _param_types = new TrickParamTypes10(_swap);
                        }
                        _time_size  = param_info->size ;
                        _unitTimeStr = param_data->units ;
                    }

                    // Byte offset of param
                    param_info->record_offset = _record_size ;

                    // Size of record 
                    _record_size += param_info->size ;

                }

                _data_offset = ftell(_fp) ;
                _record = new char[_record_size] ;
                _load_data();
            } 

        } else {
            printf("ERROR:  Couldn't open \"%s\"\n" , file_name ) ;
        }
        _process_transforms();

        // Map & Vector for access later -- must be done after processing
        // the transforms to ensure _list_params isn't resized
        for ( ii = 0  ; ii < _num_params ; ii++ ) {
            _map_params[_list_params[ii].data.name] = &(_list_params[ii]) ;
        }

    } catch (Exception& e) {
        _cleanup();
        throw e;
    }
}

// Called after processing data from the file to process any tranform parameters
void TrickBinaryRiver::_process_transforms()
{
    for (unsigned int ii = 0; ii < _transforms.size(); ii++) {
        Transformation& trans = _transforms[ii];

        // Inputs should already be there -- just need to map them to
        // the transform inputs
        const vector<string>& inames = _transforms[ii].inparams();
        bool valid = true;
        for (unsigned int jj = 0; jj < inames.size() && valid; jj++) {
            try {
                TrickParam& tp = _transforms[ii].getInparam(inames[jj]);
                if (tp.type == TrickParam::CONSTANT) continue;
            } catch (Exception& e) {
                valid = false;
                continue;
            }
            int pos = -1;
            for (unsigned int kk = 0; kk < _list_params.size(); kk++) {
                if (_list_params[kk].data.name == inames[jj]) {
                    pos = kk;
                    break;
                }
            }
            if (pos == -1) { // Param not found -- error
                // Uh oh --- No such param
                string errmsg("Can't add param ");
                errmsg += inames[jj] + " for transform "
                          + _transforms[ii].funcname()
                          + " -- transform will be disabled";
                #ifdef USE_DOUG_H
                DSF_LogWarning(const_cast<char*>(errmsg.c_str()));
                #endif
                _transforms.erase(_transforms.begin() + ii);
                --ii;
                valid = false;
                continue;
            } else {         // Existing variable -- join to existing stream
                TrickParam &tp = trans.getInparam(inames[jj]);
                tp.vals = _list_params[pos].data.vals;
            }
        }
        if (!valid) continue;

        // Add outputs from transforms
        const vector<string>& onames = _transforms[ii].outparams();
        for (unsigned int jj = 0; jj < onames.size(); jj++) {
            TrickParam &tp = trans.getOutparam(onames[jj]);
            tp.allocate(_num_recs);
            LOG_PARAM log_param;
            log_param.data = tp;

            // Kind of bogus, but it's good to put something here
            log_param.info.type = _param_types->doubleType();
            log_param.info.size = sizeof(double);
            log_param.info.record_offset = -1;

            _list_params.push_back(log_param);
            _num_params++;
        }
    }

    // Calculate transformed values
    for (int point = 0; point < _num_recs; point++) {
        for (unsigned int ii = 0; ii < _transforms.size(); ii++) {
            try {
                _transforms[ii].callTransform(point);
            } catch (Exception& e) {
                unlock();
                throw e;
            }
        }
    }
}

TrickBinaryRiver::~TrickBinaryRiver() 
{
    _cleanup();
}

void TrickBinaryRiver::_cleanup() 
{
        for ( int ii = 0 ; ii < _num_params ; ii++ ) {
                _list_params[ii].data.deallocate();
        }
        if ( _timestamps ) { 
                free( _timestamps ) ;
                _timestamps = 0 ;
        }

        if ( _fp ) {
                fclose(_fp);
                _fp = 0 ;
        }

        if ( _record ) {
                delete[] _record ;
                _record = 0 ;
        }

        for ( unsigned int ii = 0; ii < _transforms.size(); ii++) {
                _transforms[ii].close();
        }

        if (_param_types) {
            delete _param_types;
            _param_types = 0;
        }
}

void TrickBinaryRiver::_load_data() 
{

        int ii ;
        int point_idx ;
        LOG_PARAM* param ; 
        long end_offset ;

        int offset ;
        double value ;
        double time ;

        // Count num recs
        fseek(_fp, 0, SEEK_END);
        end_offset = ftell(_fp);
        _num_recs = ( end_offset - _data_offset ) / _record_size;

        if (_num_recs == 0) {
            throw Exception("file is not in Trick binary format, or has no parameters");
        }


#ifdef USE_DOUG_H
        char cmd[128] ;
        double percent ;
        int next_progress_update ;
        int next_progress_incr ;
        percent = 0.0 ;
        next_progress_incr = (int)(_num_recs / 20.0) ; 
        next_progress_update = 0 ;
        doug = (DSS_DOUG*) DSF_GetDataFromProcess(const_cast<char*>("DSS_DOUG")) ;
#endif

        // Allocate curves (note timstamps are same for all params)
        _timestamps = (double*) malloc (sizeof(double)*_num_recs) ;
        for ( ii = 0 ; ii < _num_params ; ii++ ) {
                _list_params[ii].data.allocate(_num_recs) ;
        }

        // Read data
        point_idx = 0  ;
        fseek(_fp, _data_offset , SEEK_SET) ;

        while ( fread(_record , _record_size , 1 , _fp )) {
#ifdef USE_DOUG_H
                /* Update progress bar */
                if ( point_idx == next_progress_update ) {
                        percent += 5.0 ;
                        sprintf(cmd, "simdata_p::popup_fileload_progressbar_update %f", percent );
                        if (doug) Tcl_EvalEx(doug->interp, cmd, -1, TCL_EVAL_GLOBAL );
                        next_progress_update += next_progress_incr ;
                }
#endif

                // Read in timestamp
                if ( _time_size == 8 ) {
                        double * my_time = (double *)_record ;
                        time = *my_time ;
                        time = _swap ? trick_byteswap_double(*my_time) : time ;
                }
                else {
                        float *my_time = (float *)_record ;
                        time = (double)*my_time ;
                        time = _swap ? trick_byteswap_double(*my_time) : time ;
                }
                _timestamps[point_idx] = time ;

                
                // Read all vals for all params in record
                for ( ii = 0 ; ii < _num_params ; ii++ ) {

                        param  = &(_list_params[ii]) ;

                        offset = param->info.record_offset ;
                        void *var_address = _record + offset;
                        value = _param_types->toValue(var_address, param->info.type, 
                                                      param->info.size);

                        param->data.vals[point_idx] = value ;
                }

                point_idx++ ;
         }
}


int TrickBinaryRiver::getNumParams() { 
        return (_num_params);
}

int TrickBinaryRiver::getNumPoints() 
{
        // Ints are everywhere :-(
        // Need to fix this
        return ((int) _num_recs) ;
}

const char* TrickBinaryRiver::getUnit(const char* param) 
{

        int keycount ;

        keycount = _map_params.count(param) ;
        if ( keycount == 0 ) {
                return 0 ;
        }

        return ( _map_params[param]->data.units.c_str() ) ;
}

double* TrickBinaryRiver::getVals(const char* param)
{
        int keycount ;

        keycount = _map_params.count(param) ;
        if ( keycount == 0 ) {
                return 0 ;
        }

        return ( _map_params[param]->data.vals ) ;

}

double* TrickBinaryRiver::getTimeStamps()
{
        return ( _timestamps ) ;
}

const char* TrickBinaryRiver::getFileName() 
{
        return(_fileName) ;
}

const char* TrickBinaryRiver::getTimeUnit() 
{
        return(_unitTimeStr.c_str()) ;
}
