
/*
PURPOSE:
    (Bouncing ball)
LIBRARY DEPENDENCY:
    ((DrawStuffObject.cpp))
*/

#ifndef DRAWSTUFFOBJECT_HH
#define DRAWSTUFFOBJECT_HH

class DrawStuffObject {
    public:
        virtual ~DrawStuffObject() ;
        virtual void add_object() ;
        virtual int draw() = 0 ;
        virtual void command(int cmd) ;
} ;

#endif
