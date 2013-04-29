#ifndef _CSVRIVER_H_
#define _CSVRIVER_H_

#include "DataRiver.hh"

#include <errno.h>
#include "../libcsv/csv.h"
#include "../libtvs/vsstructs.hh"
#include "../libtvs/transformation.hh"

#include <map>
#include <string>
#include <vector>

void cb_field (char* field, size_t len, void *data) ; 
void cb_line (char cc, void *data) ; 

/// Work space used during parsing
typedef struct {
        bool is_error ;                 ///< True if a parse error has occurred
        long num_recs ;                 ///< Number of recs in the file
        long num_points ;               /**< Number of points (available params)
                                          *  in the file */
        int  num_params ;               ///< Number of requested parameters
        std::vector<TrickParam> params; ///< Space for requested parameters
        int nchunks ;                   ///< Number of file chunks to read
        int curr_val_idx ;
} CSV_DATA ;

/** \ingroup libdp
 * Provides a data river implementation based on a pre-recorded Trick CSV
 * file.
 * CsvRiver is passed a Trick csv file and a set of transforms as
 * construction. It then opens the file, reads the content with the assistance
 * of libcsv, and stores the values from the file in a set of TrickParams. 
 * When all values have been read in, CsvRiver adds to the set of parameters 
 * the outputs from any requested transforms.
 */
class CsvRiver : public DataRiver {

       public:

               CsvRiver(char* file_name,                   ///< CSV filename
                        vector<Transformation>& transforms ///< Transforms list
                       );
               ~CsvRiver();   ///< Destructor

               // Implemented interface
               double*       getVals(const char* param)  ;
               const char*   getUnit(const char* param)  ;
               int           getNumPoints()        ;
               const char*   getFileName()         ;
               const char*   getTimeUnit()         ;
               double*       getTimeStamps()       ;
               int           getNumParams()        ;
               int           getNumTransforms() { return _transforms.size(); }

       protected:

               void _cleanup();            ///< Deallocate parameters
               void _process_transforms(); ///< Add processed parameter values
               string _filename ;           ///< Name of the Trick data file

               struct csv_parser* _parser;  ///< CSV parser from libcsv

               double* _timestamps ;        ///< Storage for data timestamps
               std::string _unitTimeStr ;   ///< Time variable units e.g. "s"

               /** Map of variable names to value arrays.
                   e.g. _map_vals["myvar"][3] is the third value for myvar in 
                   the stream
               */
               std::map<std::string, double*> _map_vals ;

               /** Map of variable names to units strings.
                   e.g. _map_units["myvar"] is the units for myvar
               */
               std::map<std::string, std::string>  _map_units ;

               CSV_DATA _parse_data ;  ///< Working space during parsing

               /// Copy of any incoming transforms
               vector<Transformation> _transforms; 
} ;

#endif
