#ifndef _TRICK04YIVER_H_
#define _TRICK04RIVER_H_

#include "DataRiver.hh"
#include "LogData.hh"
#include "../libtvs/vsstructs.hh"
#include "../libtvs/transformation.hh"
#include <vector>

/** \ingroup libdp 
 * Provides a data river implementation based on a pre-recorded Trick 04
 * binary file.
 * Trick04River is passed a Trick 04 binary file and a set of transforms as
 * construction. It then opens the file, reads the content, and stores the
 * values from the file in a set of TrickParams. When all values have been
 * read in, Trick04River adds to the set of parameters the outputs from any
 * requested transforms.
 */
class Trick04River : public DataRiver {

       public:

               Trick04River(char* file_name,
                            std::vector<Transformation> transforms);
               ~Trick04River();

               int           getNumPoints()        ;
               double*       getTimeStamps()       ;
               double*       getVals(const char* param)  ;

               const char*   getUnit(const char* param)  ;
               const char*   getFileName()         ;
               const char*   getTimeUnit()         ;

               int           getNumParams()        ;
               int           getNumTransforms() { return _transforms.size(); }

       protected:

               void _load_data();          ///< Load param data from the file
               void _process_transforms(); ///< Calculate transformed values
               void _cleanup();            ///< Deallocate data

               LogGroup* _lg ;             ///< Array of log groups
               LogData* _log ;             ///< Array of log data
               char _rundir[1024] ;        ///< Run directory path

               double* _timestamps ;       ///< Pointer to timestamps array
               std::vector<TrickParam> _params; ///< Storage space for values
               std::vector<Transformation> _transforms; ///< Incoming transforms

               char* _infile ;             ///< incoming Trick 04 data file
} ;

#endif
