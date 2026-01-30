/*************************************************************************
PURPOSE: (Starter class)
LIBRARY DEPENDENCY:
    (
    (starter.cpp)
    )
**************************************************************************/

#include <string>

class Starter
{
public:
    Starter() {}

    void sched();
    void custom();
    void top();
    void end();
    void automatic();
    void automatic_last();
    void deriv();
    int integ();
    void test_calls();

    int sched_calls = 0;
    int custom_calls = 0;
    int top_calls = 0;
    int automatic_calls = 0;
    int automatic_last_calls = 0;
    int deriv_calls = 0;
    int integ_calls = 0;
    int end_calls = 0;
};
