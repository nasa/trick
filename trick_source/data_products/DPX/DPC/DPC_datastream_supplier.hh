
#ifndef DPC_DATASTREAM_SUPPLIER_HH
#define DPC_DATASTREAM_SUPPLIER_HH

#include "../../Log/DataStream.hh"
#include "../../Log/DataStreamFactory.hh"

#include <vector>
#include "DPM/DPM_extfn.hh"
#include "DPM/DPM_run.hh"
#include "DPM/DPM_product.hh"
#include "DPM/DPM_time_constraints.hh"

/**
 * This class attempts to supply DataStreams according to a set of specifications.
 * @author John M. Penn
 */
class DPC_datastream_supplier {

public:

    /**
     * Constructor.
     */
    DPC_datastream_supplier( DPM_product *product );

    /**
     * Destructor.
     */
    ~DPC_datastream_supplier();

    /**
     * Return a DatasStream as specified by the arguments.
     */
    DataStream *getDataStream( const char *VarName,
                               const char *ToUnits,
                               const char *FromUnitsHint,
                               DPM_run    *Run,
                               DPM_time_constraints* time_constraints );

    DataStream *getDataStream( const char *VarName,
                               const char *ToUnits,
                               const char *FromUnitsHint,
                               const char *Machine,
                               const unsigned short Port,
                               DPM_time_constraints* time_constraints );
private:

    DPM_product *product;
    DataStreamFactory *data_stream_factory;
};

#endif
