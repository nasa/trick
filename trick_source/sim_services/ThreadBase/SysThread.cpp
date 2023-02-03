
#include <iostream>
#include <sstream>
#include <stdio.h>
#if __linux
#include <sys/syscall.h>
#include <sys/types.h>
#include <sched.h>
#endif
#include <signal.h>
#include <algorithm>

#include "trick/SysThread.hh"


// Construct On First Use to avoid the Static Initialization Fiasco
pthread_mutex_t& Trick::SysThread::list_mutex() {
    static pthread_mutex_t list_mutex = PTHREAD_MUTEX_INITIALIZER;
    return list_mutex;
} 

std::vector<Trick::SysThread *>& Trick::SysThread::all_sys_threads() {
    static std::vector<SysThread *> all_sys_threads;
    return all_sys_threads;
}

Trick::SysThread::SysThread(std::string in_name) : ThreadBase(in_name) {
    pthread_mutex_lock(&(list_mutex()));
    all_sys_threads().push_back(this);
    pthread_mutex_unlock(&(list_mutex()));
}

Trick::SysThread::~SysThread() {
    pthread_mutex_lock(&(list_mutex()));
    // TODO: would some kind of smart pointer be a better idea here?
    all_sys_threads().erase(std::remove(all_sys_threads().begin(), all_sys_threads().end(), this), all_sys_threads().end());
    pthread_mutex_unlock(&(list_mutex()));
}



int Trick::SysThread::safeShutdown() {
    pthread_mutex_lock(&(list_mutex()));

    for (SysThread * thread : all_sys_threads()) {
        thread->cancel_thread();
    }

    for (SysThread * thread : all_sys_threads()) {
        thread->join_thread();
    }

    pthread_mutex_unlock(&(list_mutex()));

    return 0;
}

// int Trick::SysThread::create_thread() {

//     pthread_attr_t attr;

//     pthread_attr_init(&attr);
//     pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);
//     pthread_create(&pthread_id, &attr, Trick::ThreadBase::thread_helper , (void *)this);

// #if __linux
// #ifdef __GNUC__
// #if __GNUC__ >= 4 && __GNUC_MINOR__ >= 2
//     if ( ! name.empty() ) {
//        std::string short_str = name.substr(0,15) ;
//        pthread_setname_np(pthread_id, short_str.c_str()) ;
//     }
// #endif
// #endif
// #endif
//     return(0) ;
// }

// void * Trick::SysThread::thread_helper( void * context ) {

//     sigset_t sigs;
//     Trick::ThreadBase * tb = (Trick::ThreadBase *)context ;

//     /* block out all signals on this thread */
//     sigfillset(&sigs);
//     pthread_sigmask(SIG_BLOCK, &sigs, NULL);

//     /* Set the cancel type to deffered, the thread will be cancelled at next cancellation point */
//     pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);

//     tb->set_pid() ;

//     /* Set thread priority and CPU affinity */
//     tb->execute_priority() ;
//     tb->execute_cpu_affinity() ;
//     return tb->thread_body() ;
// }