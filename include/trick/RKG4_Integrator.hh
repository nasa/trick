/*******************************************************************************

Purpose:
  (Define class RKG4_Integrator.)

Programmers:
  (((David Hammen) (Odyssey) (July 2012) (JEOD_2.2 #462)
    (Joint /JEOD integration framework)))

*******************************************************************************/

#ifndef RKG4_INTEGRATOR_HH
#define RKG4_INTEGRATOR_HH

#ifdef USE_ER7_UTILS_INTEGRATORS
#include "er7_utils/integration/rkg4/include/rkg4_integrator_constructor.hh"
#include "er7_utils/trick/integration/include/trick_first_order_ode_integrator.hh"


namespace Trick {

/**
 * Helper class for RKG4_Integrator
 */
class RKG4_IntegratorHelper {
protected:

   er7_utils::RKGill4IntegratorConstructor
   helper_integ_constructor; /* --
      The integrator constructor associated with RKG4 integration. */
};


/**
 * Integrator using Runge Kutta Gill 4 method.
 */
class RKG4_Integrator :
   protected RKG4_IntegratorHelper,
   public er7_utils::TrickFirstOrderOdeIntegrator {

public:

   /** Default constructor, needed by the MemoryManager. */
   RKG4_Integrator () {}

   /** Copy constructor, needed for backwards compatibility. */
   RKG4_Integrator (const RKG4_Integrator & src)
   :
      RKG4_IntegratorHelper (src),
      TrickFirstOrderOdeIntegrator (src, helper_integ_constructor)
   {}

   /** Non-default constructor, needed by getIntegrator. */
   RKG4_Integrator (int state_size, double delta_t)
   :
      RKG4_IntegratorHelper (),
      TrickFirstOrderOdeIntegrator (
         helper_integ_constructor, state_size, delta_t)
   {}

   /** Assignment operator. */
   RKG4_Integrator & operator= (RKG4_Integrator src)
   {
      helper_integ_constructor = src.helper_integ_constructor;
      TrickFirstOrderOdeIntegrator::swap (src);
      return *this;
   }

   /** Destructor. */
   virtual ~RKG4_Integrator() {}

   virtual Integrator_type get_Integrator_type() { return Runge_Kutta_Gill_4; }
};

}
#else

#include "trick/Integrator.hh"
/**
 PURPOSE: (Integrator using Runga Kutta Gill 4 method.)
 */
namespace Trick {

     /** Integrator using Runge Kutta Gill 4th Order method.
      */
    class RKG4_Integrator : public Integrator {

    public:
        /** Default Constructor. This must remain public so the MM can create these. */
        RKG4_Integrator() {};
        RKG4_Integrator( int State_size, double Dt);
        virtual ~RKG4_Integrator();

        void initialize( int State_size, double Dt);

        int integrate();

        Integrator_type get_Integrator_type() { return(Runge_Kutta_Gill_4); } ;
    };
}
#endif

#endif
