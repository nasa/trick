/*****************************************************************************
PURPOSE: (Send position to server)
*****************************************************************************/

#include "../include/cannon.h"

int cannon_send_position(
      CANNON* C )
{
        tc_write( &C->connection, (char *) &C->pos[0], sizeof(double)) ;
        tc_write( &C->connection, (char *) &C->pos[1], sizeof(double)) ;

        return 0 ;
}
