
#ifndef HEADERSEARCHDIRS_HH
#define HEADERSEARCHDIRS_HH

#include <map>
#include <vector>
#include <string>
#include <set>

#include "clang/Lex/HeaderSearch.h"
#include "clang/Lex/HeaderSearchOptions.h"
#include "clang/Lex/Preprocessor.h"

/**

  This class gathers all of the include directories and adds them to the preprocessor
  to use. Include directories are built into the compiler, inclded on the command line, or
  added by Trick itself.

  @author Alexander S. Lin

  @date July 2012

 */

class HeaderSearchDirs {

    public:
        HeaderSearchDirs(clang::HeaderSearch & in_hs ,
                         clang::HeaderSearchOptions & in_hso ,
                         clang::Preprocessor & in_pp ,
                         bool in_sim_services ) ;

        /** Add all search directories to the preprocessor.
            @param include_dirs = directories included via -I on the command line
            @param isystem_dirs = directories included via -isystem on the command line
          */
        void addSearchDirs ( std::vector<std::string> & include_dirs,
                             std::vector<std::string> & isystem_dirs ) ;

        /** Returns true if the path argument is in a user model directory
            @param path = directory path to be checked
            @return true = path is in user directory, false = path is not in a user dir.
          */
        bool isPathInUserDir (const std::string& path) ;

        /** Returns true if the path argument is in a user model or Trick directory
            @param path = directory path to be checked
            @return true = path is in user directory, false = path is not in a user dir.
          */
        bool isPathInUserOrTrickDir (const std::string& path) ;

        /** Returns true if directory is a subdirectory of an excluded directory listed
            in the TRICK_EXCLUDE environment variable.
            @param path = directory path to be checked
            @return true = path is in a system directory, false = not in system directory.
          */
        bool isPathInExclude (const std::string& path) ;

        /** Returns true if directory is a subdirectory of an excluded directory listed
            in the TRICK_ICG_EXCLUDE environment variable.
            @param path = directory path to be checked
            @return true = path is in a system directory, false = not in system directory.
          */
        bool isPathInICGExclude (const std::string& path) ;

        /** Returns true if directory is a subdirectory of an external library directory listed
            in the TRICK_EXT_LIB_DIRS environment variable.
            @param path = directory path to be checked
            @return true = path is in a system directory, false = not in system directory.
          */
        bool isPathInExtLib (const std::string& path) ;

        /** Returns true if directory is a subdirectory of a no comment directory
            in the TRICK_ICG_NOCOMMENT environment variable.
            @param path = directory path to be checked
            @return true = path is in a system directory, false = not in system directory.
          */
        bool isPathInICGNoComment (const std::string& path) ;

        /** Returns true if directory is a subdirectory of a compatible 15 directory
            in the TRICK_ICG_COMPAT15 environment variable.
            @param path = directory path to be checked
            @return true = path is in a system directory, false = not in system directory.
          */
        bool isPathInCompat15 (const std::string& path) ;

        /** Returns the TRICK_EXCLUDE directory that contains the path argument.
            @param path = path to be checked
            @return string from TRICK_EXCLUDE that contains the path.
          */
        std::string getPathInExclude (const std::string& path) ;

        /** Returns the TRICK_ICG_EXCLUDE directory that contains the path argument.
            @param path = path to be checked
            @return string from TRICK_ICG_EXCLUDE that contains the path.
          */
        std::string getPathInICGExclude (const std::string& path) ;

        /** Returns the TRICK_EXT_LIB_DIRS directory that contains the path argument.
            @param path = path to be checked
            @return string from TRICK_ICG_EXCLUDE that contains the path.
          */
        std::string getPathInExtLib (const std::string& path) ;

        /** Add all #defines from the command line and built in default predefines
            @param defines = a list of all -D<define> arguments on the command line.
          */
        void addDefines ( std::vector<std::string> & defines ) ;

        /** Add a file name to the set of files that we found TRICK_ICG used.
            @param file_name = file_name string
          */
        void addTrickICGFoundFile ( std::string file_name ) ;

    private:
        /** Are we ICG'ing the sim_services files? */
        bool sim_services ;

        /** The compiler's header search class.  We are filling this class with our directories. */
        clang::HeaderSearch & hs ;

        /** The compiler's header search options class.  We are filling this class with our directories. */
        clang::HeaderSearchOptions & hso ;

        /** The compiler's preprocessor */
        clang::Preprocessor & pp ;

        /** ${TRICK_HOME}/trick_source resolved */
        std::string trick_source_dir ;

        /** ${TRICK_HOME}/include/trick resolved */
        std::string trick_include_dir ;

        /** Adds all of the built-in system paths contained in the TRICK_CXX compiler */
        void AddCompilerBuiltInSearchDirs () ;

        /** Adds -I include paths */
        void AddUserSearchDirs ( std::vector<std::string> & include_dirs ) ;

        /** Adds -isystem include paths */
        void AddSystemSearchDirs ( std::vector<std::string> & isystem_dirs ) ;

        /** Adds ${TRICK_HOME}/trick_source to the search directories */
        void AddTrickSearchDirs () ;

        /** Create list of directories  */
        void AddDirsAndFiles(std::string env_var, std::vector<std::string> & var_list ) ;

        /** Apply all search directories to the preprocessor. */
        void ApplyHeaderSearchOptions () ;

        /** List of directoris to exclude from the TRICK_EXCLUDE environment variable */
        std::vector<std::string> exclude_dirs ;

        /** List of directoris to exclude from the TRICK_ICG_EXCLUDE environment variable */
        std::vector<std::string> icg_exclude_dirs ;

        /** List of directoris to mark as belonging in TRICK_EXT_LIB_DIRS environment variable */
        std::vector<std::string> ext_lib_dirs ;

        /** List of directoris to exclude comments from the TRICK_ICG_NOCOMMENT environment variable */
        std::vector<std::string> icg_nocomment_dirs ;

        /** List of directoris to write offsetof lines compatible with Trick 15 and earlier */
        std::vector<std::string> compat15_dirs ;

        /** set of file names with trick_icg_present */
        std::set< std::string > trick_icg_present ;

        /** Map of file names to in icg_nocomment_dir used as a cache */
        std::map< std::string , bool > icg_nocomment_files ;

} ;

#endif
