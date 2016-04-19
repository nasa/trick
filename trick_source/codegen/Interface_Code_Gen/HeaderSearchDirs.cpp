#include <iostream>
#include <sstream>
#include <stdio.h>
#include <errno.h>
#include <limits.h>
#include <stdlib.h>
#include <libgen.h>

#include "clang/Basic/TargetInfo.h"
#include "clang/Frontend/Utils.h"

#include "HeaderSearchDirs.hh"
#include "Utilities.hh"

HeaderSearchDirs::HeaderSearchDirs(clang::HeaderSearch & in_hs ,
 clang::HeaderSearchOptions & in_hso ,
 clang::Preprocessor & in_pp ,
 bool in_sim_services ) :
  hs(in_hs) ,
  hso(in_hso) ,
  pp(in_pp) ,
  sim_services(in_sim_services) {} ;

void HeaderSearchDirs::AddCompilerBuiltInSearchDirs () {

    FILE * fp ;
    char * lineptr = NULL ;
    char line[1024] ;
    size_t n = 0 ;
    ssize_t num_read ;
    const bool IsUserSupplied = false;
    const bool IsFramework = false;
    const bool IsSysRootRelative = true;

    // Add clang specific include directory first.  Only required on linux systems. :(
    // This is so that ICG will find clang friendly headers first.  gcc headers cause
    // all kinds of problems.  On macs all headers are clang friendly.
#if __linux
    std::stringstream icg_dir ;
    icg_dir << LLVM_HOME << "/lib/clang/" ;
    icg_dir << __clang_major__ << "." << __clang_minor__ ;
#if (__clang_major__ > 3) || ((__clang_major__ == 3) && (__clang_minor__ >= 4))
#ifdef __clang_patchlevel__
    icg_dir << "." << __clang_patchlevel__  ;
#endif
#endif
    icg_dir << "/include" ;
    char * resolved_path = realpath(icg_dir.str().c_str(), NULL ) ;
    if ( resolved_path != NULL ) {
        hso.AddPath(resolved_path , clang::frontend::System, IsFramework, IsSysRootRelative);
    }
#endif

    fp = popen("${TRICK_HOME}/bin/gte TRICK_CPPC" , "r") ;
    fgets(line , sizeof(line) , fp) ;
    pclose(fp) ;
    std::string trick_cppc = std::string(line) ;
    std::string command ;
    trick_cppc.erase(trick_cppc.find_last_not_of(" \n\r\t")+1) ;
    //std::cout << "TRICK_CPPC = " << trick_cppc << std::endl ;
    command = std::string("echo | ") + trick_cppc + std::string(" -v -xc++ -E - 2>&1") ;
    //std::cout << "command = " << command << std::endl ;

    fp = popen(command.c_str() , "r") ;
    bool dir_line = false ;

    while (fgets( line , sizeof(line) , fp ) != NULL ) {
        if ( !strncmp( line, "End of search list.", 19 )) {
            break ;
        } else if ( dir_line == true ) {
            //std::cout << "lineptr = " << lineptr << std::endl ;
            char * resolved_path = NULL ;
            std::string dir = std::string(line) ;
            dir.erase(dir.find_last_not_of(" \n\r\t\f\v") + 1) ;
            dir.erase(0, dir.find_first_not_of(" ")) ;
            if ( (n = dir.find(" (framework")) != std::string::npos ) {
                dir.erase(n) ;
            }
            resolved_path = almostRealPath(dir.c_str()) ;
            //std::cout << "dir = " << dir << std::endl ;
            if ( resolved_path != NULL ) {
                //std::cout << "gcc resolved_path = " << resolved_path << std::endl ;
#if (__clang_major__ == 3) && (__clang_minor__ >= 3)
                hso.AddPath(resolved_path , clang::frontend::System, IsFramework, IsSysRootRelative);
#else
                hso.AddPath(resolved_path , clang::frontend::System, IsUserSupplied, IsFramework, IsSysRootRelative);
#endif
                free(resolved_path) ;
            }
        } else if ( !strncmp( line, "#include <...>", 14 )) {
            dir_line = true ;
        }
    }
    pclose(fp) ;

    // Homebrew on Macs puts everything in /usr/local/Cellar.
    hso.AddPath("/usr/local/Cellar" , clang::frontend::System, IsFramework, IsSysRootRelative);

}

void HeaderSearchDirs::AddUserSearchDirs ( std::vector<std::string> & include_dirs ) {
    //std::cout << "num include dirs " << include_dirs.size() << std::endl ;
    int ii ;

    for  ( ii = 0 ; ii < include_dirs.size() ; ii++ ) {
        //std::cout << "include dirs " << include_dirs[ii] << std::endl ;
        char * resolved_path = almostRealPath(include_dirs[ii].c_str()) ;
        if ( resolved_path != NULL ) {
            //std::cout << "adding resolved_path = " << resolved_path << std::endl ;
#if (__clang_major__ == 3) && (__clang_minor__ >= 3)
            hso.AddPath(resolved_path , clang::frontend::Angled, false, true);
#else
            hso.AddPath(resolved_path , clang::frontend::Angled, true, false, true);
#endif
            // Add the path as a system path as well for those included files that are erroneously in <>
        }
    }

}

void HeaderSearchDirs::AddTrickSearchDirs () {
    char * trick_home = getenv("TRICK_HOME") ;

    if ( trick_home != NULL ) {
        std::string temp_dir = std::string(trick_home) + "/trick_source" ;
        char * resolved_path = almostRealPath(temp_dir.c_str() ) ;
#if (__clang_major__ == 3) && (__clang_minor__ >= 3)
        hso.AddPath(resolved_path , clang::frontend::Quoted, false, true);
#else
        hso.AddPath(resolved_path , clang::frontend::Quoted, true, false, true);
#endif
        trick_source_dir = std::string(resolved_path) ;
        free(resolved_path) ;
    }
}

void HeaderSearchDirs::AddICGExcludeDirs () {

    char * trick_icg_exclude = getenv("TRICK_ICG_EXCLUDE") ;

    if( trick_icg_exclude != NULL ) {
        std::string s = std::string(trick_icg_exclude) ;
        std::stringstream ss(s);
        std::string item;
        while(std::getline(ss, item, ':')) {
            //icg_exclude_dirs.push_back(item);
            item = trim(item) ;
            if ( ! item.empty() ) {
                char * resolved_path = realpath(item.c_str(), NULL) ;
                if ( resolved_path ) {
                    icg_exclude_dirs.push_back(std::string(resolved_path) + std::string("/"));
                } else {
                    std::cout << "Cannot find TRICK_ICG_EXCLUDE directory " << item << std::endl ;
                }
            }
        }
    }
}

void HeaderSearchDirs::AddICGNoCommentDirs () {

    char * trick_icg_nocomment = getenv("TRICK_ICG_NOCOMMENT") ;

    if( trick_icg_nocomment != NULL ) {
        std::string s = std::string(trick_icg_nocomment) ;
        std::stringstream ss(s);
        std::string item;
        while(std::getline(ss, item, ':')) {
            item = trim(item) ;
            if ( ! item.empty() ) {
                char * resolved_path = realpath(item.c_str(), NULL) ;
                if ( resolved_path ) {
                    icg_nocomment_dirs.push_back(std::string(resolved_path) + std::string("/"));
                } else {
                    std::cout << "Cannot find TRICK_ICG_NOCOMMENT directory " << item << std::endl ;
                }
            }
        }
    }
}

void HeaderSearchDirs::ApplyHeaderSearchOptions () {

    clang::ApplyHeaderSearchOptions( hs , hso , pp.getLangOpts() , pp.getTargetInfo().getTriple() ) ;
    clang::HeaderSearch::search_dir_iterator sdi ;
/*
    for ( sdi = hs.quoted_dir_begin() ; sdi != hs.quoted_dir_end() ; sdi++ ) {
       std::cout << "quoted dir " << (*sdi).getName() << std::endl ;
    }
    for ( sdi = hs.angled_dir_begin() ; sdi != hs.angled_dir_end() ; sdi++ ) {
       std::cout << "angled dir " << (*sdi).getName() << std::endl ;
    }
    for ( sdi = hs.system_dir_begin() ; sdi != hs.system_dir_end() ; sdi++ ) {
       std::cout << "system dir " << (*sdi).getName() << std::endl ;
    }
*/
}

void HeaderSearchDirs::addSearchDirs ( std::vector<std::string> & include_dirs ) {
    AddUserSearchDirs( include_dirs ) ;
    AddTrickSearchDirs() ;
    AddCompilerBuiltInSearchDirs() ;
    AddICGExcludeDirs() ;
    AddICGNoCommentDirs() ;
    ApplyHeaderSearchOptions() ;
}

bool HeaderSearchDirs::isPathInUserDir (std::string in_dir ) {

    clang::HeaderSearch::search_dir_iterator sdi ;
    for ( sdi = hs.system_dir_begin() ; sdi != hs.system_dir_end() ; sdi++ ) {
       std::string curr_dir = (*sdi).getName() ;
        if ( ! in_dir.compare(0, curr_dir.size(), curr_dir)) {
            return false ;
        }
    }
    if ( ! sim_services and ! in_dir.compare(0, trick_source_dir.size(), trick_source_dir)) {
        return false ;
    }

    return true ;
}

bool HeaderSearchDirs::isPathInUserOrTrickDir (std::string in_dir ) {

    clang::HeaderSearch::search_dir_iterator sdi ;
    for ( sdi = hs.system_dir_begin() ; sdi != hs.system_dir_end() ; sdi++ ) {
       std::string curr_dir = (*sdi).getName() ;
        if ( ! in_dir.compare(0, curr_dir.size(), curr_dir)) {
            return false ;
        }
    }
    return true ;
}

bool HeaderSearchDirs::isPathInICGExclude (std::string in_dir ) {

    std::vector<std::string>::iterator vit ;
    for ( vit = icg_exclude_dirs.begin() ; vit != icg_exclude_dirs.end() ; vit++ ) {
        if ( ! in_dir.compare(0, (*vit).size(), (*vit))) {
            return true ;
        }
    }

    return false ;
}

bool HeaderSearchDirs::isPathInICGNoComment (std::string in_file ) {

    char * resolved_path = almostRealPath(in_file.c_str() ) ;

    if ( resolved_path != NULL ) {
        std::string dir = std::string(dirname(resolved_path)) + "/";

        if ( icg_nocomment_files.find(dir) == icg_nocomment_files.end() ) {

            icg_nocomment_files[dir] = false ;
            std::vector<std::string>::iterator vit ;
            for ( vit = icg_nocomment_dirs.begin() ; vit != icg_nocomment_dirs.end() ; vit++ ) {
                if ( ! dir.compare(0, (*vit).size(), (*vit))) {
                    icg_nocomment_files[dir] = true ;
                    break ;
                }
            }
        }
        free(resolved_path) ;
        return icg_nocomment_files[dir] ;
    }
    return false ;
}

std::string HeaderSearchDirs::getPathInICGExclude (std::string in_dir ) {

    std::vector<std::string>::iterator vit ;
    for ( vit = icg_exclude_dirs.begin() ; vit != icg_exclude_dirs.end() ; vit++ ) {
        if ( ! in_dir.compare(0, (*vit).size(), (*vit))) {
            return (*vit) ;
        }
    }

    return std::string() ;
}

void HeaderSearchDirs::addDefines ( std::vector<std::string> & defines ) {

    // Add -D command line arguments as well as "#define TRICK_ICG" to the preprocessor
    unsigned int ii ;
    std::string predefines("#define TRICK_ICG\n") ;
    predefines += "#define __STRICT_ANSI__\n" ;
    for ( ii = 0 ; ii < defines.size() ; ii++ ) {
        size_t found = defines[ii].find("=") ;
        if ( found != defines[ii].npos ) {
            defines[ii].replace( found , 1 , " " ) ;
        } else {
            defines[ii] += " 1" ;
        }
        predefines += std::string("#define ") + defines[ii] + "\n" ;
    }
    pp.setPredefines(pp.getPredefines() + "\n" + predefines) ;

}
