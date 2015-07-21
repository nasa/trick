
#include <iostream>
#include <sstream>
#include <regex.h>
#include <algorithm>

#include "clang/Basic/FileManager.h"

#include "CommentSaver.hh"
#include "Utilities.hh"

CommentSaver::CommentSaver(clang::CompilerInstance & in_ci , HeaderSearchDirs & in_hsd ) : ci(in_ci) , hsd(in_hsd) {}

bool CommentSaver::HandleComment(clang::Preprocessor &PP, clang::SourceRange Comment) {
    //Comment.getBegin().dump(sm) ;

    //if ( ! sm.isInSystemHeader(Comment.getBegin()) ) {
    if ( isInUserOrTrickCode( ci , Comment.getBegin() , hsd ) ) {
        std::string file_name = ci.getSourceManager().getBufferName(Comment.getBegin()) ;
        unsigned int line_no = ci.getSourceManager().getSpellingLineNumber(Comment.getBegin()) ;
        comment_map[file_name][line_no] = Comment ;
    }

    // returning false means we did not push any text back to the stream for further reads.
    return false ;
}

std::string CommentSaver::getComment( clang::SourceRange sr ) {

    if ( sr.isValid() ) {
        /* fsl_begin and fsl_end are two pointers into the header file.  We want the text between the two pointers. */
        clang::FullSourceLoc fsl_begin(sr.getBegin() , ci.getSourceManager()) ;
        clang::FullSourceLoc fsl_end(sr.getEnd() , ci.getSourceManager()) ;
        std::string comment_text( fsl_begin.getCharacterData() ,
         (size_t)(fsl_end.getCharacterData() - fsl_begin.getCharacterData())) ;
        return comment_text ;
    }

    /* return an empty string if no comment found on this line of the file. */
    return std::string() ;
}

std::string CommentSaver::getComment( std::string file_name , unsigned int line_no ) {
    return getComment(comment_map[file_name][line_no]) ;
}

std::string CommentSaver::getTrickHeaderComment( std::string file_name ) {

    std::map < unsigned int , clang::SourceRange >::iterator cit ;

    if ( trick_header_comments.find(file_name) == trick_header_comments.end() ) {
        trick_header_comments[file_name] = std::string() ;
        for ( cit = comment_map[file_name].begin() ; cit != comment_map[file_name].end() ; cit++ ) {
            std::string comment_str = getComment((*cit).second) ;
            std::transform(comment_str.begin(), comment_str.end(), comment_str.begin(), ::toupper) ;
            if ( comment_str.find("PURPOSE") != std::string::npos ) {
                trick_header_comments[file_name] = getComment((*cit).second) ;
                break ;
            }
        }
    }

    return trick_header_comments[file_name] ;
}

void CommentSaver::getICGField( std::string file_name ) {

    /* default the icg_no flags to false */
    icg_no_found[file_name] = false ;
    icg_no_comment_found[file_name] = false ;

    std::string th_str = getTrickHeaderComment(file_name) ;
    if ( ! th_str.empty() ) {

        std::transform(th_str.begin(), th_str.end(), th_str.begin(), ::toupper) ;
        int ret ;
        regex_t reg_expr ;
        regmatch_t pmatch[10] ;
        memset(pmatch , 0 , sizeof(pmatch)) ;

        /* POSIX regular expressions are always greedy, making our job harder.
           We have to use several regular expressions to get the types.  This was
           so much easier in perl! */

        /* find the ICG field */
        ret = regcomp( &reg_expr , "(ICG:)" , REG_EXTENDED | REG_ICASE ) ;
        ret = regexec( &reg_expr , th_str.c_str() , 10 , pmatch , 0 ) ;
        regfree(&reg_expr) ;
        if ( ret != 0 ) {
            return ;
        }
        th_str = th_str.substr(pmatch[1].rm_eo) ;

        /* find the end of the ICG field */
        memset(pmatch , 0 , sizeof(pmatch)) ;
        ret = regcomp( &reg_expr , "(\\))" , REG_EXTENDED ) ;
        ret = regexec( &reg_expr , th_str.c_str() , 10 , pmatch , 0 ) ;
        regfree(&reg_expr) ;

        if ( ret != 0 ) {
            return ;
        }
        th_str = th_str.substr(0 , pmatch[1].rm_so) ;

        /* test for NoComment */
        memset(pmatch , 0 , sizeof(pmatch)) ;
        ret = regcomp( &reg_expr , "(NOCOMMENT)$" , REG_EXTENDED ) ;
        ret = regexec( &reg_expr , th_str.c_str() , 10 , pmatch , 0 ) ;
        regfree(&reg_expr) ;

        if ( ret == 0 ) {
            icg_no_comment_found[file_name] = true ;
        }

        /* test for No */
        memset(pmatch , 0 , sizeof(pmatch)) ;
        ret = regcomp( &reg_expr , "(NO)$" , REG_EXTENDED ) ;
        ret = regexec( &reg_expr , th_str.c_str() , 10 , pmatch , 0 ) ;
        regfree(&reg_expr) ;

        if ( ret == 0 ) {
            icg_no_found[file_name] = true ;
            icg_no_comment_found[file_name] = true ;
        }
    }

}

bool CommentSaver::hasICGNo( std::string file_name ) {

    if ( icg_no_found.find(file_name) == icg_no_found.end() ) {
        getICGField(file_name) ;
    }

    return icg_no_found[file_name] ;
}

bool CommentSaver::hasICGNoComment( std::string file_name ) {

    if ( icg_no_comment_found.find(file_name) == icg_no_comment_found.end() ) {
        getICGField(file_name) ;
    }

    return icg_no_comment_found[file_name] ;
}

std::set< std::string > CommentSaver::getIgnoreTypes( std::string file_name ) {
    std::set< std::string > ignore_types ;
    std::string th_str = getTrickHeaderComment(file_name) ;
    if ( ! th_str.empty() ) {
        //std::cout << "here in getIgnoreTypes\n" << th_str << std::endl ;
        std::transform(th_str.begin(), th_str.end(), th_str.begin(), ::toupper) ;

        int ret ;
        regex_t reg_expr ;
        regmatch_t pmatch[10] ;
        memset(pmatch , 0 , sizeof(pmatch)) ;

        /* POSIX regular expressions are always greedy, making our job harder.
           We have to use several regular expressions to get the types.  This was
           so much easier in perl! */

        /* find the start of the ICG_IGNORE_TYPES field */
        ret = regcomp( &reg_expr , "(ICG[ _]IGNORE[ _]TYPE(S)?:)" , REG_EXTENDED | REG_ICASE ) ;
        ret = regexec( &reg_expr , th_str.c_str() , 10 , pmatch , 0 ) ;
        regfree(&reg_expr) ;
        if ( ret != 0 ) {
            return std::set< std::string >() ;
        }
        th_str = th_str.substr(pmatch[1].rm_eo) ;

        /* find the end of the ICG_IGNORE_TYPES field */
        memset(pmatch , 0 , sizeof(pmatch)) ;
        ret = regcomp( &reg_expr , "(\\)\\s*\\))" , REG_EXTENDED ) ;
        ret = regexec( &reg_expr , th_str.c_str() , 10 , pmatch , 0 ) ;
        regfree(&reg_expr) ;
        if ( ret != 0 ) {
            return std::set< std::string >() ;
        }
        th_str = th_str.substr(0 , pmatch[1].rm_so) ;
        std::replace( th_str.begin(), th_str.end(), '(', ' ');
        std::replace( th_str.begin(), th_str.end(), ')', ' ');
        std::replace( th_str.begin(), th_str.end(), '\n', ' ');

        std::stringstream ss(th_str);
        std::string item;
        while (std::getline(ss, item, ' ')) {
            if ( ! item.empty() ) {
                //std::cout << "[31micg_ignore_types[00m " << item << std::endl ;
                ignore_types.insert(item) ;
            }
        }

    }

    return ignore_types ;
}

#include <unistd.h>
#include <sys/wait.h>

/*
  As of right now I call a perl script to parse the header comment and return
  the library dependencies because perl is awesome!
  TODO: Someday when C++11 is working on all the platforms we support we should
  rewrite this using the language built in regular expressions.
  TODO: Only fork and exec a single instance of the script.
*/

std::vector< std::string > CommentSaver::getLibraryDependencies( std::string file_name ) {

    pid_t pid ;
    int pipes[4] ;

    // Open two pipes to allow us to write to and read from child process.
    pipe(&pipes[0]) ;
    pipe(&pipes[2]) ;
    if (( pid = fork()) == 0 ) {
        // child pipes: read = pipe[2], write = pipe[1]
        close(pipes[0]) ;
        close(pipes[3]) ;
        dup2(pipes[2], STDIN_FILENO) ;
        dup2(pipes[1], STDOUT_FILENO) ;

        // exec the perl script that parses header comments.
        std::string parse_lib_deps_path = std::string(getenv("TRICK_HOME")) + "/libexec/trick/ICG_lib_deps_helper" ;
        execl(parse_lib_deps_path.c_str(), parse_lib_deps_path.c_str(), file_name.c_str(), (char *)NULL) ;
        exit(1) ;
    }
    // parent pipes: read = pipe[0], write = pipe[3]
    close(pipes[1]) ;
    close(pipes[2]) ;

    // get the header comment and send it with a end delimiter to the perl script
    std::string header = getTrickHeaderComment(file_name) + "\nEND ICG PROCESSING\n" ;
    write(pipes[3], header.c_str() , header.size()) ;

    // wait for the child process to end
    int status ;
    waitpid(pid, &status, 0) ;

    // read the result from the perl script
    int num_read ;
    char buf[4096] ;
    std::string response ;
    while (( num_read = read(pipes[0], buf, sizeof(buf) - 1) ) > 0 ) {
        buf[num_read] = 0 ;
        response += buf ;
    }

    // parse the single string result into a vector of strings.
    std::vector< std::string > lib_deps ;
    std::string::size_type pos = 0;
    std::string::size_type prev = 0;
    while ((pos = response.find("\n", prev)) != std::string::npos)
    {
        lib_deps.push_back(response.substr(prev, pos - prev));
        prev = pos + 1;
    }
    lib_deps.push_back(response.substr(prev));

    return lib_deps ;
}
