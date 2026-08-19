
#include <vector>
#include <string>
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Lex/PPCallbacks.h"
#include "HeaderSearchDirs.hh"

/*
   FindTrickICG searches preprocessor directives #if/#elif/#ifdef/#ifndef to see if they use TRICK_ICG
   If they do reference TRICK_ICG we need to mark the stack of incude files that include the TRICK_ICG.
   We also print a warning to the screen where the TRICK_ICG was found.
 */

class FindTrickICG final : public clang::PPCallbacks
{
    public:
        FindTrickICG(clang::CompilerInstance& in_ci, HeaderSearchDirs& in_hsd, bool in_print_msgs);

        // called when the file changes for a variety of reasons.
        void FileChanged(clang::SourceLocation Loc, FileChangeReason Reason, clang::SrcMgr::CharacteristicKind FileType,
                         clang::FileID PrevFID = clang::FileID()) override;

#if (LIBCLANG_MAJOR < 10) // TODO delete when RHEL 7 no longer supported
        void FileSkipped(const clang::FileEntry& SkippedFile, const clang::Token& FilenameTok,
                         clang::SrcMgr::CharacteristicKind FileType) override;
#else
        // called when a header file is skipped because of a header guard optimization.
        void FileSkipped(const clang::FileEntryRef& SkippedFile, const clang::Token& FilenameTok,
                         clang::SrcMgr::CharacteristicKind FileType) override;
#endif

    // callbacks called when the preprocessor directives of types are processed.
    void If(clang::SourceLocation Loc, clang::SourceRange ConditionRange,
            clang::PPCallbacks::ConditionValueKind ConditionValue) override;
    void Elif(clang::SourceLocation Loc, clang::SourceRange ConditionRange,
              clang::PPCallbacks::ConditionValueKind ConditionValue, clang::SourceLocation IfLoc) override;
    void Ifdef(clang::SourceLocation Loc, const clang::Token& MacroNameTok, const clang::MacroDefinition& MD) override;
    void Ifndef(clang::SourceLocation Loc, const clang::Token& MacroNameTok, const clang::MacroDefinition& MD) override;

    // print a warning about using TRICK_ICG.
    void print_header() ;

  private:

    // compiler instance to help locating file names
    clang::CompilerInstance & ci ;

    HeaderSearchDirs & hsd ;

    // Are we printing warning messages?
    bool print_msgs ;

    // Have we printed the big warning about TRICK_ICG?
    bool header_printed = false;

    // Using a vector as a stack to hold the stack of included headers we have entered.
    std::vector<std::string> included_files ;
};
