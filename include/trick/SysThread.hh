/*
    PURPOSE:
        (Trick Sys Threads implementation)
*/

#ifndef SYSTHREAD_HH
#define SYSTHREAD_HH

#include <stdio.h>
#include <pthread.h>
#include <iostream>
#include <string>
#include <vector>
#if __linux
#include <sys/types.h>
#endif
#include <unistd.h>
#include <sched.h>
#include "trick/ThreadBase.hh"


namespace Trick {

    /**
     * The purpose of this class is to ensure safe shutdown for Trick system threads, since user threads are handled separately in
     * the Trick::Threads and Executive classes.
     * 
     * This class was implemented as a solution to issue https://github.com/nasa/trick/issues/1445
     *
     * @author Jackie Deans
     *
     *      
     **/
    class SysThread : public Trick::ThreadBase {
        public:
            SysThread(std::string in_name);
            ~SysThread();

            static int ensureAllShutdown();

        private: 
            // Had to use Construct On First Use here to avoid the static initialziation fiasco
            static pthread_mutex_t& list_mutex();
            static pthread_cond_t& list_empty_cv();                 

            static std::vector <SysThread *>& all_sys_threads();

            static bool shutdown_finished;

    } ;

}

#endif

