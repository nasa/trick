/*
 * $Id: MessageSubscriber.cpp 3553 2014-06-11 19:26:39Z alin $
 */

#include "sim_services/Message/include/MessageSubscriber.hh"

Trick::MessageSubscriber::MessageSubscriber() :
 enabled(true) ,
 color(1) {}

int Trick::MessageSubscriber::set_enabled(bool yes_no) {
    enabled = yes_no ;
    return(0) ;
}

int Trick::MessageSubscriber::set_color(bool yes_no) {
    color = yes_no ;
    return(0) ;
}
