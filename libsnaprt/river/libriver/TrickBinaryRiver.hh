#ifndef _TRICKBINARYRIVER_H_
#define _TRICKBINARYRIVER_H_

#include "DataRiver.hh"
#include "TrickParamTypes.hh"
#include "../libtvs/vsstructs.hh"
#include "../libtvs/transformation.hh"

#include <iostream>
#include <map>
#include <string>
#include <vector>

static vector<Transformation> empty_transforms;

/// Metadata about a log parameter
typedef struct {
        int type ;
        int size ;
        int record_offset ;
} LOG_PARAM_INFO ;

/// Log parameter data and information
typedef struct {
        TrickParam data ;
        LOG_PARAM_INFO info ;
} LOG_PARAM ;

/** \ingroup libdp 
 * Provides a data river implementation based on a pre-recorded Trick
 * binary file.
 * TrickBinaryRiver is passed a Trick binary file and a set of transforms as
 * construction. It then opens the file, reads the content, and stores the
 * values from the file in a set of TrickParams. When all values have been
 * read in, TrickBinaryRiver adds to the set of parameters the outputs from any
 * requested transforms.
 */
class TrickBinaryRiver : public DataRiver {

       public:

               TrickBinaryRiver(char* file_name,
                                vector<Transformation>& transforms=empty_transforms);
               ~TrickBinaryRiver();

               double*      getVals(const char* param)  ;
               const char*  getUnit(const char* param)  ;
               int          getNumPoints()        ;
               const char*  getFileName()         ;
               const char*  getTimeUnit()         ;
               double*      getTimeStamps()       ;
               int          getNumParams()        ;
               std::vector<LOG_PARAM> getParamList() { return _list_params; }
               int          getNumTransforms() { return _transforms.size(); }

       protected:
               int _time_size ;            ///< Size in bytes of time values
               long _num_recs ;            ///< Number of records in the file
               double* _timestamps ;       ///< Pointer to array of timestamps
               char* _fileName ;           ///< Trick binary input file
               std::string _unitTimeStr ;  ///< Name of the time parameter

               /// Map from variable names to log parameters
               std::map<std::string, LOG_PARAM*>   _map_params ;

               /// Storage for parameters
               std::vector<LOG_PARAM> _list_params ;

               int _num_params ;          ///< Number of parameters in the file

               int _swap ;                ///< Whether byteswapping is needed
               FILE *_fp ;                ///< Trick binary input file pointer
               int _record_size ;         ///< Size of one record in bytes
               int _data_offset ;         ///< Current offset in the input file
               char * _record ;           ///< Storage space for a record
//               int _size ;

               void _load_data();          ///< Load data from the file
               void _cleanup();            ///< Deallocate data
               void _process_transforms(); ///< Calculate transformed values

               vector<Transformation> _transforms; ///< Storage for transforms
               TrickParamTypes* _param_types;      ///< Converter for .trk param values
} ;

#endif
