
#ifndef DPC_UNITCONVDATASTREAM_HH
#define DPC_UNITCONVDATASTREAM_HH

#include <string>
#include <udunits2.h>
#include "../../Log/DataStream.hh"

/**
 * DPC_UnitConvDataStream is a DataStream that performs unit conversion.
 * @author John M. Penn
 * @version
 */
class DPC_UnitConvDataStream : public DataStream {

public:

    /**
     * Constructor.
     * @param ds A pointer to the DataStream object whose values are to be
     * converted to the requested units.
     *
     * @param ToUnits If non-NULL, this specifies the units to which data values
     * are to be converted before being returned by get() or peek().
     *
     * @param FromUnitsHint If the recorded data does not contain the units
     * in which the data were recorded, but the user of this class knows what
     * those units should be, then this parameter provides a means of
     * specifying what those units are. In most cases this parameter should
     * be NULL.
     */
    DPC_UnitConvDataStream(DataStream* ds, const char *ToUnits, const char *FromUnitsHint );

    /**
     * Destructor.
     */
    ~DPC_UnitConvDataStream();

    /**
     * Get the timestamp/value pair at the current position in the DataStream
     * and move to the next position.
     * @param timestamp the timestamp value returned from the DataStream.
     * @param paramValue the parameter value returned from the DataStream.
     * @return 1 if a time/value pair was returned, 0 otherwise.
     */
    int get(double* timestamp, double* paramValue);

    /**
     * Get the timestamp/value pair at the current position in the DataStream
     * but do not move to the next position.
     * @param timestamp the timestamp value returned from the DataStream.
     * @param paramValue the parameter value returned from the DataStream.
     * @return 0 if a time/value pair was returned, -1 otherwise. Yes, I know,
     * it's inconsistent.
     */
    int peek(double* timestamp, double* paramValue);

    /**
     * Return the name of the file from which the data is being streamed.
     */
    std::string getFileName();

    /**
     * Return the data's actual units of measure, with will, if the unit specification
     * passed to the constructor was correct and compatible with the units of he source
     * DataStream. If the requested units are in error, this function will return the
     * actual (unconverted) units.
     */
    std::string getUnit();

    std::string getTimeUnit();

    /**
     * Set the DataStream to read from the beginning.
     */
    void begin();

    /**
     * Test for the end of the DataStream.
     * @return 1 if the end of the DataStream has been reached, 0 otherwise.
     */
    int end();

    /**
     * Progress forward one position in the DataStream.
     * @return 1 if we progressed, 0 otherwise.
     */
    int step();

private:

    cv_converter * cf ;
    std::string to_units ;

    DataStream *source_ds;

};

#endif
