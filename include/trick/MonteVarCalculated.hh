/*
  PURPOSE:                     (Monte carlo structures)
  REFERENCE:                   (Trick Users Guide)
  ASSUMPTIONS AND LIMITATIONS: (None)
  PROGRAMMERS:                 ((Keith Vetter) (LinCom) (7/2003))
*/

#ifndef MONTEVARCALCULATED_HH
#define MONTEVARCALCULATED_HH

#include <string>

#include "trick/MonteVar.hh"
#include "trick/reference.h"

// This block of code disowns the pointer on the python side so you can reassign
// python variables without freeing the C++ class underneath
#ifdef SWIG
%feature("compactdefaultargs","0") ;
%feature("shadow") Trick::MonteVarCalculated::MonteVarCalculated(std::string name) %{
    def __init__(self, *args):
        this = $action(*args)
        try: self.this.append(this)
        except: self.this = this
        this.own(0)
        self.this.own(0)
%}
#endif

namespace Trick {

    /**
     * A variable whose values are calculated by the user. This class is meant to provide the user with a way to fine tune
     * a Monte Carlo's progression, using results from previous runs to influence the values used in future runs. The value
     * is taken from a simulation variable named by the user. The proper way to update that reference variable is to change
     * its value in a Trick job of type <code>"monte_master_pre"</code> or <code>"monte_master_post"</code>.
     *
     * @author Alex Lin
     * @author Donna Panter
     * @author Derek Bankieris
     *
     * @date August 2010
     */
    class MonteVarCalculated : public Trick::MonteVar {

        protected:
        /** Reference. */
        REF2 * ref2; /**< \n trick_units(--) */

        public:
        /**
         * Constructs a MonteVarCalculated with the specified name and units.
         *
         * @param name the fully qualified name of the simulation variable to which this MonteVarCalculated refers
         * @param unit this variable's units
         */
        MonteVarCalculated(std::string name, std::string unit = "");

        // Describes the various properties of this variable.
        std::string describe_variable();

        protected:
        virtual std::string get_next_value();

    };

};
#endif
