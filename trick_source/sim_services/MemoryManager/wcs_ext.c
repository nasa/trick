/*
   PURPOSE: ( Functions to convert narrow (char) strings to wide (wchar_t) strings and vice-versa, as well as functions
   to determine the number of characters that would result, without actually performing a conversion.)

   PROGRAMMERS: (((John M. Penn) (L-3 Communications) (08/2006))) */

#if !( __Lynx__ || __sgi || __INTERIX)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wctype.h>
#include <limits.h>
#include "trick/wcs_ext.h"


size_t wcs_to_ncs_len(const wchar_t * wcs)
{
    return wcs_to_ncs(wcs, (char *) NULL, (size_t) 0);
}

size_t ncs_to_wcs_len(const char *ncs)
{
    return ncs_to_wcs(ncs, (wchar_t *) NULL, (size_t) 0);
}

size_t wcs_to_ncs(const wchar_t * wcs, char *ncs, size_t ncs_max_len)
{
    unsigned int i, wlen, nlen;

    if (wcs == (wchar_t *) NULL) {
        fprintf(stderr, "%s:wcs_to_ncs: Wide input character string is NULL. No conversion performed.\n", __FILE__);
        return 0;
    }

    wlen = wcslen(wcs);
    nlen = 0;
    if (ncs != NULL) {
        ncs[0] = '\0';
    }

    for (i = 0; i < wlen; i++) {
        wint_t wcint = wcs[i];
        char work_s[11];

        if (iswprint(wcint)) {
            int len;
            /* The wide character is printable in the current locale, so create a multibyte, external representation of
               it. Note that the external representation will also be printable in the current locale. */

            if (wcint == '\'') {
                sprintf(work_s, "\\\'");
            } else if (wcint == '\"') {
                sprintf(work_s, "\\\"");
            } else if (wcint == '\?') {
                sprintf(work_s, "\\\?");
            } else if (wcint == '\\') {
                sprintf(work_s, "\\\\");
            } else {
                len = wcrtomb(work_s, (wchar_t) wcint, NULL);
                work_s[len] = '\0';
            }

        } else {
            /* The wide character is not printable in the current locale so, create a printable, external
               representation of this non-printable character. */
            if (wcint <= 0xFF) {
                if (wcint == '\a') {
                    sprintf(work_s, "\\a");
                } else if (wcint == '\b') {
                    sprintf(work_s, "\\b");
                } else if (wcint == '\f') {
                    sprintf(work_s, "\\f");
                } else if (wcint == '\n') {
                    sprintf(work_s, "\\n");
                } else if (wcint == '\r') {
                    sprintf(work_s, "\\n");
                } else if (wcint == '\t') {
                    sprintf(work_s, "\\t");
                } else if (wcint == '\v') {
                    sprintf(work_s, "\\v");
                } else {
                    sprintf(work_s, "\\x%02x", wcint);
                }
            } else if (wcint <= 0xFFFF) {
                sprintf(work_s, "\\u%04X", wcint);
            } else {
                sprintf(work_s, "\\U%08X", wcint);
            }
        }

        nlen += strlen(work_s);

        if (ncs != NULL) {
            if (nlen < ncs_max_len) {
                strcat(ncs, work_s);
            } else {
                fprintf(stderr, "%s:wcs_to_ncs: Insufficient storage for the (narrow) output character string.\n",
                        __FILE__);
                return 0;
            }
        }

    }                                  /* end-for each wide character */

    return (nlen);
}

size_t ncs_to_wcs(const char *ncs, wchar_t * wcs, size_t w_max_size)
{

    mbstate_t state;
    wchar_t wc_in, wc_out = 0;
    int code = 0;
    int pstate = 0;
    size_t wlen = 0;
    size_t nbytes;
    size_t len;                 /* number of bytes remaining in the input string. */

    if (ncs == (char *) NULL) {
        fprintf(stderr, "%s:ncs_to_wcs: Narrow input character string is NULL. No conversion performed.\n", __FILE__);
        return 0;
    }
    len = strlen(ncs);

    memset(&state, '\0', sizeof(state));

    /* While we have another wide-character and there is sufficient space to store it ... The extra (len > 0 ) check is
       for the Macs because mbrtowc still returns values > 0 if len == 0 */
    while (((nbytes = mbrtowc(&wc_in, ncs, len, &state)) > 0) && (len > 0)) {

        wint_t wcint;
        int digits_remaining = 0;
        int digit = 0;

        len -= nbytes;
        ncs += nbytes;
        wcint = (wint_t) wc_in;

        if (pstate && (wcint > 0x7F)) {
            // We should only find ASCII characters in the escaped state.
            fprintf(stderr, "%s:ncs_to_wcs: Unknown escape sequence. No conversion performed.\n", __FILE__);
            return 0;
        }

        switch (pstate) {
            case 0:{
                    if (wcint == '\\') {
                        pstate = 1;
                    } else {
                        // Store the wide-character.
                        wc_out = wc_in;
                    }
                }
                break;

            case 1:{
                    switch (wcint) {
                        case '\'':
                        case '\"':
                        case '\?':
                        case '\\':{
                                wc_out = wc_in;
                                pstate = 0;
                            }
                            break;

                        case 'a':{
                                wc_out = btowc((int) '\a');
                                pstate = 0;
                            } break;
                        case 'b':{
                                wc_out = btowc((int) '\b');
                                pstate = 0;
                            } break;
                        case 'f':{
                                wc_out = btowc((int) '\f');
                                pstate = 0;
                            } break;
                        case 'n':{
                                wc_out = btowc((int) '\n');
                                pstate = 0;
                            } break;
                        case 'r':{
                                wc_out = btowc((int) '\r');
                                pstate = 0;
                            } break;
                        case 't':{
                                wc_out = btowc((int) '\t');
                                pstate = 0;
                            } break;
                        case 'v':{
                                wc_out = btowc((int) '\v');
                                pstate = 0;
                            } break;
                        case 'x':{
                                pstate = 2;
                                digits_remaining = 2;
                                code = 0;
                            }
                            break;
                        case 'u':{
                                pstate = 2;
                                digits_remaining = 4;
                                code = 0;
                            }
                            break;
                        case 'U':{
                                pstate = 2;
                                digits_remaining = 8;
                                code = 0;
                            }
                            break;
                        default:{
                                fprintf(stderr, "%s:ncs_to_wcs: Unknown escape sequence. No conversion performed.\n",
                                        __FILE__);
                                return 0;
                            }
                    }                  // switch wcint

                }
                break;                 // case 1

            case 2:{
                    if (digits_remaining > 0) {
                        if (wcint >= '0' && wcint <= '9') {
                            digit = (int) wcint - (int) '0';
                        } else if (wcint >= 'A' && wcint <= 'F') {
                            digit = (int) wcint - (int) 'A' + 10;;
                        } else if (wcint >= 'a' && wcint <= 'f') {
                            digit = (int) wcint - (int) 'a' + 10;;
                        } else {
                            // error
                        }
                        code = code * 16 + digit;
                        digits_remaining--;

                        if (digits_remaining == 0) {
                            wc_out = code;
                            pstate = 0;
                        }
                    }
                }
                break;
        }                              /* switch */

        if (pstate == 0) {
            if (wcs != NULL) {
                if (wlen < w_max_size) {
                    wcs[wlen] = wc_out;
                } else {
                    fprintf(stderr, "%s:ncs_to_wcs: Insufficient storage for the (wide) output character string.\n",
                            __FILE__);
                }
            }
            wlen++;
        }


    }                                  /* while */

    if (wcs == NULL) {
        return wlen;
    }

    if (wlen < w_max_size) {
        wcs[wlen] = 0 ;
        return wlen;
    } else {
        fprintf(stderr, "%s:ncs_to_wcs: Insufficient storage for the (wide) output character string.\n", __FILE__);
        return 0;
    }
}
#else
int wcs_to_ncs_len(void)
{
    return (0);
}

int ncs_to_wcs_len(void)
{
    return (0);
}

int wcs_to_ncs(void)
{
    return (0);
}

int ncs_to_wcs(void)
{
    return (0);
}
#endif
