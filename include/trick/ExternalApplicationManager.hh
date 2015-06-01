/*
    PURPOSE: (Manager for external applications.)
*/

#ifndef EXTERNAL_APPLICATION_MANAGER_HH
#define EXTERNAL_APPLICATION_MANAGER_HH

#include <vector>

#include "trick/ExternalApplication.hh"

namespace Trick {

    /**
     * gets the external applications currently under management
     *
     * @return the external applications currently under management
     *
     * @relates Trick::ExternalApplication
     */
    std::vector<ExternalApplication *> &get_external_applications();

    /**
     * adds <code>externalApplication</code> to the list of external applications being managed
     *
     * @param externalApplication the external application to manage
     *
     * @relates Trick::ExternalApplication
     */
    void add_external_application(ExternalApplication &externalApplication);

    /**
     * removes <code>externalApplication</code> from management if it being managed
     *
     * @param externalApplication the external application to stop managing
     *
     * @relates Trick::ExternalApplication
     */
    void remove_external_application(ExternalApplication &externalApplication);

    /**
     * removes all externalApplications from managment
     *
     * @relates Trick::ExternalApplication
     */
    void remove_all_external_applications();

    /**
     * launches all external applications under management
     *
     * @relates Trick::ExternalApplication
     */
    void launch_all_external_applications();

}
#endif
