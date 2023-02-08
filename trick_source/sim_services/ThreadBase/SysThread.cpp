
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

Trick::SysThread::SysThread(std::string in_name, bool sd) : self_deleting(sd), ThreadBase(in_name) {
    pthread_mutex_lock(&(list_mutex()));
    all_sys_threads().push_back(this);
    pthread_mutex_unlock(&(list_mutex()));
}


Trick::SysThread::~SysThread() {
    pthread_mutex_lock(&(list_mutex()));
    if (!shutdown_finished) {
        all_sys_threads().erase(std::remove(all_sys_threads().begin(), all_sys_threads().end(), this), all_sys_threads().end());
        if (all_sys_threads().size() == 0) {
            pthread_cond_signal(&(list_empty_cv()));
        }
    }
    pthread_mutex_unlock(&(list_mutex()));
}

int Trick::SysThread::ensureAllShutdown() {
    pthread_mutex_lock(&(list_mutex()));

    for (SysThread * thread : all_sys_threads()) {
        thread->cancel_thread();
    }

    auto it = all_sys_threads().begin();
    while (it != all_sys_threads().end()){
        SysThread * thread = *it;
        if (!(thread->self_deleting)) {
            thread->join_thread();
            it = all_sys_threads().erase(it);
        } else {
            ++it;
        }
    }

    while (all_sys_threads().size() != 0) {
        pthread_cond_wait(&(list_empty_cv()), &(list_mutex()));
    }

    shutdown_finished = true;
    pthread_mutex_unlock(&(list_mutex()));

    return 0;
}