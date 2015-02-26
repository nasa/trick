/**
  @file

PURPOSE:
    (Testing Embedded classes.)
ICG_IGNORE_TYPES:
    ((IgnoreType1) (IgnoreType2))

*******************************************************************************/

/*
 * $Id: BallState.hh 2519 2012-08-09 20:10:44Z alin $
 */

#ifndef _EMBEDDEDCLASSES_HH_
#define _EMBEDDEDCLASSES_HH_

/** @class BallStateInput
    @brief ball state input parameters
 */
class TopClass {
 public:

  unsigned int bf1 : 5 ;
  unsigned int bf2 : 4 ;

  class PublicEmbed{
      public:
          int ii ;
          class PublicEmbed2{
              public:
                  int jj ;
                      class PublicEmbed3{
                          public:
                              int jj ;
                              enum PublicEnum4 {
                                  five ,
                                  six
                                } ;
                      } ;
          } ;
  } ;

  enum PublicEnum {
    one ,
    two
  } ;

  double d;                   /**< trick_units(r) */

  TopClass() {} ;

 private:
  class PrivateEmbed{
      public:
          int ii ;
  } ;

  enum PrivateEnum {
    three ,
    four
  } ;

};

class IgnoreType1 {
    public:
        int ii ;
} ;

class IgnoreType2 {
    public:
        int ii ;
} ;


#endif
