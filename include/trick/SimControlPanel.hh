/*
    PURPOSE: (Class for starting the Sim Control Panel.)
*/

#ifndef SIMCONTROLPANEL_HH
#define SIMCONTROLPANEL_HH

#include "trick/ExternalApplication.hh"

namespace Trick {

    /**
     * Provides for automatic launching of the Sim Control Panel.
     *
     * @author Derek Bankieris
     */
    class SimControlPanel : public ExternalApplication {

        public:
        /** Constructor. */
        SimControlPanel() ;

        /** Toggle to automatically exit sim control panel when sim is done/shutdown.\n */
        bool auto_exit;		/**<  trick_units(--) */

        private:

        /**
         * @copydoc ExternalApplication::create_arguments_string
         */
        std::string create_arguments_string();

        /** Prevent SWIG from trying to invoke operator= on ostringstream. */
        SimControlPanel &operator=(const SimControlPanel &);

    };

}
#endif
