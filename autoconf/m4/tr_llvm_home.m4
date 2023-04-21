

AC_DEFUN([TR_LLVM_HOME],[
    AC_ARG_WITH([llvm],
                AS_HELP_STRING([--with-llvm@<:@=DIR@:>@], [LLVM root directory]),
                [LLVM_HOME="$withval"
                 AC_PATH_PROG(LLVM_CONFIG, llvm-config, no-llvm-config, "$LLVM_HOME/bin")
                 AS_IF([test "$ac_cv_path_LLVM_CONFIG" = "no-llvm-config"],AC_MSG_ERROR([could not find llvm-config]),[])
                ],
                [
                 AC_PATH_PROG(LLVM_CONFIG, llvm-config, no-llvm-config, "/bin:/usr/bin:/usr/local/bin:/sw/bin:/usr/local/opt/llvm/bin")
                 AS_IF([test "$ac_cv_path_LLVM_CONFIG" = "no-llvm-config"],AC_MSG_ERROR([could not find llvm-config]),[])
                 LLVM_HOME=`$LLVM_CONFIG --prefix`
                ]
               )
])