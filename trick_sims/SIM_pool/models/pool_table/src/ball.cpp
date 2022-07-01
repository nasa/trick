/********************************* TRICK HEADER *******************************
PURPOSE: ( Simulate balls contacting boundaries. )
LIBRARY DEPENDENCY:
    ((ball.o))
*******************************************************************************/
#include "../include/ball.hh"
// #include "trick/memorymanager_c_intf.h"

#include "trick/MemoryManager.hh"
extern Trick::MemoryManager* trick_MM;

#include <new>

Ball::Ball(double x, double y, double mass, double radius, bool isFixed, int id) :
    mass(mass),
    fixed(isFixed),
    id(id)
{
    pos[0] = x;
    pos[1] = y;
    pos[2] = 0;
}

// Ball* CreateBall(double x, double y, double mass, double radius, bool isFixed) {
//     // Ball* b = (Ball*)TMM_declare_var_s("Ball");
//     Ball *b = new Ball(x, y, mass, radius, isFixed, 0);
//     trick_MM->declare_extern_var ( b, "Ball");
//     // Ball *b = (Ball*)trick_MM->declare_var("Ball");

//     // return (new (b) Ball(x, y, mass, radius, isFixed, 0));
//     return b;
// }



