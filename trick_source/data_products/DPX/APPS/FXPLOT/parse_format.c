
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int parse_format_string(const char *user_fmt_string,   /* In */
                              int *sign,              /* Out*/
                              int *width,             /* Out*/
                              int *precision,         /* Out*/
                              int *conv_char          /* Out*/ ) {
    int done = 0;
    int state = 0;
    int ret_code = -1; // Assume error until validated.
    char *p;
    int fsign = 0;
    int fwidth = 0;
    int fprecision = 0;
    char temp_s[256];

    if (!user_fmt_string) return(ret_code);

    strncpy(temp_s, user_fmt_string, 256);
    p = temp_s;
    while (!done) {
        switch (state) {
        case 0: {
                 if (*p == '%') {
                     state = 1;
                 }
                 p++;
             } break;
        case 1: {
                 if ((*p == '+') || (*p =='-')) {
                        fsign = (int)*p;
                        state = 2; p++;
                 } else if (isdigit(*p)) {
                        fwidth = (int)(*p - '0') ;
                        state = 3; p++;
                 } else { // Error
                        // printf("ERROR 1 \'%c\'\n",*p);
                        done = 1;
                 }
             } break;
        case 2: {
                 if (isdigit(*p)) {
                        fwidth = (int)(*p - '0') ;
                        state = 3; p++;
                 } else { // Error
                        // printf("ERROR 2\n");
                        done = 1;
                 }
             } break;
        case 3: {
                 if (isdigit(*p)) {
                        fwidth = fwidth * 10 +(int)(*p - '0') ;
                        p++;
                 } else if (*p == '.') {
                        state = 4; p++;
                 } else { // Error
                        // printf("ERROR 3\n");
                        done = 1;
                 }
             } break;
        case 4: {
                 if (isdigit(*p)) {
                        fprecision = (int)(*p - '0') ;
                        state = 5; p++;
                } else { //Error
                        // printf("ERROR 4\n");
                        done = 1;
                }
        } break;
        case 5: {
                 if (isdigit(*p)) {
                        fprecision = fprecision * 10 + (int)(*p - '0') ;
                        p++;
                } else if (*p=='e'||*p=='E'||*p=='f'||*p=='g'||*p=='G') {
                        *conv_char = *p;
                        *width     = fwidth;
                        *precision = fprecision;
                        *sign      = fsign;
                        ret_code = 0;
                        done = 1;
                } else { //Error
                        // printf("ERROR 5\n");
                        done = 1;
                }
        } break;
        } //switch
    } // while

//    free( p);
    return(ret_code);
}
