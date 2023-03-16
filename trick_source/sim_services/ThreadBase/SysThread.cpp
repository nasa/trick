
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

int Trick::SysThread::shutdown_timeout = 10;
int Trick::SysThread::max_shutdown_tries = 3;

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

    // Cancel all threads
    for (SysThread * thread : all_sys_threads()) {
        thread->cancel_thread();
    }

    // Join all threads
    auto it = all_sys_threads().begin();
    while (it != all_sys_threads().end()){
        SysThread * thread = *it;

        // If a thread is marked as self deleting, deletes it's own thread info so it has to go through the SysThread destructor to finish
        // So we can't join here since we're holding the lock
        if (!(thread->self_deleting)) {
            std::cout << "Waiting for " << thread->name << " to join" << std::endl;
            thread->join_thread();
            std::cout << "Thread " << thread->name << " successfully joined" << std::endl;
            it = all_sys_threads().erase(it);
        } else {
            ++it;
        }
    }

    // Wait for the self_deleting threads to finish
    int tries = max_shutdown_tries;
    while (all_sys_threads().size() != 0 && tries-- > 0) {
        struct timespec ts;
        clock_gettime(CLOCK_REALTIME, &ts);
        ts.tv_sec += shutdown_timeout;
        pthread_cond_timedwait(&(list_empty_cv()), &(list_mutex()), &ts);
    }

    // Check if we managed to shut everything down
    if (all_sys_threads().size() != 0) {
        std::cout << "Shutdown timed out - waiting on threads [ ";
        for (SysThread * thread : all_sys_threads()) {
            std::cout << thread->name << " ";
        }
        std::cout << "]" << std::endl;

        // We're already in shutdown and failed to shutdown cleanly, so just shut it all down here
        pthread_mutex_unlock(&(list_mutex()));
        exit(-1);
    }

    // Success!
    std::cout << "Threading shutdown finished" << std::endl;
    shutdown_finished = true;
    pthread_mutex_unlock(&(list_mutex()));

    return 0;
}