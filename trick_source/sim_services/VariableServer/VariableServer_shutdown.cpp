#include "trick/VariableServer.hh"

#include "trick/message_proto.h"
#include "trick/message_type.h"

#include <pthread.h>
#include <unistd.h>
#include <vector>

namespace
{

    // How long to give session threads to finish their current command and exit before
    // giving up on them. Commands are normally sub-millisecond, so this only comes into
    // play when a client command is blocked inside a model call.
    const unsigned int SESSION_SHUTDOWN_TIMEOUT_USEC = 5000000;
    const unsigned int SESSION_SHUTDOWN_POLL_USEC    = 1000;

}

int Trick::VariableServer::shutdown() {
    // Shut down all listen threads. These only ever block in accept() and never enter the
    // Python interpreter, so an asynchronous cancel is safe for them.
    listen_thread.cancel_thread() ;
    for (auto& listen_it : additional_listen_threads) {
        listen_it.second->cancel_thread();
    }

    // Shut down all session threads.
    //
    // A session thread may be part-way through executing a client command through the
    // input processor, which holds the Python GIL (IPPython::parse()). pthread_cancel()ing
    // it there -- which is what cancel_thread() does -- can destroy the thread while it
    // still owns the GIL, orphaning the GIL so that ip.shutdown() hangs forever trying to
    // reacquire it.
    //
    // Ask each thread to stop instead and let it finish its current command and release
    // the GIL on its own. This keeps the existing vs.shutdown() -> ip.shutdown() job
    // ordering meaningful: by the time this job returns, well-behaved sessions are gone
    // and the GIL is free.
    std::vector<VariableServerSessionThread*> sessions;
    pthread_mutex_lock(&map_mutex) ;
    for (auto& thread : var_server_threads)
    {
        thread.second->request_shutdown();
        sessions.push_back(thread.second);
    }
    pthread_mutex_unlock(&map_mutex) ;

    // Wait for the threads to drop out of the map, which they do from exit_var_thread() on
    // their way out.
    //
    // This wait is deliberately bounded. A client command blocked inside a model call
    // never returns, so waiting indefinitely (or calling join_thread() straight away)
    // would just relocate the hang from ip.shutdown() into this job, where no
    // later safety net can catch it.
    unsigned int waited = 0;
    while (waited < SESSION_SHUTDOWN_TIMEOUT_USEC)
    {
        pthread_mutex_lock(&map_mutex);
        bool all_stopped = var_server_threads.empty();
        pthread_mutex_unlock(&map_mutex);

        if (all_stopped)
        {
            break;
        }

        usleep(SESSION_SHUTDOWN_POLL_USEC);
        waited += SESSION_SHUTDOWN_POLL_USEC;
    }

    // Reap the threads that stopped and abandon the ones that did not.
    //
    // Joining is done outside map_mutex on purpose: an exiting thread runs
    // exit_var_thread(), which takes map_mutex in delete_session()/delete_vst(), so
    // holding it across a join here would deadlock.
    for (auto* thread : sessions)
    {
        pthread_mutex_lock(&map_mutex);
        bool still_running = (var_server_threads.count(thread->get_pthread_id()) > 0);
        pthread_mutex_unlock(&map_mutex);

        if (still_running)
        {
            message_publish(MSG_WARNING,
                            "Trick::VariableServer::shutdown() giving up on a variable server session thread that did "
                            "not stop when asked; it is most likely blocked inside a client command. Abandoning it so "
                            "that shutdown can continue rather than hanging.\n");
            thread->abandon_thread();
        }
        else
        {
            thread->join_thread();
        }
    }

    return 0 ;
}
