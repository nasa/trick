/*******************************************************************************

Purpose:
  (Define class RKF78_Integrator.)

Programmers:
  (((David Hammen) (Odyssey) (July 2012) (JEOD_2.2 #462)
    (Joint /JEOD integration framework)))

*******************************************************************************/

#ifndef RKF78_INTEGRATOR_HH
#define RKF78_INTEGRATOR_HH

#ifdef USE_ER7_UTILS_INTEGRATORS
#include "er7_utils/integration/rkf78/include/rkf78_integrator_constructor.hh"
#include "er7_utils/trick/integration/include/first_order_ode_integrator.hh"


namespace Trick {

/**
 * Helper class for RKF78_Integrator
 */
class RKF78_IntegratorHelper {
protected:

   er7_utils::RKFehlberg78IntegratorConstructor
   helper_integ_constructor; /* --
      The integrator constructor associated with RKF78 integration. */
};


/**
 * Integrator using Runge Kutta Fehlberg 7/8 method.
 */
class RKF78_Integrator :
   protected RKF78_IntegratorHelper,
   public er7_utils::TrickFirstOrderOdeIntegrator {

public:

   /** Default constructor, needed by the MemoryManager. */
   RKF78_Integrator () {}

   /** Copy constructor, needed for backwards compatibility. */
   RKF78_Integrator (const RKF78_Integrator & src)
   :
      RKF78_IntegratorHelper (src),
      TrickFirstOrderOdeIntegrator (src, helper_integ_constructor)
   {}

   /** Non-default constructor, needed by getIntegrator. */
   RKF78_Integrator (int state_size, double delta_t)
   :
      RKF78_IntegratorHelper (),
      TrickFirstOrderOdeIntegrator (
         helper_integ_constructor, state_size, delta_t)
   {}

   /** Assignment operator. */
   RKF78_Integrator & operator= (RKF78_Integrator src)
   {
      helper_integ_constructor = src.helper_integ_constructor;
      TrickFirstOrderOdeIntegrator::swap (src);
      return *this;
   }

   /** Destructor. */
   virtual ~RKF78_Integrator() {}

   virtual Integrator_type get_Integrator_type() { return Runge_Kutta_Fehlberg_78; }
};

}
#else

#include "trick/Integrator.hh"
/**
 PURPOSE: (Integrator using Runga Kutta Fehlberg 78 method.)
 */

namespace Trick {

    /** Integrator using Runge Kutta Fehlberg 78 method.
     */
    class RKF78_Integrator : public Integrator {

    public:
        /** Default Constructor. This must remain public so the MM can create these. */
        RKF78_Integrator() {};
        RKF78_Integrator( int State_size, double Dt);
        virtual ~RKF78_Integrator();

        void initialize( int State_size, double Dt);

        int integrate();

        Integrator_type get_Integrator_type() { return(Runge_Kutta_Fehlberg_78); } ;

    };
}
#endif

#endif
