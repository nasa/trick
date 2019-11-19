/*******************************************************************************

Purpose:
  (Define class RK4_Integrator.)

Programmers:
  (((David Hammen) (Odyssey) (July 2012) (JEOD_2.2 #462)
    (Joint /JEOD integration framework)))

*******************************************************************************/

#ifndef RK4_INTEGRATOR_HH
#define RK4_INTEGRATOR_HH

#ifdef USE_ER7_UTILS_INTEGRATORS
#include "er7_utils/integration/rk4/include/rk4_integrator_constructor.hh"
#include "er7_utils/trick/integration/include/trick_first_order_ode_integrator.hh"


namespace Trick {

/**
 * Helper class for RK4_Integrator
 */
class RK4_IntegratorHelper {
protected:

   er7_utils::RK4IntegratorConstructor
   helper_integ_constructor; /* --
      The integrator constructor associated with RK4 integration. */
};


/**
 * Integrator using Runge Kutta 4 method.
 */
class RK4_Integrator :
   protected RK4_IntegratorHelper,
   public er7_utils::TrickFirstOrderOdeIntegrator {

public:

   /** Default constructor, needed by the MemoryManager. */
   RK4_Integrator () {}

   /** Copy constructor, needed for backwards compatibility. */
   RK4_Integrator (const RK4_Integrator & src)
   :
      RK4_IntegratorHelper (src),
      TrickFirstOrderOdeIntegrator (src, helper_integ_constructor)
   {}

   /** Non-default constructor, needed by getIntegrator. */
   RK4_Integrator (int state_size, double delta_t)
   :
      RK4_IntegratorHelper (),
      TrickFirstOrderOdeIntegrator (
         helper_integ_constructor, state_size, delta_t)
   {}

   /** Assignment operator. */
   RK4_Integrator & operator= (RK4_Integrator src)
   {
      helper_integ_constructor = src.helper_integ_constructor;
      TrickFirstOrderOdeIntegrator::swap (src);
      return *this;
   }

   /** Destructor. */
   virtual ~RK4_Integrator() {}

   virtual Integrator_type get_Integrator_type() { return Runge_Kutta_4; }
};

}
#else

#include "trick/Integrator.hh"
/**
 PURPOSE: (Integrator using Runga Kutta 4 method.)
 */

namespace Trick {

    /** Integrator using Runge Kutta 4 method.
     */
    class RK4_Integrator : public Integrator {

    public:
        /** Default Constructor. This must remain public so the MM can create these. */
        RK4_Integrator() {};
        RK4_Integrator( int State_size, double Dt);
        ~RK4_Integrator();

        void initialize( int State_size, double Dt);

        int integrate();

        Integrator_type get_Integrator_type() { return(Runge_Kutta_4); } ;
    };
}
#endif

#endif
