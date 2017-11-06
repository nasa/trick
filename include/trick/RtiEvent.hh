#ifndef RTIEVENT_H
#define RTIEVENT_H
/*
    PURPOSE: ( Real Time Injector Classes )
*/
#include <string>
#include <sstream>
#include <stdlib.h>

#include "trick/reference.h"
#include "trick/bitfield_proto.h"
#include "trick/memorymanager_c_intf.h"
#include "trick/message_proto.h"
#include "trick/message_type.h"

namespace Trick {

class RtiEventBase {
    public:
        RtiEventBase(REF2 * in_ref): ref(in_ref) {} ;
        virtual ~RtiEventBase() { free(ref) ; }
        virtual void do_assignment(void) = 0 ;
        virtual std::string print_val(void) = 0 ;
        void print_rti(void) ;
        void check_address(void) ;
        REF2  *ref ;
} ;

template <class T>
class RtiEventInfo : public RtiEventBase {
    public:
        RtiEventInfo( REF2 * in_ref, T in_val ) : RtiEventBase(in_ref) , value(in_val) {} ;
        virtual void do_assignment(void) {
            check_address() ;
            if ( ref->address != NULL ) {
                *(T *)ref->address = value ;
            }
        } ;
        virtual std::string print_val() { std::stringstream temp ; temp << value ; return temp.str() ; } ;
        T value ;
} ;

template <class T>
class RtiBitfieldEventInfo : public RtiEventBase {
    public:
        RtiBitfieldEventInfo( REF2 * in_ref, T in_val ) : RtiEventBase(in_ref) , value(in_val) {} ;
        virtual void do_assignment(void) {
            check_address() ;
            if ( ref->address != NULL ) {
                PUT_BITFIELD( ref->address , (int)value, sizeof(T) , ref->attr->index[0].start , ref->attr->index[0].size ) ;
            }
        } ;
        virtual std::string print_val() { std::stringstream temp ; temp << value ; return temp.str() ; } ;
        T value ;
} ;

template <class T>
RtiEventBase * createRtiEvent (char *variable, T value ) {
    Trick::RtiEventBase *rei = NULL ;
    REF2 *ref;

    ref = ref_attributes(variable);

    if (ref != NULL ) {
        //TODO: Ensure we have a scalar value here!
        switch(ref->attr->type) {
            case TRICK_CHARACTER:
                rei = new Trick::RtiEventInfo< char >( ref , (char)value);
                break;
            case TRICK_UNSIGNED_CHARACTER:
                rei = new Trick::RtiEventInfo< unsigned char >( ref , (unsigned char)value);
                break;
            case TRICK_SHORT:
                rei = new Trick::RtiEventInfo< short >( ref , (short)value);
                break;
            case TRICK_UNSIGNED_SHORT:
                rei = new Trick::RtiEventInfo< unsigned short >( ref , (unsigned short)value);
                break;
            case TRICK_INTEGER:
                rei = new Trick::RtiEventInfo< int >( ref , (int)value);
                break;
            case TRICK_UNSIGNED_INTEGER:
                rei = new Trick::RtiEventInfo< unsigned int >( ref , (unsigned int)value);
                break;
            case TRICK_LONG:
                rei = new Trick::RtiEventInfo< long >( ref , (long)value);
                break;
            case TRICK_UNSIGNED_LONG:
                rei = new Trick::RtiEventInfo< unsigned long >( ref , (unsigned long)value);
                break;
            case TRICK_LONG_LONG:
                rei = new Trick::RtiEventInfo< long long >( ref , (long long)value);
                break;
            case TRICK_UNSIGNED_LONG_LONG:
                rei = new Trick::RtiEventInfo< unsigned long long >( ref , (unsigned long long)value);
                break;
            case TRICK_FLOAT:
                rei = new Trick::RtiEventInfo< float >( ref , (float)value);
                break;
            case TRICK_DOUBLE:
                rei = new Trick::RtiEventInfo< double >( ref , (double)value);
                break;
            case TRICK_BOOLEAN:
                rei = new Trick::RtiEventInfo< bool >( ref , (bool)value);
                break;
            case TRICK_BITFIELD:
                switch (ref->attr->size) {
                    case sizeof(char):
                        rei = new Trick::RtiBitfieldEventInfo< char >( ref , (char)value);
                        break ;
                    case sizeof(short):
                        rei = new Trick::RtiBitfieldEventInfo< short >( ref , (short)value);
                        break ;
                    case sizeof(int):
                    default:
                        rei = new Trick::RtiBitfieldEventInfo< int >( ref , (int)value);
                        break ;
                    case sizeof(long long):
                        rei = new Trick::RtiBitfieldEventInfo< long long >( ref , (long long)value);
                        break ;
                }
                break;
            case TRICK_UNSIGNED_BITFIELD:
                switch (ref->attr->size) {
                    case sizeof(unsigned char):
                        rei = new Trick::RtiBitfieldEventInfo< unsigned char >( ref , (unsigned char)value);
                        break ;
                    case sizeof(unsigned short):
                        rei = new Trick::RtiBitfieldEventInfo< unsigned short >( ref , (unsigned short)value);
                        break ;
                    case sizeof(unsigned int):
                    default:
                        rei = new Trick::RtiBitfieldEventInfo< unsigned int >( ref , (unsigned int)value);
                        break ;
                    case sizeof(unsigned long long):
                        rei = new Trick::RtiBitfieldEventInfo< unsigned long long >( ref , (unsigned long long)value);
                        break ;
                }
                break;
            case TRICK_ENUMERATED:
                switch (ref->attr->size) {
                    case sizeof(char):
                        rei = new Trick::RtiEventInfo< char >( ref , (char)value);
                        break ;
                    case sizeof(short):
                        rei = new Trick::RtiEventInfo< short >( ref , (short)value);
                        break ;
                    case sizeof(int):
                    default:
                        rei = new Trick::RtiEventInfo< int >( ref , (int)value);
                        break ;
                    case sizeof(long long):
                        rei = new Trick::RtiEventInfo< long long >( ref , (long long)value);
                        break ;
                }
                break;
            default:
                message_publish(MSG_ERROR, "%s:%s:%d unsupported type %d \n",
                    __FILE__, __func__, __LINE__, ref->attr->type);
                break;
        }
    } else {
        message_publish(MSG_ERROR, "%s:%s:%d Variable Not Found <%s>\n",
            __FILE__, __func__, __LINE__, variable);
    }
    return rei;
}

}

#endif


