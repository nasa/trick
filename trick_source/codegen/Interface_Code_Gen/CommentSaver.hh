
#ifndef COMMENTSAVER_HH
#define COMMENTSAVER_HH

#include <map>
#include <set>
#include <vector>

#include "clang/Lex/Preprocessor.h"
#include "clang/Frontend/CompilerInstance.h"

#include "HeaderSearchDirs.hh"

/**

  This class is passed to the clang preprocessor as a comment handler.  The class
  saves the source locations of each comment found.  Comments may be retrived by
  file name and line number.

  @author Alexander S. Lin

  @date July 2012

 */

class CommentSaver : public clang::CommentHandler {
    public:
        CommentSaver( clang::CompilerInstance & in_ci , HeaderSearchDirs & in_hsd ) ;

        /** Called by the PreProcessor when a comment is parsed. HandleComment saves
            the comment indexed by file name and line number in the comment map
            @param PP = the preprocessor (unused)
            @param source_range = the bounding line and column of the comment
            @param always false
          */
        virtual bool HandleComment(clang::Preprocessor &PP, clang::SourceRange source_range) ;

        /** Retrieves comments out of the comment map.  Returns the full comment string
            @param sr = clang::SourceRange type marking comment location.
            @return std::string the last comment starting on that line, or empty string if none found
         */
        std::string getComment( clang::SourceRange sr ) ;

        /** Retrieves comments out of the comment map.  Returns the full comment string
            @param file_name = File name to search
            @param line_no = line number to search
            @return std::string the last comment starting on that line, or empty string if none found
         */
        std::string getComment( std::string file_name , unsigned int line_no ) ;

        /** Searches all comments found within the file_name parameter for a Trick header comment
            @param file_name = File name to search
            @return std::string a comment that matches the Trick header comment format
         */
        std::string getTrickHeaderComment( std::string file_name ) ;

        /** sets the flags for hasICGNo and hasICGNoComment
            @param file_name = File name to search
         */
        void getICGField( std::string file_name ) ;

        /** Returns if the header has a Trick header comment
            @param file_name = File name to search
            @return true = if header comment found
         */
        bool hasTrickHeader(std::string file_name ) ;

        /** Searches the Trick header comment for the ICG:(No) entry.
            @param file_name = File name to search
            @return true = ICG:(No) was found.
         */
        bool hasICGNo( std::string file_name ) ;

        /** Searches the Trick header comment for the ICG:(NoComment) entry.
            @param file_name = File name to search
            @return true = ICG:(NoComment) was found.
         */
        bool hasICGNoComment( std::string file_name ) ;

        /** Searches the Trick header comment for the ICG_EXCLUDE_TYPES field
            @param file_name = File name to search
            @return set of strings that list the type names to ignore.
         */
        std::set< std::string > getIgnoreTypes( std::string file_name ) ;

    private:
        /** The compiler's source manager.  Holds file/line info for everything. */
        clang::CompilerInstance & ci ;

        HeaderSearchDirs & hsd ;

        /** Map of file name to ICG: (No) found */
        std::map < std::string , bool > icg_no_found ;

        /** Map of file name to ICG: (NoComment) found */
        std::map < std::string , bool > icg_no_comment_found ;

        /** All comments source locations indexed by file name then line number */
        std::map < std::string , std::map < unsigned int , clang::SourceRange > > comment_map ;

        /** Store Trick header comments so we don't have to look them up over and over */
        std::map < std::string , std::string > trick_header_comments ;
} ;

#endif
