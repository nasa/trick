/*************************************************************************
PURPOSE: (Represent the state of a variable server connection.)
**************************************************************************/

#ifndef VSSESSION_HH
#define VSSESSION_HH

#include <string>
#include <vector>

#include "trick/ClientConnection.hh"
#include "trick/VariableReference.hh"
#include "trick/tc.h"
#include "trick/variable_server_message_types.h"
#include "trick/variable_server_sync_types.h"

namespace Trick
{
class VariableServerSession
{
public:
    VariableServerSession();

    virtual ~VariableServerSession();

    friend std::ostream & operator<<(std::ostream & s, const Trick::VariableServerSession & session);

    /**
     * @brief Set the connection object
     *
     * @param connection - a pointer to a ClientConnection that is initialized and ready to go
     */
    virtual void set_connection(ClientConnection * connection);

    /**
     @brief Read a message from the connection and act on it
    */
    virtual int handle_message();

    /**
     @brief Get the pause state of this thread.
    */
    virtual bool get_pause();

    /**
     @brief Set the pause state of this thread.
    */
    virtual void set_pause(bool on_off);

    /**
     @brief Get the exit command of this session.
    */
    virtual bool get_exit_cmd();

    /**
     @brief Command session to exit
    */
    virtual void set_exit_cmd();

    /**
     @brief gets the send_stdio flag.
    */
    virtual bool get_send_stdio();

    /**
     @brief sets the send_stdio flag.
    */
    virtual int set_send_stdio(bool on_off);

    // Called from different types of Trick jobs.
    // Determines whether this session should be copying at that time, and calls internal copy methods if so
    int copy_and_write_freeze(long long curr_frame);
    int copy_and_write_freeze_scheduled(long long curr_tics);
    int copy_and_write_scheduled(long long curr_tics);
    int copy_and_write_top(long long curr_frame);

    // Called from VariableServerSessionThread
    virtual int copy_and_write_async();

    /**
     @brief Copy given variable values from Trick memory to each variable's output buffer.
        cyclical indicated whether it is a normal cyclical copy or a send_once copy
    */
    virtual int copy_sim_data(std::vector<VariableReference *> & given_vars, bool cyclical);
    virtual int copy_sim_data();

    /**
     @brief Write data from the given var only to the appropriate format (var_ascii or var_binary) from variable output
     buffers to socket.
    */
    virtual int write_data(std::vector<VariableReference *> & var, VS_MESSAGE_TYPE message_type);
    virtual int write_data();

    int write_stdio(int stream, std::string text);

    void disconnect_references();

    virtual long long get_next_tics() const;

    virtual long long get_freeze_next_tics() const;

    int freeze_init();

    virtual double get_update_rate() const;

    void pause_copy();
    void unpause_copy();

    virtual VS_WRITE_MODE get_write_mode() const;
    virtual VS_COPY_MODE get_copy_mode() const;

    /************************************************************************************************/
    /*                               Variable Server Interface                                      */
    /************************************************************************************************/

    /**
     @brief @userdesc Command to add a variable to a list of registered variables for value retrieval.
        The variable server will immediately begin returning the variable values to the client at a
        frequency according to var_cycle.
        @par Python Usage:
        @code trick.var_add("<in_name>") @endcode
        @param in_name - the variable name to retrieve
        @return always 0
    */
    virtual int var_add(std::string in_name);

    /**
     @brief @userdesc Command to add a variable to a list of registered variables for value retrieval,
        and also instructs the variable server to return the value in the specified measurement units.
        The unit specification will be returned after the variable's value (var_ascii mode only).
        Specifying units as "{xx}" will use the unit specification from the variable's model code declaration,
        which is essentially a method of querying a variable's unit specification.
        The variable server will immediately begin returning the variable values/units to the client at
        a frequency according to var_cycle.
        @par Python Usage:
        @code trick.var_add("<in_name>", "<units_name>") @endcode
        @param in_name - the variable name to retrieve
        @param units_name - the desired units, specified within curly braces
        @return always 0
    */
    virtual int var_add(std::string in_name, std::string units_name);

    /**
     @brief @userdesc Command to remove a variable (previously registered with var_add)
        from the list of registered variables for value retrieval.
        @par Python Usage:
        @code trick.var_remove("<in_name>") @endcode
        @param in_name - the variable name to remove
        @return always 0
    */
    virtual int var_remove(std::string in_name);

    /**
     @brief @userdesc Command to instruct the variable server to return the value of a variable
        in the specified units (var_ascii mode only).
        The variable must have been previously registered with the var_add command.
        The unit specification will be returned after the variable's value.
        @par Python Usage:
        @code trick.var_units("<var_name>", "<units_name>") @endcode
        @param var_name - the variable name previously registered with var_add
        @param units_name - the desired units, specified within curly braces
        @return always 0
        @note trick.var_add("my_object.my_variable"); trick.var_units("my_object.my_variable","{m}")
        is the same as trick.var_add("my_object.my_variable","{m}")
    */
    virtual int var_units(std::string var_name, std::string units_name);

    /**
     @brief @userdesc Command to instruct the variable server to send a Boolean value indicating
        whether the specified variable exists
        (is a Trick processed variable in the current simulation).
        - var_binary mode: the message indicator is 1, and the returned value is a binary 0 or 1.
        - var_ascii mode: the message indicator is "1" followed by a tab, then an ASCII "0" or "1" returned value.
        .
        @par Python Usage:
        @code trick.var_exists("<in_name>") @endcode
        @param in_name - the variable name to query
        @return always 0
    */
    virtual int var_exists(std::string in_name);

    /**
     @brief @userdesc Command to immediately send the value of a comma separated list of variables
        @par Python Usage:
        @code trick.var_send_once("<in_name_list>", <number of variables in list>) @endcode
        @param in_name_list - the variables name to retrieve, comma separated
        @param num_vars - number of vars in in_name_list
        @return always 0
    */
    virtual int var_send_once(std::string in_name_list, int num_vars);

    /**
     @brief @userdesc Command to query the size of an STL container.
        Returns the size by calling the container's get_stl_size accessor function.
        - var_binary mode: message indicator is 1, followed by binary integer size
        - var_ascii mode: message indicator is "1" followed by tab, then ASCII integer size
        TODO: Check if needed to update when expanding STL container support.
              Currently only works for STL containers: std::vector, std::deque, and std:array.
        @par Python Usage:
        @code trick.var_get_stl_size("<container_name>") @endcode
        @param in_name - the STL container variable name
        @return always 0
    */
    virtual int var_get_stl_size(std::string in_name);

    /**
     @brief @userdesc Command to instruct the variable server to immediately send back the values of
        variables that are registered with the var_add command
        (typically used when var_pause is in effect). Each var_send command sends back all current values
        once instead of cyclically.
        @par Python Usage:
        @code trick.var_send() @endcode
        @return always 0
    */
    virtual int var_send();

    /**
     @brief @userdesc Command to remove all variables from the list of variables currently
        registered with the var_add command,
        so the variable server will no longer send cyclic values until new var_add commands are issued.
        @par Python Usage:
        @code trick.var_clear() @endcode
        @return always 0
    */
    virtual int var_clear();

    /**
     @brief @userdesc Turns on validating addresses before they are referenced
        @par Python Usage:
        @code trick.var_validate_address() @endcode
        @return always 0
    */
    virtual int var_validate_address(bool on_off);

    /**
     @brief @userdesc Command to instruct the variable server to output debug information.
        @par Python Usage:
        @code trick.var_debug(<level>) @endcode
        @return always 0
        @param level - 1,2,or 3, higher number increases amount of info output
    */
    virtual int var_debug(int level);

    /**
     @brief @userdesc Command to instruct the variable server to return values in ASCII format (this is the default).
        @par Python Usage:
        @code trick.var_ascii() @endcode
        @return always 0
    */
    virtual int var_ascii();

    /**
     @brief @userdesc Command to instruct the variable server to return values in binary format.
        @par Python Usage:
        @code trick.var_binary() @endcode
        @return always 0
    */
    virtual int var_binary();

    /**
     @brief @userdesc Command to instruct the variable server to return values in binary format,
        but saves space in the returned messages
        by omitting the variable names.
        @par Python Usage:
        @code trick.var_binary_nonames() @endcode
        @return always 0
    */
    virtual int var_binary_nonames();

    /**
     @brief @userdesc Command to tell the server when to copy data
        - VS_COPY_ASYNC = copies data asynchronously. (default)
        - VS_COPY_SCHEDULED = copies data as an automatic_last job in main thread
        - VS_COPY_TOP_OF_FRAME = copies data at top of frame
        @par Python Usage:
        @code trick.var_set_copy_mode(<mode>) @endcode
        @param mode - One of the above enumerations
        @return 0 if successful, -1 if error
    */
    virtual int var_set_copy_mode(int mode);

    /**
     @brief @userdesc Command to tell the server when to copy data
        - VS_WRITE_ASYNC = writes data asynchronously. (default)
        - VS_WRITE_WHEN_COPIED = writes data as soon as it's copied from sim
        @par Python Usage:
        @code trick.var_set_write_mode(<mode>) @endcode
        @param mode - One of the above enumerations
        @return 0 if successful, -1 if error
    */
    virtual int var_set_write_mode(int mode);

    /**
     @brief @userdesc Command to put the current variable server/client connection in sync mode,
        so that values to be sent to the client
        are guaranteed to be homogenous, that is from the same execution frame (the default is asynchronous).
        - async/async mode: the variable server thread itself copies the client data,
        independent of the execution frame so may not be homogenous.
        - sync/async mode: a variable server automatic_last job retrieves client requested data.
        Data is sent asynchronously on separate thread.
        - sync/sync mode: a variable server automatic_last job retrieves client requested data.
        Data is sent in same automatic_last job.
        .
        @par Python Usage:
        @code trick.var_sync(<on_off>) @endcode
        @param on_off - 0 = fully asynchronous. 1 = sync data gather, async socket write.
        2 = sync data gather, sync socket write
        @return always 0
    */
    virtual int var_sync(int on_off);

    /**
     @brief @userdesc Set the frame multiple
        @par Python Usage:
        @code trick.var_set_frame_multiple(<mult>) @endcode
        @param mult - The requested multiple
        @return 0
    */
    virtual int var_set_frame_multiple(unsigned int mult);

    /**
     @brief @userdesc Set the frame offset
        @par Python Usage:
        @code trick.var_set_frame_offset(<offset>) @endcode
        @param offset - The requested offset
        @return 0
    */
    virtual int var_set_frame_offset(unsigned int offset);

    /**
     @brief @userdesc Set the frame multiple
        @par Python Usage:
        @code trick.var_set_freeze_frame_multiple(<mult>) @endcode
        @param mult - The requested multiple
        @return 0
    */
    virtual int var_set_freeze_frame_multiple(unsigned int mult);

    /**
     @brief @userdesc Set the frame offset
        @par Python Usage:
        @code trick.var_set_freeze_frame_offset(<offset>) @endcode
        @param offset - The requested offset
        @return 0
    */
    virtual int var_set_freeze_frame_offset(unsigned int offset);

    /**
     @brief @userdesc Command to instruct the variable server to byteswap the return values
        (only has an effect in var_binary mode).
        The default is no byteswap - it is assumed server and client are same endianness.
        If not, the user must issue the var_byteswap command.
        @par Python Usage:
        @code trick.var_byteswap(<on_off>) @endcode
        @param on_off - true (or 1) to byteswap the return data, false (or 0) to return data as is
        @return always 0
    */
    virtual int var_byteswap(bool on_off);

    /**
     @brief @userdesc Command to toggle variable server logged messages to a playback file.
        All messages received from all clients will be saved to file named "playback" in the RUN directory.
        @par Python Usage:
        @code trick.set_log(<on_off>) @endcode
        @return always 0
    */
    virtual int set_log(bool on_off);

    /**
     @brief @userdesc Command to toggle variable server session log.
        Each session will create a file containing the IP and port of client and all messages.
        The file will be created in a session_log/ subdirectory under the RUN directory.
        @par Python Usage:
        @code trick.set_session_log(<on_off>) @endcode
        @return always 0
    */
    virtual int set_session_log(bool on_off);

    /**
     @brief @userdesc Command to set info messages.
        @return always 0
    */
    virtual int set_info_message(bool on);

    /**
     @brief Command to send the number of items in the var_add list.
        The variable server sends a message indicator of "3", followed by the total number of variables being sent.
    */
    virtual int send_list_size();

    /**
     @brief Special command to instruct the variable server to send the contents of the S_sie.resource file; used by TV.
        The variable server sends a message indicator of "2", followed by a tab, followed by the file contents
        which are then sent as sequential ASCII messages with a maximum size of 4096 bytes each.
    */
    virtual int send_sie_resource();

    /**
     @brief Special command to only send the class sie class information
    */
    virtual int send_sie_class();

    /**
     @brief Special command to only send the enumeration sie class information
    */
    virtual int send_sie_enum();

    /**
     @brief Special command to only send the top level objects sie class information
    */
    virtual int send_sie_top_level_objects();

    /**
     @brief Special command to send an arbitrary file through the variable server.
    */
    virtual int send_file(std::string file_name);

    /**
     @brief @userdesc Command to set the frequencty at which the variable server will send values
        of variables that have been registered using
        the var_add command. If var_cycle is not specified, the default cycle is 0.1 seconds.
        @par Python Usage:
        @code trick.var_cycle(<in_cycle>) @endcode
        @param in_cycle - the desired frequency in seconds
        @return always 0
    */
    virtual int var_cycle(double in_cycle);

    /**
     @brief @userdesc Command exit this variable server session.
        @par Python Usage:
        @code trick.var_exit() @endcode
        @return always 0
    */
    virtual int var_exit();

private:
    static int instance_counter;
    static std::string log_subdir;

    pthread_mutex_t _copy_mutex; /**<  trick_io(**) */

    ClientConnection * _connection; /**<  trick_io(**) */

    // Helper method to send a file to connection
    virtual int transmit_file(std::string sie_file);

    // Helper methods to write out formatted data
    virtual int write_binary_data(const std::vector<VariableReference *> & given_vars, VS_MESSAGE_TYPE message_type);
    virtual int write_ascii_data(const std::vector<VariableReference *> & given_vars, VS_MESSAGE_TYPE message_type);

    virtual VariableReference * find_session_variable(std::string name) const;

    std::vector<VariableReference *> _session_variables; /**<  trick_io(**) */

    // Getters and setters for internal variables
    virtual long long get_cycle_tics() const;

    virtual void set_freeze_next_tics(long long tics);
    virtual void set_next_tics(long long tics);

    virtual int get_frame_multiple() const;
    virtual int get_frame_offset() const;
    virtual int get_freeze_frame_multiple() const;
    virtual int get_freeze_frame_offset() const;
    virtual bool get_enabled() const;

    // Check settings and log to appropriate places
    void log_received_message(const std::string & msg);
    void log_connection_opened();

    bool is_session_log_open();

    void open_session_log();

    void write_to_session_log(const std::string & msg);

    /** Value set in var_cycle command.\n */
    double _update_rate; /**<  trick_io(**) */

    /** The update rate in integer tics.\n */
    long long _cycle_tics; /**<  trick_io(**) */

    /** The next call time in integer tics of the job to copy client data (sync mode).\n */
    long long _next_tics; /**<  trick_io(**) */

    /** The next call time in integer tics of the job to copy client data (sync mode).\n */
    long long _freeze_next_tics; /**<  trick_io(**) */

    /** The simulation time converted to seconds\n */
    double _time; /**<  trick_units(s) */

    /** Toggle to set variable server copy as top_of_frame, scheduled, async \n */
    VS_COPY_MODE _copy_mode; /**<  trick_io(**) */

    /** Toggle to set variable server writes as when copied or async.\n */
    VS_WRITE_MODE _write_mode; /**<  trick_io(**) */

    /** multiples of frame_count to copy data.  Only used at top_of_frame\n */
    int _frame_multiple; /**<  trick_io(**) */

    /** multiples of frame_count to copy data.  Only used at top_of_frame\n */
    int _frame_offset; /**<  trick_io(**) */

    /** multiples of frame_count to copy data.  Only used at top_of_frame\n */
    int _freeze_frame_multiple; /**<  trick_io(**) */

    /** multiples of frame_count to copy data.  Only used at top_of_frame\n */
    int _freeze_frame_offset; /**<  trick_io(**) */

    /** Toggle to tell variable server to byteswap returned values.\n */
    bool _byteswap; /**<  trick_io(**) */

    /** Toggle to tell variable server return data in binary format.\n */
    bool _binary_data; /**<  trick_io(**) */

    /** Toggle to tell variable server return data in binary format without the variable names.\n */
    bool _binary_data_nonames; /**<  trick_io(**) */

    /** Value (1,2,or 3) that causes the variable server to output increasing amounts of debug information.\n */
    int _debug; /**<  trick_io(**) */

    /** Toggle to enable/disable this variable server thread.\n */
    bool _enabled; /**<  trick_io(**) */

    /** Toggle to turn on/off variable server logged messages to a playback file.\n */
    bool _log; /**< trick_io(**)  */

    /** Toggle to turn on/off individual variable server session logs.\n */
    bool _session_log; /**< trick_io(**)  */

    /** Toggle to turn on/off debug info messages.\n */
    bool _info_msg;

    /** Message stream number for the session log file */
    int _session_log_msg_stream;

    /** Toggle to indicate var_pause commanded.\n */
    bool _pause_cmd; /**<  trick_io(**) */

    /** Save pause state while reloading a checkpoint.\n */
    bool _saved_pause_cmd; /**<  trick_io(**) */

    bool _send_stdio;

    bool _validate_address;

    /** Toggle to indicate var_exit commanded.\n */
    bool _exit_cmd; /**<  trick_io(**) */

    int _instance_num;
};
} // namespace Trick

#endif
