/*******************************************************************************

 PURPOSE:
 (DataRecordGroup class tailored to log all the intermediate_steps of the integration loop.)

 PROGRAMMERS:
 (((Thomas Brain) (Metecs) (May 2024) (--)))

 *******************************************************************************/

#ifndef INTEGRATIONJOBDATARECORDGROUP_HH
#define INTEGRATIONJOBDATARECORDGROUP_HH

#include "trick/DRAscii.hh"
#include "trick/DRBinary.hh"
#ifdef HDF5
#include "trick/DRHDF5.hh"
#endif
#include "trick/IntegLoopScheduler.hh"
#include "trick/message_proto.h"
#include "trick/message_type.h"

#ifdef SWIG
%feature("compactdefaultargs","0") ;
%define INTEG_DR_SHADOW(TYPE)
%feature("shadow") Trick::TYPE::TYPE(const std::string & in_name, Trick::IntegLoopScheduler & integSchedulerPtrIn) %{
    def __init__(self, *args):
        this = $action(*args)
        try: self.this.append(this)
        except: self.this = this
        this.own(0)
        self.this.own(0)
%}
%enddef
#endif

#ifdef SWIG
INTEG_DR_SHADOW(Trick::IntegJobDRBinary)
INTEG_DR_SHADOW(Trick::IntegJobDRAscii)
#ifdef HDF5
INTEG_DR_SHADOW(Trick::IntegJobDRHDF5)
#endif
#endif

namespace Trick
{

template<typename T> class IntegrationJobDataRecordGroupBase : public T
{
public:
#ifndef SWIG
    IntegrationJobDataRecordGroupBase() : T() {

    }
#endif

    IntegrationJobDataRecordGroupBase(const std::string & in_name,
                                     Trick::IntegLoopScheduler & integSchedulerRefIn)
        : T(in_name, false), integSchedulerPtr(&integSchedulerRefIn)
    {
        T::set_job_class("integration");
        if(dynamic_cast<DRBinary *>(this) != nullptr)
        {
            T::register_group_with_mm(this, "Trick::IntegJobDRBinary");        
        } else if(dynamic_cast<DRAscii *>(this) != nullptr)
        {
            T::register_group_with_mm(this, "Trick::IntegJobDRAscii");        
        }
        #ifdef HDF5
        else if(dynamic_cast<DRHDF5 *>(this) != nullptr)
        {
            T::register_group_with_mm(this, "Trick::IntegJobDRHDF5");
        }
        #endif
    }

    virtual ~IntegrationJobDataRecordGroupBase() = default;

    virtual int init(bool is_restart = false) override
    {
         if(integSchedulerPtr == nullptr)
        {
            message_publish(
                    MSG_ERROR,
                    "DataRecordGroup ERROR: IntegLoopScheduler pointer is NULL for group %s\n",
                    T::group_name.c_str());
            return -1;
        }
        if(!is_restart) {
            integSchedulerPtr->add_sim_object(*this);
        }
        if(integSchedulerPtr->integ_ptr != nullptr && T::write_job->sup_class_data == nullptr) {
            setIntegrator(*(integSchedulerPtr->integ_ptr));
        }
        if(T::write_job->sup_class_data == nullptr)
        {
            message_publish(
                    MSG_ERROR,
                    "DataRecordGroup ERROR: Integrator pointer is NULL for group %s\n",
                    T::group_name.c_str());
            return -1;
        }
        if(!is_restart)
        {        
            T::max_num *= MaxSteps;
        }
        return T::init(is_restart);
    }

    virtual int data_record(double in_time) override
    {
        double integTime = integPtr->time_0;
        double currLogTime;
        if(integPtr->intermediate_step == 0 && std::abs(in_time - integPtr->time) < 1.0e-12)
        {
            currLogTime = in_time;
        }
        else
        {
            currLogTime = integTime + (integPtr->intermediate_step * (integPtr->dt / MaxSteps));
        }

        // Check if we want to log every integration cycle
        T::data_record(currLogTime);
        return integPtr->intermediate_step;
    }

    virtual void setMaxSteps(const int MaxStepsIn)
    {
        MaxSteps = MaxStepsIn;
    }

    virtual void setIntegrator(Trick::Integrator & integIn)
    {
        integPtr = &integIn;        
        Integrator_type integ_type = integIn.get_Integrator_type();
        switch(integ_type)
        {
            case Euler:
                MaxSteps = 1;
                break;
            case Euler_Cromer:
                MaxSteps = 1;
                break;
            case Nystrom_Lear_2:
                MaxSteps = 2;
                break;
            case Runge_Kutta_2:
                MaxSteps = 2;
                break;
            case Modified_Midpoint_4:
                MaxSteps = 3;
                break;
            case Runge_Kutta_4:
                MaxSteps = 4;
                break;
            case Runge_Kutta_Gill_4:
                MaxSteps = 4;
                break;
            case Runge_Kutta_Fehlberg_45:
                MaxSteps = 6;
                break;
            case Runge_Kutta_Fehlberg_78:
                MaxSteps = 12;
                break;
            case ABM_Method:
                MaxSteps = 4;
                break;
            case User_Defined:
                MaxSteps = 1;
                break;
            default:
                MaxSteps = 1;
                break;
        }
        T::write_job->sup_class_data = &integPtr;
    }
   
    Trick::IntegLoopScheduler * integSchedulerPtr{};

protected:
    Trick::Integrator * integPtr{};
    int MaxSteps{4};
};

#ifdef SWIG
%template (IntegJobDRBinary) IntegrationJobDataRecordGroupBase<DRBinary>;
// %trick_cast_as(IntegJobDRBinary, IntegJobDRBinary)
%template (IntegJobDRAscii) IntegrationJobDataRecordGroupBase<DRAscii>;
// %trick_cast_as(IntegJobDRAscii, IntegJobDRAscii)
#ifdef HDF5
%template (IntegJobDRHDF5) IntegrationJobDataRecordGroupBase<DRHDF5>;
// %trick_cast_as(IntegJobDRHDF5, IntegJobDRHDF5)
#endif
#endif

typedef IntegrationJobDataRecordGroupBase<DRBinary> IntegJobDRBinary;
typedef IntegrationJobDataRecordGroupBase<DRAscii> IntegJobDRAscii;
#ifdef HDF5
typedef IntegrationJobDataRecordGroupBase<DRHDF5> IntegJobDRHDF5;
#endif

} // namespace Trick

#ifdef SWIG
%feature("compactdefaultargs","1") ;
#endif

#endif /* INTEGRATIONJOBDATARECORDGROUP_HH */
