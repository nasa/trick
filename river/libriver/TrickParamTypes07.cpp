#include "TrickParamTypes07.hh"
#include "trick_byteswap.h"

double TrickParamTypes07::toValue(void* address, int type, int size)
{
    double value = 0.0; // return value
    char * cp ;
    unsigned char * ucp ;
    short * sp ;
    unsigned short * usp ;
    int * ip ;
    unsigned int * uip ;
    long * lp ;
    unsigned long * ulp ;
    float * fp ;
    double * dp ;
    long long * llp ;
    unsigned long long * ullp ;

    switch ( type ) {

        case TRK_CHARACTER:
            cp = (char *)(address) ;
            value = (double)*cp ;
            break ;
        case TRK_UNSIGNED_CHARACTER:
            ucp = (unsigned char *)(address) ;
            value = (double)*ucp ;
            break ;
        case TRK_SHORT:
            sp = (short *)(address) ;
            value = (double) (_swap ? trick_byteswap_short(*sp) : *sp) ;
            break ;
        case TRK_UNSIGNED_SHORT:
            usp = (unsigned short *)(address) ;
            value = (double) 
                (_swap ?  trick_byteswap_short(*usp) : *usp) ;
            break ;
        case TRK_ENUMERATED:
        case TRK_INTEGER:
            ip = (int *)(address) ;
            value = (double) (_swap ?  trick_byteswap_int(*ip) : *ip) ;
            break ;
        case TRK_UNSIGNED_INTEGER:
            uip = (unsigned int *)(address) ;
            value = (double) 
                (_swap ? (unsigned int)trick_byteswap_int(*uip) : *uip) ;
            break ;
        case TRK_LONG:
            lp = (long *)(address) ;
            value = (double) (_swap ?  trick_byteswap_long(*lp) : *lp) ;
            break ;
        case TRK_UNSIGNED_LONG:
            ulp = (unsigned long *)(address) ;
            value = (double) (_swap ? (unsigned long)trick_byteswap_long(*ulp) : *ulp) ;
            break ;
        case TRK_FLOAT:
            fp = (float *)(address) ;
            value = (double) (_swap ? trick_byteswap_float(*fp) : *fp) ;
            break ;
        case TRK_DOUBLE:
            dp = (double *)(address) ;
            value = _swap ? trick_byteswap_double(*dp) : *dp ;
            break ;
        case TRK_BITFIELD:
            switch ( size ) {
                case 1 :
                cp = (char *)(address) ;
                value = (double)*cp ;
                break ;
                case 2 :
                sp = (short *)(address) ;
                value = (double) 
                     (_swap ? trick_byteswap_short(*sp) : *sp) ;
                break ;
                case 4 :
                ip = (int *)(address) ;
                value = (double)
                     (_swap ? trick_byteswap_int(*ip) : *ip) ;
                break ;
            }
            break ;
        case TRK_UNSIGNED_BITFIELD:
            switch ( size ) {
                case 1 :
                ucp = (unsigned char *) (address) ;
                value = (double)*ucp ;
                break ;
                case 2 :
                usp = (unsigned short *)(address) ;
                value = (double) (_swap ?  (unsigned short) trick_byteswap_short(*usp) : *usp);
                break ;
                case 4 :
                uip = (unsigned int *)(address) ;
                value = (double) (_swap ?  (unsigned int) trick_byteswap_int(*uip) : *uip) ;
                break ;
            }
            break ;

        case TRK_LONG_LONG:
            llp = (long long *)(address) ;
            value = (double) (_swap ?  trick_byteswap_long_long(*llp) : *llp) ;
            break ;

        case TRK_UNSIGNED_LONG_LONG:
            ullp = (unsigned long long *)(address) ;
            value = (double) (_swap ?  (unsigned long long) trick_byteswap_long_long(*ullp) : *ullp) ;
            break ;

        case TRK_BOOLEAN:
            switch ( size ) {
                case 1 :
                ucp = (unsigned char *)(address) ;
                value = (double)*ucp ;
                break ;
                case 4 :
                ip = (int *)(address) ;
                value = (double) (_swap ? trick_byteswap_int(*ip) : *ip) ;
                break ;
            }
            break ;
    }
    return value;
}
