
read = 2.0

/*=== FLOATING POINT, INTEGER, AND VOID #define CAPABILITIES ===*/

#define DBL_NUM  7.7777777777777777
#define FLT_NUM  1.2345
#define INT_NUM  12345

#define INT_NUM +     /* INT_NUM NOW = 12346 */
#define INT_NUM 12356 /* INT_NUM NOW = 12356 */

#define FLT_NUM +        /* FLT_NUM NOW = 2.2345 */
#define FLT_NUM 12.2345  /* FLT_NUM NOW = 12.2345 */

#define NO_VALUE

test.ip.c_array_types.ia[0][0][0] = INT_NUM
test.ip.c_array_types.da[0][0][0] = FLT_NUM
test.ip.c_array_types.da[0][0][1] = DBL_NUM


test.ip.c_array_types.sa[0][0][0] = 0 , 0
test.ip.c_array_types.sa[0][1][0] = 0 , 0
#ifdef NO_VALUE
test.ip.c_array_types.sa[0][0][0] = 111 , 222
#endif
#ifndef NO_VALUE
test.ip.c_array_types.sa[0][1][0] = 1 , 2
#endif

test.ip.c_array_types.sa[1][0][0] = 0 , 0
test.ip.c_array_types.sa[1][1][0] = 0 , 0
#ifdef VALUE
test.ip.c_array_types.sa[1][0][0] = 111 , 222
#endif
#ifndef VALUE
test.ip.c_array_types.sa[1][1][0] = 1 , 2
#endif

test.ip.c_array_types.usa[1][0][0] = 0 , 0
test.ip.c_array_types.usa[1][1][0] = 0 , 0
#ifdef INT_NUM
test.ip.c_array_types.usa[1][0][0] = 1111 , 2222
#endif
#ifndef INT_NUM
test.ip.c_array_types.usa[1][1][0] = 11 , 22
#endif

test.ip.c_array_types.uia[0][0][0] = 0 , 0
test.ip.c_array_types.uia[0][1][0] = 0 , 0
#ifdef FLT_NUM
test.ip.c_array_types.uia[0][0][0] = 1111 , 2222
#endif
#ifndef FLT_NUM
test.ip.c_array_types.uia[0][1][0] = 11 , 22
#endif

test.ip.c_array_types.uia[1][0][0] = 0 , 0
test.ip.c_array_types.uia[1][1][0] = 0 , 0
#if INT_NUM == 12356
test.ip.c_array_types.uia[1][0][0] = 1111 , 2222
#endif
#if INT_NUM != 12356
test.ip.c_array_types.uia[1][1][0] = 11 , 22
#endif

test.ip.c_array_types.la[0][0][0] = 0 , 0
test.ip.c_array_types.la[0][1][0] = 0 , 0
#if INT_NUM == 1
test.ip.c_array_types.la[0][0][0] = 1111 , 2222
#endif
#if INT_NUM != 1
test.ip.c_array_types.la[0][1][0] = 11 , 22
#endif

test.ip.c_array_types.la[1][0][0] = 0 , 0
test.ip.c_array_types.la[1][1][0] = 0 , 0
#if INT_NUM == 12356
test.ip.c_array_types.la[1][0][0] = 1111 , 2222
#endif
#if INT_NUM == 0
test.ip.c_array_types.la[1][1][0] = 11 , 22
#endif

test.ip.c_array_types.fa[0][0][0] = 0 , 0
test.ip.c_array_types.fa[0][1][0] = 0 , 0
#if INT_NUM != 1
test.ip.c_array_types.fa[0][0][0] = 1111 , 2222
#endif
#if INT_NUM != 2
test.ip.c_array_types.fa[0][1][0] = 11 , 22
#endif

/*=== CHARACTER STRING #define CAPABILITY ===*/

#define STR  "test"
#define F    "meowmeowmeow"
#define STRF "mooomooomooo"

test.ip.c_array_types.fa[1][0][0] = 0 , 0
test.ip.c_array_types.fa[1][1][0] = 0 , 0
#ifdef STRF
test.ip.c_array_types.fa[1][0][0] = 1111 , 2222
#endif
#ifndef STRF
test.ip.c_array_types.fa[1][1][0] = 11 , 22
#endif

#define TH ${TRICK_HOME}

test.ip.c_types.l = 123456789
test.ip.c_types.cp                   = "#(TH)"
test.ip.c_pointer_types.cpp[0][0][0] = "#(TH)" , "#(TH)";
test.ip.c_pointer_types.cpp[0][1][0] = "#(TH)" , "#(TH)"
test.ip.c_pointer_types.cpp[1][0][0] = "#{TH}" , "#(TH)"
test.ip.c_pointer_types.cpp[1][1][0] = "#(TH)" , "#(TH)"

test.ip.c_mixed_types.cpa[0][0][0] = "#(TH)" , "#{TH}"
test.ip.c_mixed_types.cpa[0][1][0] = "#(TH)" , "#(TH)"
test.ip.c_mixed_types.cpa[1][0][0] = "#(TH)" , "#(TH)"
test.ip.c_mixed_types.cpa[1][1][0] = "#(TH)" , "#(TH)"

test.ip.units_test[0][0][0].measure = feet
