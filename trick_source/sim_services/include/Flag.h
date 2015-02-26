/*******************************************************************************
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
PURPOSE:
    (Flag enumerated typedef definition.)
REFERENCE:
    (((Bailey, R.W.)
      (Trick Simulation Environment Simulation and Math Model Developer's Guide)
      (NASA:JSC #......)
      (JSC / Engineering Directorate / Automatin & Robotics Division)
      (July 1, 1991) (--)))
ASSUMPTIONS AND LIMITATIONS:
   ((None))
PROGRAMMERS:
    (((Robert W. Bailey) (LinCom) (4/92) (--) (Realtime)))
*/

/*
 * $Log: Flag.h,v $
 * Revision 5.2  2005-03-08 14:43:54-06  vetter
 * 8 space indent all code
 *
 * Revision 5.1  2004-08-05 13:08:44-05  lin
 * Bump
 *
 * Revision 4.1  2003/10/21 21:47:43  lin
 * Bump version number for 04
 *
 * Revision 3.4  2003/06/11 20:39:33  lin
 * Reverted from version 3.2
 *
 * Revision 3.2  2002/07/10 21:02:52  lin
 * Change TRICK_HOST_CPU for Linux and reduce TRICK_CFLAGS
 *
 *
 */

#ifndef FLAG_H_DEFINED
#define FLAG_H_DEFINED

/*=== Protect against 'TRUE' and 'FALSE' already being '#define'd ===*/
#ifdef TRUE
#undef TRUE
#define TRUE_RESET
#endif

#ifdef FALSE
#undef FALSE
#define FALSE_RESET
#endif

#ifdef True
#undef True
#define True_RESET
#endif

#ifdef False
#undef False
#define False_RESET
#endif

#ifdef __cplusplus
extern "C" {
#endif

        typedef enum {

                Off = 0,        
                On = 1,         
                No = 0,         
                Yes = 1,        
                False = 0,      
                True = 1,       
                Disconnect = 0, 
                Connect = 1,    
                Inactive = 0,   
                Active = 1,     
                Stop = 0,       
                Go = 1,        
                Low = 0,      
                High = 1,    
                Disable = 0,
                Enable = 1,
                TRUE = 255,     /* FORTRAN Logical TRUE */
                FALSE = 0,      /* FORTRAN Logical FALSE */
                Open = 0,       
                Closed = 1,    

                /* Stamps flags */
                OFF = 0,       
                ON = 1,       
                NO = 0,      
                YES = 1,    
                INACTIVE = 0,   
                ACTIVE = 1,    
                BACKWARD = 0, 
                FORWARD = 1, 
                BAD = 0,    
                GOOD = 1   
        } Flag;

#ifdef __cplusplus
}
#endif
typedef Flag LOGICAL_TYPE;

#ifdef TRUE_RESET
#undef TRUE_RESET
#define TRUE 1
#endif
#ifdef FALSE_RESET
#undef FALSE_RESET
#define FALSE 0
#endif

#ifdef True_RESET
#undef True_RESET
#define True 1
#endif
#ifdef False_RESET
#undef False_RESET
#define False 0
#endif

#endif
