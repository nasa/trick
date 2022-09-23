
#include "../include/tc_proto.h"

int main( int narg, char** args ) {

  TCDevice connection ;
  int num_packets ;
  char* msg ;
  int msg_len ;
  int i ;
  int verbose ;

  /* Parse args */
  verbose = 0 ;
  if ( narg < 4 || narg > 5  ) {
    fprintf(stderr,
       "USAGE: tc_client <host> <num_packets> <packet_size> [-v]\n");
    exit(-1);
  }
  if ( narg == 5 ) {
    verbose = 1 ;
  }
  num_packets = atoi(args[2]) ;
  msg_len     = atoi(args[3]) ;
  if ( msg_len < 1 || num_packets < 1 ) {
    fprintf(stderr,
       "USAGE: tc_client <host> <num_packets> <packet_size> [-v]\n");
  }

  /* Create message */
  msg = (char*) malloc ( msg_len * sizeof(char)) ;
  for ( i = 0 ; i < msg_len ; i++ ) {
     msg[i] = '#' ;
  }

  /* Zero out device */
  memset(&connection, '\0', sizeof(TCDevice));

  /* Set Server Address */
  connection.port = 7000 ;
  connection.hostname = (char*) malloc ( 16 ) ;
  strcpy(connection.hostname, args[1]) ;

  /* Get an error handler */
  if ( verbose ) {
    tc_error(&connection, 1);
  }

  /* Connect to server */
  tc_connect(&connection);

  /* Round trip to server */
  for ( i = 0 ; i < num_packets ; i++ ) {
    tc_write(&connection, msg, msg_len);
    tc_read(&connection, msg, msg_len);
  }

  return 0 ;
}
