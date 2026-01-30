/*************************************************************************
PURPOSE: (A sim object that is allocated at runtime)
LIBRARY DEPENDENCY:
    (
    )
**************************************************************************/

#include "sim_services/SimObject/include/SimObject.hh"

class DynamicObj : public Trick::SimObject
{
    // This class should be allocated dynamically
public:
    DynamicObj() {}

    int doSomething()
    {
        // Do something that we can test for here i guess
        return 0;
    }

    /**
     * Calls all jobs that are not "dynamic_event" class
     * @param curr_job - the current job instance
     * @return always 0
     */
    virtual int call_function(Trick::JobData * curr_job)
    {
        return 0;
    }

    /**
     * Calls all jobs that are "dynamic_event" class
     * @param curr_job - the current job instance
     * @return always 0
     */
    virtual double call_function_double(Trick::JobData * curr_job)
    {
        return 0;
    }

    int a;
    std::string b;
    double * c;
};
