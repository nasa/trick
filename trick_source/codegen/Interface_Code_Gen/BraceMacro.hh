#ifndef BRACEMACRO_HH
#define BRACEMACRO_HH

#define RBRACELOC getBraceRange().getEnd

#if (LIBCLANG_MAJOR >=8)
#define GETLOCEND getEndLoc
#else
#define GETLOCEND getLocEnd
#endif

#endif
