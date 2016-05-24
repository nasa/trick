
#include <iostream>
#include <sstream>
#include <regex.h>
#include <algorithm>

#include "clang/Basic/FileManager.h"

#include "CommentSaver.hh"
#include "Utilities.hh"

CommentSaver::CommentSaver(clang::SourceManager & in_sm ) : sm(in_sm) {}

bool CommentSaver::HandleComment(clang::Preprocessor &PP, clang::SourceRange Comment) {

    if ( ! sm.isInSystemHeader(Comment.getBegin()) ) {
        std::string file_name = sm.getBufferName(Comment.getBegin()) ;
        char * resolved_path = almostRealPath( file_name.c_str() ) ;
        if ( resolved_path != NULL ) {
            unsigned int line_no = sm.getSpellingLineNumber(Comment.getBegin()) ;
            comment_map[std::string(resolved_path)][line_no] = Comment ;
            free(resolved_path) ;
        }
    }

    // returning false means we did not push any text back to the stream for further reads.
    return false ;
}

std::string CommentSaver::getComment( clang::SourceRange sr ) {

    if ( sr.isValid() ) {
        /* fsl_begin and fsl_end are two pointers into the header file.  We want the text between the two pointers. */
        clang::FullSourceLoc fsl_begin(sr.getBegin() , sm) ;
        clang::FullSourceLoc fsl_end(sr.getEnd() , sm) ;
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

    std::string ret ;
    char * resolved_path = almostRealPath( file_name.c_str() ) ;
    if ( resolved_path != NULL ) {
        if ( trick_header_comments.find(file_name) == trick_header_comments.end() ) {
            trick_header_comments[resolved_path] = std::string() ;
            for ( cit = comment_map[resolved_path].begin() ; cit != comment_map[resolved_path].end() ; cit++ ) {
                std::string comment_str = getComment((*cit).second) ;
                std::transform(comment_str.begin(), comment_str.end(), comment_str.begin(), ::toupper) ;
                if ( comment_str.find("PURPOSE") != std::string::npos ) {
                    trick_header_comments[resolved_path] = comment_str ;
                    break ;
                }
            }
        }
        ret = trick_header_comments[resolved_path] ;
        free(resolved_path) ;
    }

    return ret ;

}

void CommentSaver::getICGField( std::string file_name ) {

    /* default the icg_no flags to false */
    icg_no_found[file_name] = false ;
    icg_no_comment_found[file_name] = false ;

    std::string th_str = getTrickHeaderComment(file_name) ;
    if ( ! th_str.empty() ) {

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

bool CommentSaver::hasTrickHeader( std::string file_name ) {
    std::string th_str = getTrickHeaderComment(file_name) ;
    return (! th_str.empty()) ;
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

