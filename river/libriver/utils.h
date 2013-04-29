#ifndef UTILS_H
#define UTILS_H
#include <string>

/** \ingroup libdp
 *  \file 
 *  Utility functions for handling file paths
 */

/// Parse the directory name off a filename
int get_dirname( char* dir_name, char* file_name) ;

/// Parse the tail off a path
int get_tailname(char* full_name, char* tail_name) ;

/// Parse the directory name off a file path and return it
std::string get_string_dirname(const std::string& file_path) ;

/// Parse the bare filename off a file path and return it
std::string get_string_filename(const std::string& file_path) ;
#endif
