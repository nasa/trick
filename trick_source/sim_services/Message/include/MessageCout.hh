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
        (Trick runtime simulation executive parameter definition.) 
    REFERENCE: 
        (((Bailey, R.W, and Paddock, E.J.) (Trick Simulation Environment) 
          (NASA:JSC #37943) 
          (JSC / Engineering Directorate / Automation and Robotics Division)
          (June 1994) (--))) 
    ASSUMPTIONS AND LIMITATIONS:
        ((Only 64 levels of nested input data file inclusion.)) 
    PROGRAMMERS: 
        (((Robert W. Bailey) (LinCom) (4/92) (--) (Realtime)) 
         ((Robert W. Bailey) (LinCom) (6/1/91) (Trick-CR-00000) (Initial Release)))
*/

/* 
 * $Id: MessageCout.hh 1207 2010-10-27 19:50:51Z lin $
 */

/**
 * @file MessageCout.hh
 */

#ifndef _EXECMESSAGECOUT_HH_
#define _EXECMESSAGECOUT_HH_

#include <iostream>
#include "sim_services/Message/include/MessageSubscriber.hh"

namespace Trick {

	/**
	 * This MessageCout is a class that inherits from MessageSubscriber.
	 * It defines a type of MessageSubscriber with its received message sending to the standard output stream.
	 */
    class MessageCout : public MessageSubscriber {

        public:

            /**
             @brief The constructor.
             */
            MessageCout() ;

            /**
             @brief Output header & message to standard output stream.
             */
            virtual void update( unsigned int level , std::string header , std::string message ) ;

    } ;

}

#endif

