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

#ifndef MESSAGE_HS_FILE_HH
#define MESSAGE_HS_FILE_HH

#include "trick/MessageFile.hh"
#include <fstream>
#include <iostream>
#include <string>

namespace Trick
{

/**
 * MessageHSFile writes all messages of levels 0-99 to the send_hs file
 */
class MessageHSFile : public MessageFile
{
public:
    /**
     @brief The constructor.
     */
    MessageHSFile();

    /**
     @brief Output message to the file.
     */
    virtual void update(unsigned int level, std::string header, std::string message);
};

} // namespace Trick

#endif
