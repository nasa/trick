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
 * $Id: PlaybackFile.hh 1884 2011-08-25 18:36:19Z lin $
 */

/**
 * @file PlaybackFile.hh
 */

#ifndef _EXECPLAYBACKFILE_HH_
#define _EXECPLAYBACKFILE_HH_

#include <string>
#include <iostream>
#include <fstream>
#include "sim_services/Message/include/MessageFile.hh"

namespace Trick {

    /**
     * This PlaybackFile is a class that inherits from MessageFile.
     * It defines a type of MessageSubscriber with its received message sending to a file.
     */
    class PlaybackFile : public MessageFile {

        public:

            PlaybackFile() ;

            /**
             @brief Output message to the file.
             */
            virtual void update( unsigned int level , std::string header , std::string message ) ;
    } ;

}

#endif

