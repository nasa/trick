/*
PURPOSE:
    ( Multi-Dt Integrator "Sim object" )
PROGRAMMERS:
    (((Thomas Brain) (METECS)  (2025)))
*/

#ifndef MULTIDTINTEGLOOPSCHEDULER_HH
#define MULTIDTINTEGLOOPSCHEDULER_HH

// Local includes
#include "trick/IntegLoopScheduler.hh"

// System includes
#include <math.h>

#ifdef SWIG
// We want SWIG to ignore add_sim_object(Trick::SimObject *)
// so only add_sim_object(Trick::SimObject &) can be called in input file.
%ignore Trick::MultiDtIntegLoopScheduler::add_sim_object(Trick::SimObject *) ;
#endif

namespace Trick
{

/**
 * This class is a Scheduler that provides the ability to integrate a
 * collection of sim object instances over time at multiple integration
 * rates similar to the IntegLoopScheduler class. The
 * MutliDtIntegLoopScheduler class integrates the same class of jobs
 * as IntegLoopScheduler. The primary difference is that this class may
 * have multiple integration rates specified for executing the integration
 * loop up to each point in time. For instance, if the user specifies
 * a 0.01 integration rate and a 0.015 integration rate, the scheduler will integrate
 * using a dt for each step and reschedules itself so that each rate is satisfied
 * i.e. it will integrate at:
 *  - 0.000->0.010
 *  - 0.010->0.015
 *  - 0.015->0.020
 *  - 0.020->0.030
 *  - 0.030->0.040
 *  - 0.040->0.045
 *
 * Because it acts as a self-scheduler that updates its job
 * rate with the Trick::Executive, this class complies with the standard Trick job
 * scheduling scheme.
 */
class MultiDtIntegLoopScheduler : public Trick::IntegLoopScheduler
{
public:
    /**
     * Non-default constructor.
     * @param in_cycle   The time interval at which the loop's integrate
     *                   function will be called. At least one is required.
     * @param parent_so  The Trick simulation object that contains this
     *                   IntegLoopScheduler object.
     */
    MultiDtIntegLoopScheduler(double in_cycle, Trick::SimObject * parent_so);

    /**
     * Default constructor.
     * @note This exists for checkpoint/restart. Other users should
     * not call this function.
     */
    MultiDtIntegLoopScheduler();

    /**
     * Destructor.
     */
    virtual ~MultiDtIntegLoopScheduler() {}

    /**
     * Compute the cycle tics and next tics values for each user-specified rate
     */
    void initialize_rates();

    /**
     * Integrate state to the current simulation time.
     * Simulation time is advanced prior to calling this function.
     * Calculate the next cycle tic from the required rates and
     * tell the Executive scheduler so that the next current simulation time
     * is the next integ rate.
     */
    int integrate();

    /**
     * Add an integration rate to this loop scheduler
     * @param integRateIn  New integration rate in seconds
     * @return vector index of the added rate
     */
    size_t add_rate(const double integRateIn);

    /**
     * Get the total number of rates for this IntegLoop instance
     * @return total number of rates
     */
    size_t get_num_rates();

    /**
     * Get the integration rate according to rate index
     * @return cycle in seconds or -1.0 if error
     */
    double get_rate(const size_t rateIdx = 0);

    /**
     * Change the interval between calls to the integ_loop job.
     * @param cycle  New integration cycle time, in Trick seconds.
     */
    virtual int set_integ_cycle (double cycle);

    /**
     * Updates an integration rate by index and cycle. Calling with 0
     * index is equivalent to set_integ_cycle
     * @param rateIdx  New integration rate in seconds
     * @param integRateIn index of the added rate
     * @return Zero = success, non-zero = failure (rateIdx is invalid).
     */
    virtual int set_integ_rate(const size_t rateIdx, const double integRateIn);

protected:
    /**
     * Time in tics of the next required integration time.
     */
    long long next_tic;

    /**
     * Vector of integration rates in seconds.
     */
    std::vector<double> integ_rates;

    /**
     * Loop through the required integration rates and calculate the
     * next integration time in tics.
     * @return Next integration time in tics,
     */
    long long calculate_next_integ_tic();

    /**
     * Loop through the required integration rates and calculate the
     * next integration time in seconds.
     * @return Next integration time in seconds,
     */
    double calculate_next_integ_time();

    /**
     * Vector of next integration time in tics for each rate in the integRates vector
     */
    std::vector<long long> integ_next_tics;

    /**
     * Vector of integration rate in tics for each rate in the integRates vector
     */
    std::vector<long long> integ_cycle_tics;

    /**
     * Run-time vector of integration rate indices that will be processed this integration frame
     */
    std::vector<size_t> indices_to_process;

    /**
     * Find the associated integ_loop job from this object's
     * parnet_sim object
     * @return Pointer to integ_loop kob
     */
    JobData * find_integ_loop_job();

    /**
     * Vector of next integration time in tics for each rate in the integRates vector
     */
    std::vector<long long> integNextTics;
    /**
     * Vector of integration rate in tics for each rate in the integRates vector
     */
    std::vector<long long> integCycleTics;
};

} // namespace Trick

#endif /* MULTIDTINTEGLOOPSCHEDULER_HH */
