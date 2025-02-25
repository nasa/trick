
// Local includes
#include "trick/MultiDtIntegLoopScheduler.hh"

// Trick includes
#include "trick/exec_proto.h"
#include "trick/exec_proto.hh"
#include "trick/message_proto.h"

// System includes
#include <sstream>


// Anonymous namespace for local functions
namespace
{
/**
 * Process the enabled job in the provided job queue.
 * @param job_queue  Job queue to be processed.
 */
inline void call_jobs(Trick::ScheduledJobQueue & job_queue)
{
    Trick::JobData * curr_job;
    job_queue.reset_curr_index();
    while((curr_job = job_queue.get_next_job()) != NULL)
    {
        curr_job->call();
    }
}
} // namespace

namespace Trick
{

/**
 Non-default constructor.
 @param in_cycle The time interval at which the loop's integrate function is called
 @param in_parent_so Pointer to the parent sim object.
 */
MultiDtIntegLoopScheduler::MultiDtIntegLoopScheduler(double in_cycle, Trick::SimObject *parent_so)
    : IntegLoopScheduler(in_cycle, parent_so)
{
    integRates.push_back(in_cycle);
    next_tic = (long long)round(in_cycle * Trick::JobData::time_tic_value);
}

/**
 Default constructor
 */
MultiDtIntegLoopScheduler::MultiDtIntegLoopScheduler()
{
    next_tic = (long long)round(next_cycle * Trick::JobData::time_tic_value);
}

/**
 * Integrate state to the current simulation time.
 * Simulation time is advanced prior to calling this function.
 * Calculate the next cycle tic from the required rates and
 * tell the Executive scheduler so that the next current simulation time
 * is the next integ rate.
 */
int MultiDtIntegLoopScheduler::integrate()
{
    double t_end = exec_get_sim_time();
    double t_start = t_end - next_cycle; // This is the time of the current state vector.
    long long t_end_tics = exec_get_time_tics();
    int status;

    if(t_end_tics != next_tic)
    {
        std::stringstream ss;
        ss << "Integ Scheduler ERROR: Integrate loop scheduled tic " << t_end_tics << " and next_cycle_tic "
           << next_tic << " are not equal.";
        exec_terminate_with_return(-1, __FILE__, __LINE__, ss.str().c_str());
    }

    std::vector<size_t> indicesToProcess;
    for(size_t cycleIndex = 0; cycleIndex < integNextTics.size(); ++cycleIndex)
    {
        long long integNextTic = integNextTics[cycleIndex];
        if(integNextTic == next_tic)
        {
            indicesToProcess.push_back(cycleIndex);
        }
    }

    double var_next_cycle = t_end - t_start;
    // Call all of the jobs in the pre-integration job queue.
    if(trick_curr_integ == nullptr)
    {
        if(integ_ptr != nullptr)
        {
            trick_curr_integ = integ_ptr;
            trick_curr_integ->time = t_start;
            trick_curr_integ->dt = var_next_cycle;
            trick_curr_integ->target_integ_time = t_start + var_next_cycle;
        }
        else
        {
            integ_jobs.reset_curr_index();
            Trick::JobData * curr_job = integ_jobs.get_next_job();
            if(curr_job != nullptr)
            {
                void * sup_class_data = curr_job->sup_class_data;
                // Jobs without supplemental data use the default integrator.
                if(sup_class_data == NULL)
                {
                    trick_curr_integ = integ_ptr;
                }
                // Non-null supplemental data:
                // Resolve as a pointer-to-a-pointer to a Trick::Integrator.
                else
                {
                    trick_curr_integ = *(static_cast<Trick::Integrator **>(sup_class_data));
                }

                if(trick_curr_integ == NULL)
                {
                    message_publish(MSG_ERROR,
                                    "Integ Scheduler ERROR: "
                                    "Integrate job has no associated Integrator.\n");
                }
            }
        }
    }

    call_jobs(pre_integ_jobs);

    // Integrate sim objects to the current time.
    status = integrate_dt(t_start, var_next_cycle);
    if(status != 0)
    {
        return status;
    }

    // Process dynamic events, if any.
    if(!dynamic_event_jobs.empty())
    {
        status = process_dynamic_events(t_start, var_next_cycle);

        if(status != 0)
        {
            return status;
        }
    }

    // Call the jobs in the derivative job queue one more time if indicated.
    if(get_last_step_deriv())
    {
        call_jobs(deriv_jobs);
    }

    // Call all of the jobs in the post-integration job queue.
    call_jobs(post_integ_jobs);

    for(auto indexToProcess : indicesToProcess)
    {
        integNextTics[indexToProcess] += integCycleTics[indexToProcess];
    }

    t_start = t_end;

    next_tic = calculate_next_integ_tic();
    Trick::JobData * found_job = exec_get_curr_job();
    double next_time = (double)next_tic / (double)Trick::JobData::time_tic_value;
    found_job->next_tics = next_tic;
    next_cycle = next_time - t_end;
    trick_curr_integ = nullptr;

    return 0;
}

void MultiDtIntegLoopScheduler::initialize_rates()
{
    long long curr_tics = exec_get_time_tics();

    // Check for the overall integration cycle from S_define
    bool isNominalCycleAdded = false;
    for(auto integRate : integRates)
    {
        if(fabs(nominal_cycle - integRate) < 1.0e-10)
        {
            isNominalCycleAdded = true;
        }
    }
    if(!isNominalCycleAdded)
    {
        integRates.push_back(nominal_cycle);
    }

    for(auto integRate : integRates)
    {
        long long cycle_tics = (long long)round(integRate * Trick::JobData::time_tic_value);
        integCycleTics.push_back(cycle_tics);
        integNextTics.push_back(curr_tics + cycle_tics);
    }

    next_tic = calculate_next_integ_tic();
    double next_time = (double)next_tic / (double)Trick::JobData::time_tic_value;
    double nominal_cycle_stored = nominal_cycle;
    set_integ_cycle(next_time);
    nominal_cycle = nominal_cycle_stored;
}

/**
 * Add an integration rate to this loop scheduler
 * @param integRateIn  New integration rate in seconds
 */
void MultiDtIntegLoopScheduler::addRate(const double integRateIn)
{
    integRates.push_back(integRateIn);
}

/**
 * Loop through the required integration rates and calculate the
 * next integration time in tics.
 * @return Next integration time in tics,
 */
long long MultiDtIntegLoopScheduler::calculate_next_integ_tic()
{
    long long ticOfCycleToProcess = std::numeric_limits<long long>::max();

    for(size_t cycleIndex = 0; cycleIndex < integNextTics.size(); ++cycleIndex)
    {
        long long integNextTic = integNextTics[cycleIndex];

        if(integNextTic < ticOfCycleToProcess)
        {
            ticOfCycleToProcess = integNextTic;
        }
    }
    return ticOfCycleToProcess;
}

/**
 * Loop through the required integration rates and calculate the
 * next integration time in seconds.
 * @return Next integration time in seconds,
 */
double MultiDtIntegLoopScheduler::calculate_next_integ_time()
{
    long long ticOfCycleToProcess = std::numeric_limits<long long>::max();

    for(size_t cycleIndex = 0; cycleIndex < integNextTics.size(); ++cycleIndex)
    {
        long long integNextTic = integNextTics[cycleIndex];

        if(integNextTic < ticOfCycleToProcess)
        {
            ticOfCycleToProcess = integNextTic;
        }
    }

    return (double)ticOfCycleToProcess / (double)Trick::JobData::time_tic_value;
}

}
