/*
PURPOSE:
    (floating point number round off macro.)
REFERENCE:
    ((None))
ASSUMPTIONS AND LIMITATIONS:
    ()
PROGRAMMERS:
    (((Robert W. Bailey) (LinCom Corp) (4/92) (--) (Realtime)))
*/

#ifndef ROUNDOFF_H
#define ROUNDOFF_H

#define TRICK_ROUNDOFF_MAX 1.e15

#define ROUNDOFF(nval,res,val)\
{\
   double fraction , real , sval , my_remainder ;\
   if( val < (TRICK_ROUNDOFF_MAX*res) ) { \
      real = (double)((long)val) ;\
      fraction = val - real ;\
      sval = fraction / res ;\
      my_remainder = sval - (double)((long)sval) ;\
      if ( val > 0 ) {\
         if( my_remainder <= 0.5 ) {\
            nval = real + (double)((long)sval)*res ;\
         }\
         else {\
            nval = real + (double)((long)sval+1)*res ;\
         }\
      }\
      else {\
         if( my_remainder >= -0.5 ) {\
            nval = real + (double)((long)sval)*res ;\
         }\
         else {\
            nval = real + (double)((long)sval-1)*res ;\
         }\
      }\
   }\
   else { \
      nval = val;\
   }\
}

#endif
