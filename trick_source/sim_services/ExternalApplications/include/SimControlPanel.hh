/*
    PURPOSE: (Class for starting the Sim Control Panel.)
*/

/*
 * $Id: SimControlPanel.hh 3592 2014-07-01 16:01:00Z hchen3 $
 */

#ifndef _SIMCONTROLPANEL_HH_
#define _SIMCONTROLPANEL_HH_

#include "sim_services/ExternalApplications/include/ExternalApplication.hh"

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
