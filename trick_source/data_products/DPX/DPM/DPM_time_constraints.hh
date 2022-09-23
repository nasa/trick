
#ifndef DPM_TIME_CONSTRAINTS_HH
#define DPM_TIME_CONSTRAINTS_HH

#define T_TOLERANCE 1E-37
/**
 * This class represents a set of time constraints that may be applied to a
 * DataStream (via DPM_TimeCstrDataStream ). They constrain a DataStream :
 * - to start no earlier than a specified start-time.
 * - to end no later than a specified stop-time.
 * - to return values at a (timestamp) frequency, not higher than a specified frequency-limit.
 * @author John M. Penn
 */

class DPM_time_constraints {

public:

  /**
   * Constructor.
   */
  DPM_time_constraints();

  /**
   * Constructor.
   */
  DPM_time_constraints(double start, double stop, double freq);

  /**
   * Destructor.
   */
  ~DPM_time_constraints() {}

  /**
   * Return the Start-time (in seconds).
   */
  double getStart();

  /**
   * Return the Stop-time (in seconds).
   */
  double getStop();

  /**
   * Return the Frequency-limit (in Hertz).
   */
  double getFrequency();

  /**
   * Return the reciprocal of the frequency-limit.
   */
  double getPeriod();

  /**
   * Add two sets of time constraints. The sum will be as restrictive as either.
   */
  friend DPM_time_constraints operator+ ( DPM_time_constraints &tc1,
					  DPM_time_constraints &tc2);
private:

  double start_time;  /**< minimum timestamp */
  double stop_time;   /**< maximum timestamp */
  double frequency;   /**< maximum sample frequency */
  void Initialize();
};

#endif
