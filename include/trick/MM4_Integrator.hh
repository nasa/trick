/*******************************************************************************

Purpose:
  (Define class MM4_Integrator.)

Programmers:
  (((David Hammen) (Odyssey) (July 2012) (JEOD_2.2 #462)
    (Joint /JEOD integration framework)))

*******************************************************************************/

#ifndef MM4_INTEGRATOR_HH
#define MM4_INTEGRATOR_HH

#ifdef USE_ER7_UTILS_INTEGRATORS
#include "er7_utils/integration/mm4/include/mm4_integrator_constructor.hh"
#include "er7_utils/trick/integration/include/trick_second_order_ode_integrator.hh"


namespace Trick {

/**
 * Helper class for MM4_Integrator
 */
class MM4_IntegratorHelper {
protected:

   er7_utils::MM4IntegratorConstructor
   helper_integ_constructor; /* --
      The integrator constructor associated with MM4 integration. */
};


/**
 * Integrator using a modified midpoint approach.
 */
class MM4_Integrator :
   protected MM4_IntegratorHelper,
   public er7_utils::TrickSecondOrderOdeIntegrator {

public:

   /** Default constructor, needed by the MemoryManager. */
   MM4_Integrator () {}

   /** Copy constructor, needed for backwards compatibility. */
   MM4_Integrator (const MM4_Integrator & src)
   :
      MM4_IntegratorHelper (src),
      TrickSecondOrderOdeIntegrator (src, helper_integ_constructor)
   {}

   /** Non-default constructor, needed by getIntegrator. */
   MM4_Integrator (int state_size, double delta_t)
   :
      MM4_IntegratorHelper (),
      TrickSecondOrderOdeIntegrator (
         helper_integ_constructor, state_size, delta_t)
   {}

   /** Assignment operator. */
   MM4_Integrator & operator= (MM4_Integrator src)
   {
      helper_integ_constructor = src.helper_integ_constructor;
      TrickSecondOrderOdeIntegrator::swap (src);
      return *this;
   }

   /** Destructor. */
   virtual ~MM4_Integrator() {}

   virtual Integrator_type get_Integrator_type() { return Modified_Midpoint_4; }
};

}
#else

#include "trick/Integrator.hh"
/**
 PURPOSE: (Integrator using Modified Midpoint method.)
 */

namespace Trick {

    class MM4_Integrator : public Integrator {

    public:
        /** Default Constructor. This must remain public so the MM can create these. */
        MM4_Integrator() {};
        MM4_Integrator( int State_size, double Dt);
        virtual ~MM4_Integrator();

        void initialize( int State_size, double Dt);

        int integrate();

        Integrator_type get_Integrator_type() { return(Modified_Midpoint_4); } ;
    };
}
#endif

#endif
