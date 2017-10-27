/*
  PURPOSE:                     (Monte carlo structures)
  REFERENCE:                   (Trick Users Guide)
  ASSUMPTIONS AND LIMITATIONS: (None)
  PROGRAMMERS:                 ((Keith Vetter) (LinCom) (7/2003))
*/

#ifndef MONTEVARFILE_HH
#define MONTEVARFILE_HH

#include <iostream>
#include <fstream>
#include <string>

#include "trick/MonteVar.hh"

// This block of code disowns the pointer on the python side so you can reassign
// python variables without freeing the C++ class underneath
#ifdef SWIG
%feature("compactdefaultargs","0") ;
%feature("shadow") Trick::MonteVarFile::MonteVarFile(std::string name, std::string file_name, unsigned int column) %{
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
     * A variable whose values are read from a file. Values should be listed in columns. Multiple variables
     * may utilize the same file, and even the same column (resulting in identical values). For example,
     * the following file could be used to populate two MonteVarFiles:
     *
     * @code
     * #values.txt: MonteVarFile variable values
     * 1    10
     * 2    20
     * 3    30
     * 4    40
     * 5    50
     * @endcode
     *
     * One could then create two variables as such:
     *
     * @code
     * MonteVarFile *variable1 = new MonteVarFile(string("ball.obj.state.input.position[0]"), string("RUN_example/values.txt"), 1);
     * MonteVarFile *variable2 = new MonteVarFile(string("ball.obj.state.input.position[1]"), string("RUN_example/values.txt"), 2);
     * @endcode
     *
     * <code>variable1</code>'s values will progress starting from 1, through 2, 3, 4, and 5.
     * <code>variable2</code>'s values will be 10, 20, 30, 40, 50.
     * Note that the column number begins at 1, not 0.
     *
     * @author Alex Lin
     * @author Donna Panter
     * @author Derek Bankieris
     *
     * @date August 2010
     */
    class MonteVarFile : public Trick::MonteVar {

        protected:
        /** The name of the file containing this variable's values. */
        std::string file_name;            /**< \n trick_units(--) */

        /** The column within the file correpsonding to this variable. */
        unsigned int column;              /**< \n trick_units(--) */

        /** The input file stream. */
        std::ifstream *input_file_stream; /**< \n trick_units(--) */

        private:
        // Used to store the current position of the stream for file opening and closing.
        std::streampos stream_position;

        public:
        /**
         * Constructs a MonteVarFile with the specified name, file name, column, and units.
         *
         * @param name the fully qualified name of the simulation variable to which this MonteVarFile refers
         * @param file_name the name of the file containing this variable's values
         * @param column the column (starting at 1)  within the file corresponding to this variable
         * @param unit this variable's units
         */
        MonteVarFile(std::string name, std::string file_name, unsigned int column, std::string unit = "");

        /** Destructor. */
        virtual ~MonteVarFile();
        /**
         * Sets the file name
         *
         * @param in_file_name the name of the file containing this variable's values
         */

        
        void set_file_name(std::string in_file_name);
        
        /**
         * Sets the column
         *
         * @param in_column the column (starting at 1)  within the file corresponding to this variable
         */
        void set_column(unsigned int in_column);
        
        // Describes the various properties of this variable.
        std::string describe_variable();

        protected:
        virtual std::string get_next_value();

    } ;

} ;

#endif
