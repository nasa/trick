
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
        virtual ~DrawStuffObject() {}
        void add_object() ;
        virtual int draw() = 0 ;
} ;

#endif
