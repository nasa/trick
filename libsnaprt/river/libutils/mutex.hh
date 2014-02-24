#ifndef MUTEX_HH
#define MUTEX_HH

#ifndef WIN32
#include <pthread.h>
#else
#include <windows.h>
#endif

/** \ingroup libtc
 * AutoLock class intended to ensure that locking and unlocking of mutexes
 * is handled across exceptions/returns.
 */
class Mutex {
  public:
#ifndef WIN32
    Mutex() { pthread_mutex_init(&_theMutex, NULL); }
    ~Mutex() { pthread_mutex_destroy(&_theMutex); }
    void Lock() { pthread_mutex_lock(&_theMutex); }
    int Trylock() { return pthread_mutex_trylock(&_theMutex); }
    void Unlock() { pthread_mutex_unlock(&_theMutex); }
#else
    Mutex() { InitializeCriticalSection(&theSection); }
	~Mutex() { DeleteCriticalSection(&theSection); }
    void Lock() { EnterCriticalSection(&theSection); }
    // Windows "Try" function returns the opposite of pthread's, so we need
    // to negate the return value.
    int Trylock() { return (TryEnterCriticalSection(&theSection) ? 0 : 1); }
    void Unlock() { LeaveCriticalSection(&theSection); }
#endif
  private:
#ifndef WIN32
    pthread_mutex_t _theMutex;
#else
	// Windows does not like it if I call this variable _theSection
    CRITICAL_SECTION theSection;
#endif
};
#endif
