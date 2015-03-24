
#ifndef DPC_TIMECSTRDATASTREAM_HH
#define DPC_TIMECSTRDATASTREAM_HH
#include <iostream> // FOR DEBUGGING

#include <string>
#include "DPM/DPM_time_constraints.hh"
#include "../Log/DataStream.hh"

/**
 * DPC_TimeCstrDataStream is a DataStream. The timestamp/value pairs that it supplies will not:
 * - have a timestamp earlier than time_constraints->start_time.
 * - have a timestamp later than time_constraints->stop_time.
 * Furthermore:
 * - the difference between timestamps will be no smaller than time_constraints->period.
 *  (period = 1/frequency)
 *
 * @author John M. Penn
 */
class DPC_TimeCstrDataStream : public DataStream {

public:
  /**
   * Constructor.
   * @param ds A reference to the DataStream object to be constrained.
   * @param time_constraints The time constraints to be applyed.
   */
  DPC_TimeCstrDataStream(DataStream* ds,
			    DPM_time_constraints *time_constraints );

  /**
   * Destructor.
   */
  ~DPC_TimeCstrDataStream();

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
   * @return 1 if a time/value pair was returned, 0 otherwise.
   */
  int peek(double* timestamp, double* paramValue);

  /**
   * Return the name of the file from which the data is being streamed.
   */
  std::string getFileName();

  /**
   * Return the data's units of measure.
   */
  std::string getUnit();

  /**
   * Return the units of measure for timestamps.
   */
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

  DataStream *ds;
  double tstart;
  double tstop;
  double period;
  int bix;
  int eos[2];
  double time[2];
  double value[2];
};

#endif
