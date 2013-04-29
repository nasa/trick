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
/** \ingroup libdp
    A variable class 
    @author Keith Vetter
    @version May 2002
    @see Unit
*/



#ifndef _DP_VAR_H_
#define _DP_VAR_H_

#include <string>
#include <vector>

#include "unit.hh"

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
                   VAR_CHAR, VAR_SHORT, VAR_INT, VAR_LONG, VAR_UNSIGNED_CHAR, 
                   VAR_UNSIGNED_SHORT, VAR_UNSIGNED_INTEGER, VAR_UNSIGNED_LONG,
                   VAR_FLOAT, VAR_DOUBLE, VAR_LONG_LONG, VAR_UNSIGNED_LONG_LONG,
                   VAR_VOID, VAR_NONE }  ;

               // Get Var Info
               const char* getVarName();
               std::string setVarName(const char*);

               const char* getVarNameNoDims();

               const char* getTimeName();
               std::string setTimeName(const char*);

               const char* getLabel();
               std::string setLabel(const char*);

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

               const vector<string>& getVariableNames() { return _varnames; }

               void setLineStyle(const char *);
               void setLineColor(const char *);
               void setSymbolStyle(const char *);
               void setSymbolSize(const char *);

               string getLineStyle() { return _lineStyle; }
               string getLineColor() { return _lineColor; }

               // Compare var names (not counting dims)
               int   compareNames( const char* name );

               bool isProgramOutput ;
     
       private:

               int _calcNumDimensions();
               int _calcByteSize();
               void _generateVariableNames();

               std::string _varName;   // e.g. ball.state[0-4].out.force[0-5]
               std::string _shortName; // No dims -> e.g ball.state.out.force
               std::string _label;      // e.g. "position"

               std::string _timeName;   // name of time variable e.g. sys.exec.out.time
               std::string _timeLabel;   // e.g. "time"

               int   _byteSize;  // a[0-5]: 6*(4bytes/double) = 24 bytes in all 
               int   _nDimensions; // a[0-3].b[1-6].c[3] has 4*6*1=24 dimensions

               enum   enumType _varType;
               double _minRange;       // Min value variable can be
               double _maxRange;       // Max value variable can be
               double _scaleFactor;    // Value scale factor (multiple)
               double _bias;           // Value Offset (addition)

               /** @see Unit() */
               Unit*  _unit ;
               Unit*  _timeUnit ;

               std::vector<std::string> _varnames; // Fully expanded set of variable names

               string _lineStyle;
               string _lineColor;
               string _symbolStyle;
               string _symbolSize;
};

#endif
