#include "thread.hh"
#include <iostream>
#include <string>
using std::cerr;
using std::endl;

Thread::Thread(const std::string& name):_state(THREAD_NONE), _name(name) 
{}

Thread::Thread(Thread_Function fn, void* arg, const std::string& name): 
                                            _state(THREAD_NONE) 
{ 
    Start(fn, arg);
} 

void Thread::Start(Thread_Function fn, void* arg) { 
#ifndef WIN32
    pthread_create(&_handle.handle, NULL, fn, arg);
#else
    _handle.handle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) fn, 
                                  arg, 0, NULL);
#endif

    if (!_handle.handle) {
        _state = THREAD_ERROR;
        throw Exception("Unable to start thread");
    } else {
        _state = THREAD_STARTED;
    }
}

void Thread::Join()  { 
#ifndef WIN32
    pthread_join(_handle.handle, NULL); 
#else
    DWORD ret = WaitForSingleObject(_handle.handle, INFINITE);
    if (ret != WAIT_OBJECT_0) {
        cerr << "Thread ERROR with code " << ret 
             << " Error: " << GetLastError() << endl; 
    } else {
        cerr << "Thread joined successfully" << endl;
    }
#endif
    _handle.handle = 0;
    _state = THREAD_DEAD;
}

