/*
  PURPOSE:                     (Monte carlo structures)
  REFERENCE:                   (Trick Users Guide)
  ASSUMPTIONS AND LIMITATIONS: (None)
  PROGRAMMERS:                 ((Keith Vetter) (LinCom) (7/2003))
*/

#ifndef MONTEVARFIXED_HH
#define MONTEVARFIXED_HH

#include <string>

#include "MonteVar.hh"

namespace Trick {

    /**
     * A variable whose value is fixed.
     *
     * @author Alex Lin
     * @author Donna Panter
     * @author Derek Bankieris
     *
     * @date August 2010
     */
    class MonteVarFixed : public Trick::MonteVar {

        public:
        /**
         * Constructs a MonteVarFixed with the specified name, value, and units.
         *
         * @param name the fully qualified name of the simulation variable to which this MonteVarFixed refers
         * @param value this variable's value
         * @param unit this variable's units
         */
        MonteVarFixed(std::string name, double value, std::string unit = "");

        protected:
        virtual std::string get_next_value();

    };

};
#endif
