
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
#include <time.h>

#include "trick/SysThread.hh"

bool Trick::SysThread::shutdown_finished = false;


// Construct On First Use to avoid the Static Initialization Fiasco
pthread_mutex_t& Trick::SysThread::list_mutex() {
    static pthread_mutex_t list_mutex = PTHREAD_MUTEX_INITIALIZER;
    return list_mutex;
} 

pthread_cond_t& Trick::SysThread::list_empty_cv() {
    static pthread_cond_t list_empty_cv = PTHREAD_COND_INITIALIZER;
    return list_empty_cv;
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
    if (!shutdown_finished) {
        all_sys_threads().erase(std::remove(all_sys_threads().begin(), all_sys_threads().end(), this), all_sys_threads().end());
    }
    pthread_mutex_unlock(&(list_mutex()));
}

int Trick::SysThread::ensureAllShutdown() {
    pthread_mutex_lock(&(list_mutex()));

    // Cancel all threads
    for (SysThread * thread : all_sys_threads()) {
        thread->cancel_thread();
    }

    // Join all threads
    for (SysThread * thread : all_sys_threads()) {
        thread->join_thread();
    }

    // Success!
    shutdown_finished = true;
    pthread_mutex_unlock(&(list_mutex()));

    return 0;
}