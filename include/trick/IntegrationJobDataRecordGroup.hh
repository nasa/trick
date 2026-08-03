/*******************************************************************************

 PURPOSE:
 (DataRecordGroup class tailored to log all the intermediate_steps of the integration loop.)

 PROGRAMMERS:
 (((Thomas Brain) (Metecs) (Aug 2026) (--)))

 *******************************************************************************/

#ifndef INTEGRATIONJOBDATARECORDGROUP_HH
#define INTEGRATIONJOBDATARECORDGROUP_HH

#include "trick/DRAscii.hh"
#include "trick/DRBinary.hh"
#include "trick/DRHDF5.hh"
#include "trick/IntegLoopScheduler.hh"

#ifdef SWIG
%feature("compactdefaultargs","0") ;
%feature("shadow") Trick::IntegJobDRBinary::IntegJobDRBinary(const std::string& in_name, Trick::IntegLoopScheduler& integSchedulerRefIn) %{
    def __init__(self, *args):
        this = $action(*args)
        try: self.this.append(this)
        except: self.this = this
        this.own(0)
        self.this.own(0)
%}

%feature("shadow") Trick::IntegJobDRAscii::IntegJobDRAscii(const std::string& in_name, Trick::IntegLoopScheduler& integSchedulerRefIn) %{
    def __init__(self, *args):
        this = $action(*args)
        try: self.this.append(this)
        except: self.this = this
        this.own(0)
        self.this.own(0)
%}

%feature("shadow") Trick::IntegJobDRHDF5::IntegJobDRHDF5(const std::string& in_name, Trick::IntegLoopScheduler& integSchedulerRefIn) %{
    def __init__(self, *args):
        this = $action(*args)
        try: self.this.append(this)
        except: self.this = this
        this.own(0)
        self.this.own(0)
%}
#endif

namespace Trick
{

    class IntegJobDRBinary : public DRBinary
    {
        public:
            IntegJobDRBinary() = default;
            IntegJobDRBinary(const std::string& in_name, Trick::IntegLoopScheduler& integSchedulerRefIn);

            virtual int init(bool is_restart = false) override;
            virtual int data_record(double in_time) override;

            Trick::IntegLoopScheduler* integSchedulerPtr { };
    };

    class IntegJobDRAscii : public DRAscii
    {
        public:
            IntegJobDRAscii() = default;
            IntegJobDRAscii(const std::string& in_name, Trick::IntegLoopScheduler& integSchedulerRefIn);

            virtual int init(bool is_restart = false) override;
            virtual int data_record(double in_time) override;

            Trick::IntegLoopScheduler* integSchedulerPtr { };
    };

    class IntegJobDRHDF5 : public DRHDF5
    {
        public:
            IntegJobDRHDF5() = default;
            IntegJobDRHDF5(const std::string& in_name, Trick::IntegLoopScheduler& integSchedulerRefIn);

            virtual int init(bool is_restart = false) override;
            virtual int data_record(double in_time) override;

            Trick::IntegLoopScheduler* integSchedulerPtr { };
    };

} // namespace Trick

#ifdef SWIG
%feature("compactdefaultargs","1") ;
#endif

#endif /* INTEGRATIONJOBDATARECORDGROUP_HH */
