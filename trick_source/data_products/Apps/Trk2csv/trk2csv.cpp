/*
 * Revision 9.1  2008/07/14 15:00:00  wwoodard
 * Insert revision comments
 *
 */

#include <vector>
#include <iostream>
#include "Log/TrickBinary.hh"
#include <string.h>
#include <stdlib.h>

static const char *usage_doc[] = {
"-------------------------------------------------------------------------",
" trk2csv -                                                               ",
" generates a comma-separated value (CSV) file from a Trick binary data   ",
" file. CSV files are a common means of sharing data between applications.",
"                                                                         ",
" Usage: trk2csv [-help] [-csv csv_file_name ] trk_log_file_name          ",
"                                                                         ",
"-------------------------------------------------------------------------"};
#define N_USAGE_LINES (sizeof(usage_doc)/sizeof(usage_doc[0]))

void print_doc(char *doc[], int nlines) {
    int i;
    for (i=0; i < nlines; i++) { 
        cerr << doc[i] << '\n';
    }
    cerr.flush();
}

void usage() {
    print_doc((char **)usage_doc,N_USAGE_LINES); 
}

void missing_parameter(char *option) {
     cerr << "The \"" << option << "\" option requires a parameter.\n";
     cerr.flush();
     usage();
     exit(EXIT_FAILURE);
}

int main(int argc, char* argv[])
{
    double t, y ;
    char *trk_file_name = NULL;
    char *csv_file_name = NULL;
    FILE *csv_fp;
  
    int number_of_parameters;
    char ** param_names;
    char ** param_units;
  
    int i, done;
    char *prog_name = argv[0];
  
    DataStream* each_ds;
    vector <DataStream*> ds_list;
    // idx is used when comaring with the size() of the vector
    vector <DataStream*>::size_type idx;
    
    if (argc <= 1 ) {
      cerr << prog_name << ": No arguments were supplied.\n";
      cerr.flush();
      usage();
      exit(EXIT_FAILURE);
    } else {
      int i = 1;
      char *option;
      while ( i < argc ) {
            option = argv[i++];
            if (strcmp("-help",option)==0) {
                usage();
                exit(EXIT_SUCCESS);
            } else if (strcmp("-csv",option)==0) {
                if (i<argc) {
                    csv_file_name = argv[i++];
                } else {  
                    missing_parameter(option);
                }
            } else if (i == argc) {
                trk_file_name = option;
            } else {
                cerr << "\"" << option << "\" is not a valid option.\n";
                cerr.flush();
                usage();
                exit(EXIT_FAILURE);
            }
      }
    } 

    if (trk_file_name == NULL) {
      cerr << prog_name << ": No Trick binary data file name was supplied.\n";
      cerr.flush();
      usage();
      exit(EXIT_FAILURE);
    }
  
    if (csv_file_name != NULL) {
      if (( csv_fp = fopen(csv_file_name, "w") ) == 0) {
            cerr << "Couldn't open \" << csv_file_name << \" for reading\n";
            cerr.flush();
            exit(EXIT_FAILURE);
      }
    } else {
      csv_fp = stdout;
    }
  
    number_of_parameters = TrickBinaryGetNumVariables(trk_file_name);
  
    if (number_of_parameters == 0) {
        cerr << "No parameters found in the Trk data log file.\n";
      cerr.flush();
      exit(EXIT_FAILURE);
    } 
  
    if (( param_names = TrickBinaryGetVariableNames(trk_file_name)) == NULL ) {
      cerr << "Unable get parameter names from the Trk data log file.\n";
      cerr.flush();
      exit(EXIT_FAILURE);
    }
  
    if (( param_units = TrickBinaryGetVariableUnits(trk_file_name)) == NULL ) {
        cerr << "Unable get parameter units from the Trk data log file.\n";
        cerr.flush();
        exit(EXIT_FAILURE);
    }
  
    for ( i=0; i<number_of_parameters; i++ ) {
      if (i == 0) {
            fprintf(csv_fp,"%s {%s}",param_names[0], param_units[i]);
      } else {
            fprintf(csv_fp,",%s {%s}",param_names[i], param_units[i]);
      }
    
      if (( each_ds = new TrickBinary(trk_file_name, param_names[i] )) == NULL) {
            cerr << ".\n";
            cerr.flush();
            exit(EXIT_FAILURE);
      } else {
            ds_list.push_back(each_ds);
        }
    }

    fprintf(csv_fp,"\n");
  
    done = 0;
    while ( !done ) {
      for ( idx = 0; idx < ds_list.size(); idx++ ) {
            if ( ds_list[idx]->get( &t, &y) == 0 ) {
                done = 1;
                break;
            }
            if ( idx != 0) {
                fprintf(csv_fp,",");
            } 
            fprintf(csv_fp,"%.15G",y);
      }
      fprintf(csv_fp,"\n");
    }
  
    // release memory for the DataStream list
    // vector clear function won't delete objects created by new
    for ( idx = 0; idx < ds_list.size(); idx++) {
        delete ds_list[idx];
    }
    ds_list.clear();
    
    // relese memory for the name list
    for (i = 0; i < number_of_parameters; i ++) {
        delete[] param_names[i];
    }
        
    return 0 ;
}
