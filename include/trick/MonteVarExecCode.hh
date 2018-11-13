/*
  PURPOSE: (Monte carlo structures to inject executable logic based on MC
            dispersions)
  ASSUMPTIONS AND LIMITATIONS:
    (The order in which this type of MonteVar gets added to trick_mc.mc is
     significant.  The purpose of adding code-injection points is to act upon
     the values of dispersed variables; those variables must therefore have
     been dispersed BEFORE this code is executed.  This MonteVar instance must
     be added AFTER any variables used by the code.)
  PROGRAMMERS:                 ((Gary Turner) (OSR) (11/2018))
*/

/*
 * $Id: MonteVarExecCode.hh $
 */

#ifndef _MONTEVAREXECCODE_HH_
#define _MONTEVAREXECCODE_HH_

#include <string>

#include "MonteVar.hh"

#ifdef SWIG
%feature("compactdefaultargs","0") ;
%feature("shadow") Trick::MonteVarExecCode::MonteVarExecCode(std::string name) %{
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
     * A variable whose value specifies a Python filename to be executed
     * IMPORTANT NOTE:
     * The order in which this type of MonteVar gets added to trick_mc.mc is
     * significant.  The purpose of adding code-injection points is to act upon
     * the values of dispersed variables; those variables must therefore have
     * been dispersed BEFORE this code is executed.  This MonteVar instance must
     * be added AFTER any variables used by the code.
     *
     * @author Gary Turner
     *
     * @date November 2018
     */
    class MonteVarExecCode : public Trick::MonteVar {

        public:
        /**
         * Constructs a MonteVarExecCode with the specified filename
         *
         * @param name the address of the Python file relative to the execution directory
         */
        MonteVarExecCode(std::string name);

        // Composite the various elements of this MonteVar.
        virtual std::string describe_variable();

        protected:
        virtual std::string get_next_value();

    };

};
#endif
