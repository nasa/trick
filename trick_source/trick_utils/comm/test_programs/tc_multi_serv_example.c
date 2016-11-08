
/*
 * tc_client_example.c
 *  This program uses trickcomm "tc" routines to create a client that
 *  connects to a server and writes and reads 10 messages.
 */


#include "../include/attributes.h"
#include "../include/tc_proto.h"
#include <string.h>


void sigint_hndlr( int sig );
void stupid( int sig );

TCDevice** device ;
TrickErrorHndlr   err_hndlr;
int num_devices ;
char other_tag[256] = "test_tag";
pthread_t thread_id ;

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

ATTRIBUTES attrBUFFER[] = {
{ "i1", "int", "--", "",
  "Not Specified",
  3,5,sizeof(int),0,0,Language_C,0,
  0,(char*)0, 0 } ,
{ "s1", "short", "--", "",
  "Not Specified",
  3,3,sizeof(short),0,0,Language_C,0,
  0,(char*)0, 0 } ,
{ "s2", "short", "--", "",
  "Not Specified",
  3,3,sizeof(short),0,0,Language_C,0,
  0,(char*)0, 0 } ,
{ "c1", "char", "--", "",
  "Not Specified",
  3,0,sizeof(char),0,0,Language_C,0,
  0,(char*)0, 0 } ,
{ "c2", "char", "--", "",
  "Not Specified",
  3,0,sizeof(char),0,0,Language_C,0,
  0,(char*)0, 0 } ,
{ "c3", "char", "--", "",
  "Not Specified",
  3,0,sizeof(char),0,0,Language_C,0,
  0,(char*)0, 0 } ,
{ "c4", "char", "--", "",
  "Not Specified",
  3,0,sizeof(char),0,0,Language_C,0,
  0,(char*)0, 0 } ,
{ "f1", "float", "--", "",
  "Not Specified",
  3,9,sizeof(float),0,0,Language_C,0,
  0,(char*)0, 0 } ,
{ "b1", "int", "--", "",
  "Not Specified",
  3,11,0,0,0,Language_C,0,
  0,(char*)0, 0,{{5,27}} } ,
{ "b2", "int", "--", "",
  "Not Specified",
  3,11,0,0,0,Language_C,0,
  0,(char*)0, 0,{{4,23}} } ,
{ "b3", "int", "--", "",
  "Not Specified",
  3,11,0,0,0,Language_C,0,
  0,(char*)0, 0,{{7,16}} } ,
{ "b4", "int", "--", "",
  "Not Specified",
  3,11,0,0,0,Language_C,0,
  0,(char*)0, 0,{{13,3}} } ,
{ "b5", "int", "--", "",
  "Not Specified",
  3,11,0,0,0,Language_C,0,
  0,(char*)0, 0,{{3,0}} } ,
{ "i2", "int", "--", "",
  "Not Specified",
  3,5,sizeof(int),0,0,Language_C,0,
  0,(char*)0, 0 } ,
{ "d1", "double", "--", "",
  "Not Specified",
  3,10,sizeof(double),0,0,Language_C,0,
  0,(char*)0, 0 } ,
{ "" }
} ;

void init_attrBUFFER() {

   int i = 0;
   static int initialized ;

   if ( initialized ) {
      return ;
   }
   initialized = 1 ;

   attrBUFFER[i++].offset = offsetof(BUFFER,i1) ;
   attrBUFFER[i++].offset = offsetof(BUFFER,s1) ;
   attrBUFFER[i++].offset = offsetof(BUFFER,s2) ;
   attrBUFFER[i++].offset = offsetof(BUFFER,c1) ;
   attrBUFFER[i++].offset = offsetof(BUFFER,c2) ;
   attrBUFFER[i++].offset = offsetof(BUFFER,c3) ;
   attrBUFFER[i++].offset = offsetof(BUFFER,c4) ;
   attrBUFFER[i++].offset = offsetof(BUFFER,f1) ;
   attrBUFFER[i++].offset = ADDR_BF(BUFFER,f1,0) ;
   attrBUFFER[i++].offset = ADDR_BF(BUFFER,f1,0) ;
   attrBUFFER[i++].offset = ADDR_BF(BUFFER,f1,0) ;
   attrBUFFER[i++].offset = ADDR_BF(BUFFER,f1,0) ;
   attrBUFFER[i++].offset = ADDR_BF(BUFFER,f1,0) ;
   attrBUFFER[i++].offset = offsetof(BUFFER,i2) ;
   attrBUFFER[i++].offset = offsetof(BUFFER,d1) ;
}



void *continuous_connect(void *in) {

   TCDevice* temp_device ;
   int status ;

fprintf(stderr,"Starting continuous_connect %d\n", pthread_self());
   /* continuously look for and connect to new clients */
   while (1) {
      temp_device = malloc(sizeof(TCDevice));
      memset((void *)temp_device,'\0',sizeof(TCDevice));
      status = tc_multiconnect(temp_device , "server" , other_tag, &err_hndlr);
      if (status == TC_SUCCESS) {
         device = realloc(device , (num_devices+1)*sizeof(TCDevice*)) ;
         device[num_devices++] = temp_device ;
      }
   }

   return((void *)NULL) ;
}

int main( int argc, char *argv[] ) {

   int       ctr;
   BUFFER    buffer;
   int       i, j;
   int       any_valid ;
   pthread_t thread ;
   int       nbytes ;



   if (argc >= 2) {
      if (!strcmp(argv[1] , "-h")) {
         fprintf(stderr,"usage: %s [tag_name]\n",argv[0]);
         exit(0) ;
      }
      else
         strcpy(other_tag , argv[1]) ;
   }

   init_attrBUFFER() ;

   thread_id = pthread_self() ;

   num_devices = 0 ;

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

   trick_error_init(&err_hndlr,
                    (TrickErrorFuncPtr)NULL,
                    (TrickErrorDataPtr)NULL,
                    TRICK_ERROR_TRIVIAL);

   pthread_create(&thread , NULL, continuous_connect , NULL) ;

   buffer.i1 = 0 ;
   buffer.s1 = 1 ;
   buffer.c1 = 2 ;
   buffer.f1 = 3 ;
   buffer.d1 = 4 ;

   buffer.b1 = 8 ;
   buffer.b2 = 7 ;
   buffer.b3 = 43 ;
   buffer.b4 = 357 ;
   buffer.b5 = 1 ;

/*
   buffer.b1 = 15 ;
   buffer.b2 = 7 ;
   buffer.b3 = 95 ;
   buffer.b4 = 7853 ;
   buffer.b5 = 6 ;
*/

   ctr = 0;
   while( 1 ) {
      any_valid = 0 ;
      /* write a message to all valid devices */
      for ( i = 0  ; i < num_devices ; i++ ) {
         if (tc_isValid(device[i])) {
            nbytes = tc_write_byteswap(device[i],(char *)&buffer,sizeof(buffer),attrBUFFER);
            if (nbytes != 0 ) {
               fprintf(stderr,"server: wrote [%08x] to client %d\n",buffer.i1++, device[i]->socket);
               any_valid = 1 ;
            }
         }
         else {
            /* free a device that is no longer valid and move all valid
               devices down in the array to fill the hole */
            free(device[i]);
            for ( j = i ; j < (num_devices-1) ; j++ ) {
               device[j] = device[j+1] ;
            }
            num_devices-- ;
            i-- ;
         }
      }
      if (any_valid) ctr++ ;
#ifdef __WIN32__
      Sleep(500) ;
#else
      usleep(500000) ;
#endif
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
   int i ;

   if (pthread_equal(pthread_self() , thread_id)) {
      fprintf(stderr,"Disconnecting devices...\n" );

      for( i = 0 ; i < num_devices ; i++) {
         if (device[i] != (TCDevice*)NULL) {
            tc_disconnect(device[i]);
         }
      }

   }

   /* Exit the application. */
   exit(0);

}

