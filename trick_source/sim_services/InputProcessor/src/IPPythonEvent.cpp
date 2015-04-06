/*
   PURPOSE: ( Python input processor event capability )
   REFERENCE: ( Trick Simulation Environment )
   ASSUMPTIONS AND LIMITATIONS: ( None )
   CLASS: ( N/A )
   LIBRARY DEPENDENCY: ( None )
   PROGRAMMERS: ( Alex Lin NASA 2009 )
*/

#include <iostream>
#include <string>
#include <stdlib.h>
#include <string.h>

#include "sim_services/InputProcessor/include/IPPythonEvent.hh"
#include "sim_services/InputProcessor/include/IPPython.hh"
#include "sim_services/MemoryManager/include/MemoryManager.hh"
#include "sim_services/Executive/include/exec_proto.h"
#include "sim_services/Message/include/message_proto.h"
#include "sim_services/Message/include/message_type.h"
#include "sim_services/MemoryManager/include/memorymanager_c_intf.h"
#include "sim_services/Executive/include/exec_proto.hh"

/* Global singleton pointer to the memory manager */
//TODO Use external MM interface
extern Trick::MemoryManager * trick_MM ;

/* static variables in the IPPythonEvent class */
Trick::IPPython * Trick::IPPythonEvent::ip ;
Trick::MTV * Trick::IPPythonEvent::mtv ;
bool Trick::IPPythonEvent::info_msg = false ;

Trick::condition_t::condition_t() {
    enabled = 0 ;
    hold = 0 ;
    fired = 0 ;
    fired_count = 0 ;
    fired_time = -1.0 ;
    ref = NULL ;
    job = NULL ;
}

Trick::action_t::action_t() {
    enabled = 0 ;
    ran = 0 ;
    ran_count = 0 ;
    ran_time = -1.0 ;
    job = NULL ;
    act_type = 0 ;
}

//Constructor
Trick::IPPythonEvent::IPPythonEvent() {


    is_user_event = true ;
    manual = false ;
    manual_fired = false ;
    condition_count = 0 ;
    cond_all = false ;
    action_count = 0 ;
    fired_count = 0 ;
    fired_time = -1.0 ;
    ran_count = 0 ;
    ran_time = -1.0 ;
    fired = false ;
    hold = false ;
    ran = false ;
    act = NULL;
    cond = NULL;
}

Trick::IPPythonEvent::~IPPythonEvent() {
}

void Trick::IPPythonEvent::set_python_processor(Trick::IPPython * in_ip) {
    ip = in_ip ;
}

void Trick::IPPythonEvent::set_mtv(Trick::MTV * in_mtv) {
    mtv = in_mtv ;
}

//Command to make event evaluation require that ALL of event's conditions be true to make action(s) run.
int Trick::IPPythonEvent::condition_all() {
    cond_all = true ;
    return(0);
}

//Command to make event evaluation require that ANY of event's conditions be true to make action(s) run (default).
int Trick::IPPythonEvent::condition_any() {
    cond_all = false ;
    return(0);
}

//Command to manually fire the event next cycle and hold it fired (enter manual mode, bypasses normal condition processing).
void Trick::IPPythonEvent::manual_on() {
    manual = true ;
    manual_fired = true ;
    hold = true ;
    fired = false ;
}

//Command to manually fire the event once NOW (enter manual mode, bypasses normal condition processing).
void Trick::IPPythonEvent::manual_fire() {
    manual = true ;
    manual_fired = true ;
    hold = false ;
    fired = false ;
    process_user_event(exec_get_time_tics()) ;
}

//Command to manually set the event as not fired (enter manual mode, bypasses normal condition processing).
void Trick::IPPythonEvent::manual_off() {
    manual = true ;
    manual_fired = false ;
    hold = false ;
    fired = false ;
}

//Command to return to normal event processing (needed to end manual mode after any manual commands).
void Trick::IPPythonEvent::manual_done() {
    manual = false ;
    manual_fired = false ;
    hold = false ;
}

void Trick::IPPythonEvent::add() {
    added = true ;
}

void Trick::IPPythonEvent::remove() {
    added = false ;
}

// Command to turn on info messages
void Trick::IPPythonEvent::set_event_info_msg_on() {
    info_msg = true;
}

// Command to turn off info messages
void Trick::IPPythonEvent::set_event_info_msg_off() {
    info_msg = false;
}

void Trick::IPPythonEvent::restart() {
    int jj ;

    for (jj=0; jj<condition_count; jj++) {
        if (cond[jj].cond_type==1) { // condition variable
            cond[jj].ref = ref_attributes((char*)cond[jj].str.c_str());
        }
        if (cond[jj].cond_type==2) { // condition job
            cond[jj].job = exec_get_job(cond[jj].str.c_str(),1);
        }
    }
    for (jj=0; jj<action_count; jj++) {
        if (act[jj].act_type!=0) { // action job
            act[jj].job = exec_get_job(act[jj].str.c_str(),1);
        }
    }

}

//Command to create a new condition using a model variable (or reset an existing condition variable), num is index starting at 0.
int Trick::IPPythonEvent::condition_var(int num, std::string varname, std::string comment) {
    /** @par Detailed Design: */
    /** @li Find the variable reference for the varname string and pass it to condition() */ 
    REF2* ref = ref_attributes((char*)varname.c_str());
    /** @li Emit an error if specified varname does not exist. */
    if (ref==NULL) {
        message_publish(MSG_WARNING, "Event condition variable %s not found. Setting condition to False.\n", varname.c_str()) ;
        condition(num, "False", comment, NULL, NULL );
    } else {
        condition(num, varname, comment, ref, NULL );
    }
    return(0);
}

//Command to create a new condition using a model job (or reset an existing condition job), num is index starting at 0.
int Trick::IPPythonEvent::condition_job(int num, std::string jobname, std::string comment) {
    /** @par Detailed Design: */
    /** @li Find the job for the jobname string and pass it to condition() */ 
    JobData *job = exec_get_job(jobname.c_str(),1);
    /** @li Emit an error if specified jobname does not exist. */
    if (job==NULL) {
        message_publish(MSG_WARNING, "Event condition job %s not found. Setting condition to False.\n", jobname.c_str()) ;
        condition(num, "False", comment, NULL, NULL );
    } else {
        if (! job->job_class_name.compare("malfunction")) {
            // enable it if it's a malf job because they are not in any queue
            job->disabled = false;
        }
        condition(num, jobname, comment, NULL, job );
    }
    return(0);
}

//Command to create a new condition and set its input string (or reset an existing condition string), num is index starting at 0.
int Trick::IPPythonEvent::condition(int num, std::string str, std::string comment, REF2* ref, JobData* job) {
    /** @par Detailed Design: */

    if (num == condition_count) {
        /** @li Add a new condition when num is sequential, i.e. it is equal to condition_count */ 
        condition_count++;
        if (condition_count == 1) {
            cond = (Trick::condition_t *)TMM_declare_var_s("Trick::condition_t[1]");
        } else {
            cond = (Trick::condition_t *)TMM_resize_array_1d_a(cond, condition_count);
        }
        cond[num].fired_count = 0;
        cond[num].fired_time = -1.0;
    }
    if ((num >=0) && (num < condition_count)) {
        /** @li This is either a new condition or user is changing the condition. */
        /** @li Initialize condition variables - default as enabled. */
        cond[num].ref = ref ;
        cond[num].job = job ;
        cond[num].enabled = true;
        cond[num].hold = false;
        cond[num].fired = false;
        if (ref != NULL) {
            cond[num].cond_type = 1;
        } else if (job != NULL) {
            cond[num].cond_type = 2;
        } else cond[num].cond_type = 0;
        cond[num].str = str;
        // comment is for display in mtv, if not supplied create a comment containing up to 50 characters of cond string
        if (comment.empty()) {
            cond[num].comment = str.substr(0,50);
        } else {
            cond[num].comment = comment;
        }
        // dummy must contain max conditions used in any event so it can replace any event in mtv when deleted
#if 0
        if (num == ip->dummy_event.condition_count) {
            ip->dummy_event.condition(num, "XXX_DELETED_COND");
        }
#endif
    } else {
        /** @li Emit an error if specified index num is not sequential. */
        message_publish(MSG_WARNING, "Event condition not added: condition number %d is not sequential.\n", num) ;
    }
    return(0);
}

//Command to set an existing condition to hold, so that when it fires it stays in the fired state.
int Trick::IPPythonEvent::condition_hold_on(int num) {

    if ((num >=0) && (num < condition_count)) {
        cond[num].hold = true ;
    } else {
        message_publish(MSG_WARNING, "Event condition hold not set. Condition number %d is invalid.\n", num) ;
    }
    return(0);
}

//Command to set an existing condition to not hold its fired state.
int Trick::IPPythonEvent::condition_hold_off(int num) {

    if ((num >=0) && (num < condition_count)) {
        cond[num].hold = false ;
    } else {
        message_publish(MSG_WARNING, "Event condition hold not set. Condition number %d is invalid.\n", num) ;
    }
    return(0);
}

//Command to enable an existing condition (default is enabled) so that it is evaluated during event processing.
int Trick::IPPythonEvent::condition_enable(int num) {

    if ((num >=0) && (num < condition_count)) {
        cond[num].enabled = true ;
    } else {
        message_publish(MSG_WARNING, "Event condition not enabled. Condition number %d is invalid.\n", num) ;
    }
    return(0);
}

//Command to disable an existing condition so that it is not evaluated during event processing.
int Trick::IPPythonEvent::condition_disable(int num) {

    if ((num >=0) && (num < condition_count)) {
        cond[num].enabled = false ;
    } else {
        message_publish(MSG_WARNING, "Event condition not disabled. Condition number %d is invalid.\n", num) ;
    }
    return(0);
}

//Accessor function to test if the condition, indicated by num, fired (is currently in the fired state).
bool Trick::IPPythonEvent::condition_fired(int num) {

    if ((num >=0) && (num < condition_count)) {
        return (cond[num].fired ? true : false) ;
    } else {
        message_publish(MSG_WARNING, "Event condition fired state not returned. Condition number %d is invalid.\n", num) ;
    }
    return(false);
}

//Accessor function to see how many times the condition, indicated by num, fired.
int Trick::IPPythonEvent::condition_fired_count(int num) {

    if ((num >=0) && (num < condition_count)) {
        return (cond[num].fired_count) ;
    } else {
        message_publish(MSG_WARNING, "Event condition fired count not returned. Condition number %d is invalid.\n", num) ;
    }
    return(0) ;
}

//Accessor function to see when was the last time the condition, indicated by num, fired.
double Trick::IPPythonEvent::condition_fired_time(int num) {

    if ((num >=0) && (num < condition_count)) {
        return (cond[num].fired_time) ;
    } else {
        message_publish(MSG_WARNING, "Event condition fired time not returned. Condition number %d is invalid.\n", num) ;
    }
    return(0.0) ;
}

//Accessor function to return the condition string, indicated by num.
std::string Trick::IPPythonEvent::condition_string(int num) {

    if ((num >=0) && (num < condition_count)) {
        return (cond[num].str) ;
    } else {
        message_publish(MSG_WARNING, "Event condition string not returned. Condition number %d is invalid.\n", num) ;
    }
    return "" ;
}

//Command to create a new action using a model job (or reset an existing action job), num is index starting at 0.
int Trick::IPPythonEvent::action_job(int num, std::string jobname, std::string comment) {
    /** @par Detailed Design: */
    /** @li Find the job for the jobname string and pass it to action() */
    JobData *job = exec_get_job(jobname.c_str(),1);
    /** @li Emit an error if specified jobname does not exist. */
    if (job==NULL) {
        message_publish(MSG_WARNING, "Event action job %s not found. No action was added.\n", jobname.c_str()) ;
    } else {
        if (! job->job_class_name.compare("malfunction")) {
            // enable it if it's a malf job because they are not in any queue
            job->disabled = false;
        }
        action(num, jobname, comment, job, 3 );
    }
    return(0);
}

//Command to create a new action to turn a model job ON, num is index starting at 0.
int Trick::IPPythonEvent::action_job_on(int num, std::string jobname, std::string comment) {
    /** @par Detailed Design: */
    /** @li Find the job for the jobname string and pass it to action() */ 
    JobData *job = exec_get_job(jobname.c_str(),1);
    /** @li Emit an error if specified jobname does not exist. */
    if (job==NULL) {
        message_publish(MSG_WARNING, "Event action job %s not found. No action was added.\n", jobname.c_str()) ;
    } else {
        action(num, jobname, comment, job, 1 );
    }
    return(0);
}

//Command to create a new action to turn a model job OFF, num is index starting at 0.
int Trick::IPPythonEvent::action_job_off(int num, std::string jobname, std::string comment) {
    /** @par Detailed Design: */
    /** @li Find the job for the jobname string and pass it to action() */ 
    JobData *job = exec_get_job(jobname.c_str(),1);
    /** @li Emit an error if specified jobname does not exist. */
    if (job==NULL) {
        message_publish(MSG_WARNING, "Event action job %s not found. No action was added.\n", jobname.c_str()) ;
    } else {
        action(num, jobname, comment, job, 2 );
    }
    return(0);
}

//Command to create a new action and set its input string (or reset an existing action string), num is index starting at 0.
int Trick::IPPythonEvent::action(int num, std::string str, std::string comment, JobData *job, int act_type) {
    /** @par Detailed Design: */
 
    if (num == action_count) {
        /** @li Add a new action when num is sequential, i.e. it is equal to action_count */ 
        action_count++;
        if (action_count == 1) {
            act = (Trick::action_t *)TMM_declare_var_s("Trick::action_t[1]");
        } else {
            act = (Trick::action_t *)TMM_resize_array_1d_a(act, action_count);
        }
        act[num].ran_count = 0;
        act[num].ran_time = -1.0;
    }
    if ((num >=0) && (num < action_count)) {
        /** @li This is either a new action or user is changing the action. */
        /** @li Initialize action variables - default as enabled. */
        act[num].job = job ;
        act[num].act_type = act_type ;
        act[num].enabled = true;
        act[num].ran = false;
        act[num].str = str;
        // comment is for display in mtv, if not supplied create a comment containing up to 50 characters of act string
        if (comment.empty()) {
            act[num].comment = str.substr(0,50);
        } else {
            act[num].comment = comment;
        }
        // dummy must contain max actions used in any event so it can replace any event in mtv when deleted
#if 0
        if (num == ip->dummy_event.action_count) {
            ip->dummy_event.action(num, "XXX_DELETED_ACT");
        }
#endif
    } else {
        /** @li Emit an error if specified index num is not sequential. */
        message_publish(MSG_WARNING, "Event action not added: action number %d is not sequential.\n", num) ;
    }
    return(0);
}

//Command to enable an existing action (default is enabled) so that it is run when fired during event processing.
int Trick::IPPythonEvent::action_enable(int num) {

    if ((num >=0) && (num < action_count)) {
        act[num].enabled = true ;
    } else {
        message_publish(MSG_WARNING, "Event action not enabled. Action number %d is invalid.\n", num) ;
    }
    return(0);
}

//Command to disable an existing action so that it is not run when fired during event processing.
int Trick::IPPythonEvent::action_disable(int num) {

    if ((num >=0) && (num < action_count)) {
        act[num].enabled = false ;
    } else {
        message_publish(MSG_WARNING, "Event action not disabled. Action number %d is invalid.\n", num) ;
    }
    return(0);
}

//Accessor function to test if the action, indicated by num, ran.
bool Trick::IPPythonEvent::action_ran(int num) {

    if ((num >=0) && (num < action_count)) {
        return (act[num].ran ? true : false) ;
    } else {
        message_publish(MSG_WARNING, "Event action ran state not returned. Action number %d is invalid.\n", num) ;
    }
    return(false);
}

//Accessor function to see how many times the action, indicated by num, ran.
int Trick::IPPythonEvent::action_ran_count(int num) {

    if ((num >=0) && (num < action_count)) {
        return (act[num].ran_count) ;
    } else {
        message_publish(MSG_WARNING, "Event action ran count not returned. Action number %d is invalid.\n", num) ;
    }
    return(0) ;
}

//Accessor function to see when was the last time the action, indicated by num, ran.
double Trick::IPPythonEvent::action_ran_time(int num) {

    if ((num >=0) && (num < action_count)) {
        return (act[num].ran_time) ;
    } else {
        message_publish(MSG_WARNING, "Event action ran time not returned. Action number %d is invalid.\n", num) ;
    }
    return(0.0) ;
}

int Trick::IPPythonEvent::process( long long curr_time ) {

    if ( active || manual_fired ) {
            /** @li If it's a user's input event, process it by calling Trick::IPPython::process_triggers. */
            if ( is_user_event ) {
                // it's an input file event
                process_user_event(curr_time) ;
            /** @li Otherwise if it's a trick event (like a read), process it. */
            } else {
                // it's a read event
                active = false ;
                ip->parse(act[0].str) ;
                // keep stats so mtv will show when it ran
                fired_count++ ;
                fired_time = curr_time ;
                ran = true ;
                ran_count++ ;
                ran_time = curr_time ;
                act[0].ran = true ;
                act[0].ran_count++ ;
                act[0].ran_time = curr_time ;
            }
    }
    return 0 ;
}

bool Trick::IPPythonEvent::process_user_event( long long curr_time ) {

    int ii ;
    int return_val ;
    bool it_fired, it_ran;

    fired = false ;
    ran = false ;
    /** @li No need to evaluate any conditions if in manual mode. */
    if (! manual) {
        hold = false ;
        /** @li Loop thru all conditions. */
        for (ii=0; ii<condition_count; ii++) {
            /** @li Skip condition if it's been disabled. */
            if (! cond[ii].enabled ) {
                cond[ii].fired = false ;
                continue ;
            }
            /** @li No need to evaluate condition if previously fired and hold is on. */
            if (cond[ii].hold && cond[ii].fired) {
                ;
            } else {
                /** @li Evaluate the condition and set its fired state. */
                cond[ii].fired = false ;
                return_val = 0 ;
                if (cond[ii].ref != NULL) {
                // if it's a variable, get it as a boolean
                    if ( cond[ii].ref->pointer_present ) {
                        cond[ii].ref->address = follow_address_path(cond[ii].ref) ;
                    }
                    if ( cond[ii].ref->address != NULL ) {
                        return_val = *(bool *)cond[ii].ref->address ;
                    }
                } else if (cond[ii].job != NULL) {
                // if it's a job, get its return value
                    bool save_disabled_state = cond[ii].job->disabled;
                    cond[ii].job->disabled = false;
                    return_val = cond[ii].job->call();
                    cond[ii].job->disabled = save_disabled_state;
                } else {
                // otherwise use python to evaluate string
                    std::string full_in_string ;
                    ip->parse_condition(cond[ii].str, return_val) ;
                }
                if (return_val) {
                //TODO: write to log/send_hs that trigger fired
                    cond[ii].fired = true ;
                    cond[ii].fired_count++ ;
                    cond[ii].fired_time = curr_time ;
                }
            } // end evaluate condition
            /** @li If cond_all is true, only set event fired/hold after all enabled conditions evaluated. */
            if (ii==0) {
                fired = cond[ii].fired ;
                hold  = cond[ii].hold ;
            }  else {
                if (cond_all) {
                    fired &= cond[ii].fired ;
                    hold  &= cond[ii].hold ;
                } else {
                    fired |= cond[ii].fired ;
                    hold  |= cond[ii].hold ;
                }
            }
        } //end condition loop
    }
    it_fired = manual_fired || fired ;
    /** @li Set the event's fired state...cond_all: if all conditions fired , otherwise if any condition fired. */
    if (it_fired) {
        fired_count++ ;
        fired_time = curr_time ;
        if (info_msg) {
            message_publish( MSG_INFO , "%12.6f Event %s fired.\n" , exec_get_sim_time() , name.c_str()) ;
        }
        if (!manual_fired) {
            active = false ;
        }
    }

    /** @li If the user specified any actions, run them here if a condition fired or manually fired. */
    it_ran = false ;
    if (it_fired && (action_count > 0)) {
        /** @li Loop thru all actions. */
        for (ii=0; ii<action_count; ii++) {
            /** @li No need to run action if it's been disabled. */
            if (! act[ii].enabled ) {
                act[ii].ran = false ;
                continue ;
            }
            /** @li Run the action and set its ran state. */
            if (act[ii].job != NULL) {
            // if it's a job, do what the action type tells you
                switch (act[ii].act_type) {
                    case 0 : // python, should not get here
                        break;
                    case 1 : // On
                        act[ii].job->disabled = false;
                        break;
                    case 2 : // Off
                        act[ii].job->disabled = true;
                        break;
                    case 3 : // Call
                        bool save_disabled_state = act[ii].job->disabled;
                        act[ii].job->disabled = false;
                        act[ii].job->call();
                        act[ii].job->disabled = save_disabled_state;
                        break;
                }
            } else {
            // otherwise use python to evaluate string
                ip->parse(act[ii].str) ;
            }
            it_ran = true ;
            act[ii].ran = true ;
            act[ii].ran_count++ ;
            act[ii].ran_time = curr_time ;
            ran = true ;
        }
        /** @li Leave event fired state on if hold is on. */
        manual_fired &= hold ;
        fired &= hold ;
    }
    /** @li Set the event's ran state if any actions were run. */
    if (it_ran) {
        ran_count++ ;
        ran_time = curr_time ;
    }

    /** @li Return true if the event fired. */
    return(it_fired) ;
}

