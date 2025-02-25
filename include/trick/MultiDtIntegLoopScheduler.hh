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

namespace Trick {

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
 *  - 0->0.01
 *  - 0.01->0.015
 *  - 0.015->0.02
 *  - 0.02->0.03
 *  - 0.03->0.04
 *  - 0.04->0.055
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
     */
    void addRate(const double integRateIn);

    /**
     * Vector of integration rates in seconds.
     */
    std::vector<double> integRates;

protected:
    /**
     * Time in tics of the next required integration time.
     */
    long long next_tic;

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
    std::vector<long long> integNextTics;
    /**
     * Vector of integration rate in tics for each rate in the integRates vector
     */
    std::vector<long long> integCycleTics;
};

}

#endif /* JEOD_MODELS_UTILS_INTEGRATION_INCLUDE_MULTIDTINTEGLOOPSCHEDULER_HH_ */
