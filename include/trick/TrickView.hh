/*
    PURPOSE: (Class for starting Trick View.)
*/

#ifndef TRICKVIEW_HH
#define TRICKVIEW_HH

#include <string>
#include "trick/ExternalApplication.hh"

namespace Trick {

    /**
     * Provides for automatic launching of Trick View.
     *
     * @author Derek Bankieris
     */
    class TrickView : public ExternalApplication {

        public:

        /** Constructor. */
        TrickView();

        /**
         * @copydoc ExternalApplication::set_arguments
         */
        void set_arguments(std::string args);

        /**
         * Sets whether or not only strip charts should be displayed.
         *
         * @param strip_charts_only whether or not only strip charts should be displayed
         */
        void set_strip_charts_only(bool strip_charts_only);

        /**
         * Gets whether or not only strip charts should be displayed.
         *
         * @return whether or not only strip charts should be displayed
         */
        bool get_strip_charts_only();

        /**
         * Sets the file to be automatically loaded when this application launches.
         *
         * @param file the file to be automatically loaded when this application launches
         */
        void set_auto_open_file(std::string file);

        /**
         * Gets the file to be automatically loaded when this application launches.
         *
         * @return the file to be automatically loaded when this application launches
         */
        std::string get_auto_open_file();

        /**
         * Sets the file to be automatically loaded and set when this application launches.
         *
         * @param file the file to be automatically loaded and set when this application launches
         */
        void set_auto_open_and_set_file(std::string file);

        /**
         * Gets the file to be automatically loaded and set when this application launches.
         *
         * @return the file to be automatically loaded and set when this application launches
         */
        std::string get_auto_open_and_set_file();

        /**
         * Sets the file to be automatically set when this application launches.
         *
         * @param file the file to be automatically set when this application launches
         */
        void set_auto_set_file(std::string file);

        /**
         * Gets the file to be automatically set when this application launches.
         *
         * @return the file to be automatically set when this application launches
         */
        std::string get_auto_set_file();

        private:

        bool strip_charts_only;
        bool strip_charts_only_set;

        std::string open_file;
        bool open_file_set;

        std::string open_set_file;
        bool open_set_file_set;

        std::string set_file;
        bool set_file_set;

        /**
         * @copydoc ExternalApplication::create_arguments_string
         */
        std::string create_arguments_string();

        /** Prevent SWIG from trying to invoke operator= on ostringstream. */
        TrickView &operator=(const TrickView &);

    };

}
#endif
