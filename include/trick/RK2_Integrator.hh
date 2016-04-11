/*******************************************************************************

Purpose:
  (Define class RK2_Integrator.)

Programmers:
  (((David Hammen) (Odyssey) (July 2012) (JEOD_2.2 #462)
    (Joint /JEOD integration framework)))

*******************************************************************************/

#ifndef RK2_INTEGRATOR_HH
#define RK2_INTEGRATOR_HH

#ifdef USE_ER7_UTILS_INTEGRATORS
#include "er7_utils/integration/rk2_heun/include/rk2_heun_integrator_constructor.hh"
#include "er7_utils/trick/integration/include/first_order_ode_integrator.hh"


namespace Trick {

/**
 * Helper class for RK2_Integrator
 */
class RK2_IntegratorHelper {
protected:

   er7_utils::RK2HeunIntegratorConstructor
   helper_integ_constructor; /* --
      The integrator constructor associated with RK2 integration. */
};


/**
 * Integrator using Runge Kutta 2 method.
 */
class RK2_Integrator :
   protected RK2_IntegratorHelper,
   public  er7_utils::TrickFirstOrderOdeIntegrator {

public:

   /** Default constructor, needed by the MemoryManager. */
   RK2_Integrator () {}

   /** Copy constructor, needed for backwards compatibility. */
   RK2_Integrator (const RK2_Integrator & src)
   :
      RK2_IntegratorHelper (src),
      TrickFirstOrderOdeIntegrator (src, helper_integ_constructor)
   {}

   /** Non-default constructor, needed by getIntegrator. */
   RK2_Integrator (int state_size, double delta_t)
   :
      RK2_IntegratorHelper (),
      TrickFirstOrderOdeIntegrator (
         helper_integ_constructor, state_size, delta_t)
   {}

   /** Assignment operator. */
   RK2_Integrator & operator= (RK2_Integrator src)
   {
      helper_integ_constructor = src.helper_integ_constructor;
      TrickFirstOrderOdeIntegrator::swap (src);
      return *this;
   }

   /** Destructor. */
   virtual ~RK2_Integrator() {}

   virtual Integrator_type get_Integrator_type() { return Runge_Kutta_2; }
};

}
#else

#include "trick/Integrator.hh"
namespace Trick {

    /** Integrator using Runga Kutta 2 method.
     */
    class RK2_Integrator : public Integrator {

    public:
        /** Default Constructor. This must remain public so the MM can create these. */
        RK2_Integrator() {};
        RK2_Integrator( int State_size, double Dt);
        virtual ~RK2_Integrator();

        void initialize( int State_size, double Dt);

        int integrate();

        Integrator_type get_Integrator_type() { return(Runge_Kutta_2); } ;
    };
}

#endif

#endif
