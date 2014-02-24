#ifdef USE_DOUG_H
#include "doug.h"
static DSS_DOUG* doug = 0 ; 
#endif

#include "HDF5River.hh"
#include "HDF5Utils.hh"

#include "trick_byte_order.h"
#include "trick_byteswap.h"
#include "../libutils/exception.hh"

#include <float.h>
#include <cstdio>
#include <math.h>

using std::string;
using std::vector;
using std::map;

herr_t HDF5River::_process_dataset(hid_t loc_id, const char* name, 
                                   const char* units, unsigned int index)
{
    HDFPT dataset(loc_id, name);
    hid_t dataset_table = dataset.hid();

    if ( H5PTcreate_index(dataset_table) < 0 ) { 
        throw Exception("Couldn't set param index");
    }

    hsize_t num_records;
    if ( H5PTget_num_packets(dataset_table, &num_records) < 0 ) {
        throw Exception("Couldn't get number of records");
    }

    _num_recs = num_records;

    // Fill in this param
    _list_params[index].name = name;
    _list_params[index].units = units;
    _list_params[index].incoming_val = 0;
    _list_params[index].allocate(num_records);
    double* vals = _list_params[index].vals; // Quick alias
    
    if ( H5PTget_next(dataset_table, num_records, (void*) vals ) < 0 ) {
        throw Exception("Couldn't get val for param " + string(name));
    } else {
//        for (unsigned int ii = 0; ii < num_records; ii++) {
//            cout << vals[ii] << endl;
//        }
    }
     
    return 0;
}

HDF5River::HDF5River(char * file_name,
                                   vector<Transformation>& transforms) :
                                           _transforms(transforms)
{
    _file_name = file_name;

    try {
        /*!
         * Open the HDF5 file.
         */
        HDFFile fileHDF(file_name, H5F_ACC_RDONLY);
        hid_t file = fileHDF.hid();

        HDFGroup root(file, "/");
        hid_t root_group = root.hid();

        HDFGroup header(file, "/header");
        hid_t header_group = header.hid();

        HDFPT pnames(header_group, "param_names");
        hid_t param_names = pnames.hid();

        HDFPT punits(header_group, "param_units");
        hid_t param_units = punits.hid();

        if ( H5PTcreate_index(param_names) < 0 ) { 
            throw Exception("Couldn't set param name index ");
        }

        hsize_t num_records;
        if ( H5PTget_num_packets(param_names, &num_records) < 0 ) {
            throw Exception("Couldn't get number of records ");
        }

        _list_params.resize(num_records + transforms.size());
        _num_params = num_records;

        if ( H5PTcreate_index(param_units) < 0 ) { 
            throw Exception("Couldn't set param unit index ");
        }


#ifdef USE_DOUG_H
        char cmd[128] ;
        double percent = 0.0 ;
        doug = (DSS_DOUG*) DSF_GetDataFromProcess("DSS_DOUG") ;
#endif

        char param_name[1024];
        char param_unit[1024];
        for (unsigned int ii = 0; ii < num_records; ii++) {
            if ( H5PTget_next(param_names, 1, (void*) param_name ) < 0 ) {
                throw Exception("Couldn't get param names");
            }
            if ( H5PTget_next(param_units, 1, (void*) param_unit) < 0 ) {
                throw Exception("Couldn't get param units");
            }

            if (_process_dataset(root_group, param_name, 
                                 param_unit, ii) < 0) {
                throw Exception("Couldn't get data for params");
            }

#ifdef USE_DOUG_H
            /* Update progress bar */
            percent += 100.0/num_records;
            sprintf(cmd, "simdata_p::popup_fileload_progressbar_update %f", percent );
            if (doug) Tcl_EvalEx(doug->interp, cmd, -1, TCL_EVAL_GLOBAL );
#endif

        }

        _process_transforms();
 
        // Map & Vector for access later -- must be done after processing
        // the transforms to ensure _list_params isn't resized
        for ( unsigned int ii = 0  ; ii < _num_params ; ii++ ) {
            _map_params[_list_params[ii].name] = &(_list_params[ii]) ;
        }

    } catch (Exception& e) {
        _cleanup();
        throw e;
    }
}

// Called after processing data from the file to process any tranform parameters
void HDF5River::_process_transforms()
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
                if (_list_params[kk].name == inames[jj]) {
                    pos = kk;
                    break;
                }
            }
            if (pos == -1) { // Param not found -- error
                // Uh oh --- variable server doesn't know the param
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
                tp.vals = _list_params[pos].vals;
            }
        }
        if (!valid) continue;

        // Add outputs from transforms
        const vector<string>& onames = _transforms[ii].outparams();
        for (unsigned int jj = 0; jj < onames.size(); jj++) {
            TrickParam &tp = trans.getOutparam(onames[jj]);
            tp.allocate(_num_recs);
            _list_params.push_back(tp);
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

HDF5River::~HDF5River() 
{
    _cleanup();
}

void HDF5River::_cleanup() 
{
        for ( unsigned int ii = 0 ; ii < _num_params ; ii++ ) {
                _list_params[ii].deallocate();
        }

        for ( unsigned int ii = 0; ii < _transforms.size(); ii++) {
                _transforms[ii].close();
        }
}

int HDF5River::getNumParams() { 
        return (_num_params);
}

int HDF5River::getNumPoints() 
{
        // Ints are everywhere :-(
        // Need to fix this
        return ((int) _num_recs) ;
}

const char* HDF5River::getUnit(const char* param) 
{

        int keycount ;

        keycount = _map_params.count(param) ;
        if ( keycount == 0 ) {
                return 0 ;
        }

        return ( _map_params[param]->units.c_str() ) ;
}

double* HDF5River::getVals(const char* param)
{
        int keycount ;

        keycount = _map_params.count(param) ;
        if ( keycount == 0 ) {
                return 0 ;
        }

        return ( _map_params[param]->vals ) ;

}

double* HDF5River::getTimeStamps()
{
//        return ( _timestamps ) ;
        return _list_params[0].vals;
}

const char* HDF5River::getFileName() 
{
        return(_file_name.c_str()) ;
}

const char* HDF5River::getTimeUnit() 
{
        return _list_params[0].name.c_str();
//        return(_unitTimeStr.c_str()) ;
}
