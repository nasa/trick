#ifndef _DATA_RIVER_FACTORY_H_
#define _DATA_RIVER_FACTORY_H_

#include "DataRiver.hh"
#include "../libtvs/transformation.hh"
#include <vector>
using std::vector;
class VSClientConnector;

struct VSMessage;

/** \ingroup libdp
 *  \file
 *  Contains factory methods to construct a data river
 */

/// Factory function for a live data river
DataRiver* get_data_river( VSClientConnector* connector, double rate,
                           int numnames, char** names, 
                           vector<Transformation>& transforms,
                           void (*callback)(VSMessage& msg)) ;

/// Factory function for a file-based data river
DataRiver* get_data_river( char* path,
                           vector<Transformation>& transforms );

#endif


