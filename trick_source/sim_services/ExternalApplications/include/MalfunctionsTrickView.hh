/*
    PURPOSE: (Class for starting Malfunctions Trick View.)
*/

/*
 * $Id: MalfunctionsTrickView.hh 2126 2012-01-17 22:08:56Z dbankie $
 */

#ifndef _MALFUNCTIONSTRICKVIEW_HH_
#define _MALFUNCTIONSTRICKVIEW_HH_

#include "sim_services/ExternalApplications/include/ExternalApplication.hh"

namespace Trick {

    /**
     * Provides for automatic launching of Malfunctions Trick View.
     *
     * @author Derek Bankieris
     */
    class MalfunctionsTrickView : public ExternalApplication {

        public:
        /** Constructor. */
        MalfunctionsTrickView() ;

        private:

        /**
         * @copydoc ExternalApplication::create_arguments_string
         */
        std::string create_arguments_string();

        /** Prevent SWIG from trying to invoke operator= on ostringstream. */
        MalfunctionsTrickView &operator=(const MalfunctionsTrickView &);

    };

}
#endif
