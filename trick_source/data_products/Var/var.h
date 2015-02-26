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

/*
 * $Id: var.h 1831 2011-07-27 21:34:24Z lin $
 */

/**
    A variable class 
    @author Keith Vetter
    @version May 2002
    @see Unit
*/



#ifndef _DP_VAR_H_
#define _DP_VAR_H_

#include <string>
using namespace std ;

#include "units/include/Unit.hh"

/*
 * Variable Class
 */
class Var {

       public:

               Var();
               ~Var();
               Var(const Var &src);

               const Var &operator= (Var& right) ;      // Assignment

               enum  enumType {
                   CHAR, SHORT, INT, LONG, UNSIGNED_CHAR, UNSIGNED_SHORT, 
	            UNSIGNED_INTEGER, UNSIGNED_LONG, FLOAT, DOUBLE, LONG_LONG, 
	            UNSIGNED_LONG_LONG, VOID, NONE }  ;

               // Get Var Info
               const char* getVarName();
               string setVarName(const char*);

               const char* getVarNameNoDims();

               const char* getTimeName();
               string setTimeName(const char*);

               const char* getLabel();
               string setLabel(const char*);

               int   getSize();
               int   getTypeSize();
               int   getNumDims();
               int   getDimOffset(const char*);
               int   getNumParms();

               enumType getType();
               int  setType(char*);

               double getMinRange();
               void setMinRange(double);

               double getMaxRange();
               void setMaxRange(double);

               double getScaleFactor();
               void setScaleFactor(double);

               double getBias();
               void setBias(double);

               Unit* getUnit();      
               Unit* setUnit(const char *);

               Unit* getTimeUnit();      
               Unit* setTimeUnit(const char *);

               // Compare var names (not counting dims)
               int   compareNames( const char* name );

               bool isProgramOutput ;
     
       private:

               int calcNumDimensions();
               int calcByteSize();

               string varName_;   // e.g. ball.state[0-4].out.force[0-5]
               string shortName_; // No dims -> e.g ball.state.out.force
               string label_;      // e.g. "position"

               string timeName_;   // name of time variable e.g. sys.exec.out.time
               string timeLabel_;   // e.g. "time"

               int   byteSize_;  // a[0-5]: 6*(4bytes/double) = 24 bytes in all 
               int   nDimensions_; // a[0-3].b[1-6].c[3] has 4*6*1=24 dimensions

               enum   enumType varType_;
               double minRange_;       // Min value variable can be
               double maxRange_;       // Max value variable can be
               double scaleFactor_;    // Value scale factor (multiple)
               double bias_;           // Value Offset (addition)

               /** @see Unit() */
               Unit*  unit_ ;
               Unit*  timeUnit_ ;



};

#endif
