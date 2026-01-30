/*
    PURPOSE:
        (Track custom log files.)
    ASSUMPTIONS AND LIMITATIONS:
        (())
    PROGRAMMERS:
        (((Jackie Deans) (CACI) (4/23) (--) ())
*/

#ifndef MESSAGE_CUSTOM_MANAGER_HH
#define MESSAGE_CUSTOM_MANAGER_HH

#include "trick/MessageCustomFile.hh"
#include "trick/MessageSubscriber.hh"
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

namespace Trick
{

/**
 * MessageCustomFile writes all messages its level to the given file
 */
class MessageCustomManager : public MessageSubscriber
{
public:
    /**
     @brief The constructor.
     */
    MessageCustomManager();
    virtual ~MessageCustomManager();

    virtual int open_custom_message_file(std::string file_name, std::string subscriber_name, int level = -1);

    /**
     @brief Output message to the file.
     */
    virtual void update(unsigned int level, std::string header, std::string message);

    int restart();

protected:
    std::vector<MessageCustomFile *> _custom_message_files;
};

} // namespace Trick

#endif
