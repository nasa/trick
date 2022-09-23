/*
 * Usage: dr_client <host> <port>
 *
 * Description: This is a client to Trick sim's data record server.
 *              The matching write from the server side is called "dr_comm_send"
 *              and can be found in the
 *              $TRICK_HOME/trick_source/sim_services/exec directory.
 *
 *              The data packet coming from the server contains a file to write to,
 *              and a packet of data (checkpoint, s_job_execution or data recording
 *              files). So dr_client reads the data, and dumps it to the file
 *              specified by the server.
 *
 * Example: dr_client wave 7777
 *
 * Create by: Keith Vetter, 7/23/2002
 *
 */

#include "../include/tc_proto.h"

#define MAX_FILES 128
typedef struct {
   int   n_files ;
   char* file_name[MAX_FILES] ;
   FILE* fp_data_files[MAX_FILES] ;
} DataFiles ;

int  dr_comm_receive( TCDevice* connection, char** file_name, char** msg ) ;
void dump_data (  DataFiles* df,
                 TCDevice* connection, char** file_name, char** msg ) ;


int main( int narg, char** args ) {

  DataFiles df ;
  TCDevice connection ;
  int ret ;
  char* file_name ;
  char* msg ;

  /* Zero out device */
  memset(&connection, '\0', sizeof(TCDevice));

  /* Initialize some stuff  */
  file_name = ( char* ) malloc ( 256 ) ;
  msg       = ( char* ) malloc ( 1024 ) ;
  df.n_files = 0 ;

  /* Parse args */
  if ( narg != 3  ) {
    fprintf(stderr, "USAGE: dr_client <host> <port> \n");
    exit(-1);
  }
  connection.hostname = (char*) malloc ( strlen(args[1]) + 1 ) ;
  strcpy(connection.hostname, args[1]);
  connection.port = atoi(args[2]) ;

  /* Connect to server */
  ret = tc_connect(&connection);
  if ( ret != TC_SUCCESS ) {
     printf("ERROR: Couldn't connect to server on host \"%s\" port %d. \n",
             connection.hostname, connection.port);
     exit(-1);
  }

  /* Read data from server and dump data */
  dump_data( &df, &connection, &file_name, &msg);

  return 0 ;
}

int dr_comm_receive( TCDevice* connection, char** file_name, char** msg ) {

  int size_filename ;
  int size_msg ;

  tc_read(connection, (char*) &size_filename, 4);
  tc_read(connection, (char*) &size_msg, 4);
  tc_read(connection, *file_name, size_filename);
  tc_read(connection, *msg, size_msg);

  return( size_msg ) ;
}

void dump_data ( DataFiles* df,
                 TCDevice* connection, char** file_name, char** msg ) {

   int size_msg ;
   int ii,jj ;
   int found ;
   FILE* fp ;

   while ( 1 ) {

      /* Get data from dr server */
      size_msg = dr_comm_receive( connection, file_name, msg ) ;

      /* Have we received all data? */
      if ( *file_name[0] == '\0' ) {
         break ;
      }

      /* Which file to write to? */
      found = 0 ;
      for ( ii = 0 ; ii < df->n_files ; ii++ ) {
         if ( ! strcmp( df->file_name[ii], *file_name ) ) {
            found = 1 ;
            break ;
         }
      }

      if ( found == 0 ) {
         /* New file, open it and save off file_name */
         fp = fopen( *file_name, "w" ) ;
         df->fp_data_files[ii] = fp ;
         df->n_files++ ;
         df->file_name[ii] = ( char* ) malloc ( strlen(*file_name) + 1 ) ;
         strcpy( df->file_name[ii], *file_name) ;
      } else {
         /* Existing file, point to it */
         fp = df->fp_data_files[ii] ;
      }

      /* Write record to the file */
      if ( fp != NULL ) {
         fwrite( *msg, size_msg, 1, fp ) ;
      } else {
         fprintf(stderr, "ERROR: Can't open \"%s\"\n", *file_name);
         exit(-1);
      }
   }
}
