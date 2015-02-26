/*
    PURPOSE:
        (Trick executive exception class )
*/

/*
 * $Id: Exec_exception.hh 2184 2012-02-14 19:37:28Z lin $
 */

#ifndef _EXEC_EXCEPTION_HH_
#define _EXEC_EXCEPTION_HH_

#include <stdexcept>
#include <string>

namespace Trick {

    /**
     * This class defines the exception created by C binded
     * exec_terminate_with_return(int , const char *, int , const_char *)
     *
     * @author Alexander S. Lin
     *
     */

    class Exec_exception : public std::exception {

        public:

            /** error code to return */
            int ret_code ;      /**< trick_io(**) */

            /** file name the error occurred */
            std::string file ;       /**< trick_io(**) */

            /** message associated with error */
            std::string message ;    /**< trick_io(**) */

            /** This constructor assignes ret_code, file, and message to the incoming arguments */
            Exec_exception( int in_ret , std::string in_file , int line , std::string in_message ) ;
            virtual ~Exec_exception() throw () ;
            virtual const char* what() const throw() ;
    } ;

}

#endif

