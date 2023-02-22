#ifndef TEST_CONNECTION_HH
#define TEST_CONNECTION_HH

#include "trick/ClientConnection.hh"
#include <iostream>
#include <vector>
#include <queue>
#include <string.h>

class TestConnection : public Trick::ClientConnection {
    public: 
        ~TestConnection () {
            for (char * message : binary_messages_written) {
                free (message);
            }
        }
    
        int initialize() {
            valid = true;
        }

        int write (const std::string& message) {
            if (!valid)
                return -1;

            ascii_messages_written.emplace_back(message);
            return 0;
        }

        int write (char * message, int size) {
            if (!valid)
                return -1;

            char * msg_copy = (char *) malloc(size+1);
            memcpy(msg_copy, message, size+1);
            binary_messages_written.push_back(msg_copy);

            return 0;
        }

        std::string read  (int max_len = MAX_CMD_LEN) {
            if (queued_messages.empty()) {
                return "";
            }

            std::string ret = queued_messages.front();
            queued_messages.pop();
            return ret;
        }

        int disconnect () {
            valid = false;
            return 0;
        }

        std::string get_client_tag () {
            return client_tag;
        }

        int set_client_tag(std::string tag) {
            client_tag = tag;
        }

        int set_block_mode (int mode) {
            return 0;
        }


        std::queue <std::string> queued_messages;
        std::vector <std::string> ascii_messages_written;
        std::vector <char *> binary_messages_written;
        

        std::string client_tag;
        bool valid = true;

};


#endif