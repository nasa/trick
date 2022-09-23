
#ifndef PARSE_FORMAT_H
#define PARSE_FORMAT_H

#ifdef __cplusplus
extern "C" {
#endif

int parse_format_string( const char *user_fmt_string,   /* In */
                         int *sign,              /* Out*/
                         int *width,             /* Out*/
                         int *precision,         /* Out*/
                         int *conv_char          /* Out*/ );

#ifdef __cplusplus
}
#endif

#endif
