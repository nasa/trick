#ifdef USE_DOUG_H
#include "doug.h"
static DSS_DOUG* doug = 0 ; 
#endif

#include "Trick04River.hh"
#include "utils.h"
#include "../libutils/exception.hh"
#include "../libtvs/transformation.hh"

#include <iostream>
using std::cerr;
using std::endl;

// Filename is *.000 file
Trick04River::Trick04River(char * _logfile,
                           vector<Transformation> transforms) : 
                                   _lg(0), _transforms(transforms)
{
    unsigned int ii ;
    bool found ;
    char tail[256] ;

    _infile = strdup(_logfile) ;

    get_dirname( _rundir, _logfile ) ;
    get_tailname ( tail, _logfile ) ;

    try {
        _lg = new LogGroup( _rundir ) ;

        // Get data log associated with _logfile
        found = 0 ;
        for ( ii = 0 ; ii < _lg->log.size(); ii++ ) {
            if ( !strncmp(tail,
                          _lg->log[ii]->getBinaryFileName(),
                          strlen(_lg->log[ii]->getBinaryFileName()))) 
            {
                _log = _lg->log[ii] ;
                found = 1 ;
            }
        }
        if ( ! found ) {
            throw Exception(string("ERROR: Trick04River: Can't init ")
                            + _logfile + "\n");
        }

        _load_data();
        _process_transforms();

    } catch (Exception& e) {
        _cleanup();
        throw e;
    }
}

Trick04River::~Trick04River() 
{
        _cleanup();
}

void Trick04River::_cleanup() 
{
        int ii ;
        free( _infile ) ;
        delete _lg ;

        for ( ii = 0 ; ii < _params.size() ; ii++ ) {
                _params[ii].deallocate();
        }
}

double* Trick04River::getVals(const char* param)
{ 
    for (unsigned int ii = 0; ii < _params.size(); ii++) {
        if (_params[ii].name == param) return _params[ii].vals;
    }
    return 0;
}

int Trick04River::getNumParams() 
{
    return _params.size();
}

double* Trick04River::getTimeStamps()
{
        return( _timestamps ) ;
}

const char* Trick04River::getUnit(const char* param)
{ 
    for (unsigned int ii = 0; ii < _params.size(); ii++) {
        if (_params[ii].name == param) return _params[ii].units.c_str();
    }
    return 0;
}

int Trick04River::getNumPoints()
{
        return( _log->getNumRecords() ) ;
}

const char* Trick04River::getFileName()
{
        return( _infile ) ; 
}

const char* Trick04River::getTimeUnit()
{
        return( _log->getUnit("sys.exec.out.time")->getUnitName() ) ;
}


void Trick04River::_load_data() 
{
        int ii, jj ;
        int num_params ;
        int num_recs ;


        num_params = _log->getNumVars() ;
        num_recs = _log->getNumRecords() ; 

        if (num_recs == 0) {
            throw Exception("file is not in Trick04 format, or has no parameters");
        }

        // Allocate
        vector<string> allparams;
        for ( ii = 0 ; ii < num_params ; ii++ ) {
                const vector<string>& params = _log->getVar(ii)->getVariableNames();
                for ( jj = 0; jj < params.size(); jj++) {
                   _params.push_back( TrickParam(params[jj],
                                         TrickParam::DIRECT,
                                         _log->getUnit(params[jj].c_str())->getUnitName())
                                    );
                   _params[_params.size()-1].allocate(num_recs);
                }
         }

        // Doug progress bar
#ifdef USE_DOUG_H
        char cmd[128] ;
        double percent ;
        int next_progress_update ;
        int next_progress_incr ;
        percent = 0.0 ;
        next_progress_incr = (int)(num_recs / 20.0) ;
        next_progress_update = 0 ;
        doug = (DSS_DOUG*) DSF_GetDataFromProcess("DSS_DOUG") ;
#endif

        // Load all data into vals_ array
        _log->convertToDoubleYes();
        _log->rewind();
        unsigned int record = 0 ;
        while ( _log->getNextRecord() != NULL) {

#ifdef USE_DOUG_H
                /* Update progress bar */
                if ( record == next_progress_update ) {
                        percent += 5.0 ;
                        sprintf(cmd, "simdata_p::popup_fileload_progressbar_update %f", 
                                     percent );
                        if (doug) Tcl_EvalEx(doug->interp, cmd, -1, TCL_EVAL_GLOBAL );
                        next_progress_update += next_progress_incr ;
                }
#endif

                for ( unsigned int param = 0; param < _params.size() ; param++ ) {
                        _params[param].vals[record] = *((double *) _log->getNextVal());
                }

                record++ ;
        }

        // Set timestamps pointer 
        int tidx = _log->getParamIdx("sys.exec.out.time");
        _timestamps = _params[tidx].vals ;
}

void Trick04River::_process_transforms()
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
            for (unsigned int kk = 0; kk < _params.size(); kk++) {
                if (_params[kk].name == inames[jj]) {
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
                tp.vals = _params[pos].vals;
            }
        }
        if (!valid) continue;

        // Add outputs from transforms
        const vector<string>& onames = _transforms[ii].outparams();
        for (unsigned int jj = 0; jj < onames.size(); jj++) {
            TrickParam &tp = trans.getOutparam(onames[jj]);
            tp.allocate(_log->getNumRecords());
            _params.push_back(tp);
        }
    }

    // Calculate transformed values
    for (int point = 0; point < _log->getNumRecords(); point++) {
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
