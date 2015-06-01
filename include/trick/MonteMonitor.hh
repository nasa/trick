/*
    PURPOSE: (Class for starting Monte Monitor.)
*/

#ifndef MONTEMONITOR_HH
#define MONTEMONITOR_HH

#include "trick/ExternalApplication.hh"

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
