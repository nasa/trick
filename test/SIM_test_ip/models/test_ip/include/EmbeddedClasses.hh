/**
  @file

PURPOSE:
    (Testing Embedded classes.)
ICG_IGNORE_TYPES:
    ((IgnoreType1) (IgnoreType2))

*******************************************************************************/

#ifndef EMBEDDEDCLASSES_HH
#define EMBEDDEDCLASSES_HH

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
