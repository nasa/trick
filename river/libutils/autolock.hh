#ifndef AUTOLOCK_HH
#define AUTOLOCK_HH

#include "mutex.hh"

/** \ingroup libtc
 * AutoLock class intended to ensure that locking and unlocking of mutexes
 * is handled across exceptions/returns.
 */
class AutoLock {
  public:
    AutoLock(Mutex* mutex): _theMutex(mutex) {
        _theMutex->Lock();
    }
    ~AutoLock() {
        _theMutex->Unlock();
    }
  private:
    Mutex* _theMutex;
};
#endif
