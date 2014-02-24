#ifndef THREAD_HH
#define THREAD_HH

#include "dsp.h"
#include "exception.hh"
#include <iostream>
#include <string>
using std::cerr;
using std::endl;

class Thread {
  public:
    enum ThreadState {
        THREAD_ERROR = -2,
        THREAD_DEAD = -1,
        THREAD_NONE = 0,
        THREAD_STARTED = 1,
        THREAD_RUNNING = 2
    };

    struct Handle {
        Handle(): handle(0) {}
#ifdef WIN32
        HANDLE handle;
#else
        pthread_t handle;
#endif
    };

    typedef void* (*Thread_Function)(void*);

    Thread(const std::string& name="");
    Thread(Thread_Function fn, void* arg, const std::string& name);

    // Optional function for applications to set a "running" state beyond "star
    // The goal is to allow a thread to do initialization before indicating its
    void setState(Thread::ThreadState state) {
        _state = state;
    }

    void Start(Thread_Function fn, void* arg);

    bool isDead() {
        return (_state == THREAD_DEAD);
    }

    bool isError() {
        return (_state == THREAD_ERROR);
    }

    bool isRunning() {
        return (_state == THREAD_RUNNING);
    }

    bool isStarted() {
        return (_state == THREAD_STARTED || _state == THREAD_RUNNING);
    }

    void Join();

  private:
    Handle _handle;
    ThreadState _state;
    std::string _name;
};
#endif
