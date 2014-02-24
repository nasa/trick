#ifndef _HDF5RIVER_H_
#define _HDF5RIVER_H_

#include "DataRiver.hh"
#include "../libtvs/vsstructs.hh"
#include "../libtvs/transformation.hh"

#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "hdf5/hdf5.h"
#include "hdf5/H5PTpublic.h"

/** \ingroup libdp 
 * Provides a data river implementation based on a pre-recorded Trick
 * binary file.
 * HDF5River is passed a Trick binary file and a set of transforms as
 * construction. It then opens the file, reads the content, and stores the
 * values from the file in a set of TrickParams. When all values have been
 * read in, HDF5River adds to the set of parameters the outputs from any
 * requested transforms.
 */
class HDF5River : public DataRiver {

    public:
 
        HDF5River(char* file_name,
                         vector<Transformation>& transforms);
        ~HDF5River();
 
        double*      getVals(const char* param)  ;
        const char*  getUnit(const char* param)  ;
        int          getNumPoints()        ;
        const char*  getFileName()         ;
        const char*  getTimeUnit()         ;
        double*      getTimeStamps()       ;
        int          getNumParams()        ;
        int          getNumTransforms() { return _transforms.size(); }
 
    protected:
        long _num_recs ;            ///< Number of records in the file
        std::string _unitTimeStr ;  ///< Name of the time parameter
        std::string _file_name;
 
        /// Map from variable names to log parameters
        std::map<std::string, TrickParam*>   _map_params ;
 
        /// Storage for parameters
        std::vector<TrickParam> _list_params ;
 
        hsize_t _num_params ;          ///< Number of parameters in the file
 
        void _load_data();          ///< Load data from the file
        void _cleanup();            ///< Deallocate data
        void _process_transforms(); ///< Calculate transformed values
        herr_t _process_dataset(hid_t loc_id, const char* name,
                                const char* unit, unsigned int index);
 
        vector<Transformation> _transforms; ///< Storage for transforms
} ;
 
#endif
