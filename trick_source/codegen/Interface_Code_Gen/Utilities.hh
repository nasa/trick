
#ifndef _ICGUTILITIES_H_
#define _ICGUTILITIES_H_

#include "clang/Frontend/CompilerInstance.h"
#include "clang/Basic/FileManager.h"
#include "clang/Basic/SourceLocation.h"
#include "HeaderSearchDirs.hh"

std::string trim( const std::string& str, const std::string& whitespace = " \t\n\r" ) ;
bool isInUserCode( clang::CompilerInstance & ci , clang::SourceLocation sl , HeaderSearchDirs & hsd ) ;
bool isInUserOrTrickCode( clang::CompilerInstance & ci , clang::SourceLocation sl , HeaderSearchDirs & hsd ) ;
std::string getFileName( clang::CompilerInstance & ci , clang::SourceLocation sl , HeaderSearchDirs & hsd ) ;
char * almostRealPath( const char * in_path ) ;

#endif
