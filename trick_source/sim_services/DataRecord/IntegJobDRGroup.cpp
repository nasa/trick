#include "trick/IntegrationJobDataRecordGroup.hh"
#include "trick/message_proto.h"
#include "trick/message_type.h"


int getStepsForIntegType(Integrator_type integ_type)
{
    static int stepsPerType[User_Defined+1]{};
    if(stepsPerType[Euler] == 0)
    {
        stepsPerType[Euler] = 1;
        stepsPerType[Euler_Cromer] = 1;
        stepsPerType[Nystrom_Lear_2] = 2;
        stepsPerType[Runge_Kutta_2] = 2;
        stepsPerType[Modified_Midpoint_4] = 3;
        stepsPerType[Runge_Kutta_4] = 4;
        stepsPerType[Runge_Kutta_Gill_4] = 4;
        stepsPerType[Runge_Kutta_Fehlberg_45] = 6;
        stepsPerType[Runge_Kutta_Fehlberg_78] = 12;
        stepsPerType[ABM_Method] = 4;
        stepsPerType[User_Defined] = 1;
    }

    return stepsPerType[integ_type];
}

template<typename T, typename U>
void register_integgroup_wih_mm(T & grp, const std::string & nameIn)
{
    grp.U::set_job_class("integration");
    grp.U::register_group_with_mm(&grp, nameIn.c_str());
}

template<typename T, typename U>
int integgroup_init(T & grp, bool is_restart)
{
    if (grp.integSchedulerPtr == nullptr)
    {
        message_publish(MSG_ERROR, "DataRecordGroup ERROR: IntegLoopScheduler pointer is NULL for group %s\n",
                        grp.group_name.c_str());
        return -1;
    }
    if (!is_restart)
    {
        grp.integSchedulerPtr->add_sim_object(grp);
    }
    if (grp.integSchedulerPtr->integ_ptr != nullptr && grp.write_job->sup_class_data == nullptr)
    {
        // setIntegrator(*(grp.integSchedulerPtr->integ_ptr));
        grp.write_job->sup_class_data = &(grp.integSchedulerPtr->integ_ptr);
    }
    if (grp.write_job->sup_class_data == nullptr)
    {
        message_publish(MSG_ERROR, "DataRecordGroup ERROR: Integrator pointer is NULL for group %s\n",
                        grp.group_name.c_str());
        return -1;
    }
    if (!is_restart)
    {
        grp.max_num *= getStepsForIntegType(grp.integSchedulerPtr->integ_ptr->get_Integrator_type());
    }
    return grp.U::init(is_restart);
}


template<typename T, typename U>
int integgroup_data_record(T & grp, double in_time)
{
    Trick::Integrator * integPtr = grp.integSchedulerPtr->integ_ptr;
    double integTime = integPtr->time_0;
    int MaxSteps = getStepsForIntegType(integPtr->get_Integrator_type());
    double currLogTime;
    if (integPtr->intermediate_step == 0 && std::abs(in_time - integPtr->time) < 1.0e-12)
    {
        currLogTime = in_time;
    }
    else
    {
        currLogTime = integTime + (integPtr->intermediate_step * (integPtr->dt / MaxSteps));
    }

    // Check if we want to log every integration cycle
    grp.U::data_record(currLogTime);
    return integPtr->intermediate_step;
}

Trick::IntegJobDRBinary::IntegJobDRBinary(const std::string& in_name, Trick::IntegLoopScheduler& integSchedulerRefIn)
    : Trick::DRBinary(in_name, false), integSchedulerPtr(&integSchedulerRefIn)
{
    // DRBinary::set_job_class("integration");
    // DRBinary::register_group_with_mm(this, "Trick::IntegJobDRBinary");
    register_integgroup_wih_mm<IntegJobDRBinary, DRBinary>(*this, "Trick::IntegJobDRBinary");
}

int Trick::IntegJobDRBinary::init(bool is_restart)
{
    return integgroup_init<IntegJobDRBinary, DRBinary>(*this, is_restart);
}

int Trick::IntegJobDRBinary::data_record(double in_time)
{
    return integgroup_data_record<IntegJobDRBinary, DRBinary>(*this, in_time);
}

Trick::IntegJobDRAscii::IntegJobDRAscii(const std::string& in_name, Trick::IntegLoopScheduler& integSchedulerRefIn)
    : Trick::DRAscii(in_name, false), integSchedulerPtr(&integSchedulerRefIn)
{
    // DRBinary::set_job_class("integration");
    // DRBinary::register_group_with_mm(this, "Trick::IntegJobDRBinary");
    register_integgroup_wih_mm<IntegJobDRAscii, DRAscii>(*this, "Trick::IntegJobDRBinary");
}

int Trick::IntegJobDRAscii::init(bool is_restart)
{
    return integgroup_init<IntegJobDRAscii, DRAscii>(*this, is_restart);
}

int Trick::IntegJobDRAscii::data_record(double in_time)
{
    return integgroup_data_record<IntegJobDRAscii, DRAscii>(*this, in_time);
}

Trick::IntegJobDRHDF5::IntegJobDRHDF5(const std::string& in_name, Trick::IntegLoopScheduler& integSchedulerRefIn)
    : Trick::DRHDF5(in_name, false), integSchedulerPtr(&integSchedulerRefIn)
{
    // DRBinary::set_job_class("integration");
    // DRBinary::register_group_with_mm(this, "Trick::IntegJobDRBinary");
    register_integgroup_wih_mm<IntegJobDRHDF5, DRHDF5>(*this, "Trick::IntegJobDRBinary");
}

int Trick::IntegJobDRHDF5::init(bool is_restart)
{
    return integgroup_init<IntegJobDRHDF5, DRHDF5>(*this, is_restart);
}

int Trick::IntegJobDRHDF5::data_record(double in_time)
{
    return integgroup_data_record<IntegJobDRHDF5, DRHDF5>(*this, in_time);
}
