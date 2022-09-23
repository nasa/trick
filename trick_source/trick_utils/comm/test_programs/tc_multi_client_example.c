
/*
 * tc_client_example.c
 *  This program uses trickcomm "tc" routines to create a client that
 *  connects to a server and writes and reads 10 messages.
 */

#include "../include/tc_proto.h"
#include <string.h>

void sigint_hndlr( int sig );
void stupid( int sig );

TCDevice* device ;

typedef struct {
   int i1 ;
   short s1 ;
   short s2 ;
   char c1 ;
   char c2 ;
   char c3 ;
   char c4 ;
   float f1 ;

   int b1 : 5 ;
   int b2 : 4 ;
   int b3 : 7 ;
   int b4 : 13 ;
   int b5 : 3 ;

   int i2 ;

   double d1 ;
} BUFFER ;

int main( int argc, char *argv[] ) {

   int       ctr;
   int       tc_rc;
   BUFFER    buffer ;
   char      other_tag[256] = "test_tag" ;
   TrickErrorHndlr   err_hndlr;
   int       *me ;
   int       status ;

   if (argc >= 2) {
      if (!strcmp(argv[1] , "-h")) {
         fprintf(stderr,"usage: %s [tag_name]\n",argv[0]);
         exit(0) ;
      }
      else {
         strcpy(other_tag , argv[1]) ;
      }
   }

#ifndef __WIN32__
   /* Register the interupt signal handler. */
   {
     struct sigaction sigact; /* sigaction() sys call parm */
     memset ( &sigact , 0 , sizeof(struct sigaction)) ;
     sigact.sa_flags = 0;
     sigact.sa_handler = (void(*)())sigint_hndlr;
     if ( sigaction( SIGINT, &sigact, NULL ) < 0 )
        perror("sigaction() failed for SIGINT");
#if (__sun)
     sigact.sa_handler = (void(*)())stupid;
     if ( sigaction( SIGALRM, &sigact, NULL ) < 0 )
        perror("sigaction() failed for SIGALRM");
#endif

   }
#endif

   memset((void *)&err_hndlr,'\0',sizeof(TrickErrorHndlr));
   trick_error_init(&err_hndlr,
                    (TrickErrorFuncPtr)NULL,
                    (TrickErrorDataPtr)NULL,
                    TRICK_ERROR_TRIVIAL);

   /* It is possible for multiple multiconnects to try and
      initiate simultaneously causing some of the connects
      to fail.  This happens if all of the clients have the
      same tag and are connecting to the same server.  This is
      the V.R. scenario.  Continue trying to connect
      until successful.
    */
   do {
      device = malloc(sizeof(TCDevice)) ;
      memset((void *)device,'\0',sizeof(TCDevice));
      status = tc_multiconnect(device , "client" , other_tag , &err_hndlr);
      if (status != TC_SUCCESS) {
         fprintf(stderr,"Error from tc_multiconnect... trying again\n");
      }
   } while (status != TC_SUCCESS) ;

   ctr = 1;
   me = ((int *)&buffer.f1 +1) ;
   while( 1 ) {
      while ( (tc_isValid(device)) && (!tc_pending(device)) )
#if __WIN32__
         Sleep(1);
#else
         usleep(1000);
#endif
      if (tc_isValid(device)) {
         tc_rc = tc_read(device,(char *)&buffer,sizeof(buffer));
         if (tc_rc != 0) {
            fprintf(stderr,"client: read msg %d from server:\n",ctr++);
            fprintf(stderr,"        i1 = %08x, s1 = %d, c1 = %d, f1 = %f, d1 = %f\n",
             buffer.i1, buffer.s1, buffer.c1, buffer.f1, buffer.d1 ) ;
            fprintf(stderr,"        b1 = %d , b2 = %d, b3 = %d, b4 = %d, b5 = %d\n",
             buffer.b1, buffer.b2, buffer.b3, buffer.b4, buffer.b5 ) ;
            fprintf(stderr,"        me = %08x\n", *me ) ;
#if __WIN32__
            Sleep(500) ;
#else
            usleep(500000);
#endif
         }
      }
      else {
         fprintf(stderr,"Server side disconnected... exiting\n");
         tc_disconnect(device);
         break ;
      }
   }

} /* end main */

#if (__sun)
/* This bogus handler is in here because the Sun machines
   sometimes generate a bogus SIGALRM from usleeps that
   cause the program to exit.  This handler just catches
   that signal and continues processing
 */

void stupid( int sig ) {

}
#endif

/* Interupt signal handler. */
void sigint_hndlr( int sig )
{

   fprintf(stderr,"client: disconnected...\n" );

   if (tc_isValid(device))
      tc_disconnect(device);

   /* Exit the application. */
   exit( 1 );
}

