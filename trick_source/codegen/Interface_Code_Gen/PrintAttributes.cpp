
#include <iostream>
#include <fstream>
#include <libgen.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <stdio.h>
#include <limits.h>

#include "clang/Frontend/CompilerInstance.h"
#include "clang/Basic/FileManager.h"

#include "PrintAttributes.hh"
#include "PrintFileContentsBase.hh"
#include "PrintAttributesFactory.hh"
#include "HeaderSearchDirs.hh"
#include "CommentSaver.hh"
#include "ClassValues.hh"
#include "EnumValues.hh"
#include "Utilities.hh"

PrintAttributes::PrintAttributes(int in_attr_version , HeaderSearchDirs & in_hsd ,
  CommentSaver & in_cs , clang::CompilerInstance & in_ci, bool in_force , bool in_sim_services_flag ,
  std::string in_output_dir ) :
   hsd(in_hsd) ,
   cs(in_cs) ,
   ci(in_ci) ,
   force(in_force) ,
   sim_services_flag( in_sim_services_flag ) ,
   output_dir( in_output_dir )
{
    printer = createFileContents(in_attr_version) ;
}

/**
@details

In order for us to create an io_src file for an include the header must:

1.  Reside in a user directory ( not system dirs like /usr/include
2.  Not reside in an excluded directory
3.  Not have ICG: (No) present in the Trick header

If the header file meets all of these conditions, a PrintFileContentsBase
instance is created for that file name.

 */

bool PrintAttributes::isIOFileOutOfDate(std::string header_file_name, std::string io_file_name) {
    struct stat header_stat ;
    struct stat io_stat ;
    int ret ;

    stat( header_file_name.c_str() , &header_stat ) ;
    ret = stat( io_file_name.c_str() , &io_stat ) ;

    if ( ret == 0 ) {
        // If the force flag is true, return that the io file is out of date.
        // Otherise test if the header is newer than the io file.
        return ( force || (header_stat.st_mtime > io_stat.st_mtime) ) ;
    }
    return true ;
}

static void _mkdir(const char *dir) {
    char tmp[PATH_MAX];
    char *p = NULL;
    struct stat buf ;
    size_t len;

    snprintf(tmp, sizeof(tmp),"%s",dir);
    len = strlen(tmp);
    if(tmp[len - 1] == '/') {
        tmp[len - 1] = 0;
    }
    for(p = tmp + 1; *p; p++)
        if(*p == '/') {
            *p = 0;
            if ( stat( tmp , &buf ) != 0 ) {
                int returnValue = mkdir(tmp, S_IRWXU | S_IRWXG | S_IRWXO);
                if (returnValue) {
                    std::cout << bold(color(ERROR, "Error")) << "      Unable to create " << quote(bold(tmp)) << " for writing: " << strerror(errno) << std::endl;
                    return ;
                }
            }
            *p = '/';
        }
    if ( stat( tmp , &buf ) != 0 ) {
        int returnValue = mkdir(tmp, S_IRWXU | S_IRWXG | S_IRWXO);
        if (returnValue) {
            std::cout << bold(color(ERROR, "Error")) << "      Unable to create " << quote(bold(tmp)) << " for writing: " << strerror(errno) << std::endl;
            return ;
        }
    }
}

// this is a subset of tests on the header file to determine if this file is not excluded for any reason.
bool PrintAttributes::isFileIncluded(std::string header_file_name) {
    // several tests require the real path of the header file.
    char * realPath = almostRealPath(header_file_name.c_str()) ;

    if ( realPath != NULL ) {
        // Only include user directories (not system dirs like /usr/include)
        if ( hsd.isPathInUserDir(realPath) ) {
            // Don't process files in excluded directories
            if ( (hsd.isPathInExclude(realPath) == false) and (hsd.isPathInICGExclude(realPath) == false) ) {
                // Only include files that do not have ICG: (No)
                // hasICGNo uses original header name, not the real path
                if ( ! cs.hasICGNo(header_file_name) ) {
                    return true ;
                }
            }
        }
    }
    return false ;
}

bool PrintAttributes::openIOFile(const std::string& header_file_name) {
    /**
     * There are a lot of conditions to be met in order to open an IO file.  We store the headers
     * we have visited so we don't have to retest the them each time a new class/enum is processed.
     */
    if (visited_files.find(header_file_name) != visited_files.end()) {
        // We have visited this header before. If there is a valid name, append to the existing IO file.
        if (out_of_date_io_files.find(header_file_name) != out_of_date_io_files.end()) {
            outfile.open(out_of_date_io_files[header_file_name].c_str(), std::fstream::app);
            return true ;
        }
        return false;
    }

    // add it to the list of visited files
    visited_files.insert(header_file_name) ;

    // several tests require the real path of the header file
    char* temp = almostRealPath(header_file_name.c_str());

    if (!temp) {
        std::cout << bold(color(WARNING, "Warning")) << "    ICG could not resolve realpath of " << quote(bold(header_file_name)) << std::endl;
        return false;
    }

    const std::string realPath = std::string(temp);
    const bool verboseBuild = getenv("TRICK_VERBOSE_BUILD");

    /**
     * Exclude files:
     * - in system directories
     * - in TRICK_EXCLUDE directories
     * - in TRICK_ICG_EXCLUDE directories
     * - in TRICK_EXT_LIB_DIRS directories
     * - whose Trick header comments preclude ICG
     */
    const std::string skipping = color(INFO, "Skipping   ");

    if (!hsd.isPathInUserDir(realPath)) {
        return false;
    }

    if (hsd.isPathInExclude(realPath)) {
        if (verboseBuild) {
            std::cout << skipping << "TRICK_EXCLUDE: " << underline(realPath, hsd.getPathInExclude(realPath).size()) << std::endl;
        }
        return false;
    }

    if (hsd.isPathInICGExclude(realPath)) {
        if (verboseBuild) {
            std::cout << skipping << "TRICK_ICG_EXCLUDE: " << underline(realPath, hsd.getPathInICGExclude(realPath).size()) << std::endl;
        }
        return false;
    }

    if (hsd.isPathInExtLib(realPath)) {
        if (verboseBuild) {
            std::cout << skipping << "TRICK_EXT_LIB_DIRS: " << underline(realPath, hsd.getPathInExtLib(realPath).size()) << std::endl;
        }
        ext_lib_io_files.insert(header_file_name) ;
        return false;
    }

    if (cs.hasICGNo(header_file_name)) {
        if (verboseBuild) {
            std::cout << skipping << "ICG: (NO): " << realPath << std::endl;
        }
        icg_no_files.push_back(realPath);
        return false;
    }

    // map the header to its IO file
    const std::string io_file_name = createIOFileName(realPath) ;
    all_io_files[header_file_name] = io_file_name ;

    // make the parent directories
    char* name = strdup(io_file_name.c_str());
    _mkdir(dirname(name));
    free(name);

    // no further processing is required if it's not out of date
    if (!isIOFileOutOfDate(realPath, io_file_name)) {
        std::cout << "Not out of date: " << io_file_name << std::endl;
        return false;
    }

    // add it to the map of out of date IO files
    out_of_date_io_files[header_file_name] = io_file_name ;

    // write header information
    std::cout << color(INFO, "Writing    ") << out_of_date_io_files[header_file_name] << std::endl;
    outfile.open(out_of_date_io_files[header_file_name].c_str());
    printer->printIOHeader(outfile, header_file_name);
    if (!cs.hasTrickHeader(header_file_name) ) {
        std::cout << bold(color(WARNING, "Warning    ") + header_file_name) << std::endl
            << "           No Trick header comment found" << std::endl;
    }

    // get the ignored types from this header
    ignored_types[header_file_name] = cs.getIgnoreTypes(header_file_name) ;

    return true ;
}

/** Determines the io_file_name based on the given header file name */
std::string PrintAttributes::createIOFileName(std::string header_file_name) {
    std::string dir_name ;
    std::string base_name ;
    std::string io_file_name ;
    size_t found ;
    char cwd[1024] ;
    char * temp_str = strdup(header_file_name.c_str()) ;

    base_name = std::string(basename(temp_str)) ;
    found = base_name.find_last_of(".") ;
    base_name = std::string("io_") + base_name.substr(0,found) + std::string(".cpp") ;

    dir_name = std::string(dirname(temp_str)) ;
    if ( ! dir_name.compare(".") ) {
        dir_name = std::string(getcwd(cwd, 1024)) ;
    } else if ( ! dir_name.compare(0, 1, ".") ) {
        dir_name = std::string(getcwd(cwd, 1024)) + std::string("/") + dir_name ;
    }

    free(temp_str) ;

    if ( hsd.isPathInUserDir( dir_name ) ) {
        if ( ! output_dir.empty() ) {
            io_file_name = output_dir + "/" + base_name ;
        } else {
            // Put all of the sim_services io_files in ${TRICK_HOME}/trick_source/sim_services/include/io_src unless
            // it is in er7_utils.  The er7_utils io_files have duplicate file names so the overwrite each other
            // leave those in their respective directories.
            if ( sim_services_flag ) {
                if ( dir_name.length() >= 8 and ! dir_name.compare(dir_name.size() - 8 , dir_name.size() , "/include" )) {
                    if ( dir_name.length() < 13 or dir_name.compare(dir_name.size() - 13 , dir_name.size() , "trick/include" )) {
                        dir_name.replace(dir_name.size() - 8 , dir_name.size() , "") ;
                    }
                }
                if ( dir_name.find("er7_utils") == std::string::npos ) {
                    io_file_name = std::string(getenv("TRICK_HOME")) + "/trick_source/sim_services/include/io_src/" + base_name ;
                } else {
                    io_file_name = dir_name + "/io_src/" + base_name ;
                }
            } else {
                //TODO: only use build directory if we are ICG'ing a sim
                // All files go into a build directory based in the current directory.
                io_file_name =  std::string("build") + dir_name + "/" + base_name ;
            }
        }
        return io_file_name ;
    }

    return std::string() ;
}

void PrintAttributes::printClass( ClassValues * cv ) {
    // If the class name is not empty and the filename is not empty
    if ( (! cv->getName().empty()) and !cv->getFileName().empty() ) {
        // If we have not processed this class before
        if ( processed_classes[cv->getFileName()].find(cv->getFullyQualifiedMangledTypeName()) ==
             processed_classes[cv->getFileName()].end() ) {
            // mark the class as processed.
            processed_classes[cv->getFileName()].insert(cv->getFullyQualifiedMangledTypeName()) ;
            // If the class name is not specified as ignored in Trick header ICG_IGNORE field.
            if ( ignored_types[cv->getFileName()].find(cv->getName()) == ignored_types[cv->getFileName()].end() and
                 ignored_types[cv->getFileName()].find(cv->getFullyQualifiedName()) == ignored_types[cv->getFileName()].end() ) {
                // if we are successful in opening the io_src file
                if ( openIOFile( cv->getFileName()) ) {
                    // print the attributes
                    printer->printClass(outfile, cv) ;
                    // close the io_src file
                    outfile.close() ;
                }

                // if we are successful in opening the map file
                char * realPath = almostRealPath(cv->getFileName().c_str()) ;
                if ( realPath != NULL ) {
                    if ( isFileIncluded( cv->getFileName()) or hsd.isPathInExtLib(realPath)) {
                         printer->printClassMap(class_map_outfile, cv) ;
                    }
                    free(realPath) ;
                }
            }
        }
    }
}

void PrintAttributes::printEnum( EnumValues * ev ) {
    // If the enum name is not empty and the filename is not empty
    if ( (! ev->getName().empty()) and !ev->getFileName().empty() ) {
        // If we have not processed this enum before
        if ( processed_enums[ev->getFileName()].find(ev->getFullyQualifiedName()) == processed_enums[ev->getFileName()].end() ) {
            // mark the enum as processed.
            processed_enums[ev->getFileName()].insert(ev->getFullyQualifiedName()) ;
            // If the enum name is not specified as ignored in Trick header ICG_IGNORE field.
            if ( ignored_types[ev->getFileName()].find(ev->getName()) == ignored_types[ev->getFileName()].end() and
                 ignored_types[ev->getFileName()].find(ev->getFullyQualifiedName()) == ignored_types[ev->getFileName()].end()) {
                // if we are successful in opening the io_src file
                if ( openIOFile( ev->getFileName()) ) {
                    // print the attributes and close the io_src file
                    printer->printEnum(outfile, ev) ;
                    outfile.close() ;
                }

                // if we are successful in opening the map file
                if ( isFileIncluded( ev->getFileName())) {
                     printer->printEnumMap(enum_map_outfile, ev) ;
                }
            }
        }
    }
}

void PrintAttributes::removeMapFiles() {
    //remove("io_src/class_map.cpp") ;
    //remove("io_src/enum_map.cpp") ;
}

void PrintAttributes::createMapFiles() {
    struct stat buf ;
    std::string class_map_function_name ;
    std::string enum_map_function_name ;

    if ( sim_services_flag ) {
        map_dir = "trick_source/sim_services/include/io_src" ;
        class_map_function_name = "populate_sim_services_class_map" ;
        enum_map_function_name = "populate_sim_services_enum_map" ;
    } else {
        map_dir = "build" ;
        class_map_function_name = "populate_class_map" ;
        enum_map_function_name = "populate_enum_map" ;
    }

    if ( stat( map_dir.c_str() , &buf ) != 0 ) {
        if ( mkdir( map_dir.c_str() , 0755 ) != 0 ) {
            // dir does not exist and cannot make the directory.
            std::cout << bold(color(WARNING, "Warning")) << "    Unable to create " << quote(bold(map_dir)) << " for writing" << std::endl ;
        }
    }

    // Write processed code to temporary files
    class_map_outfile.open(std::string(map_dir + "/.class_map.cpp").c_str()) ;
    printer->printClassMapHeader(class_map_outfile, class_map_function_name ) ;
    enum_map_outfile.open(std::string(map_dir + "/.enum_map.cpp").c_str()) ;
    printer->printEnumMapHeader(enum_map_outfile, enum_map_function_name ) ;
}

void PrintAttributes::closeMapFiles() {
    printer->printClassMapFooter(class_map_outfile) ;
    class_map_outfile.close() ;

    printer->printEnumMapFooter(enum_map_outfile) ;
    enum_map_outfile.close() ;

    // If we wrote any new io_src files, move the temporary class and enum map files to new location
    if ( out_of_date_io_files.size() > 0 ) {
        std::ifstream class_map(std::string(map_dir + "/.class_map.cpp").c_str()) ;
        std::ifstream enum_map(std::string(map_dir + "/.enum_map.cpp").c_str()) ;
        std::ofstream combined_map(std::string(map_dir + "/class_map.cpp").c_str()) ;
        combined_map << class_map.rdbuf() << enum_map.rdbuf() ;
    } else {
        remove( std::string(map_dir + "/.class_map.cpp").c_str() ) ;
        remove( std::string(map_dir + "/.enum_map.cpp").c_str() ) ;
    }
}

void PrintAttributes::addEmptyFiles() {
    // Make a list of the empty files we processed.
    // This list is written to the ICG_processed file and used by other processors.
    clang::SourceManager::fileinfo_iterator fi ;
    for ( fi = ci.getSourceManager().fileinfo_begin() ; fi != ci.getSourceManager().fileinfo_end() ; fi++ ) {
        const clang::FileEntry * fe = (*fi).first ;
        std::string header_file_name = fe->getName() ;
        if ( visited_files.find(header_file_name) == visited_files.end() ) {
            visited_files.insert(header_file_name) ;
            // several tests require the real path of the header file.
            char * realPath = almostRealPath(header_file_name.c_str()) ;
            if ( realPath != NULL ) {
                // Only include user directories (not system dirs like /usr/include)
                if ( hsd.isPathInUserDir(realPath) ) {
                    if ( hsd.isPathInExclude(realPath) == false ) {
                        if ( hsd.isPathInICGExclude(realPath) == false ) {
                            // Only include files that do not have ICG: (No)
                            // hasICGNo uses original header name, not the real path
                            if ( ! cs.hasICGNo(header_file_name) ) {
                                // Don't process files in excluded directories
                                if ( hsd.isPathInExtLib(realPath) == false ) {
                                    std::string io_file_name = createIOFileName(std::string(realPath)) ;
                                    empty_header_files.insert(realPath) ;
                                } else {
                                    ext_lib_io_files.insert(realPath) ;
                                }
                            }
                        }
                    }
                }
                free(realPath) ;
            }
        }
    }
}

//TODO: Move this into PrintFileContents10.
void PrintAttributes::printIOMakefile() {
    std::ofstream makefile_io_src ;
    std::ofstream makefile_ICG ;
    std::ofstream link_io_objs ;
    std::ofstream ICG_processed ;
    std::ofstream ext_lib ;
    unsigned int ii ;

    // Don't create a makefile if we didn't process any files.
    if ( out_of_date_io_files.empty() ) {
       return ;
    }

    std::cout << color(INFO, "Writing") << "    Makefile_io_src" << std::endl ;

    makefile_io_src.open("build/Makefile_io_src") ;
    makefile_io_src << "TRICK_SYSTEM_CXXFLAGS += \\" << std::endl
        << "    -std=c++11 \\" << std::endl
        << "    -Wno-invalid-offsetof \\" << std::endl
        << "    -Wno-old-style-cast \\" << std::endl
        << "    -Wno-write-strings \\" << std::endl
        << "    -Wno-unused-variable" << std::endl
        << std::endl
        << "ifeq ($(IS_CC_CLANG), 0)" << std::endl
        << "    TRICK_SYSTEM_CXXFLAGS += -Wno-unused-local-typedefs -Wno-unused-but-set-variable" << std::endl
        << "endif" << std::endl
        << std::endl
        << "ifndef TRICK_VERBOSE_BUILD" << std::endl
        << "    PRINT_IO_INC_LINK = @echo \"$(call COLOR,Linking)    IO objects\"" << std::endl
        << "endif" << std::endl
        << std::endl
        << "IO_OBJ_FILES =" ;

    std::map< std::string , std::string >::iterator mit ;
    for ( mit = all_io_files.begin() ; mit != all_io_files.end() ; mit++ ) {
        size_t found ;
        found = (*mit).second.find_last_of(".") ;
        makefile_io_src << " \\\n    $(CURDIR)/" << (*mit).second.substr(0,found) << ".o" ;
    }

    makefile_io_src << " \\\n    $(CURDIR)/build/class_map.o" << std::endl
        << std::endl
        << "$(IO_OBJ_FILES): \%.o : \%.cpp" << std::endl
        << "\t$(PRINT_IO_COMPILE)" << std::endl
        << "\t$(ECHO_CMD)$(TRICK_CPPC) $(TRICK_CXXFLAGS) $(TRICK_SYSTEM_CXXFLAGS) -MMD -MP -c $< -o $@" << std::endl
        << std::endl
        << "-include $(IO_OBJ_FILES:.o=.d)" << std::endl
        << std::endl
        << "OBJECTS      += $(LIB_DIR)/io_src.o" << std::endl
        << "LINK_OBJECTS += $(LIB_DIR)/io_src.o" << std::endl
        << std::endl
        << "$(S_MAIN): $(LIB_DIR)/io_src.o" << std::endl
        << std::endl
        << "$(LIB_DIR)/io_src.o : $(IO_OBJ_FILES) | $(LIB_DIR)" << std::endl
        << "\t$(PRINT_IO_INC_LINK)" << std::endl
        << "\t$(ECHO_CMD)$(LD) $(LD_PARTIAL) -o $@ $(LD_FILELIST)build/link_io_objs" << std::endl
        << std::endl ;

    makefile_io_src.close() ;

    /*
       Makefile_ICG lists all headers as dependencies of Makefile_io_src
       causing ICG to run if any header file changes.

       link_io_objs lists all io_src object files to be partially
       linked into a single io_object.

       ICG_process lists all header files to be used by SWIG.
     */
    makefile_ICG.open("build/Makefile_ICG") ;
    link_io_objs.open("build/link_io_objs") ;
    ICG_processed.open("build/ICG_processed") ;

    makefile_ICG << "build/Makefile_io_src:" ;
    for ( mit = all_io_files.begin() ; mit != all_io_files.end() ; mit++ ) {
        makefile_ICG << " \\\n    " << (*mit).first ;
        size_t found ;
        found = (*mit).second.find_last_of(".") ;
        link_io_objs << (*mit).second.substr(0,found) << ".o" << std::endl ;
        ICG_processed << (*mit).first << std::endl ;
    }
    makefile_ICG.close() ;

    // Create the list of empty (of classes/enums) header files to be written to ICG_processed.
    addEmptyFiles() ;
    std::set< std::string >::iterator sit ;
    for ( sit = empty_header_files.begin() ; sit != empty_header_files.end() ; sit++ ) {
        ICG_processed << (*sit) << std::endl ;
    }
    ICG_processed.close() ;

    link_io_objs << "build/class_map.o" << std::endl ;
    link_io_objs.close() ;

    ext_lib.open("build/ICG_ext_lib") ;
    for ( sit = ext_lib_io_files.begin() ; sit != ext_lib_io_files.end() ; sit++ ) {
        ext_lib << (*sit) << std::endl ;
    }
    ext_lib.close() ;
}

void PrintAttributes::printICGNoFiles() {
    if ( ! sim_services_flag ) {
        std::vector< std::string >::iterator it ;
        std::ofstream icg_no_outfile("build/ICG_no_found") ;
        for ( it = icg_no_files.begin() ; it != icg_no_files.end() ; it++ ) {
            icg_no_outfile << (*it) << std::endl ;
        }
        icg_no_outfile.close() ;
    }
}
