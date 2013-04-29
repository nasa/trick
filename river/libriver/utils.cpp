#include "utils.h"
#include <string.h>
#include <string>

using std::string;

// Given file_path, get dir that file_path is in
string get_string_dirname(const string& file_path)
{
        int ii;

        string dir_name;
        for (ii = file_path.size() - 1; ii >= 0; ii--) {
                if (file_path[ii] == '/') {
                        dir_name = file_path.substr(0, ii);
                        break;
                }
        }

        if (dir_name.size() == 0) {
                if (file_path[0] == '/') {
                        // Case where file begins with "/"
                        // but no other slashes e.g. "/myfile"
                       dir_name = "/"; 
                } else {
                        // Case where there was no "/", e.g. "myfile"
                        dir_name = ".";
                }
        }
        return dir_name;
}

// Given file path, get the bare filename
string get_string_filename(const string& file_path)
{
        int ii;

        string filename;

        // If file path is empty, or ends in a slash, return an empty name
        if (file_path.size() == 0 || file_path[file_path.size()-1] == '/') {
            return filename;
        }

        // We know the last char of file_path is not null, so start 
        // looking for slashes at len-2
        for (ii = file_path.size() - 2; ii >= 0; ii--) {
                if (file_path[ii] == '/') {
                        filename = file_path.substr(ii+1);
                        break;
                }
        }

        // If no slash found, all of file path is the file name
        if (filename.size() == 0) {
                filename = file_path;
        }
        return filename;
}

// Given file_name, get dir that file_name is in
// Assumes dir_name is allocated
int get_dirname(char* dir_name, char* file_name)
{
        int ii;

        strcpy(dir_name, file_name);

        for (ii = strlen(dir_name) - 1; ii >= 0; ii--) {
                if (dir_name[ii] == '/') {
                        dir_name[ii] = '\0';
                        break;
                }
        }

        if (dir_name[0] == '\0') {
                // Case where file begins with "/"
                // but no other slashes e.g. "/myfile"
                strcpy(dir_name, "/");
        } else if (strlen(dir_name) == strlen(file_name)) {
                // Case where there was no "/", e.g. "myfile"
                strcpy(dir_name, ".");
        }

        return (0);
}

// Given file_name, return string after last "/" 
// If no "/", return entire string
// If "/" is last char, return NULL
// I'm not going to think about a file called /// either
int get_tailname(char* tail_name, char* full_name)
{
        unsigned int ii;
        int slash_idx ;
        char* chptr ;

        slash_idx = -1 ;
        for ( ii = 0; ii < strlen(full_name); ii++ ) {
                if ( full_name[ii] == '/' ) {
                        slash_idx = ii ;
                }
        }

        slash_idx++ ;
        if ( slash_idx == (int) strlen(full_name) ) {
                tail_name[0] = '\0' ;
        } else {
                chptr = &(full_name[slash_idx]) ;
                strcpy( tail_name, chptr ) ;
        }

        return (0);
}
