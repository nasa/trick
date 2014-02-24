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
    A class interface for variables 
    @author Keith Vetter
    @version May 2002
*/


#include <stdio.h>
#include <ctype.h>
#include <float.h>

#include <string>
#include <sstream>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <cstring>
using std::cerr;
using std::cin;
using std::endl;
using std::string;
using std::vector;
using std::stringstream;

struct LevelInfo {
    int startidx;
    int endidx;
    int numnames;
    string name;
};

#include "var.hh"
#ifdef WIN32
#    define strcasecmp strcmpi
#endif

#define VAR_DELETE \
       delete[] str ;  \
       delete[] v1name ; \
       delete[] v2name ; \
       delete[] offsets ; \
       delete[] sizedims ; \
       delete[] sizeproducts ; \
       delete[] v1.dims ; \
       delete[] v2.dims ; \
       for ( i = 0 ; i < nParms ; i++ ) {  \
          delete[] v1.parName[i] ; \
          delete[] v2.parName[i] ; \
       } \
       delete[] v1.parName ; \
       delete[] v2.parName ;

/** Variable Constructor */
Var::Var() {

       _scaleFactor = 1.0 ;
       _bias = 0 ;
       _minRange = -DBL_MAX ;
       _maxRange = DBL_MAX ;
       _varType = VAR_NONE ;
       _unit = new Unit() ;
       _timeUnit = new Unit() ;
       _byteSize = 0 ;
       isProgramOutput = 0 ;

}

/** Var Destructor */
Var::~Var() {

        delete _unit ;
        delete _timeUnit ;
}

/** Copy constructor */
Var::Var(const Var &src) :
               _varName(src._varName),
               _shortName(src._shortName),
               _label(src._label),
               _timeName(src._timeName),
               _timeLabel(src._timeLabel),
               _byteSize(src._byteSize),
               _nDimensions(src._nDimensions),
               _varType(src._varType),
               _minRange(src._minRange),
               _maxRange(src._maxRange),
               _scaleFactor(src._scaleFactor),
               _bias(src._bias),
               _lineStyle(src._lineStyle),
               _lineColor(src._lineColor),
               _symbolStyle(src._symbolStyle),
               _symbolSize(src._symbolSize)
{
// TODO: Is it right to new these?
               _unit = new Unit(*src._unit);
               _timeUnit = new Unit(*src._timeUnit);
}

/**
 * Get number dimensions a variable has e.g. a[8].b[9].c has 72 dimensions
 *
 * varName -must- be set before calling this
 */
int Var::getNumDims() {

       if ( _varName[0] == '\0' ) {
               printf("ERROR: Must set varName before calling getNumDims().\n");
               return( -1 ) ;
       } 

       // numDimensions was previously calculated 
       return ( _nDimensions ) ;
}

/** Get the string name of var e.g. "a[8].b[9].c" */
const char* Var::getVarName(  ) {
       return( _varName.c_str() ) ;
}

const char* Var::getTimeName(  ) {
       return( _timeName.c_str() ) ;
}

string Var::setTimeName(const char * name ) {
       _timeName = name ;
       return( _timeName) ;
}

/**  Get variable name e.g. getVarName() returns the string "a[8].b[9].c" */
double Var::getScaleFactor() {
       return ( _scaleFactor ) ;
}

/** Get variable's bias (offset or shift) 
 *   
 *  bias is the shift value for a variable
 *  So for a list of values 1.0, 2.0, 3.0 with a bias of say 8.0 
 *  My list becomes 9.0, 10.0, 11.0 
 */
double Var::getBias() {
       return ( _bias ) ;
}

/** Get variable's minimal possible value
 *
 */
double Var::getMinRange() {
       return ( _minRange ) ;
}

/** Get variable's maximal possible value
 *
 */
double Var::getMaxRange() {
       return ( _maxRange ) ;
}

/** Get variable's unit 
 *  @see Unit
 */
Unit* Var::getUnit() {
       return ( _unit ) ;
}

Unit* Var::setUnit(const char * _unitname) {
       _unit->setUnitName(_unitname) ;
       return ( _unit ) ;
}

Unit* Var::getTimeUnit() {
       return ( _timeUnit ) ;
}

Unit* Var::setTimeUnit(const char * _unitname) {
       _timeUnit->setUnitName(_unitname) ;
       return ( _timeUnit ) ;
}

/** 
 * Return variable name without dimensions
 * e.g. if varName == a[0-9].b[1-2].c 
 *         a.b.c will be returned
 */
const char* Var::getVarNameNoDims( ) {

       int ii, jj ;
       int len;
       char * temp_name ;
    
       // Reallocate short name
       len = _varName.length() ; 
       temp_name = new char[len + 1] ;
    
       jj = 0 ;
       for ( ii = 0; ii < len ; ii++ ) {
               temp_name[jj] = _varName[ii];
               if ( _varName[ii] == '[' || _varName[ii] == '(' ) {
                       jj--;
                       while ( _varName[ii] != ']' && 
                               _varName[ii] != ')' && ii < len ) {
                               ii++;
                       }
               }
               jj++;
       }
       temp_name[jj] = '\0' ;
       _shortName = temp_name ;
       delete[] temp_name ;
    
       return(_shortName.c_str());
}

/** Compare two names (ignoring dimensions)  Returns 0 if same */
int Var::compareNames( const char* name ) {

       int ret ;

       Var* v = new Var() ;
       v->setType("void");                  // All new vars need a type
       v->setVarName(name) ;
       if ( v->getSize() == -1 ) {
               fprintf(stderr, "ERROR: \"%s\" is not a valid variable name.\n",
                                name);
               return( -1 );
       }

       ret = strcmp(this->getVarNameNoDims(),v->getVarNameNoDims());
       delete v ;
   
       return(ret);
}

/**
 * Returns index offset of name relative to Var (doesn't return byte offset)
 *
 * e.g. Say the variable is a.b[3-24].c[8],
 *      getDimOffset("a.b[7].c[8]") returns 4
 *      since a.b[7] is the 4rth element of a.b[3-24].c[8] .
 *      a.b[3].c[8] is 0th, a.b[4].c[8] is the 1rst etc...
 */ 
int Var::getDimOffset( const char* param ) {

       unsigned int i,j,k ;
       char* str ;
       char* v1name ;
       char* v2name ;

       // Need this for sizing of arrays
       const unsigned int nParms = getNumParms() ;

       enum DIMENSION { MULTI, SINGLE, NONE } ;

       typedef struct  {
               enum DIMENSION dimensional ;
               int lower ;
               int upper ;
       } DIM ;

       struct variable {

               // a[0-1].b[8].c has 3 params -> "a" "b" "c"
               unsigned int  numParams ;

               /* 
                * a[0-1].b[8].c has (0,1),(8,8),(?,?) as dims
                * a, b & c are MULTI, SINGLE & NONE dimensional
                */
               DIM * dims; 
    
               // a[0-1].b[8].c parNames "a[0-1]" "b[8]" & "c"
               char** parName ; 

       } v1, v2 ;

       int offset ;
       int* offsets      = new int[nParms] ;  // Used for calculating offset
       int* sizedims     = new int[nParms];   //             ""
       int* sizeproducts = new int[nParms] ;  //             ""
       v1.dims           = new DIM[nParms] ;
       v2.dims           = new DIM[nParms] ;

       // Allocate parName 2-d array
       v1.parName        = new char*[nParms] ;
       v2.parName        = new char*[nParms] ;
       int len = _varName.size() + 1 ;
       for ( i = 0 ; i < nParms ; i++ ) { 
               v1.parName[i] = new char[len] ;
               v2.parName[i] = new char[len] ;
       }
  
       // Do some allocation
       // Added nParms, since multidims expanded outgrow original string
       // e.g. a[0-2][2-3].b[0-9].c -> a[0-2].[2-3].b[0-9].c
       str    = new char[_varName.size()+1+nParms] ;
       v1name = new char[_varName.size()+1+nParms] ;
       v2name = new char[strlen(param)+1+nParms] ;
  
       // Initialize v1 and v2 strings
       strcpy(v1name, _varName.c_str());
       strcpy(v2name, param);

       /* 
        * To handle multi dim specs, do the following ugliness
        * change things like a[0].b[0-2][9-18][3-4] -> a[0].b[0-2].[9-18].[3-4] 
        * Unfortunately, this was an afterthought, and it was hurried (sorry)
        */
       j = 0 ;
       for ( i = 0 ; i < strlen(v1name) ; i++ ) {
               if ( v1name[i] == ']' && v1name[i+1] == '[' ) {
                       str[j] = ']' ; j++;
                       str[j] = '.' ; j++;
               } else {
                       str[j] = v1name[i] ; j++ ;
               }
       }
       str[j] = '\0' ;
       strcpy(v1name, str);

       j = 0 ;
       for ( i = 0 ; i < strlen(v2name) ; i++ ) {
               if ( v2name[i] == ']' && v2name[i+1] == '[' ) {
                        str[j] = ']' ; j++;
                        str[j] = '.' ; j++;
               } else {
                        str[j] = v2name[i] ; j++ ;
               }
      }
      str[j] = '\0' ;
      strcpy(v2name, str);
    
    
      /* Get param names out of variable name */ 
      /* a[0-1].b[8].c -> "a[0-1]", "b[8]", "c" */
      j = 0 ;
      v1.numParams = 0 ;
      for ( i = 0 ; i < strlen(v1name) ; i++ ) {
               str[j] = v1name[i] ; j++ ;
               if ( v1name[i] == '.' ) {
                       str[j-1] = '\0' ;
                       strcpy(v1.parName[v1.numParams], str);
                       j = 0 ;
                       v1.numParams++ ;
               } 
      }
      str[j] = '\0' ;
      strcpy(v1.parName[v1.numParams], str);
      v1.numParams++ ;
      
      /* Get dimensions for each param name */
      for ( i = 0 ; i < v1.numParams; i++ ) {

               v1.dims[i].dimensional = NONE ;
               v1.dims[i].lower = -1 ;
               v1.dims[i].upper = -1 ;

               for ( j = 0 ; j < strlen(v1.parName[i]) ; j++ ) {

                       if ( v1.parName[i][j] == '[' ) {
                               j++ ;
    
                               /* Assign lower dimension of param */
                               k = 0 ;
                               while ( isdigit( v1.parName[i][j] ) ) {
                                       str[k] = v1.parName[i][j] ;
                                       k++ ; j++ ;
                               }
                               str[k] = '\0' ;
                               v1.dims[i].lower = atoi(str) ;
    
                               /* Handle upper dimension */
                               if ( v1.parName[i][j] == '-' ) {
                                       /* Multi-dimensional */
                                       v1.dims[i].dimensional = MULTI ;
                                       k = 0 ;
                                       j++ ;
                                       while ( isdigit( v1.parName[i][j] ) ) {
                                               str[k] = v1.parName[i][j] ;
                                               k++ ; j++ ;
                                       }
                                       str[k] = '\0' ;
                                       v1.dims[i].upper = atoi(str) ;
                               } else {
                                       /* Single-dimesional */
                                       v1.dims[i].dimensional = SINGLE ;
                                       v1.dims[i].upper = v1.dims[i].lower ;
                               }
                       }
               }
      }
    
      /* Get param names out of variable name */ 
      /* a[0-1].b[8].c -> "a[0-1]", "b[8]", "c" */
      j = 0 ;
      v2.numParams = 0 ;
      for ( i = 0 ; i < strlen(v2name) ; i++ ) {
               str[j] = v2name[i] ; j++ ;
               if ( v2name[i] == '.' ) {
                       str[j-1] = '\0' ;
                       strcpy(v2.parName[v2.numParams], str);
                       j = 0 ;
                       v2.numParams++ ;
               } 
      }
      str[j] = '\0' ;
      strcpy(v2.parName[v2.numParams], str);
      v2.numParams++ ;
      
    
      /* Get dimensions for each param name */
      for ( i = 0 ; i < v2.numParams; i++ ) {
               v2.dims[i].dimensional = NONE ;
               v2.dims[i].lower = -1 ;
               v2.dims[i].upper = -1 ;
               for ( j = 0 ; j < strlen(v2.parName[i]) ; j++ ) {

                       if ( v2.parName[i][j] == '[' ) {
                               j++ ;
    
                               /* Assign lower dimension of param */
                               k = 0 ;
                               while ( isdigit( v2.parName[i][j] ) ) {
                                       str[k] = v2.parName[i][j] ;
                                       k++ ; j++ ;
                               }
                               str[k] = '\0' ;
                               v2.dims[i].lower = atoi(str) ;
    
                               /* Handle upper dimension */
                               if ( v2.parName[i][j] == '-' ) {
                                       /* Multi-dimensional */
                                       v2.dims[i].dimensional = MULTI ;
                                       k = 0 ;
                                       j++ ;
                                       while ( isdigit( v2.parName[i][j] ) ) {
                                               str[k] = v2.parName[i][j] ;
                                               k++ ; j++ ;
                                       }
                                       str[k] = '\0' ;
                                       v2.dims[i].upper = atoi(str) ;
                               } else {
                                       /* Single-dimesional */
                                       v2.dims[i].dimensional = SINGLE ;
                                       v2.dims[i].upper = v2.dims[i].lower ;
                               }
                       }
               }
      }
    
      /* First make sure that v2 is in v1's space */
      for ( i = 0 ; i < v1.numParams ; i++ ) {
               if ( v1.dims[i].dimensional == MULTI ) {
                       if ( v2.dims[i].lower < v1.dims[i].lower || 
                               v2.dims[i].upper > v1.dims[i].upper )  {
                               VAR_DELETE ;
                               return ( -1 ) ; /* v2 not in v1's space */ 
                       } 
               }
               if ( v1.dims[i].dimensional == SINGLE ) {
                       if ( v1.dims[i].lower != v2.dims[i].lower ) {
                               VAR_DELETE ;
                               return ( -1 ) ; /* v2 not in v1's space */ 
                       }
               }
      }
    
      /* Next make sure that v2 is a single dimensional var */
      for ( i = 0 ; i < v2.numParams ; i++ ) {
               if ( v2.dims[i].dimensional == MULTI ) {
                       VAR_DELETE ;
                       return(-1);
               }
      }
    
      /* Calculate offset of v2 into v1's space */
    
      /* First, get all offsets into each dimension & size of each dimension */
      for ( i = 0 ; i < v1.numParams ; i++ ) {
               offsets[i] = v2.dims[i].upper - v1.dims[i].lower ;
               sizedims[i] = v1.dims[i].upper - v1.dims[i].lower + 1 ;
      }
    
      /* 
       * Now go through magical algorithm to get offset 
       * Think of it like an odometer where each cylinder has a different
       * number of numbers, and the numbers on each cylinder can
       * range from any start to stop point
       *
       * say v1 = x0[a0-b0].x1[a1-b1]...xN[aN-bN]
       * say v2 = x0[y0].x1[y1]...xN[yN]
       *
       * let si = ai - bi + 1  (i = 0..N)
       * let oi = yi - ai      (i = 0..N) 
       *
       * offset= sN*sN-1*...s1(o0) + sN*sN-1*...s2(o1) + ... + sN(oN-1) + oN + 1
       * 
       * Then offset = offset - 1 for indexing 
       */ 
       for ( i = 0 ; i < v1.numParams ; i++ ) {
               sizeproducts[i] = 1 ;
               for ( j = i+1 ; j < v1.numParams ; j++ ) {
                       sizeproducts[i] = sizeproducts[i]*sizedims[j] ;
               }
       }
       offset = 0 ;
       for ( i = 0 ; i < v1.numParams ; i++ ) {
               offset = offset + sizeproducts[i]*offsets[i] ;
               if ( offset < 0 ) {
                       fprintf(stderr, "ERROR: Integer overflow "
                                       "calculating offset!\n");
                       return(-1);
               }
       }
    
       VAR_DELETE ;
    
       return ( offset ) ;
}

/**
 * Get byte size of variable e.g.
 *
 *  a[0-5]: 6*(4bytes/double) = 24 bytes in all 
 *  
 */
int Var::getSize(  ) {
       return( _byteSize ) ;
}

/** Set variable name string */
string Var::setVarName( const char* name ) {

       // Reallocate _varName
       _varName = name ;
    
       // Set number of dimensions for variable 
       _nDimensions = _calcNumDimensions();
       _generateVariableNames();
    
       // Set byte size 
       if ( _varType != VAR_NONE ) {
          _byteSize = _calcByteSize();
       }
       
       return( _varName ) ;
}

/** Get variable type (double, float, int ...) */
Var::enumType Var::getType ( ) {
       return( _varType ) ;
}

/** Get variable's type size (double->8, float->4 ...)*/
int Var::getTypeSize () {
   
       // I am avoiding another switch statement on type here
    
       int totalSize ;
       int numDims ;

       // Get total byte size for variable and number of dimensions
       totalSize = _calcByteSize() ;
       numDims   = getNumDims() ; 
   
       // So type size will be total size divided by number of dims
       return ( totalSize/numDims ) ;
}

/** Set variable type (double, float, int...) */
int Var::setType ( char* type ) {
   
       if         ( !strcasecmp(type, "char") ) {
                _varType = VAR_CHAR ;
       } else if  ( !strcasecmp(type, "short") ) {    
                _varType = VAR_SHORT ;
       } else if  ( !strcasecmp(type, "int") ) {    
                _varType = VAR_INT ;
       } else if  ( !strcasecmp(type, "long") ) {    
                _varType = VAR_LONG ;
       } else if  ( !strcasecmp(type, "unsigned_char") ) {    
                _varType = VAR_UNSIGNED_CHAR ;
       } else if  ( !strcasecmp(type, "unsigned_short") ) {    
                _varType = VAR_UNSIGNED_SHORT ;
       } else if  ( !strcasecmp(type, "unsigned_int") ) {    
                _varType = VAR_UNSIGNED_INTEGER ;
       } else if  ( !strcasecmp(type, "unsigned_long") ) {    
                _varType = VAR_UNSIGNED_LONG ;
       } else if  ( !strcasecmp(type, "float") ) {    
                _varType = VAR_FLOAT ;
       } else if  ( !strcasecmp(type, "double") ) {    
                _varType = VAR_DOUBLE ;
       } else if  ( !strcasecmp(type, "long_long") ) {    
                _varType = VAR_LONG_LONG ;
       } else if  ( !strcasecmp(type, "unsigned_long_long") ) {    
                _varType = VAR_UNSIGNED_LONG_LONG ;
       } else if  ( !strcasecmp(type, "void") ) {    
                _varType = VAR_VOID ;
       } else {
                _varType = VAR_NONE ;
                return( -1 ) ;
       }
    
       return ( 0 ) ;
}

/** Calculates and returns -total- bytesize of variable */ 
int Var::_calcByteSize() {

       int size;
       int totalSize;

       // Make sure that num dimensions set (user should never see this)
       if ( _nDimensions < 1 ) {
               printf("ERROR: nDimensions must be set before "
                      "calculating size.\n");
               return( -1 );
       }

       switch ( _varType ) {
               case VAR_CHAR:
                        size = sizeof(char); 
                        break;
               case VAR_SHORT:
                        size = sizeof(short); 
                        break;
               case VAR_INT:
                        size = sizeof(int); 
                        break;
               case VAR_LONG:
                        size = sizeof(long); 
                        break;
               case VAR_UNSIGNED_CHAR:
                        size = sizeof(unsigned char); 
                        break;
               case VAR_UNSIGNED_SHORT:
                        size = sizeof(unsigned short); 
                        break;
               case VAR_UNSIGNED_INTEGER:
                        size = sizeof(unsigned int); 
                        break ;
               case VAR_UNSIGNED_LONG:
                        size = sizeof(unsigned long); 
                        break ;
               case VAR_FLOAT:
                        size = sizeof(float); 
                        break;
               case VAR_DOUBLE:
                        size = sizeof(double); 
                        break;
               case VAR_LONG_LONG:
                        size = sizeof(long long); 
                        break ;
               case VAR_UNSIGNED_LONG_LONG:
                        size = sizeof(unsigned long long); 
                        break ;
               case VAR_VOID:
                        size = 0;
                        break;
               case VAR_NONE:
                        printf("ERROR: Type must be set before "
                               "calculating size.\n");
                        return( -1 );
                        break;
               default:
                        printf("ERROR: Type must be set before "
                               "calculating size.\n");
                        return( -1 );
       }

       totalSize = size*_nDimensions ;
        
       return( totalSize );
}



/* Calculate all variable names e.g.
 *
 * Input a[0-4].b[8-9], 
 * Generates: a[0].b[8], a[0].b[9], a[1].b[8] etc.
 */
void Var::_generateVariableNames() {

    vector<LevelInfo> levels;
    LevelInfo levelinfo;

    int i,j;
    int stringLen;
    char* dim1;
    char* dim2;
 
    // Dims can't be longer than varName
    stringLen = _varName.size();
    dim1 = new char[stringLen+1] ;
    dim2 = new char[stringLen+1] ;
    
    string name;
    for ( i = 0; i < stringLen; i++ ) {
        if (_varName[i] == ']') {
            throw (string("ERROR: Missing open bracket in ") + _varName);
        } else if (_varName[i] == '[') {
            j = 0;
            while ( _varName[i] != ']' ) {
                i++ ;

                // Missing close bracket?
                if ( i == stringLen ) {
                    throw (string("ERROR: Missing close bracket in ") + _varName);
                }

                // Get dimension 1          
                j = 0;
                dim1[0] = '\0';
                while ( isdigit(_varName[i]) ) {
                    dim1[j] = _varName[i] ;
                    i++; j++;

                    if ( i == stringLen  ) {
                        throw (string("ERROR: Missing close bracket in ") + _varName);
                    }
                    if ( j > 7 ) {
                        throw (string("ERROR: Dimension spec too long "));
                    }
                }
                dim1[j] = '\0';

                // Get dimension 2
                j = 0 ;
                if ( _varName[i] == '-' ) {
                    i++ ;

                    // Multi-dimensional
                    while ( isdigit(_varName[i]) ) {
                        dim2[j] = _varName[i] ;
                        i++; j++;

                        if ( i == stringLen  ) {
                            throw (string("ERROR: Missing close bracket in ") + _varName);
                        }
                        if ( j > 7 ) {
                            throw (string("ERROR: Dim spec too long in ") + _varName);
                        }
                           
                    }
                    dim2[j] = '\0';

                } else if ( _varName[i] == ']' ) {   

                    // Single dimensional
                    strcpy(dim2, dim1);
                } else {
                    throw (string("ERROR: Dimension has syntax error with ") + _varName);
                }
            }
            
            if ( dim1[0] == '\0' || dim2[0] == '\0' ) {
                    throw (string("ERROR: Bad syntax in dimension specification for ") + _varName);
            }
 
            // Calculate number of dimensions
            levelinfo.startidx = atoi(dim1);
            levelinfo.endidx   = atoi(dim2);
            levelinfo.numnames = levelinfo.endidx - levelinfo.startidx + 1;
            levelinfo.name     = name;
            levels.push_back(levelinfo);
            name = "";
        } else {
            name += _varName[i];
        }
    }             

    // Trailing parameter part without dimensions
    if (name != "") {
        levelinfo.startidx = 0;
        levelinfo.endidx   = 0;
        levelinfo.numnames = 0;
        levelinfo.name = name;
        levels.push_back(levelinfo);
    }
 
    // Free dims
    delete[] dim1 ;
    delete[] dim2 ;
 
    int totalnames = 1;
    for (unsigned int ii = 0; ii < levels.size(); ii++) {
        if (levels[ii].numnames > 0) {
            totalnames *= levels[ii].numnames;
        }
    }
    _varnames.resize(totalnames);
      
    int blocksize = totalnames;
    for (unsigned int ii = 0; ii < levels.size(); ii++) {
        // terminating portion -- gets repeated in every generated name
        if (levels[ii].numnames == 0) {
            for (int jj = 0; jj < totalnames; jj++) {
                _varnames[jj] += levels[ii].name;
            }
        } else {
            blocksize /= levels[ii].numnames;
            int numblocks = totalnames / blocksize;
            for (int jj = 0; jj < numblocks; jj++) {
                int currentidx = levels[ii].startidx + (jj % levels[ii].numnames);
                for (int kk = 0; kk < blocksize; kk++) {
                    stringstream strm;
                    strm << levels[ii].name << "[" 
                         << currentidx << "]";
                    _varnames[jj*blocksize + kk] += strm.str();
                }
            }
        }
    }
}

/* Calculate and return total number of dimensions of variable e.g.
 *
 * Input a[0-4].b[8-9], 
 * Returns: 5*2 = 10 dimensions
 */
int Var::_calcNumDimensions() {

       int i,j;
       int stringLen;
       char* dim1;
       char* dim2;
       int  d1, d2;
       int  numDims; 
       int  numDimSpecs;
    
       // Dims can't be longer than varName
       stringLen = _varName.size();
       dim1 = new char[stringLen+1] ;
       dim2 = new char[stringLen+1] ;
       
       numDimSpecs = 0 ;
       numDims = 1 ;
       for ( i = 0; i < stringLen; i++ ) {
                
                if ( _varName[i] == ']' ) {
                               cerr << "ERROR: Missing open bracket in"
                                    << _varName << endl ;
                               return( -1 );
                }
    
                if ( _varName[i] == '[' ) {
    
                       j = 0;
                       while ( _varName[i] != ']' ) {
    
                              i++ ;
    
                              // Missing close bracket?
                              if ( i == stringLen ) {
                                       cerr << "ERROR: Missing close bracket in"
                                            << _varName << endl ;
                                       return( -1 ) ;
                              }
    
                              // Get dimension 1          
                              j = 0;
                              dim1[0] = '\0';
                              while ( isdigit(_varName[i]) ) {
                                          
                                      dim1[j] = _varName[i] ;
                                     
                                       i++; j++;
    
                                       if ( i == stringLen  ) {
                                              cerr << "ERROR: Missing close "
                                                   << "bracket in " 
                                                   << _varName << endl ;
                                              return( -1 ) ;
                                       }
                                       if ( j > 7 ) {
                                              printf("ERROR: Dimension spec "
                                                     "too long.\n"); 
                                              return( -1 ) ;
                                       }
                              }
                              dim1[j] = '\0';
    
                              // Get dimension 2
                              j = 0 ;
                              if ( _varName[i] == '-' ) {
    
                                       i++ ;
    
                                       // Multi-dimensional
                                       while ( isdigit(_varName[i]) ) {
                                      
                                               dim2[j] = _varName[i] ;
                                        
                                               i++; j++;
       
                                               if ( i == stringLen  ) {
                                                       cerr << "ERROR: Missing "
                                                            << "close bracket" 
                                                            << " in "
                                                            << _varName
                                                            << endl ;
                                                       return( -1 ) ;
                                               }
                                               if ( j > 7 ) {
                                                       cerr << "ERROR: Dim "
                                                            << "spec too long " 
                                                            << "in "
                                                            << _varName
                                                            << endl ;
                                                       return( -1 ) ;
                                               }
                                      
                                       }
                                       dim2[j] = '\0';
    
                               } else if ( _varName[i] == ']' ) {   
    
                                       // Single dimensional
                                       strcpy(dim2, dim1);
    
                               } else {
                                       cerr << "ERROR: Dimension has syntax "
                                            << "error with "
                                            << _varName << endl ; 
                                       return( -1 ) ;
                               }
                       }
               
                       if ( dim1[0] == '\0' || dim2[0] == '\0' ) {
                               cerr << "ERROR: Bad syntax in dimension "
                                    << "specification for " 
                                    << _varName << endl ;
                               return( -1 );
                       }
    
                       // Calculate number of dimensions
                       d1 = atoi(dim1);
                       d2 = atoi(dim2);
                       numDims = numDims * ( d2 - d1 + 1 ) ;
    
                       // Increment number of dimension specs
                       numDimSpecs++ ;
               }
       }             
    
       // Free dims
       delete[] dim1 ;
       delete[] dim2 ;
    
       return( numDims );
}

/** Return number of parameters in variable e.g.
  * a[0-9].b[2-3].c has 3 parms "a[0-9], b[2-3] & c"
  *
  * For multi-dimensional vars it's weird
  * a[0-9][0-2].b[2-3].c has 4 parms "a[0-9], a[0-2], b[2-3] & c"
  */
int Var::getNumParms() {
  
       unsigned int i ;
       int nParms ;

       if ( _varName[0] == '\0' ) {
               return(0);
       }

       nParms = 1 ;
       for ( i = 0 ; i < _varName.size() ; i++ ) {
               if ( _varName[i] == '.' ) {
                       nParms++ ; 
               }
               if ( _varName[i] == ']' && _varName[i+1] == '['  ) {
                       nParms++ ;
               }        
       }

       return(nParms);
}

/** Set variable's minimum range */
void Var::setMinRange(double minRange) {
       _minRange = minRange ;
}

/** Set variable's maximum range */
void Var::setMaxRange(double maxRange) {
       _maxRange = maxRange ;
}

/** Set variable's scale factor */
void Var::setScaleFactor(double scaleFactor) {
       _scaleFactor = scaleFactor ;
}

/** Set variable's bias @see getBias() */
void Var::setBias(double bias) {
       _bias = bias ;
}

/** Assignment */
const Var& Var::operator= ( Var& right )
{
        if ( this == &right ) {
                fprintf(stderr, "Attempted self assignment in Xvar "
                                "operator=. \n");
                return *this;
        }

        setVarName( right.getVarName() ) ;
        setTimeName( right.getTimeName() ) ;
        setMinRange( right.getMinRange() ) ;
        setMaxRange( right.getMaxRange() ) ;
        setScaleFactor( right.getScaleFactor() ) ;
        setBias( right.getBias() ) ;
        setLabel( right.getLabel() ) ;
        setUnit( right.getUnit()->getUnitName() ) ;
        setTimeUnit( right.getTimeUnit()->getUnitName() ) ;
        isProgramOutput = right.isProgramOutput ;
        _shortName   = right._shortName;
        _timeLabel   = right._timeLabel;
        _byteSize    = right._byteSize;
        _nDimensions = right._nDimensions;
        _varType     = right._varType;
        _lineStyle   = right._lineStyle;
        _lineColor   = right._lineColor;
        _symbolStyle = right._symbolStyle;
        _symbolSize  = right._symbolSize;

        return *this ;  // Allows concatenated assignments
}

/** Get the label of the var */
const char* Var::getLabel(  ) {
       return( _label.c_str() ) ;
}

/** Get the label of the var */
string Var::setLabel(const char * label ) {
       _label = label ;
       return( _label ) ;
}


void Var::setLineStyle(const char * lineStyle)
{
    _lineStyle = lineStyle ;
}

void Var::setLineColor(const char * lineColor)
{
    _lineColor = lineColor ;
}

void Var::setSymbolStyle(const char * symbolStyle)
{
    _symbolStyle = symbolStyle ;
}

void Var::setSymbolSize(const char * symbolSize)
{
    _symbolSize = symbolSize ;
}
