/********************************* TRICK HEADER *******************************
PURPOSE:
    (To not be trickified)
LIBRARY DEPENDENCY:
    ((DO_NOT_TRICKIFY.o))
*******************************************************************************/

/* System include files. */
#include <iostream>

/* Model include files. */
#include "DO_NOT_TRICKIFY.hh"

// Default consructor.
Do_Not_Trickify::Do_Not_Trickify()
{
    std::cout << "WE SHOULD NOT BE HERE!" << std::endl;
}

// Destructor.
Do_Not_Trickify::~Do_Not_Trickify()
{
    std::cout << "WE REALLY SHOULD NOT BE HERE!" << std::endl;
}

int main()
{
    Do_Not_Trickify dnt{};
}
