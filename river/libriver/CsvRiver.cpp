#ifdef USE_DOUG_H
#include "doug.h"
static DSS_DOUG* doug = 0 ; 
#endif

#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <iostream>
#include <fstream>
using std::cerr;
using std::endl;
using std::ifstream;

#include "CsvRiver.hh"
#include "../libutils/exception.hh"
#include "../libtvs/vsstructs.hh"


// Called after processing data from the file to process any tranform parameters
void CsvRiver::_process_transforms()
{
    for (unsigned int ii = 0; ii < _transforms.size(); ii++) {
        Transformation& trans = _transforms[ii];

        // Inputs should already be there -- just need to map them to
        // the transform inputs
        const vector<string>& inames = _transforms[ii].inparams();
        bool valid = true;
        for (unsigned int jj = 0; jj < inames.size() && valid; jj++) {
            // See if the param is constant. Should not throw an exception
            // since we're loading the list from param names!
            try {
                TrickParam& tp = _transforms[ii].getInparam(inames[jj]);
                if (tp.type == TrickParam::CONSTANT) continue;
            } catch (Exception& e) {
                valid = false;
                continue;
            }
            int pos = -1;
            for (unsigned int kk = 0; kk < _parse_data.params.size(); kk++) {
                if (_parse_data.params[kk].name == inames[jj]) {
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
                tp.vals = _parse_data.params[pos].vals;
            }
        }
        if (!valid) continue;

        // Add outputs from transforms
        const vector<string>& onames = _transforms[ii].outparams();
        for (unsigned int jj = 0; jj < onames.size(); jj++) {
            TrickParam &tp = trans.getOutparam(onames[jj]);
            tp.allocate(_parse_data.num_points);
            _parse_data.params.push_back(tp);
            _parse_data.num_params++;
        }
    }

    // Calculate transformed values
    for (int point = 0; point < _parse_data.num_points; point++) {
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

void add_param(CSV_DATA* parse_data, TrickParam::ParamType type)
{
    parse_data->num_params++ ;
    TrickParam tp("", type);
    parse_data->params.push_back(tp);
}

// Process a single FIELD in a CSV file (not a whole record)
void cb_field (char *field, size_t len, void *data) { 

    size_t ii, jj, kk ;
    int csize ;
    int nparams ;
    char strval[256] ;
    CSV_DATA* parse_data ;
    char* fieldstr;
    char name[1024] ;
    char units[255] ;

    parse_data = (CSV_DATA*)data ;

    if ( parse_data->is_error ) {
        // Once an error occurs, don't process anymore
        return ;
    }

    if ( parse_data->num_recs == 0 ) { 

        // Params are in top record in csv file
        add_param(parse_data, TrickParam::DIRECT);

        // Copy in name and units
        ii = parse_data->num_params - 1 ;
        fieldstr  = new char[len+1];
        strncpy(fieldstr, field, len) ;
        fieldstr[len] = '\0' ;

        int tokens = sscanf(fieldstr, "%s {%[^}]}", name, units);
        parse_data->params[ii].name = name;
        if (tokens > 1) {
            parse_data->params[ii].units = units;
        } else {
            parse_data->params[ii].units = "--";
        }

        delete [] fieldstr;

    } else {

        // This is a data record 

        ii = parse_data->curr_val_idx ;
        jj = parse_data->num_points ; 

        
        if ( ii >= (size_t) parse_data->num_params ) {
            // Must be a corrupt csv file
            // Number of data columns exceed num params
            parse_data->is_error = 1 ;
            return ;
        }

        if ( ii == 0 && jj == 0  ) {
            // First data record
            csize = (parse_data->nchunks)*1000;
            nparams = parse_data->num_params ;
            for ( kk = 0 ; kk < (size_t) nparams ; kk++ ) {
                parse_data->params[kk].allocate(csize) ;
            }
        }

        if ( jj > 0 && jj%1000 == 0 ) {
            if ( ii == 0 ) {
                parse_data->nchunks++ ;
            }
            csize = (parse_data->nchunks)*1000;

            // Will throw exception if out of memory
            parse_data->params[ii].reallocate(csize);
        }

        strncpy(strval, field, len) ;
        strval[len] = '\0' ;
        parse_data->params[ii].vals[jj] = strtod(strval, NULL) ;
        parse_data->curr_val_idx++ ;
    }
}

void cb_line (char c, void *data) { 

        CSV_DATA* parse_data ;
        parse_data = (CSV_DATA*)data ;

        if ( parse_data->is_error ) {
                // Once an error occurs, don't process anymore
                return ;
        }

        if ( parse_data->num_recs != 0 ) { 
                parse_data->num_points++; 
        } 
        parse_data->num_recs++; 
        parse_data->curr_val_idx = 0 ;

}

static int is_space(char c) {
        if (c == CSV_SPACE || c == CSV_TAB) {
                return 1;
        }
        return 0;
}

static int is_term(char c) {
        if (c == CSV_CR || c == CSV_LF) {
                return 1;
        }
        return 0;
}


CsvRiver::CsvRiver(char * file_name,
                   vector<Transformation>& transforms) : 
                              _filename(file_name),
                              _transforms(transforms)
{
    int ii ;
    char* buf = 0;
    size_t nbytes ;
    unsigned char options = 0;
    string line;

#ifdef USE_DOUG_H
    long num_bytes_file ;
    long num_bytes_read = 0;
    char cmd[128] ;
    double percent ;
#endif

    _timestamps = 0 ; 

    _parse_data.is_error = 0 ;
    _parse_data.num_recs = 0 ;
    _parse_data.num_points = 0 ;
    _parse_data.num_params = 0 ;
    _parse_data.nchunks = 1 ;
    _parse_data.curr_val_idx = 0 ;
     try {
        ifstream filestrm(_filename.c_str());
        if (! filestrm.good()) {
            char msg[1024];
            sprintf(msg, "Couldn't open %s\n", _filename.c_str());
            throw Exception(msg) ;
        }

#ifdef USE_DOUG_H
        percent = 0.0 ;
        filestrm.seekg(0, ios::end);
        num_bytes_file = filestrm.tellg() ;
        filestrm.seekg(0, ios::beg);
        doug = (DSS_DOUG*) DSF_GetDataFromProcess("DSS_DOUG") ;
        int last_percent = 0;
#endif
        if (csv_init(&_parser, options) != 0) {
                _parser = 0;
                throw Exception("Failed to initialize csv parser");
        }

        csv_set_space_func(_parser, is_space);
        csv_set_term_func(_parser, is_term);
        getline(filestrm, line);
        while (! filestrm.eof() ) {
            line += "\n"; // Needed for libcsv to recognize new line
            nbytes = line.size();
            if (buf) delete [] buf;
            buf = new char[nbytes+1];
            strcpy(buf, line.c_str());

#ifdef USE_DOUG_H
            /* Update progress bar */
            num_bytes_read += nbytes;
            percent = (double) num_bytes_read * 100.0 / num_bytes_file;
            if (percent - last_percent > 10) {
                sprintf(cmd, 
                        "simdata_p::popup_fileload_progressbar_update %lf", 
                        percent );
                if (doug) {
                    Tcl_EvalEx(doug->interp, cmd, -1, TCL_EVAL_GLOBAL );
                }
                last_percent = percent;
             }
#endif

                // This is a bit heuristic, but it should detect most attempts
                // to pass off a binary file as CSV
                for (unsigned int ii = 0; ii < nbytes; ii++) {
                    if (!isprint(buf[ii]) && !isspace(buf[ii])) { 
                        throw Exception("Invalid or corrupted CSV file");
                    }
                }

                if (csv_parse(_parser, buf, 
                              nbytes, cb_field, cb_line, 
                              &_parse_data) != nbytes) {

                        char msg[1024];
                        sprintf(msg, 
                                "Error while parsing file: %s", 
                                csv_strerror(csv_error(_parser)));
                        throw Exception(msg);
                }

                if ( _parse_data.is_error ) {
                        // Clean up and bail
                        throw Exception("Unknown error occurred while parsing file");
                }
                getline(filestrm, line);
        }
        if (buf) delete [] buf;
        buf = 0;

        // Now do transforms
        _process_transforms();

        /* Some sanity checks */ 
        if ( _parse_data.num_params == 0 ) return ; // No values in file, bail

#ifdef USE_DOUG_H
        /* Tell Doug all data loaded */
        sprintf(cmd, "simdata_p::popup_fileload_progressbar_update 100.0");
        if (doug) Tcl_EvalEx(doug->interp, cmd, -1, TCL_EVAL_GLOBAL );
#endif

        // Set val and unit mappings
        for ( ii = 0 ; ii < _parse_data.num_params ; ii++ ) {
                _map_vals[_parse_data.params[ii].name]  = _parse_data.params[ii].vals ;
                _map_units[_parse_data.params[ii].name] = _parse_data.params[ii].units ;
        }   
    } catch (Exception& e) {
        if (buf) delete [] buf;
        _cleanup();
        throw e;
    }
}

void CsvRiver::_cleanup()
{
    int ii ;

    if (_parser) {
        csv_fini(_parser, cb_field, cb_line, &_parse_data);
        csv_free(_parser);
        _parser = 0;
    }

    // Deallocate parameter values
    for ( ii = 0 ; ii < _parse_data.num_params ; ii++ ) {
        _parse_data.params[ii].deallocate();
    }
    _parse_data.num_params = 0;

    // Close out transform libraries
    for (unsigned int ii = 0; ii < _transforms.size(); ii++) {
        _transforms[ii].close();
    }
}

CsvRiver::~CsvRiver() 
{
    _cleanup();
}


double* CsvRiver::getVals(const char* param)
{
        int keycount ;

        keycount = _map_vals.count(param) ;
        if ( keycount == 0 ) {
                return 0 ;
        }

        return( _map_vals[param] ) ;
}

const char* CsvRiver::getUnit(const char* param) 
{
        int keycount ;

        keycount = _map_units.count(param) ;
        if ( keycount == 0 ) {
                return 0 ;
        }

        return( _map_units[param].c_str() ) ;
}

int CsvRiver::getNumPoints() 
{
        return ( _parse_data.num_points ) ;
}

const char* CsvRiver::getFileName()
{
        return _filename.c_str() ;
}

const char* CsvRiver::getTimeUnit()
{
        // TODO: This should be generic
        return( getUnit("sys.exec.out.time") ) ;
}

double* CsvRiver::getTimeStamps()
{
        int keycount ;
        keycount = _map_vals.count("sys.exec.out.time") ;
        if ( keycount == 0 ) {
                // Assume first column is time
                return _parse_data.params[0].vals ;
        }

        return( _map_vals["sys.exec.out.time"] ) ;
}

int CsvRiver::getNumParams()
{
        int np ;

        if ( _parse_data.num_points == 0 ) {
                // If no points, set num params to 0
                np = 0 ;
        } else { 
                np = (int)_parse_data.num_params  ;
        }

        return ( np ) ; 
}
