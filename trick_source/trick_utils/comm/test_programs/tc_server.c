
#include "../include/tc_proto.h"
#ifndef __WIN32__
#include <sys/time.h>
#endif
#include <sys/types.h>
#include <stdlib.h>


int main( int narg, char** args ) {

  TCDevice listen_device ;
  TCDevice connection ;

  int verbose ;
  int nbytes ;
  int num_packets ;
  char *msg ;
  int msg_len ;
  int i ;

#if __WIN32__
   FILETIME start, stop;
   LARGE_INTEGER temp_start, temp_stop, total ;
#else
     struct timeval tp ;
     double time1, time2;
#endif
  double sum_time ;

#if __WIN32__
  memset(&total,0,sizeof(LARGE_INTEGER));
#endif

  /* Parse args */
  verbose = 0 ;
  if ( narg < 3 || narg > 4 ) {
    fprintf(stderr, "USAGE: tc_server <num_packets> <packet_size> [-v]\n");
    exit(-1);
  }
  if ( narg == 4 ) {
    verbose = 1 ;
  }
  num_packets = atoi(args[1]) ;
  msg_len     = atoi(args[2]) ;
  if ( msg_len < 1 || num_packets < 1 ) {
    fprintf(stderr, "USAGE: tc_server <num_packets> <packet_size> [-v]\n");
  }

  /* Create message */
  msg = (char*) malloc ( msg_len * sizeof(char)) ;
  for ( i = 0 ; i < msg_len ; i++ ) {
     msg[i] = '#' ;
  }

  /* Zero out devices */
  memset(&listen_device, '\0', sizeof(TCDevice));
  memset(&connection,    '\0', sizeof(TCDevice));

  /* Get an error handler */
  if ( verbose ) {
    tc_error(&connection, 1) ;
    tc_error(&listen_device, 1) ;
  }

  /* Accept connection */
  listen_device.port = 7000 ;
  tc_init(&listen_device) ;
  tc_accept(&listen_device, &connection);

  sum_time = 0 ;
  for ( i = 0 ; i < num_packets ; i++ ) {

     /* Get initial time */
#if __WIN32__
	 GetSystemTimeAsFileTime(&start);
#else
     gettimeofday( &tp , (struct timezone *)NULL ) ;
     time1 = (double)(tp.tv_sec) + ( (double)(tp.tv_usec) / 1000000.0 ) ;
#endif
     nbytes = tc_read(&connection, msg, msg_len) ;
     nbytes = tc_write(&connection, msg, msg_len) ;

     /* Get final time */
#if __WIN32__
	 GetSystemTimeAsFileTime(&stop);
	 temp_start.LowPart = start.dwLowDateTime ;
	 temp_start.HighPart = start.dwHighDateTime ;
	 temp_stop.LowPart = stop.dwLowDateTime ;
	 temp_stop.HighPart = stop.dwHighDateTime ;
	 total.QuadPart = total.QuadPart + temp_stop.QuadPart - temp_start.QuadPart ;
#else
     gettimeofday( &tp , (struct timezone *)NULL ) ;
     time2 = (double)(tp.tv_sec) + ((double)(tp.tv_usec) / 1000000.0 ) ;
     time2 = (double)(tp.tv_sec) + ((double)(tp.tv_usec) / 1000000.0 ) ;

     sum_time = sum_time + ( time2 - time1 ) ;
#endif
  }

#if __WIN32__
  sum_time = (double)(total.QuadPart * 0.0000001 ) ;
#endif

  fprintf(stderr, "Avg packet round trip : %lf \n",
                  sum_time/(double)num_packets );

  return 0 ;
}
