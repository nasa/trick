
set ( DP_EQPARSE_SRC
  eqparse
  eqparse_chkvalid
  eqparse_error
  eqparse_evaluate
  eqparse_fillno
  eqparse_funcsub
  eqparse_math
  eqparse_negcheck
  eqparse_operatorcheck
  eqparse_postfix
  eqparse_stack
  eqparse_takeinput
  eqparse_test
)

add_library( dp_eqparse STATIC ${DP_EQPARSE_SRC})

