#include "trick/IntegratorVectorIntf.hh"
#include "trick/integrator_c_intf.h"
#include "trick/Integrator.hh"
#include "trick/message_proto.h"
#include "trick/message_type.h"
#include <vector>

extern Trick::Integrator* trick_curr_integ;

static bool check_integ_size(const char* func_name, const std::vector<double>& vec)
{
    if (trick_curr_integ == NULL)
    {
        message_publish(MSG_ERROR, "Integ %s ERROR: trick_curr_integ is not set.\n", func_name);
        return false;
    }
    if ((int)vec.size() != trick_curr_integ->num_state)
    {
        message_publish(MSG_ERROR,
            "Integ %s ERROR: vector size %zu does not match num_state %d.\n",
            func_name, vec.size(), trick_curr_integ->num_state);
        return false;
    }
    return true;
}

void load_state_vec(const std::vector<double>& states)
{
    if (!check_integ_size("load_state_vec", states)) return;
    for (unsigned int i = 0; i < states.size(); ++i)
    {
        load_indexed_state(i, states[i]);
    }
}

void load_deriv_vec(const std::vector<double>& derivs)
{
    if (!check_integ_size("load_deriv_vec", derivs)) return;
    for (unsigned int i = 0; i < derivs.size(); ++i)
    {
        load_indexed_deriv(i, derivs[i]);
    }
}

void load_deriv2_vec(const std::vector<double>& derivs2)
{
    if (!check_integ_size("load_deriv2_vec", derivs2)) return;
    for (unsigned int i = 0; i < derivs2.size(); ++i)
    {
        load_indexed_deriv2(i, derivs2[i]);
    }
}

void unload_state_vec(std::vector<double>& states)
{
    if (!check_integ_size("unload_state_vec", states)) return;
    for (unsigned int i = 0; i < states.size(); ++i)
    {
        states[i] = unload_indexed_state(i);
    }
}
