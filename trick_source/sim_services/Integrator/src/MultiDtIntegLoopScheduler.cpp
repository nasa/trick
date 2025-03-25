
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
MultiDtIntegLoopScheduler::MultiDtIntegLoopScheduler(double in_cycle, Trick::SimObject * parent_so)
    : IntegLoopScheduler(in_cycle, parent_so)
{
    integ_rates.push_back(in_cycle);
    next_tic = (long long)round(in_cycle * Trick::JobData::time_tic_value);
}

/**
 Default constructor
 */
MultiDtIntegLoopScheduler::MultiDtIntegLoopScheduler()
{
    integ_rates.push_back(next_cycle);
    next_tic = (long long)round(next_cycle * Trick::JobData::time_tic_value);
}

Trick::JobData * MultiDtIntegLoopScheduler::find_integ_loop_job()
{
    Trick::JobData * found_job = NULL;
    size_t loops_found = 0;
    for(std::vector<Trick::JobData *>::iterator vit = parent_sim_object->jobs.begin();
        vit != parent_sim_object->jobs.end();
        ++vit)
    {
        JobData * vit_job = *vit;
        if(!vit_job->job_class_name.compare("integ_loop"))
        {
            ++loops_found;
            found_job = vit_job;
        }
    }

    // Not finding exactly one integ_loop job is an error.
    if(loops_found != 1)
    {
        message_publish(MSG_ERROR,
                        "Integ Scheduler ERROR: %s\n",
                        (loops_found == 0) ? "integ_loop job not found." : "multiple integ_loop jobs found.");
        return NULL;
    }

    return found_job;
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
        ss << "Integ Scheduler ERROR: Integrate loop scheduled tic " << t_end_tics << " and next_cycle_tic " << next_tic
           << " are not equal.";
        exec_terminate_with_return(-1, __FILE__, __LINE__, ss.str().c_str());
    }

    indices_to_process.clear();
    for(size_t cycleIndex = 0; cycleIndex < integ_next_tics.size(); ++cycleIndex)
    {
        long long integNextTic = integ_next_tics[cycleIndex];
        if(integNextTic == next_tic)
        {
            indices_to_process.push_back(cycleIndex);
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

    for(size_t indProcIdx = 0; indProcIdx < indices_to_process.size(); ++indProcIdx)
    {
        size_t index_to_process = indices_to_process[indProcIdx];
        integ_next_tics[index_to_process] += integ_cycle_tics[index_to_process];
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

    integ_cycle_tics.resize(integ_rates.size());
    integ_next_tics.resize(integ_rates.size());
    for(size_t ii = 0; ii < integ_rates.size(); ++ii)
    {
        double integ_rate = integ_rates[ii];
        long long cycle_tics = (long long)round(integ_rate * Trick::JobData::time_tic_value);
        integ_cycle_tics[ii] = cycle_tics;
        integ_next_tics[ii] = curr_tics + cycle_tics;
    }

    next_tic = calculate_next_integ_tic();
    double next_time = (double)next_tic / (double)Trick::JobData::time_tic_value;
    double nominal_cycle_stored = nominal_cycle;
    set_integ_cycle(next_time);
    nominal_cycle = nominal_cycle_stored;
}

/**
 * Updates an integration rate by index and cycle. Calling with 0
 * index is equivalent to set_integ_cycle
 * @param rateIdx  New integration rate in seconds
 * @param integRateIn index of the added rate
 * @return Zero = success, non-zero = failure (rateIdx is invalid).
 */
int MultiDtIntegLoopScheduler::set_integ_rate(const size_t rate_idx, const double integRateIn)
{
    if(rate_idx >= integ_rates.size())
    {
        message_publish(MSG_ERROR, "Integ Scheduler ERROR: set_integ_cycle: invalid rate idx %lu\n", rate_idx);
        return 1;
    }

    JobData * found_job = find_integ_loop_job();

    // Found exactly one integ_loop job.
    // Set the cycle rate for that one job and for this object.
    // Note: This assumes that the one found job pertains to this scheduler.
    if(found_job != NULL)
    {
        // Get previous rate and cycle
        double prev_integ_rate = integ_rates[rate_idx];
        long long prev_cycle_tics = (long long)round(prev_integ_rate * Trick::JobData::time_tic_value);

        long long curr_tics = exec_get_time_tics();

        // Instead of found_job->set_cycle (in_cycle);
        long long cycle_tics = (long long)round(integRateIn * Trick::JobData::time_tic_value);

        // Instead of found_job->set_next_call_time (curr_tics);
        long long next_tics = found_job->next_tics;
        if(next_tics > curr_tics)
        {
            next_tics = curr_tics - (curr_tics % cycle_tics) + cycle_tics;
        }
        else
        {
            next_tics = curr_tics;
        }

        integ_rates[rate_idx] = integRateIn;
        integ_cycle_tics.resize(integ_rates.size());
        integ_next_tics.resize(integ_rates.size());
        integ_cycle_tics[rate_idx] = cycle_tics;
        integ_next_tics[rate_idx] = next_tics;

        next_tic = calculate_next_integ_tic();
        double next_time = (double)next_tic / (double)Trick::JobData::time_tic_value;
        found_job->next_tics = next_tic;
        next_cycle = next_time - exec_get_sim_time();
        if(rate_idx == 0)
        {
            nominal_cycle = integRateIn;
        }

        return 0;
    }

    return 1;
}

/**
 * Change the interval between calls to the integ_loop job.
 * @param cycle  New integration cycle time, in Trick seconds.
 * @return Zero = success, non-zero = failure (integ_loop job error).
 */
int MultiDtIntegLoopScheduler::set_integ_cycle(double in_cycle)
{
    return set_integ_rate(0, in_cycle);
}

/**
 * Add an integration rate to this loop scheduler
 * @param integRateIn  New integration rate in seconds
 * @return vector index of the added rate
 */
size_t MultiDtIntegLoopScheduler::add_rate(const double integRateIn)
{
    integ_rates.push_back(integRateIn);
    return integ_rates.size() - 1;
}

/**
 * Get the total number of rates for this IntegLoop instance
 * @return total number of rates
 */
size_t MultiDtIntegLoopScheduler::get_num_rates()
{
    return integ_rates.size();
}

/**
 * Get the integration rate according to rate index
 * @return cycle in seconds
 */
double MultiDtIntegLoopScheduler::get_rate(const size_t rateIdx)
{
    if(rateIdx < integ_rates.size())
    {
        return integ_rates[rateIdx];
    }
    return -1.0;
}

/**
 * Loop through the required integration rates and calculate the
 * next integration time in tics.
 * @return Next integration time in tics,
 */
long long MultiDtIntegLoopScheduler::calculate_next_integ_tic()
{
    long long ticOfCycleToProcess = std::numeric_limits<long long>::max();

    for(size_t cycleIndex = 0; cycleIndex < integ_next_tics.size(); ++cycleIndex)
    {
        long long integNextTic = integ_next_tics[cycleIndex];

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

    for(size_t cycleIndex = 0; cycleIndex < integ_next_tics.size(); ++cycleIndex)
    {
        long long integNextTic = integ_next_tics[cycleIndex];

        if(integNextTic < ticOfCycleToProcess)
        {
            ticOfCycleToProcess = integNextTic;
        }
    }

    return (double)ticOfCycleToProcess / (double)Trick::JobData::time_tic_value;
}

} // namespace Trick
