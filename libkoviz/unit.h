/******************************************************************************r
*                                                                              *
* Trick Simulation Environment Software                                        *
*                                                                              *
* Copyright (c) 1996,1997 LinCom Corporation, Houston, TX                      *
* All rights reserved.                                                         *
*                                                                              *
* Copyrighted by LinCom Corporation and proprietary to it. Any unauthorized    *
* use of Trick Software including source code, object code or executables is   *
* strictly prohibited and LinCom assumes no liability for such actions or      *
* results thereof.                                                             *
*                                                                              *
* Trick Software has been developed under NASA Government Contracts and        *
* access to it may be granted for Government work by the following contact:    *
*                                                                              *
* Contact: Charles Gott, Branch Chief                                          *
*          Simulation and Graphics Branch                                      *
*          Automation, Robotics, & Simulation Division                         *
*          NASA, Johnson Space Center, Houston, TX                             *
*                                                                              *
*******************************************************************************/

#ifndef _DP_UNIT_
#define _DP_UNIT_

#include <QTextStream>
#include <string>
#include <stdexcept>
using namespace std ;

/** \ingroup libdp
    Class used for unit conversions
    @author Keith Vetter
    @version May 2002
*/
class Unit {

      public:

        Unit();
        Unit(const char *name);
        ~Unit();


        int isUnit(const char *);
        const char *getUnitName();
        string setUnitName(const char *);

        // Conversions between units
        static double convert(double value,
                              const QString& from, const QString& to);
        static double convert(double value,
                              const char* from, const char* to);
        double convert(double val, const char *unitStr);
        double convert(double val, Unit * u);
        double convert(const char *unitStr);
        double convert(Unit * u);

        // Are units in same family? e.g. NM == kNcm
        bool canConvert(Unit *);
        //bool canConvert(const char* u); // convenience

      private:

#define NUM_UNIT_TYPES      12  // Types are time,length ...
#define MAX_UNITS_FOR_TYPE  12  // Length has 10 units including null str
#define MAX_LEN_UNIT_STR    32  // Don't do NMs2gdB... for over 32 chars

        static QString _err_string;
        static QTextStream _err_stream;

        int _isPrimitive(const char *);  // Primitives are things like "s", "ft"...
        int _isOperand(const char *);    // Operands are /,*,2,3
        int _getPrimitiveType(const char *);     // Given a primitive string, 
        // this returns
        // the unit type that prim belongs to
        // Types are 0 - NUM_UNIT_TYPES

        double _getPrimitiveConversion(const char *);
        int _getNextPrimitiveOrOperand();        // Scan unit str looking for primitives
        void _resetNextPrimitive();      // Reset scan for GetNextPrimitive

        string _unitName;        // e.g. "lbf", "NM/s2"

        static const char *const
            _unitStrings[NUM_UNIT_TYPES][MAX_UNITS_FOR_TYPE];
        static const double _conversions[NUM_UNIT_TYPES][MAX_UNITS_FOR_TYPE];
        int _currPrimitiveIndex;        // Index used by GetNextPrimitive
        char _currPrimitive[MAX_LEN_UNIT_STR];  // Used by GetNextPrimitive
};
#endif
