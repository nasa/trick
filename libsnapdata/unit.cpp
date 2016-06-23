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
/**
    File: unit.cpp
    @author Keith Vetter
    @version May 2002
*/

/*
 *
 * This is a very small subset of unit conversions.  I'm looking at this now,
 * in 2013 and see how fragile it is.  It was really the ability to not
 * use operators that made this so messy.  For instance, one can say kgm/s2.
 * If I redo this, I'll use a lexxer.
 *
 * I just looked at this comment in 2014
 * and totally agree that it needs to go,
 * especially in light of Tim using delimiters
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "unit.h"

/**
 * Unit strings
 */
const char *const Unit::_unitStrings[NUM_UNIT_TYPES][MAX_UNITS_FOR_TYPE] = {
        {"s", "min", "hr", "day", "ms", "us", ""},      // Time
        {"m", "M", "ft", "in", "mm", "cm",              // Length
         "km", "yd", "mi", "nm", "kft", ""},
        {"r", "d", "as", "am", "rev", "mr", ""},        // Angle
        {"kg", "sl", "lbm", "g", "mt", ""},             // Mass
        {"N", "kN", "oz", "lbf", ""},                   // Force
        {"v", "kv", ""},                                // Voltage
        {"amp", "mamp", ""},                            // Current
        {"ohm", "mohm", ""},                            // Resistance
        {"C", "K", "R", "F", ""},                       // Temperature
        {"dB", ""},                                     // Sound
        {"J", "mJ", "uJ", "kJ", "MJ", ""},              // Energy
        {"--", "cnt", "one", "1", ""}                   // Unitless
};

/**
 *  Conversion factors 
 */
const double Unit::_conversions[NUM_UNIT_TYPES][MAX_UNITS_FOR_TYPE] = {

        // Time
        {
         1.000000000000,        // s -> s
         60.000000000000,       // s -> min
         3600.000000000000,     // s -> hr
         86400.00000000000,     // s -> day
         0.001000000000,        // s -> ms
         0.000001000000         // s -> us
         },

        // Length
        {
         1.000000000000,        // M -> M 
         1.000000000000,        // M -> m 
         0.3048000000000000,    // M -> ft
         0.0254000000000000,    // M -> in
         0.0010000000000000,    // M -> mm
         0.010000000000000,     // M -> cm
         1000.000000000000,     // M -> km
         0.9143999999999998,    // M -> yd
         1609.344000000000,     // M -> mi
         1852.000000000000,     // M -> nm
         304.80000000000000     // M -> kft
         },

        // Angle
        {
         1.000000000000,        // r -> M 
         0.0174532925199433,    // r -> d 
         4.848136811095362e-06, // r -> as 
         0.0002908882086657216, // r -> am 
         6.28318530717958647,   // r -> rev 
         0.001000000000000      // r -> mr 
         },

        // Mass 
        {
         1.000000000000,        // kg -> kg 
         14.59390293000000,     // kg -> sl 
         0.4535923697760192,    // kg -> lbm 
         0.0010000000000000,    // kg -> g 
         1000.000000000000      // kg -> mt 
         },

        // Force
        {
         1.000000000000,        // N -> N 
         1000.000000000000,     // N -> kN 
         0.2780138509537812,    // N -> oz 
         4.4482216152605        // N -> lbf 
         },

        // Voltage
        {
         1.000000000000,        // v -> v 
         1000.000000000000      // v -> kv
         },

        // Current 
        {
         1.000000000000,        // amp -> amp 
         0.0010000000000000     // amp -> mamp
         },

        // Resistance
        {
         1.000000000000,        // ohm -> ohm 
         0.0010000000000000,    // ohm -> mohm
         0.0                    // ohm -> ""
         },

        // Temperature
        {
         1.000000000000,        // C -> C 
         -273.15,               // C -> K  
         -273.15, 0.555555555555556,    // C -> R 
         -17.7777777777778, 0.555555555555556   // C -> F
         },

        // Sound
        {
         1.000000000000,        // dB -> dB 
         0.0                    // dB -> ""
         },

        // Energy
        {
         1.000000000000,        // J->J
         0.001000000000,        // J->mJ
         0.000000100000,        // J->uJ
         1000.000000000,        // J->kJ
         1000000.000000         // J->MJ
         },

        // Unitless
        {
         1.0,                   // -- -> --
         1.0,                   // -- -> cnt
         1.0,                   // -- -> one
         1.0                    // -- -> 1
         }
};

/**
 * For error exception
 */
QString Unit::_err_string;
QTextStream Unit::_err_stream(&_err_string);

/**
 * Unit Constructor
 */
Unit::Unit()
{
        _unitName =  "--";
}

/**
 * Unit Constructor that takes unit name
 */
Unit::Unit(const char *name)
{
    bool isError = false;

    if (strlen(name) < MAX_LEN_UNIT_STR) {

        if (isUnit(name)) {
            _unitName = name ;
        } else {
            isError = true;
        }
    } else {
        isError = false;
    }

    if ( isError ) {
        _err_stream  << "treat [error]: Unit::Unit(const char* unitName) "
                     << "called with bad or unsupported "
                     <<  "unitName=\"" << name << "\".\n";
        throw std::runtime_error(_err_string.toAscii().constData());
    }
}

/**
 * Unit Destructor
 */
Unit::~Unit()
{
        // Nothing to do
}

/**  Get the unit name string e.g. "in", "mi", "kg" */
const char *Unit::getUnitName()
{
        return (_unitName.c_str());
}

/**  Get the unit name string e.g. "in", "mi", "kg" */
string Unit::setUnitName(const char * name )
{
        _unitName = name ;
        return (_unitName);
}

// Reset scan for GetNextPrimitive
void Unit::_resetNextPrimitive()
{
        _currPrimitiveIndex = 0;
}

//
// In a nutshell, this member takes unitName and passes 
// primitive units AND operands of unitName after successive calls.
//
// E.g. NM/s2 would return N, M, /, s, 2 after 5 successive calls
//
int Unit::_getNextPrimitiveOrOperand()
{

        /*
         * This assumes that the unit name is valid (i.e. it has been checked
         * by IsUnit.
         * I also depends on currPrimitiveIndex.  If there is a need to 
         * redo the primitive scan, then run ResetNextPrimitive
         */

        /*
         * Returns 0 if there is no next primitive (done with name)
         * Otherwise it returns 1
         */

        char str[32];
        int i, j, k;
        int lenUnitName;

        lenUnitName = _unitName.length() ;

        // Have we already scanned the entire unit string ?
        if (_currPrimitiveIndex >= lenUnitName) {
                _currPrimitive[0] = '\0';
                return (0);
        }

        j = 0;
        str[0] = '\0';
        for (i = _currPrimitiveIndex; i < lenUnitName; i++) {

                // Handle "*,/,2,3"
                if (_unitName[i] == '*' || _unitName[i] == '/' ||
                    _unitName[i] == '2' || _unitName[i] == '3') {

                        str[0] = _unitName[i];
                        str[1] = '\0';
                        break;
                }

                str[j] = _unitName[i];
                str[j + 1] = '\0';

                // Found a primitive unit type
                if (_isPrimitive(str)) {

                        // sl,rev,day,dB,min,amp,mm,ms,mohm,min
                        // have conflicts with
                        // s ,r  ,d  ,d ,mi ,am ,m ,m ,m,  ,m
                        if (!strcmp(str, "s")) {
                                k = i + 1;
                                if (k < lenUnitName && _unitName[i + 1] == 'l') {
                                        // Slugs -vs- Seconds conflict 
                                        // (slugs win)
                                        strcpy(str, "sl");
                                        i = i + 1;
                                }
                        } else if (!strcmp(str, "m")) {
                                k = i + 1;
                                if (k < lenUnitName &&
                                        (_unitName[i + 1] == 's' ||
                                         _unitName[i + 1] == 'm' ||
                                         _unitName[i + 1] == 'J' )) {
                                        // ms -vs- s conflict (ms wins)
                                        // or
                                        // mm -vs- m (mm wins)
                                        if ( _unitName[i+1] == 's' ) {
                                            strcpy(str, "ms");
                                        } else if ( _unitName[i+1] == 'J' ) {
                                            // mJ is millijoules, not m*J
                                            strcpy(str, "mJ");
                                        } else {
                                            strcpy(str, "mm");
                                        }
                                        i = i + 1;
                                } else if ( i+2 < lenUnitName &&
                                            _unitName[i+1] == 'i' &&
                                            _unitName[i+2] == 'n' ) {
                                    strcpy(str, "min");
                                    i = i+2;
                                } else {
                                    k = i + 3;
                                        if (k < lenUnitName
                                            && _unitName[i + 1] == 'o'
                                            && _unitName[i + 2] == 'h'
                                            && _unitName[i + 3] == 'm' ) {
                                                // mohm beats m ohm
                                                strcpy(str, "mohm");
                                                i = i + 3;
                                        }
                                }
                        } else if (!strcmp(str, "M")) {
                            k = i + 1;
                            if (k < lenUnitName && _unitName[i + 1] == 'J') {
                                // MJ is Megajoules, not M*J
                                strcpy(str, "MJ");
                                i = i + 1;
                            }
                        } else if (!strcmp(str, "r")) {
                                k = i + 2;
                                if (k < lenUnitName && _unitName[i + 1] == 'e'
                                    && _unitName[i + 2] == 'v') {
                                        // Revolutions -vs- Radians (revs win)
                                        strcpy(str, "rev");
                                        i = i + 2;
                                }
                        } else if (!strcmp(str, "d")) {
                                k = i + 1;
                                if (k < lenUnitName && _unitName[i + 1] == 'B') {
                                        // Decibals -vs- Degrees (dB wins)
                                        strcpy(str, "dB");
                                        i = i + 1;
                                } else {
                                        if (k < lenUnitName
                                            && _unitName[i + 1] == 'a'
                                            && _unitName[i + 2] == 'y') {
                                                // degrees -vs- days (days win)
                                                strcpy(str, "day");
                                                i = i + 2;
                                        }
                                }
                        } else if (!strcmp(str, "mi")) {
                                k = i + 1;
                                if (k < lenUnitName && _unitName[i + 1] == 'n') {
                                        // Minutes -vs- miles (miles wins)
                                        strcpy(str, "min");
                                        i = i + 1;
                                }
                        } else if (!strcmp(str, "am")) {
                                k = i + 1;
                                if (k < lenUnitName && _unitName[i + 1] == 'p') {
                                        // Amp -vs- arc minutes (Amps wins)
                                        strcpy(str, "amp");
                                        i = i + 1;
                                }
                        }
                        // We found it, so break
                        break;

                } else {
                        j++;
                }
        }

        _currPrimitiveIndex = i + 1;
        strcpy(_currPrimitive, str);

        return (1);
}

/**
 * Compare two unit constructs to see if they are the same
 * overall type (same family). Are we comparing apples to apples???
 *
 * Return 1 if in same family, 0 otherwise
 * 
 * e.g. 
 *     1: "kg" & "g" are same type
 *     2: "kgNM/s2" & "gkNcm/hr2" are same type
 *     3: "M" & "s" are not the same since feet & seconds are NOT same family
 *     4: "kgNM/s2" & "gNM/s3" are not same since s2 & s3 different dims
 *     5: "kgNM" & "gNs" are not same type since M & s are diff
 */
bool Unit::_canConvert(Unit * u)
{

        // Assume that units being compared are valid 
        // (IsUnit check already done) 

        int ret1, ret2;

        u->_resetNextPrimitive();
        this->_resetNextPrimitive();

        while (1) {

                ret1 = u->_getNextPrimitiveOrOperand();
                ret2 = this->_getNextPrimitiveOrOperand();

                if (ret1 != ret2) {
                        // u & this have different number of primitive units/ops
                        return (false);
                }

                if (ret1 == 0 && ret2 == 0) {
                        // All comparisons have been done on each unit
                        // They are the same
                        return (true);
                }

                if (!strcmp(u->_currPrimitive, this->_currPrimitive)) {
                        // Identical, so continue go on to next primitives 
                        continue;
                } else {
                        if (_isOperand(u->_currPrimitive)
                            || _isOperand(this->_currPrimitive)) {
                                // Operands weren't identical, so not same type
                                return (false);
                        }

                        if (u->_getPrimitiveType(u->_currPrimitive) ==
                            this->_getPrimitiveType(this->_currPrimitive)) {
                                continue;
                        } else {
                            // This is a complete hack because I'm going to
                            // either rewrite this thing, use the Tricklab
                            // version or get something off the internet
                            //
                            // The hack is  because people use N*m and
                            // by convention write the English unit
                            // "backwards" as ft*lbf
                            //
                            // All this does is check if next operand is *,
                            // and allows commutativity

                            int tPrev0 = u->_getPrimitiveType(u->_currPrimitive);
                            int tPrev1 = _getPrimitiveType(_currPrimitive);

                            ret1 = u->_getNextPrimitiveOrOperand();
                            ret2 = this->_getNextPrimitiveOrOperand();
                            if (ret1 != ret2) return (false);

                            // Next primitive must be * operand
                            if ( strcmp(u->_currPrimitive,"*") ||
                                 strcmp(this->_currPrimitive,"*") ) {
                                return (false);
                            }

                            ret1 = u->_getNextPrimitiveOrOperand();
                            ret2 = this->_getNextPrimitiveOrOperand();
                            if (ret1 != ret2) return (false);

                            int tNext0 = u->_getPrimitiveType(u->_currPrimitive);
                            int tNext1 = _getPrimitiveType(_currPrimitive);

                            if ( tPrev0 == tNext1 && tPrev1 == tNext0 ) {
                                // e.g. N*m ~ ft*lbf which is convertable
                                continue;
                            }

                            return (false);
                        }
                }
        }
}

/**
 * Check and see if something like "NMs2*slkvrev" is valid or not
 *
 * By the way that ugly thing is valid!!!
 */
int Unit::isUnit(const char *unitStr)
{


        char str[32];
        int i, j, k;
        int lenUnitStr;
        int lastPrimitive;      // Index into last char of last primitive parsed
        char prevChar;

        j = 0;
        lenUnitStr = strlen(unitStr);
        for (i = 0; i < lenUnitStr; i++) {

                // Handle "*,/,2,3"
                if (unitStr[i] == '*' || unitStr[i] == '/' ||
                    unitStr[i] == '2' || unitStr[i] == '3') {

                        if (i == 0) {
                                // Error: Units don't start with *,/,2,3
                                return (0);
                        }

                        prevChar = unitStr[i - 1];

                        if (isalpha(prevChar) || prevChar == '1' ) {
                                // OK: E.g. NM2, NMs2/ft, 1/s
                                continue;

                        } else {

                                if (prevChar == '*' || prevChar == '/') {
                                        // Error: e.g. NM**, NM*/, NM/2, NM*3
                                        return (0);
                                }
                                if (prevChar == '2' || prevChar == '3') {
                                        if (unitStr[i] != '/'
                                            && unitStr[i] != '*') {
                                                // Error: e.g. NM22, NM&3
                                                return (0);
                                        } else {
                                                // OK: E.g. NM2/s 
                                                continue;
                                        }
                                }
                        }
                }

                str[j] = unitStr[i];
                str[j + 1] = '\0';

                // Found a primitive unit type
                if (_isPrimitive(str)) {

                        // sl,rev,day,dB,min,amp,mm,ms,mohm
                        // have conflicts with
                        // s ,r  ,d  ,d ,mi ,am ,m ,m ,m
                        if (!strcmp(str, "s")) {
                                k = i + 1;
                                if (k < lenUnitStr && unitStr[i + 1] == 'l') {
                                        // Slugs -vs- Seconds conflict 
                                        // (slugs win)
                                        strcpy(str, "sl");
                                        i = i + 1;
                                }
                        } else if (!strcmp(str, "m")) {
                                k = i + 1;
                                if (k < lenUnitStr &&
                                        (unitStr[i + 1] == 's'||
                                         unitStr[i + 1] == 'm' ||
                                         unitStr[i + 1] == 'J' ) ) {
                                        // ms -vs- m conflict
                                        // mJ -vs- m conflict
                                        // mm -vs- m conflict
                                        if ( unitStr[i+1] == 's' ) {
                                            strcpy(str, "ms");
                                        } else if ( unitStr[i+1] == 'J' ) {
                                            strcpy(str, "mJ");
                                        } else {
                                            strcpy(str, "mm");
                                        }
                                        i = i + 1;
                                } else if ( i+2 < lenUnitStr &&
                                            _unitName[i+1] == 'i' &&
                                            _unitName[i+2] == 'n' ) {
                                    strcpy(str, "min");
                                    i = i+2;
                                } else {
                                        k = i + 3;
                                        if (k < lenUnitStr
                                            && _unitName[i + 1] == 'o'
                                            && _unitName[i + 2] == 'h'
                                            && _unitName[i + 3] == 'm' ) {
                                                // mohm beats m ohm
                                                strcpy(str, "mohm");
                                                i = i + 3;
                                        }
                                }
                        } else if (!strcmp(str, "M")) {
                            k = i + 1;
                            if (k < lenUnitStr && unitStr[i + 1] == 'J') {
                                // MJ is Megajoules, not M*J
                                strcpy(str, "MJ");
                                i = i + 1;
                            }
                        } else if (!strcmp(str, "r")) {
                                k = i + 2;
                                if (k < lenUnitStr && unitStr[i + 1] == 'e'
                                    && unitStr[i + 2] == 'v') {
                                        // Revolutions -vs- Radians (revs win)
                                        strcpy(str, "rev");
                                        i = i + 2;
                                }
                        } else if (!strcmp(str, "d")) {
                                k = i + 1;
                                if (k < lenUnitStr && unitStr[i + 1] == 'B') {
                                        // Decibals -vs- Degrees (dB wins)
                                        strcpy(str, "dB");
                                        i = i + 1;
                                } else {
                                        k = i + 2;
                                        if (k < lenUnitStr
                                            && unitStr[i + 1] == 'a'
                                            && unitStr[i + 2] == 'y') {
                                                // degrees -vs- days (days win)
                                                strcpy(str, "day");
                                                i = i + 2;
                                        }
                                }
                        } else if (!strcmp(str, "mi")) {
                                k = i + 1;
                                if (k < lenUnitStr && unitStr[i + 1] == 'n') {
                                        // Minutes -vs- miles (miles wins)
                                        strcpy(str, "min");
                                        i = i + 1;
                                }
                        } else if (!strcmp(str, "am")) {
                                k = i + 1;
                                if (k < lenUnitStr && unitStr[i + 1] == 'p') {
                                        // Amp -vs- am (Amps wins)
                                        strcpy(str, "amp");
                                        i = i + 1;
                                }
                        }

                        lastPrimitive = i;
                        j = 0;

                } else {
                        j++;
                }
        }

        // Was last primitive counted on end of unit str
        // Or right before a trailing "2 or 3"
        if ((lastPrimitive == lenUnitStr - 1) ||
            ((lastPrimitive == lenUnitStr - 2) &&
             ((unitStr[lenUnitStr - 1] == '2') ||
              (unitStr[lenUnitStr - 1] == '3')))) {
                return (1);
        } else {
                return (0);
        }
}

/** Is unit string primitive e.g. kg - NOT kgM/s2 which is an aggregate */
int Unit::_isPrimitive(const char *unitStr)
{

        int i, j;

        for (i = 0; i < NUM_UNIT_TYPES; i++) {
                for (j = 0; j < MAX_UNITS_FOR_TYPE; j++) {
                        if (_unitStrings[i][j][0] == '\0') {
                                break;
                        }

                        if (!strcmp(unitStr, _unitStrings[i][j])) {
                                return (1);
                        }
                }
        }

        return (0);
}

/**
 * Returns conversion factor for primitive
 *
 * e.g. getPrimitiveConversion("min") returns 60 (sec is base of unit type)
 */
double Unit::_getPrimitiveConversion(const char *unitStr)
{
        int type;
        int i;

        type = _getPrimitiveType(unitStr);
        if (type == -1) {
                return (0.0);
        }

        for (i = 0; i < MAX_UNITS_FOR_TYPE; i++) {
                if (_unitStrings[type][i][0] == '\0') {
                        return (0.0);
                }

                if (!strcmp(unitStr, _unitStrings[type][i])) {
                        return (_conversions[type][i]);
                }
        }

        return(0.0);
}

/**
 * Return type associated with string 
 * 
 * e.g. getPrimitiveType("min") -> returns idx to "sec"
 */
int Unit::_getPrimitiveType(const char *unitStr)
{
        int i, j;

        for (i = 0; i < NUM_UNIT_TYPES; i++) {
                for (j = 0; j < MAX_UNITS_FOR_TYPE; j++) {
                        if (_unitStrings[i][j][0] == '\0') {
                                break;
                        }

                        if (!strcmp(unitStr, _unitStrings[i][j])) {
                                return (i);
                        }
                }
        }

        // Couldn't find it 
        return (-1);
}

/**
 * Is string passed in an operand (*,/,2,3) ?
 */
int Unit::_isOperand(const char *str)
{

        if (strlen(str) == 1 &&
            (str[0] == '/' || str[0] == '*' ||
             str[0] == '2' || str[0] == '3')) {
                return (1);
        } else {
                return (0);
        }
}



double Unit::convert(double value, const char *from, const char *to)
{
    Unit fromUnit(from);
    Unit toUnit(to);

    if ( !fromUnit.isUnit(from) ) {
        _err_stream << "treat [error]: Unit::convert() cannot convert "
                    << " the value " << value << " from unit \"" << from
                    <<  "\" to unit \"" << to << "\" because "
                     << "\"" << from << "\" is not a recognized unit.\n";
        throw std::runtime_error(_err_string.toAscii().constData());
    }
    if ( !toUnit.isUnit(to) ) {
        _err_stream << "treat [error]: Unit::convert() cannot convert "
                    << " the value " << value << " from unit \"" << from
                    <<  "\" to unit \"" << to << "\" because "
                     << "\"" << to << "\" is not a recognized unit.\n";
        throw std::runtime_error(_err_string.toAscii().constData());
    }

    if ( !fromUnit._canConvert(&toUnit) ) {

        _err_stream << "treat [error]: Unit::convert() cannot convert "
                    << " the value " << value << " from unit \"" << from
                    <<  "\" to unit \"" << to << "\" because "
                    << "\"" << from << " and \"" << to
                    << "\" are not in the same family.\n";
        throw std::runtime_error(_err_string.toAscii().constData());
    }

    double scale = fromUnit.convert(&toUnit);

    return(scale*value);
}


/**
 * Compute the scale factor to convert from one unit to another
 *
 * E.g.
 *      u1 = ft/s
 *      u2 = in/hr
 *      
 *   then u1.Convert(&u2) returns 43800 (u1 -> u2) 
 */
double Unit::convert(Unit * u)
{

        int ret1, ret2;
        int lastPrimWasOperand; // 0 or 1
        char lastOperand[2];
        double scale;
        double lastScale;
        double conversion1, conversion2;

        // Are units same basic type
        if (!this->_canConvert(u)) {
                if ( !strcmp(u->getUnitName(), "--") || 
                        !strcmp(this->getUnitName(), "--") ) {
                        // If any of the units are "--" there will be
                        // no conversion
                        return(1.0);
                }

                fprintf(stderr,
                        "ERROR: Unit conversion: Units must be same type.\n");
                fprintf(stderr,
                        "       Incompatible units were \"%s\" and \"%s\" \n",
                        this->_unitName.c_str(), u->_unitName.c_str());
                return (0);
        }
        // Get ready to scan units
        u->_resetNextPrimitive();
        this->_resetNextPrimitive();

        // Initialize operand to multiplication
        lastOperand[0] = '*';
        lastPrimWasOperand = 0;

        // Do conversion !!! 
        scale = 1;
        while (1) {

                // Scanning unit, one primitive at a time
                ret1 = u->_getNextPrimitiveOrOperand();
                if (ret1 != 1) {
                        // Finished scanning over unit
                        break;
                }
                ret2 = this->_getNextPrimitiveOrOperand();

                if (_isOperand(u->_currPrimitive)) {

                        // Show that last primitive parsed was an operand
                        lastPrimWasOperand = 1;

                        if (u->_currPrimitive[0] == '*' ||
                            u->_currPrimitive[0] == '/') {
                                lastOperand[0] = u->_currPrimitive[0];
                                continue;
                        } else {
                                // Handle square & cube   
                                if (u->_currPrimitive[0] == '2') {
                                        // Square
                                        scale = scale * lastScale;
                                } else {
                                        // Cube
                                        scale = scale * lastScale * lastScale;
                                }
                                // After square & cube, operand defaults to '*'
                                lastOperand[0] = '*';
                        }
                } else {

                        // If last primitive parsed wasn't
                        // operand make default of mult
                        // This is part of handling things 
                        // like NMs2 where no '*' present
                        if (!lastPrimWasOperand) {
                                lastOperand[0] = '*';
                        }
                        lastPrimWasOperand = 0;

                        // Get conversion factors primitives
                        conversion1 =
                            u->_getPrimitiveConversion(u->_currPrimitive);
                        conversion2 =
                            this->_getPrimitiveConversion(this->_currPrimitive);

                        if (lastOperand[0] == '*') {
                                lastScale = conversion2 / conversion1;
                        } else {
                                lastScale = conversion1 / conversion2;
                        }
                        scale = scale * lastScale;
                }
        }

        return (scale);
}

/**
 * Convert val from one unit to another
 *
 * E.g.
 *      u1 = ft
 *      u2 = in
 *      
 *   then u1.Convert(3, &u2) returns 36 (inches) 
 */
double Unit::convert(double val, Unit * u)
{

        double scale;

        scale = this->convert(u);

        return (scale * val);

}

/**
 * Convert val from one unit to another (defined by string)
 *
 * E.g.
 *      u1 = ft
 *      u2 = "in"
 *      
 *   then u1.Convert(3, &u2) returns 36 (inches) 
 */
double Unit::convert(double val, const char *unitStr)
{
        Unit u = Unit(unitStr);
        if (!u.isUnit(unitStr)) {
                return (0);
        }

        return (this->convert(val, &u));
}

/**
 * Return conversion factor from one unit to another (defined by string)
 *
 * E.g.
 *      u1 = ft
 *      u2 = "in" 
 *      
 *   then u1.Convert(&u2) returns 12 (inches) 
 */
double Unit::convert(const char *unitStr)
{
        Unit u = Unit(unitStr);
        if (!u.isUnit(unitStr)) {
                return (0);
        }

        return (this->convert(&u));
}
