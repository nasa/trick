/*
PURPOSE:
    (Data recording management parameter definition.)
PROGRAMMERS:
    (((Alex Lin) (NASA) (2009)))
*/

#ifndef DATARECORDGROUP_HH
#define DATARECORDGROUP_HH

#include <vector>
#include <string>
#include <fstream>
#include <pthread.h>

#include "trick/SimObject.hh"
#include "trick/reference.h"

namespace Trick {

    /**
     * The DR_Freq enumeration represents the possible Trick data recording frequency.
     */
    enum DR_Freq {
        DR_Always = 0,          /* Record every record */
        DR_Changes = 1,         /* Record only when a data item has changed
                                   since last pass */
        DR_Changes_Step = 2     /* Record only when data item has changed, but
                                   record both before and after records.
                                   Creates a step plot output instead of a
                                   point to point plot */
    } ;

    /**
     * The DR_Buffering enumeration represents the possible Trick data recording buffering options.
     */
    enum DR_Buffering {
        DR_Buffer = 0,          /**< use a ring buffer, a second thread writes data to disk */
        DR_No_Buffer = 1,       /**< no buffering, always write to disk immediately */
        DR_Ring_Buffer = 2,     /**< use a ring buffer, only writing the ring to disk at shutdown */
        DR_Thread_Buffer = 0,   /**< use a ring buffer, a second thread writes data to disk */
        DR_Not_Specified = 3    /**< Unknown type */
    } ;

    class DataRecordBuffer {
        public:
            char *buffer;       /* ** generic holding buffer for data */
            char *curr_buffer;  /* ** current position in the buffer */
            char *last_value;   /* ** holding buffer for last value, used for DR_Changes_step */
            REF2 * ref ;        /* ** size/address/units information of variable */
            bool ref_searched ; /* ** reference information has been searched */
            std::string name ;      /* ** actual name of the variable to record */
            std::string alias ;      /* ** alias name used in data recording files */
            DataRecordBuffer() ;
            ~DataRecordBuffer() ;
    } ;

    class DataRecordGroup : public Trick::SimObject {

        public:

            /** Yes = record data, No = do not record data.\n */
            bool record;                /**< trick_io(*io) trick_units(--) */

            /** Yes = group has been initialized.\n */
            bool inited;                /**< trick_io(*io) trick_units(--) */

            /** Unique name for group output data file.\n */
            std::string group_name;          /**< trick_io(*io) trick_units(--) */

            /** Directory to store log file(s).\n */
            std::string output_dir;          /**< trick_io(*io) trick_units(--) */

            /** Log file name with appropriate extension.\n */
            std::string file_name;           /**< trick_io(*io) trick_units(--) */

            /**  Determines frequency of recording data record, typically from enum DR_Freq.\n */
            DR_Freq freq;               /**< trick_io(*io) trick_units(--) */

            /** Start time for data recording.\n */
            double start;               /**< trick_io(*io) trick_units(s) */

            /** Cycle time for data recording.\n */
            double cycle;               /**< trick_io(*io) trick_units(s) */

            /*  Fake attributes to use for data recording.\n */
            ATTRIBUTES time_value_attr; /**< trick_io(**) */

            /** Number of variable names to save in a checkpoint.\n */
            unsigned int num_variable_names ; /** trick_units(--) */
            /** List of variable names to save in a checkpoint.\n */
            char ** variable_names ;    /** trick_units(--) */
            /** List of variable aliases to save in a checkpoint.\n */
            char ** variable_alias ;    /** trick_units(--) */

            /** Vector of buffers - one for every variable added with Trick::DataRecordGroup::add_variable.\n */
            std::vector <Trick::DataRecordBuffer *> rec_buffer;     /**< trick_io(**) trick_units(--) */

            /** Number of change variable names to save in a checkpoint.\n */
            unsigned int num_change_variable_names ; /** trick_units(--) */
            /** List of change variable names to save in a checkpoint.\n */
            char ** change_variable_names ;    /** trick_units(--) */
            /** List of change variable aliases to save in a checkpoint.\n */
            char ** change_variable_alias ;    /** trick_units(--) */

            /** Vector of buffers - one for every change variable added with Trick::DataRecordGroup::add_change_variable.\n */
            std::vector <Trick::DataRecordBuffer *> change_buffer;     /**< trick_io(**) trick_units(--) */

            /** Maximum records to hold in memory before writing.\n */
            unsigned int max_num;       /**< trick_io(*io) trick_units(--) */

            /** Current buffering record number.\n */
            unsigned int buffer_num;    /**< trick_io(**) trick_units(--) */

            /** Current write to file record number.\n */
            unsigned int writer_num;    /**< trick_io(**) trick_units(--) */

            /** Maximum file size for data record file in bytes.\n */
            uint64_t max_file_size;    /**< trick_io(**) trick_units(--) */
           
            /** Current file size for data record file in bytes.\n */
            uint64_t total_bytes_written;    /**< trick_io(**) trick_units(--) */

            /** Bool to signify that the warning for reaching max filesize has been printed */
            bool max_size_warning;

            /** Buffer to hold formatted data ready for disk or other destination.\n */
            char * writer_buff ;        /**< trick_io(**) trick_units(--) */

            /**  Little_endian or big_endian indicator.\n */
            std::string byte_order;          /**< trick_io(*io) trick_units(--) */

            /**  Yes = record all doubles as floats.\n */
            bool single_prec_only;      /**< trick_io(*io) trick_units(--) */

            /**  Type of buffering.\n */
            DR_Buffering buffer_type ;  /**< trick_io(*io) trick_units(--) */

            /**  The job class name for this recording group.\n */
            std::string job_class ;          /**< trick_io(*io) trick_units(--) */

            /** Pointer to the write job.  */
            Trick::JobData * write_job ; /**< trick_io(**) */

            /**
             @brief Constructor that creates a new data recording group with the given @c in_name.
             @param in_name - the new data recording group name
            */
            DataRecordGroup( std::string in_name = "" ) ;

            ~DataRecordGroup() ;

            /**
             @brief gets the group name
            */
            const std::string & get_group_name() ;

            /**
             @brief registers the group with the memory manager.
            */
            void register_group_with_mm(void * address , const char * type) ;

            /**
             @brief @userdesc Command to set the job class of the group's data_record() function (default is data_record class).
             @par Python Usage:
             @code <dr_group>.set_job_class("<in_class>") @endcode
             @param in_class - name of the job class
             @return always 0
            */
            int set_job_class(std::string in_class) ;

            /**
             @brief @userdesc Command to set the rate at which the group's data is recorded (default is 0.1).
             @par Python Usage:
             @code <dr_group>.set_cycle(<in_cycle>) @endcode
             @param in_cycle - the recording rate in seconds
             @return always 0
            */
            int set_cycle(double in_cycle) ;

            /**
             @brief @userdesc Command to set the phase where the group's data is record (default is 60000).
             @par Python Usage:
             @code <dr_group>.set_phase(<in_phase) @endcode
             @param in_phase - the recording phase between 0-65535
             @return always 0
            */
            int set_phase(unsigned short in_phase) ;

            /**
             @brief @userdesc Command to set the group's recording frequency (default is DR_Always).
             @par Python Usage:
             @code <dr_group>.set_freq(<in_freq>) @endcode
             @param in_freq - Trick::DataRecordGroup::DR_Freq
             @return always 0
            */
            int set_freq(DR_Freq in_freq) ;

            /**
             @brief @userdesc Command to set the size of each variable buffer in the group used to hold recorded variable values (default is 100000).
             The buffer size will be set during initialization in Trick::DataRecordGroup::init.
             @par Python Usage:
             @code <dr_group>.set_max_buffer_size(<num>) @endcode
             @param num - the buffer size in bytes
             @return always 0
            */
            virtual int set_max_buffer_size(int num) ;

            /**
             @brief @userdesc Command to set the buffer type of the group, DR_Buffer, DR_Ring, DR_No_Buffer.
             This tells the data record group when it is allowed to write data to disk.
             @par Python Usage:
             @code <dr_group>.set_buffer_type(<buffer_type>) @endcode
             @param type - the buffer type
             @return always 0
            */
            virtual int set_buffer_type(int buffer_type) ;

            /**
             @brief @userdesc Command to set the max file size in bytes.
             This tells the data record group when it stops writing to the disk.
             @par Python Usage:
             @code <dr_group>.set_max_file_size(<buffer_type>) @endcode
             @param type - the file size in bytes
             @return always 0
            */
            virtual int set_max_file_size(uint64_t bytes) ;


            /**
             @brief @userdesc Command to print double variable values as single precision (float) in the log file to save space.
             @par Python Usage:
             @code <dr_group>.set_single_prec_only(<in_single_prec_only>) @endcode
             @param in_single_prec_only - boolean true indicates print doubles as single precision
             @return always 0
            */
            virtual int set_single_prec_only(bool in_single_prec_only) ;

            /**
             @brief @userdesc Command to set the thread of execution for this log group
             @par Python Usage:
             @code <dr_group>.set_thread(<num>) @endcode
             @param num - child thread to run data_recording
             @return always 0
            */
            virtual int set_thread(unsigned int in_thread_id) ;

            //TODO: create SimObject enable/disable routines

            /**
             @brief @userdesc Command to turn on data recording for this group (the default).
             @par Python Usage:
             @code <dr_group>.enable() @endcode
             @return always 0
            */
            virtual int enable() ;

            /**
             @brief @userdesc Command to turn off data recording for this group.
             @par Python Usage:
             @code <dr_group>.disable() @endcode
             @return always 0
            */
            virtual int disable() ;

            /**
             @brief Initialize data recording, add data_record job to the executive scheduler.
             @returns always 0
            */
            virtual int init() ;

            /**
             @brief Write out data recording variables to read back in during restart.
             @returns always 0
            */
            virtual int checkpoint() ;

            /**
             @brief Delete memory allocated in the checkpoint job.
             @returns always 0
            */
            virtual void clear_checkpoint_vars() ;

            /**
             @brief restart the data record group from checkpoint.
             @returns always 0
            */
            virtual int restart() ;

            /**
             @brief Restart data recording, add data_record job to the executive scheduler.
             @returns always 0
            */
            virtual int dmtcp_restart() ;

            /**
             @brief Writes format specific information to the top line of the log header file.
             @returns always 0
            */
            virtual int format_specific_header(std::fstream & outstream) = 0 ;

            /**
             @brief Create and write initial info to log file, implemented in derived groups
             @returns always 0
            */
            virtual int format_specific_init() = 0 ;

            /**
             @brief Transfer data in recording buffer to disk, implemented in derived group classes DRAscii, DRBinary, DRHDF5.
             @returns always 0
            */
            virtual int format_specific_write_data(unsigned int writer_offset) = 0 ;

            /**
             @brief Shutdown loggroup. implemented in derived groups.
             @returns always 0
            */
            virtual int format_specific_shutdown() = 0 ;

            /**
             @brief Create and write out variable data to the log header file needed by data products.
             @returns always 0
            */
            virtual int write_header() ;

            /**
             @brief @userdesc Command to add a variable to be recorded.
             @par Python Usage:
             @code <dr_group>.add_variable("<in_name>" [,"<alias>"]) @endcode
             @param in_name - the name of the variable to record
             @param alias - (optional) the name to call the variable in the log file instead of @e in_name
             @return always 0
            */
            virtual int add_variable(std::string in_name , std::string alias = "" ) ;

            /**
             @brief @userdesc Command to remove a variable from recording.
             @par Python Usage:
             @code <dr_group>.remove_variable("<in_name>") @endcode
             @param in_name - the name of the variable to not record
            */
            virtual void remove_variable(std::string in_name) ;

            /**
             @brief @userdesc Command to remove all variables from recording.
             @par Python Usage:
             @code <dr_group>.remove_all_variable() @endcode
            */
            virtual void remove_all_variables() ;

            /**
             @brief This routine allows users to add variables through an already created REF2 structure.
             @param ref2 - variable reference of the variable to record
             @returns always 0
            */
            virtual int add_variable(REF2 * ref2) ;

            /**
             @brief @userdesc Command to add a change variable (when frequency is DR_Changes or DR_Changes_Step).
             @par Python Usage:
             @code <dr_group>.add_change_variable("<in_name>") @endcode
             @param in_name - the name of the change variable to watch
             @return 0 if successful, -1 if variable not found
            */
            virtual int add_change_variable(std::string in_name) ;

            /**
             @brief Copy group's variable values from memory to recording buffer.
             @param in_time - current simulation time in seconds
             @returns always 0
            */
            virtual int data_record(double in_time) ;

            /**
             @brief Transfer data in recording buffer to disk, implemented in derived group classes DRAscii, DRBinary, DRHDF5.
             @returns always 0
            */
            virtual int write_data(bool must_write = false) ;

            /**
             @brief Clean up and close log file, implemented in derived group classes DRAscii, DRBinary, DRHDF5.
             @returns always 0
            */
            virtual int shutdown() ;

            /**
             @brief The executive scheduler's interface to data_record job.
             @returns always 0
            */
            virtual int call_function( Trick::JobData * curr_job ) ;

            /**
             @brief Executive scheduler interface not currently used.
             @returns always 0
            */
            virtual double call_function_double( Trick::JobData * curr_job ) ;

            /**
             @brief Utility function to convert the variables type to a string.
             @returns always 0
            */
            std::string type_string(int item_type, int item_size) ;

        protected:
            /**
             @brief This routine adds the sys.exec.out.time variable to the data record group
             @returns always 0
            */
            virtual int add_time_variable() ;

            /** Max number of digits to expect per recorded value.\n */
            static const unsigned int record_size = 25; /**< trick_io(**) trick_units(--) */

            /** Data thread condition mutex.  */
            pthread_mutex_t buffer_mutex;    /**< trick_io(**) */

            /** Current time saved in Trick::DataRecordGroup::data_record.\n */
            double curr_time ;          /**< trick_io(*i) trick_units(--) */

    } ;

} ;


#endif
