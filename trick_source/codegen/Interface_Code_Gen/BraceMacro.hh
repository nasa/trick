#ifndef BRACEMACRO_HH
#define BRACEMACRO_HH

#if (LIBCLANG_MAJOR > 3) || ((LIBCLANG_MAJOR == 3) && (LIBCLANG_MINOR >= 9))
#define RBRACELOC getBraceRange().getEnd
#else
#define RBRACELOC getRBraceLoc
#endif

#endif
