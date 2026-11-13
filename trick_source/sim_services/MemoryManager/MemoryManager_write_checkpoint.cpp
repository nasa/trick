#include "trick/MemoryManager.hh"

#include <algorithm> // std::sort()
#include <errno.h>
#include <fstream>
#include <sstream>
#include <stdio.h>  // fflush()
#include <stdlib.h> // free()
#include <string.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#ifndef MAP_ANONYMOUS
#define MAP_ANONYMOUS MAP_ANON
#endif

// Values of the result a forked checkpoint writer reports through shared memory.
enum
{
    FORKED_WRITE_NOT_REPORTED = 0, // Reset by the parent before fork; still set if the writer ended before reporting.
    FORKED_WRITE_SUCCESS      = 1, // The forked checkpoint writer completed the write without errors.
    FORKED_WRITE_FAILED       = 2 // The forked checkpoint writer reported errors (written with errors, or not written).
};

// GreenHills stuff
#if ( __ghs )
#include "ghs_stubs.h"
#endif

// MEMBER FUNCTION
void Trick::MemoryManager::execute_checkpoint( std::ostream& out_s ) {

    ALLOC_INFO_MAP::iterator pos;
    ALLOC_INFO* alloc_info;
    char name[256];
    int local_anon_var_number;
    int extern_anon_var_number;

    // 1) Generate declaration statements for each the allocations that we are managing.
    out_s << "// Variable Declarations." << std::endl;
    out_s.flush();

    local_anon_var_number = 0;
    extern_anon_var_number = 0;

    // Give names to the anonymous declarations
    // Also search each allocation for STLs.
    // STLs will be added to the dependencies vector.
    int n_depends = dependencies.size();
    for (int ii = 0 ; ii < n_depends ; ii ++) {
        alloc_info = dependencies[ii];
        /** Generate temporary names for anonymous variables. */
        if (alloc_info->name == NULL) {
            // Skip naming individual C-string allocations to eliminate unused declarations
            // Individual char arrays (size=1, num_index=1) represent single strings that are
            // included in char* arrays, so we don't need separate allocations for them
            if (alloc_info->type == TRICK_CHARACTER && alloc_info->size == 1 && alloc_info->num_index == 1
                && alloc_info->stcl == TRICK_LOCAL)
            {
                // Skip naming this individual char array allocation to prevent unused declarations
                continue;
            }

            if ( alloc_info->stcl == TRICK_LOCAL) {
                snprintf( name, sizeof(name), "%s%d", local_anon_var_prefix, local_anon_var_number++);
                alloc_info->name = strdup( name);
            } else if (alloc_info->stcl == TRICK_EXTERN) {
                snprintf( name, sizeof(name), "%s%d", extern_anon_var_prefix, extern_anon_var_number++);
                alloc_info->name = strdup( name);
                /** @b NOTE: We should not write declarations for external
                    anonymous variables, because we should not reload them.*/
            } else {
                emitError("write_checkpoint: This is bad. ALLOC_INFO object is messed up.\n") ;
                checkpoint_write_success = false;
            }
        }
        get_stl_dependencies(alloc_info);
    }

    // Write a declaration statement for all of the LOCAL variables,
    n_depends = dependencies.size();
    for (int ii = 0 ; ii < n_depends ; ii ++) {
        alloc_info = dependencies[ii];
        if ( alloc_info->stcl == TRICK_LOCAL) {
            currentCheckPointAgent->write_decl( out_s, alloc_info);
        }
    }

    // Write a "clear_all_vars" command.
    if (reduced_checkpoint) {
        out_s << std::endl << std::endl << "// Clear all allocations to 0." << std::endl;
        out_s << "clear_all_vars();" << std::endl;
    }

    // 2) Dump the contents of each of the dynamic and mapped allocations.
    out_s << std::endl << std::endl << "// Variable Assignments." << std::endl;
    out_s.flush();

    for (int ii = 0 ; ii < n_depends ; ii ++) {
        alloc_info = dependencies[ii];
        // Skip naming individual C-string allocations to eliminate unused declarations.
        // Individual char arrays (size=1, num_index=1) represent single strings that are
        // included in char* arrays, so we don't need separate allocations for them.
        // Also, safety check for NULL name before pushing to name stack later.
        if (!(alloc_info->type == TRICK_CHARACTER && alloc_info->size == 1 && alloc_info->num_index == 1
              && alloc_info->stcl == TRICK_LOCAL)
            && alloc_info->name != NULL)
        {
            write_var(out_s, alloc_info);
            out_s << std::endl;
        }
    }

    // Free all of the temporary names that were created for the checkpoint.

    for (int ii = 0 ; ii < n_depends ; ii ++) {
        alloc_info = dependencies[ii];
        // If the temporary-variable prefix occurs at the beginning of the name ...
        if ((alloc_info->name != NULL) &&
            (( strstr( alloc_info->name, local_anon_var_prefix ) == alloc_info->name ) ||
             ( strstr( alloc_info->name, extern_anon_var_prefix) == alloc_info->name ))) {
                free( alloc_info->name);
                alloc_info->name = NULL;
        }
    }

    // Delete the variables created by STLs. Remove memory in reverse order.
    std::vector<ALLOC_INFO*>::reverse_iterator it ;
    for ( it = stl_dependencies.rbegin() ; it != stl_dependencies.rend() ; it++ ) {
        delete_var((*it)->start) ;
    }

    out_s.flush();
    if (out_s.fail())
    {
        emitError("write_checkpoint: An error occurred while writing to the checkpoint output stream.\n");
        checkpoint_write_success = false;
    }
    if (!checkpoint_write_success)
    {
        emitWarning("write_checkpoint: Checkpoint was written with errors and may not restore correctly."
                    " See get_checkpoint_write_state() / TMM_get_checkpoint_write_state().");
    }
}

// Reset the checkpoint write error state before starting a new checkpoint.
void Trick::MemoryManager::begin_checkpoint_write()
{
    // Called at the start of every public write_checkpoint(). Collects any earlier forked
    // checkpoint so its result can't overwrite this one, then resets the write state to true.
    // Calling it more than once for the same write is harmless.
    wait_for_checkpoint_write();
    checkpoint_write_success = true;
}

// MEMBER FUNCTION
bool Trick::MemoryManager::get_checkpoint_write_state()
{
    // Read the result while still holding the lock, so it can't change between collecting
    // a forked writer and returning.
    pthread_mutex_lock(&checkpoint_write_mutex);
    wait_for_checkpoint_write_locked();
    bool state = checkpoint_write_success;
    pthread_mutex_unlock(&checkpoint_write_mutex);
    return state;
}

// MEMBER FUNCTION
int Trick::MemoryManager::fork_checkpoint_writer()
{
    pthread_mutex_lock(&checkpoint_write_mutex);

    // Collect any earlier forked checkpoint so only one result is pending.
    wait_for_checkpoint_write_locked();
    reset_forked_checkpoint_result();

    // The child's code locks mm_mutex. Holding it across fork() makes sure no other thread
    // owns it at that moment, so the child's copy can be unlocked and its maps are consistent.
    // Flush buffered output first, otherwise the child inherits a copy and prints it again.
    std::cout.flush();
    fflush(nullptr);
    pthread_mutex_lock(&mm_mutex);
    pid_t pid = fork();
    pthread_mutex_unlock(&mm_mutex);

    if (pid > 0)
    {
        checkpoint_write_pid = (int)pid;
    }
    pthread_mutex_unlock(&checkpoint_write_mutex);
    return (int)pid;
}

// MEMBER FUNCTION
void Trick::MemoryManager::reset_forked_checkpoint_result()
{
    if (checkpoint_write_shared == nullptr)
    {
        void* shared = mmap(nullptr, sizeof(*checkpoint_write_shared), PROT_READ | PROT_WRITE,
                            MAP_SHARED | MAP_ANONYMOUS, -1, 0);
        if (shared != MAP_FAILED)
        {
            checkpoint_write_shared = static_cast<int*>(shared);
        }
    }
    if (checkpoint_write_shared != nullptr)
    {
        volatile int* shared = checkpoint_write_shared;
        *shared              = FORKED_WRITE_NOT_REPORTED;
    }
}

// MEMBER FUNCTION
void Trick::MemoryManager::report_forked_checkpoint_write()
{
    // volatile (not atomic) is enough here: the parent only reads the value after the child
    // has exited and been reaped, and process termination orders the child's writes.
    if (checkpoint_write_shared != nullptr)
    {
        volatile int* shared = checkpoint_write_shared;
        *shared              = checkpoint_write_success ? FORKED_WRITE_SUCCESS : FORKED_WRITE_FAILED;
    }
}

// MEMBER FUNCTION
void Trick::MemoryManager::wait_for_checkpoint_write()
{
    // Only one thread at a time can wait for the forked child. The thread holding the lock
    // collects the result and sets checkpoint_write_pid to -1. Other threads block on the lock,
    // then find pid -1 and use that result without waiting.
    pthread_mutex_lock(&checkpoint_write_mutex);
    wait_for_checkpoint_write_locked();
    pthread_mutex_unlock(&checkpoint_write_mutex);
}

// MEMBER FUNCTION
void Trick::MemoryManager::wait_for_checkpoint_write_locked()
{
    if (checkpoint_write_pid <= 0)
    {
        return;
    }

    int status = 0;
    pid_t ret;
    do
    {
        ret = waitpid(checkpoint_write_pid, &status, 0);
    } while (ret == -1 && errno == EINTR);
    int saved_errno = errno;

    if (ret > 0)
    {
        // If shared memory is available, the child must also have reported its result there;
        // this catches a stray exit(0) or exit(1) that didn't come from the checkpoint writer.
        bool reported         = true;
        bool reported_success = true;
        if (checkpoint_write_shared != nullptr)
        {
            volatile int* shared = checkpoint_write_shared;
            int result           = *shared;
            reported             = (result != FORKED_WRITE_NOT_REPORTED);
            reported_success     = (result == FORKED_WRITE_SUCCESS);
        }

        // Signal: child died abnormally. Exit code > 1 or no reported result: abnormal exit.
        // Exit code 1 with a reported result is the child's own reported failure, so no extra warning.
        if (WIFSIGNALED(status))
        {
            emitWarning("write_checkpoint: Forked checkpoint process ended abnormally (signal "
                        + std::to_string(WTERMSIG(status)) + "). Checkpoint write state set to false.");
        }
        else if (WIFEXITED(status) && (WEXITSTATUS(status) > 1 || !reported))
        {
            emitWarning("write_checkpoint: Forked checkpoint process ended abnormally (exit code "
                        + std::to_string(WEXITSTATUS(status)) + (reported ? "" : ", no result reported")
                        + "). Checkpoint write state set to false.");
        }
        // success means the child exited normally with exit code 0 and reported success.
        checkpoint_write_success = WIFEXITED(status) && WEXITSTATUS(status) == 0 && reported && reported_success;
    }
    else if (saved_errno == ECHILD && checkpoint_write_shared != nullptr)
    {
        // The child was already reaped, e.g. by a SIGCHLD handler (exec_set_trap_sigchld),
        // so use the result it reported through shared memory.
        // In theory the pid could already have been reused by another child of this process
        // before this waitpid() ran; that window is narrow and only exists with the trap on.
        volatile int* shared = checkpoint_write_shared;
        int result           = *shared;
        if (result == FORKED_WRITE_NOT_REPORTED)
        {
            emitWarning("write_checkpoint: Forked checkpoint process ended abnormally before reporting its result."
                        " Checkpoint write state set to false.");
        }
        checkpoint_write_success = (result == FORKED_WRITE_SUCCESS);
    }
    else
    {
        emitWarning("write_checkpoint: Unable to get the result of the forked checkpoint process ("
                    + std::string(strerror(saved_errno)) + "). Checkpoint write state is unknown and set to false.");
        checkpoint_write_success = false;
    }
    checkpoint_write_pid = -1;
}

// MEMBER FUNCTION
void Trick::MemoryManager::set_checkpoint_write_state(bool state) { checkpoint_write_success = state; }

// Local sort function used in write_checkpoint.
static bool alloc_info_id_compare(ALLOC_INFO * lhs, ALLOC_INFO * rhs) { return ( lhs->id < rhs->id ) ; }

// MEMBER FUNCTION
void Trick::MemoryManager::write_checkpoint( std::ostream& out_s) {

    ALLOC_INFO_MAP::iterator pos;
    ALLOC_INFO* alloc_info;
    begin_checkpoint_write();
    dependencies.clear();
    stl_dependencies.clear();

    pthread_mutex_lock(&mm_mutex);
    for ( pos=alloc_info_map.begin() ; pos!=alloc_info_map.end() ; pos++ ) {
        alloc_info = pos->second;
        dependencies.push_back(alloc_info);
    }

    // Sort the dependencies by ALLOC_INFO.id.
    std::sort( dependencies.begin() , dependencies.end() , alloc_info_id_compare) ;
    pthread_mutex_unlock(&mm_mutex);

    execute_checkpoint( out_s );

}

// MEMBER FUNCTION
void Trick::MemoryManager::write_checkpoint(const char* filename) {
    // Collect any pending forked checkpoint before opening the file, so its result can't
    // overwrite an open failure below and a child still writing the same file isn't truncated.
    begin_checkpoint_write();
    std::ofstream outfile(filename, std::ios::out);

    if (outfile.is_open()) {
        write_checkpoint( outfile);
    } else {
        std::stringstream message;
        message << "Couldn't open \"" << filename << "\".";
        emitError(message.str());
        checkpoint_write_success = false;
    }
}

// MEMBER FUNCTION
void Trick::MemoryManager::write_checkpoint( std::ostream& out_s, const char* var_name) {
    begin_checkpoint_write();
    dependencies.clear();
    stl_dependencies.clear();

    pthread_mutex_lock(&mm_mutex);
    get_alloc_deps_in_allocation( var_name);
    pthread_mutex_unlock(&mm_mutex);

    execute_checkpoint( out_s );
}

// MEMBER FUNCTION
void Trick::MemoryManager::write_checkpoint(const char* filename, const char* var_name) {
    // Collect any pending forked checkpoint before opening the file, so its result can't
    // overwrite an open failure below and a child still writing the same file isn't truncated.
    begin_checkpoint_write();
    std::ofstream out_s( filename, std::ios::out);
    if (out_s.is_open()) {
        write_checkpoint( out_s, var_name);
    } else {
        std::stringstream message;
        message << "Couldn't open \"" << filename << "\".";
        emitError(message.str());
        checkpoint_write_success = false;
    }
}

// MEMBER FUNCTION
void Trick::MemoryManager::write_checkpoint( std::ostream& out_s, std::vector<const char*>& var_name_list) {

    const char* var_name;
    int n_names;

    begin_checkpoint_write();
    dependencies.clear();
    stl_dependencies.clear();

    n_names = var_name_list.size();
    for (int ii=0; ii< n_names; ii++) {
        var_name = var_name_list[ii];
        pthread_mutex_lock(&mm_mutex);
        get_alloc_deps_in_allocation(var_name);
        pthread_mutex_unlock(&mm_mutex);
    }

    execute_checkpoint( out_s );
}

// MEMBER FUNCTION
void Trick::MemoryManager::write_checkpoint(const char* filename, std::vector<const char*>& var_name_list) {
    // Collect any pending forked checkpoint before opening the file, so its result can't
    // overwrite an open failure below and a child still writing the same file isn't truncated.
    begin_checkpoint_write();
    std::ofstream out_s( filename, std::ios::out);

    if (out_s.is_open()) {
        write_checkpoint( out_s, var_name_list);
    } else {
        std::cerr << "ERROR: Couldn't open \""<< filename <<"\"." << std::endl;
        std::cerr.flush();
        checkpoint_write_success = false;
    }
}
