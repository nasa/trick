/*
  PURPOSE:                     (Monte carlo structures)
  REFERENCE:                   (Trick Users Guide)
  ASSUMPTIONS AND LIMITATIONS: (None)
  PROGRAMMERS:                 ((Keith Vetter) (LinCom) (7/2003))
*/

#ifndef MONTEVARFIXED_HH
#define MONTEVARFIXED_HH

#include <string>

#include "trick/MonteVar.hh"

// This block of code disowns the pointer on the python side so you can reassign
// python variables without freeing the C++ class underneath
#ifdef SWIG
%feature("compactdefaultargs","0") ;
%feature("shadow") Trick::MonteVarFixed::MonteVarFixed(std::string name, double value) %{
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
        
        void set_value(double in_value);
        
        // Describes the properties of this variable.
        std::string describe_variable();

        protected:
        virtual std::string get_next_value();

    };

};
#endif
