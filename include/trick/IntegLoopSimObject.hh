/*
    PURPOSE:
        (Trick Integration sim object)
*/

#ifndef INTEG_LOOP_SIMOBJECT_HH
#define INTEG_LOOP_SIMOBJECT_HH

#include "trick/IntegLoopScheduler.hh"
#include "trick/Integrator.hh"
#include "trick/SimObject.hh"
#include "trick/JobData.hh"

#ifdef SWIG
// SWIG macros are not normally run on classes in sim_services.  We can add the the processed code manually
%trick_swig_class_typemap(IntegLoopSimObject, IntegLoopSimObject)
#endif

class IntegLoopSimObject : public Trick::SimObject {

    public:
        Trick::IntegLoopScheduler integ_sched ;  // trick_io(*io)

        IntegLoopSimObject() : integ_sched(0.01, this) {
            add_jobs(0.01, 0) ;
        }

        IntegLoopSimObject(double in_cycle, unsigned int child, Trick::SimObject* s_obj, ... ) : integ_sched(in_cycle, this) {
            va_list ap ;
            Trick::SimObject* next_sobj;

            va_start(ap, s_obj);
            next_sobj = s_obj;
            while (next_sobj != (Trick::SimObject*)NULL) {
                integ_sched.add_sim_object( *next_sobj );
                next_sobj = va_arg(ap, Trick::SimObject*);
            };

            add_jobs(in_cycle, child) ;
        }

        // Adds common set of jobs for all constructors.
        void add_jobs(double in_cycle, unsigned int child) ;

        virtual int call_function( Trick::JobData * curr_job ) ;
        virtual double call_function_double( Trick::JobData * curr_job ) ;

        Trick::Integrator * getIntegrator( Integrator_type Alg, unsigned int State_size ) {
            return integ_sched.getIntegrator(Alg , State_size) ;
        }

        int set_integ_cycle( double in_cycle ) {
            return integ_sched.set_integ_cycle(in_cycle) ;
        }
} ;

#ifdef SWIG
// SWIG macros are not normally run on classes in sim_services.  We can add the the processed code manually
%trick_cast_as(IntegLoopSimObject, IntegLoopSimObject)
#endif

#endif
