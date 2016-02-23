/*****************************************************************************
PURPOSE: (Initialize communications with a server) 
*****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include "../include/cannon.h"
#include "sim_services/include/release.h"

int cannon_init_graphics(
      CANNON* C )
{
        int ret, num_attempts ;

        /* Launch graphics server */
        system("cd $HOME/trick_models/cannon/graphics ; cannon &");

        /* Initialize clien't connection to server */
        C->connection.port = 9000 ;
        C->connection.hostname = (char*) malloc( 16 ) ;
        strcpy( C->connection.hostname, "localhost");

        /* Shutup status messages */
        tc_error( &C->connection, 0 ) ;

        /* Client connect to server : try for 5 seconds */
        num_attempts = 0 ;
        while ( 1 ) {

                ret = tc_connect( &C->connection ) ;
                if ( ret == TC_SUCCESS ) {
                        break ;
                } else {
                        num_attempts++ ;
                        if ( num_attempts == 500000 ) {
                                fprintf(stderr, "Couldn't connect to "
                                                 "server...\n");
                                exit(-1);
                        }
                        RELEASE_1(); /* Pause a microsecond */
                }
        }

        return 0 ; 
}
