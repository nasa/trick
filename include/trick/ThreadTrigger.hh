/*
    PURPOSE:
        (Trick Thread trigger implementation)
*/

#ifndef THREADTRIGGER_HH
#define THREADTRIGGER_HH

#include <iostream>
#include <pthread.h>

namespace Trick {

    enum ThreadTriggerType {
        TT_MUTEX,
        TT_FLAG,
        TT_EVENTFD,
        TT_FUTEX
    } ;

    class ThreadTriggerBase {
        public:
            ThreadTriggerType tt_type ;
            ThreadTriggerBase(ThreadTriggerType in_tt_type) ;
            virtual ~ThreadTriggerBase() ;
            virtual void init() ;
            virtual void fire() = 0 ;
            virtual void wait() = 0 ;
            virtual void dump( std::ostream & oss ) = 0 ;
    } ;

    class ThreadTriggerMutex : public Trick::ThreadTriggerBase {
        public:
            ThreadTriggerMutex() ;
            virtual void init() ;
            virtual void fire() ;
            virtual void wait() ;
            virtual void dump( std::ostream & oss ) ;
        protected:
            /** Condition variable to control start of thread processing */
            pthread_cond_t go_cv;           /**< trick_io(**) */
            /** Mutex to control start of thread processing */
            pthread_mutex_t go_mutex;       /**< trick_io(**) */
    } ;

    class ThreadTriggerFlag : public Trick::ThreadTriggerBase {
        public:
            ThreadTriggerFlag() ;
            virtual void fire() ;
            virtual void wait() ;
            virtual void dump( std::ostream & oss ) ;
        protected:
            /** Trigger from master to start thread processing */
            volatile bool frame_trigger;              /**< trick_io(**) */
    } ;

    class ThreadTriggerEventFD : public Trick::ThreadTriggerBase {
        public:
            ThreadTriggerEventFD() ;
            virtual void fire() ;
            virtual void wait() ;
            virtual void dump( std::ostream & oss ) ;
        protected:
            /** condition variable to control start of thread using eventfd */
            int efd;            /**< trick_io(**) */
    } ;

    class ThreadTriggerFutex : public Trick::ThreadTriggerBase {
        public:
            ThreadTriggerFutex() ;
            virtual void fire() ;
            virtual void wait() ;
            virtual void dump( std::ostream & oss ) ;
        protected:
            /** condition variable for futex */
            int futex_addr; /**< trick_io(**) */
    } ;

    /* Container to hold all of the trigger types.  This avoids having to
       allocate each trigger type when changing from one type to another */
    class ThreadTriggerContainer {

        public:
            ThreadTriggerContainer() ;
            ThreadTriggerBase * getThreadTrigger() ;
            void setThreadTrigger( ThreadTriggerType trigger_type ) ;

        protected:
            ThreadTriggerType trigger_type ;
            ThreadTriggerMutex ttMutex ;
            ThreadTriggerFlag ttFlag ;
            ThreadTriggerEventFD ttEventFD ;
            ThreadTriggerFutex ttFutex ;

            ThreadTriggerBase * ttBase ;

    } ;
}
#endif
