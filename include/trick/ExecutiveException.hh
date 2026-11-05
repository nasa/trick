/*
    PURPOSE:
        (Trick executive exception class )
*/

#ifndef EXEC_EXCEPTION_HH
#define EXEC_EXCEPTION_HH

#include <stdexcept>
#include <string>

namespace Trick {

    /**
     * This class defines the exception thrown by Trick's executive to signal
     * an error or termination condition. It is thrown directly from several
     * Executive methods, as well as by exec_terminate() and
     * exec_terminate_with_return().
     *
     * @author Alexander S. Lin
     *
     */

    class ExecutiveException : public std::exception {

        public:

            /** error code to return */
            int ret_code ;      /**< trick_io(**) */

            /** file name and line number ("file:line") where the error occurred */
            std::string file ;       /**< trick_io(**) */

            /** message associated with error */
            std::string message ;    /**< trick_io(**) */

            /** This constructor assigns ret_code and message to the incoming arguments,
                and sets file to "in_file:line" */
            ExecutiveException( int in_ret , std::string in_file , int line , std::string in_message ) ;
            virtual ~ExecutiveException();
            virtual const char* what() const noexcept;
    } ;

}

#endif
