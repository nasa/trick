
#ifndef PRINTATTRIBUTES_HH
#define PRINTATTRIBUTES_HH

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <set>

namespace clang {
    class CompilerInstance ;
}

class ClassValues ;
class EnumValues ;
class PrintFileContentsBase ;
class HeaderSearchDirs ;
class CommentSaver ;
class PrintFileContentsBase ;

/**

  This class creates a PrintFileContentsBase class for each include file that we
  have parsed.  It then adds all of the classes and enums into PrintFileContentBase
  classes keyed by the files they were found.

  @author Alexander S. Lin

  @date July 2012

 */

class PrintAttributes {
    public:
        PrintAttributes( int attr_version , HeaderSearchDirs & hsd , CommentSaver & cs ,
         clang::CompilerInstance & in_ci, bool force , bool sim_services, std::string output_dir ) ;

        /** Prints all of the processed classes and enumerations */
        virtual void removeMapFiles() ;
        virtual void createMapFiles() ;
        virtual void closeMapFiles() ;

        /** Create empty io_files for headers that contain no class/struct information */
        virtual void printEmptyFiles() ;

        /** Prints list of files that contain ICG:(No) in the Trick header */
        virtual void printICGNoFiles() ;

        /** Prints a class to the io_src file */
        virtual void printClass( ClassValues * in_class) ;

        /** Prints an enum to the io_src file */
        virtual void printEnum( EnumValues * in_enum) ;

    protected:

        /** Directory to put class and enum map files */
        std::string map_dir ;

        /** Output stream to be used to print individual io_src files */
        std::ofstream outfile ;

        /** Output stream to be used for class_map */
        std::ofstream class_map_outfile ;

        /** Output stream to be used for enum_map */
        std::ofstream enum_map_outfile ;

        /** The header search directories */
        HeaderSearchDirs & hsd ;

        /** Comments for all of the header files */
        CommentSaver & cs ;

        /** Compiler instance */
        clang::CompilerInstance & ci ;

        /** Version specific attributes printer */
        PrintFileContentsBase * printer ;

        /** Force all io_src files to be written */
        bool force ;

        /** We are processing sim_services */
        bool sim_services_flag ;

        /** We are specifying an output directory for all files */
        std::string output_dir ;

        bool openIOFile(std::string header_file_name) ;

        bool isFileIncluded(std::string header_file_name) ;
        bool isIOFileOutOfDate(std::string header_file_name, std::string io_file_name ) ;
        bool doesIODirectoryExist(std::string io_file_name ) ;

        /** Determines the io_file_name based on the given header file name
            @param header_file_name = full path to header file
            @return string of full path to io_src file name.
          */
        std::string createIOFileName(std::string header_file_name) ;

        /** set of all visited files */
        std::set< std::string > visited_files ;

        /** map of open files to the io_src file */
        std::map< std::string , std::string > io_files ;

        /** List of files that have ICG: No */
        std::vector< std::string > icg_no_files ;

        /** map of ignored types sorted by file */
        std::map< std::string , std::set< std::string > > ignored_types ;
        /** map of processed classes sorted by file */
        std::map< std::string , std::set< std::string > > processed_classes ;
        /** map of processed enums sorted by file */
        std::map< std::string , std::set< std::string > > processed_enums ;

} ;

#endif
