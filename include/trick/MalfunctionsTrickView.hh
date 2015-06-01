/*
    PURPOSE: (Class for starting Malfunctions Trick View.)
*/

#ifndef MALFUNCTIONSTRICKVIEW_HH
#define MALFUNCTIONSTRICKVIEW_HH

#include "trick/ExternalApplication.hh"

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
