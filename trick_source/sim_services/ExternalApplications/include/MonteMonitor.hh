/*
    PURPOSE: (Class for starting Monte Monitor.)
*/

/*
 * $Id: MonteMonitor.hh 2126 2012-01-17 22:08:56Z dbankie $
 */

#ifndef _MONTEMONITOR_HH_
#define _MONTEMONITOR_HH_

#include "sim_services/ExternalApplications/include/ExternalApplication.hh"

namespace Trick {

    /**
     * Provides for automatic launching of Monte Monitor.
     *
     * @author Derek Bankieris
     */
    class MonteMonitor : public ExternalApplication {

        public:
        /** Constructor. */
        MonteMonitor();

        private:

        /** Prevent SWIG from trying to invoke operator= on ostringstream. */
        MonteMonitor &operator=(const MonteMonitor &);

    };

}
#endif
