/*
    PURPOSE: (Class for starting Trick Stripchart.)
*/

#ifndef STRIPCHART_HH
#define STRIPCHART_HH

#include "trick/ExternalApplication.hh"

namespace Trick {

    /**
     * Provides for automatic launching of Trick Stripchart.
     *
     * @author Warwick Woodard
     */
    class StripChart : public ExternalApplication {

        public:

        /** Constructor. */
        StripChart() ;

        /**
         * @copydoc ExternalApplication::set_arguments
         */
        void set_arguments(std::string args);

        /**
         @userdesc Command to set a stripchart input filename.
         @param filename - A Non-null string tells Trick to automatically launch
         the stripchart application at execution and to stripchart data from the
         file during execution.
         */
        void set_input_file(std::string filename);

        /**
         @userdesc Command to get the stripchart input filename.
         @return The stripchart input filename string, or Null.
         */
        std::string get_input_file();

        protected:

        /** Input file for strip charting variables. */
        std::string input_file;

        private:

        /** Indicates the the user has specified an input file. */
        bool file_set;

        /**
         * @copydoc ExternalApplication::create_arguments_string
         */
        std::string create_arguments_string();

        /** Prevent SWIG from trying to invoke operator= on ostringstream. */
        StripChart &operator=(const StripChart &);

    };

}
#endif
