#include <string.h>
#include <stdio.h>
#include <stddef.h>
#include <wchar.h>
#include <ctype.h>
#include <stdint.h>
#include "trick/unicode_utils.h"

/* Maintainer: John M. Penn */

size_t ucodepoint_to_utf32(unsigned int codePoint, int32_t *out) {

    if (codePoint < 0xd800 || codePoint >= 0xe000) { /* Not Surrogate */
        *out = (int32_t)codePoint;
        return 1;
    } else {
        fprintf(stderr,"%s:ERROR: 0x%08x is reserved for UTF-16, as a surrogate codepoint.\n", __FUNCTION__, codePoint);
    }
    return 0;
}

size_t ucodepoint_to_utf16(unsigned int codePoint, int16_t (*out)[2]) {

    if (codePoint > 0x10ffff) {
        fprintf(stderr,"%s:ERROR: Invalid Unicode value (too big): 0x%04x.\n", __FUNCTION__, codePoint);
        return 0;
    } else if (codePoint > 0xffff) {
        /* High-surrogate code points are in the range U+D800–U+DBFF. 
         * Low-surrogate code points are in the range U+DC00–U+DFFF. 
         * A high-surrogate code point followed by a low-surrogate code point form a
         * surrogate pair in UTF-16 to represent code points greater than U+FFFF.
         */
        (*out)[0] = (int16_t)(0xd800 + (codePoint >> 10));    /* Create High Surrogate */
        (*out)[1] = (int16_t)(0xdc00 + (codePoint & 0x03ff)); /* Create Low Surrogate */
        return 2;
    } else if (codePoint < 0xd800 || codePoint >= 0xe000) { /* Not Surrogate */
        (*out)[0] = (int16_t)(codePoint);
        return 1;
    } else {
        fprintf(stderr,"%s:ERROR: Invalid Unicode value (surrogate): 0x%04x.\n", __FUNCTION__, codePoint);
    }
    return 0;
}

size_t ucodepoint_to_utf8(unsigned int codePoint, char (*out)[4]) {

    if (codePoint <= 0x7f) { /* ASCII */
        (*out)[0] = (char)codePoint;                           /* 0xxxxxxx 0x00..0x7F*/
        return 1;

    } else if (codePoint <= 0x7ff) { /* Two-byte Sequence */
        (*out)[0] = (char)(0xc0 | ((codePoint >> 6) & 0x1f));  /* 110xxxxx 0xC0..0xDF*/
        (*out)[1] = (char)(0x80 | (codePoint & 0x3f));         /* 10xxxxxx */
        return 2;

    } else if (codePoint <= 0xffff) { /* Three byte Sequence */
        (*out)[0] = (char)(0xe0 | ((codePoint >> 12) & 0x0f)); /* 1110xxxx 0xE0..0xEF*/
        (*out)[1] = (char)(0x80 | ((codePoint >> 6)  & 0x3f)); /* 10xxxxxx */
        (*out)[2] = (char)(0x80 | (codePoint & 0x3f));         /* 10xxxxxx */
        return 3;

    } else { /* Four-byte Sequence */
        (*out)[0] = (char)(0xf0 | ((codePoint >> 18) & 0x07)); /* 11110xxx 0xF0..0xF7*/
        (*out)[1] = (char)(0x80 | ((codePoint >> 12) & 0x3f)); /* 10xxxxxx */
        (*out)[2] = (char)(0x80 | ((codePoint >> 6)  & 0x3f)); /* 10xxxxxx */
        (*out)[3] = (char)(0x80 | (codePoint & 0x3f));         /* 10xxxxxx 0x80..0xBF */
        return 4;
    }
    return 0;
}

size_t utf8_to_printable_ascii(const char *in, char *out, size_t outSize) {

    int state = 0;
    unsigned int codePoint;
    char wks[11];

    if (out == NULL) {
        fprintf(stderr,"%s:ERROR: ASCII char pointer (out) is NULL. No conversion performed.\n", __FUNCTION__);
        return 0;
    }
    out[0] = 0;

    if (in == NULL) {
        fprintf(stderr,"%s:ERROR: UTF8 char-pointer (in) is NULL. No conversion performed.\n", __FUNCTION__);
        return 0;
    }

    while (*in != 0) {
        unsigned char ch = *in;
        switch (state) {
            case 0: {
                if (ch >= 0xf0) {          // Start of a 4-byte sequence.
                    codePoint = ch & 0x07; // Extract low 3 bits
                    state = 3;
                } else if (ch >= 0xe0) {   // Start of a 3-byte sequence.
                    codePoint = ch & 0x0f; // Extract low 4 bits
                    state = 2;
                } else if (ch >= 0xc0) {   // Start of a 2-byte sequence.
                    codePoint = ch & 0x1f; // Extract low 5 bits
                    state = 1;
                } else if (ch >= 0x80) {   // We should never find a continuation byte in isolation.
                    fprintf(stderr,"%s:ERROR: UTF8 string (in) appears to be corrupted.\n", __FUNCTION__);
                    state = 99;
                } else {                   // ASCII        
                    if (ch == '\a') {
                        sprintf(wks,"\\a");
                    } else if (ch == '\b') {
                        sprintf(wks,"\\b");
                    } else if (ch == '\f') {
                        sprintf(wks,"\\f");
                    } else if (ch == '\n') {
                        sprintf(wks,"\\n");
                    } else if (ch == '\r') {
                        sprintf(wks,"\\r");
                    } else if (ch == '\t') {
                        sprintf(wks,"\\t");
                    } else if (ch == '\v') {
                        sprintf(wks,"\\v");
                    } else if (isprint(ch)) {
                        sprintf(wks,"%c",ch);
                    } else {
                        sprintf(wks,"\\x%02x",ch);
                    }
                    if ((strlen(out)+strlen(wks)) < outSize-1) {
                        strcat(out, wks);
                    } else {
                        fprintf(stderr,"%s:ERROR: Insufficient room in (out) array.\n", __FUNCTION__);
                        state = 99;
                    }
                }
            } break;
            case 1: { // Expecting one continuation byte.
                if ((ch & 0xc0) == 0x80) { // If the next char is a continuation byte ..
                    codePoint = (codePoint << 6) | (ch & 0x3f); // Extract low 6 bits
                    state = 0;

                    if (codePoint <= 0xffff) {
                        sprintf(wks,"\\u%04x", codePoint); 
                    } else {
                        sprintf(wks,"\\U%08x", codePoint); 
                    }
                    if ((strlen(out)+strlen(wks)) < outSize-1) {
                        strcat(out, wks);
                    } else {
                        fprintf(stderr,"%s:ERROR: Insufficient room in (out) array.\n", __FUNCTION__);
                        state = 99;
                    }

                } else {
                    fprintf(stderr,"%s:ERROR: UTF8 string appears to be corrupted.\n", __FUNCTION__);
                    state = 99;
                }
            } break;
            case 2: { /* Expecting two continuation bytes. */
                if ((ch & 0xc0) == 0x80) {
                    codePoint = (codePoint << 6) | (ch & 0x3f);
                    state = 1;
                } else {
                    fprintf(stderr,"%s:ERROR: UTF8 string appears to be corrupted.\n", __FUNCTION__);
                    state = 99;
                }
            } break;
            case 3: { /* Expecting three continuation bytes. */
                if ((ch & 0xc0) == 0x80) {
                    codePoint = (codePoint << 6) | (ch & 0x3f);
                    state = 2;
                } else {
                    fprintf(stderr,"%s:ERROR: UTF8 string appears to be corrupted.\n", __FUNCTION__);
                    state = 99;
                }
            } break;
            default: { 
                out[0] = 0;
                return 0;
            } break;
        }
        in ++;
    }

    /* If we finished in state 0, then we're good. Just
       terminate the string, otherwise we had an error. */
    if (state == 0) {
        return strlen(out);
    } else {
        out[0] = 0;
        return 0;
    }
}

/* Un-escapes ASCII and Unicode escape sequences, and encodes them into UTF-8. */
size_t ascii_to_utf8(const char *in, char *out, size_t outSize) {

    unsigned int codePoint = 0;
    size_t len = 0;
    int state = 0;
    int digitsExpected = 0;

    if (out == NULL) {
        fprintf(stderr,"%s:ERROR: ASCII char pointer (out) is NULL. No conversion performed.\n", __FUNCTION__);
        return 0;
    }
    out[0] = 0;

    if (in == NULL) {
        fprintf(stderr,"%s:ERROR: UTF8 char-pointer (in) is NULL. No conversion performed.\n", __FUNCTION__);
        return 0;
    }

    while (*in != 0) {
        unsigned char ch = *in;
        if (ch > 0x7f) { /* All input characters must be ASCII. */
            fprintf(stderr,"%s:ERROR: ASCII string (in) contains non-ASCII values.\n", __FUNCTION__);
            out[0] = 0; 
            return 0;
        }
        /* All escaped characters will be un-escaped. */
        switch(state) {
            case 0: { // Normal State
                if (ch =='\\') {
                    state = 1;
                } else {
                    out[len++] = ch;
                }
            } break;
            case 1: { // Escaped State ( that is: we've found a '\' character.)
                switch(ch) {
                    case '\'':
                    case '\"':
                    case '\?':
                    case '\\': {
                        out[len++] = ch; state = 0;
                    } break;

                    case 'a': { out[len++] = '\a';  state = 0; } break;
                    case 'b': { out[len++] = '\b';  state = 0; } break;
                    case 'f': { out[len++] = '\f';  state = 0; } break;
                    case 'n': { out[len++] = '\n';  state = 0; } break;
                    case 'r': { out[len++] = '\r';  state = 0; } break;
                    case 't': { out[len++] = '\t';  state = 0; } break;
                    case 'v': { out[len++] = '\b';  state = 0; } break;
                    case 'x': { digitsExpected = 2; state = 2; } break;
                    case 'u': { digitsExpected = 4; state = 2; } break;
                    case 'U': { digitsExpected = 8; state = 2; } break;
                    default : {
                    }
                } // switch ch
            } break;
            case 2: { // Escaped Unicode ( that is: we've found '\x', '\u' or '\U'.)
                 int digit = 0;
                 if (ch >= '0' && ch <= '9') {
                     digit = ch - (int)'0';
                 } else if (ch >= 'A' && ch <= 'F') {
                     digit = ch - (int)'A' + 10;
                 } else if (ch >= 'a' && ch <= 'f') {
                     digit = ch - (int)'a' + 10;
                 } else {
                     fprintf(stderr,"%s:ERROR: Insufficient hexidecimal digits following"
                                    " \\x, \\u, or \\U escape code in char string (in).\n", __FUNCTION__);
                     out[0] = 0;
                     return 0;
                 }
                 codePoint = codePoint * 16 + digit;
                 digitsExpected -- ;
                 if ( digitsExpected == 0 ) {
                    char temp[4];
                    size_t count = ucodepoint_to_utf8(codePoint, &temp);
                    if (count < (outSize-len)) {
                        memcpy( &out[len], temp, sizeof(char) * count );
                        len += count;
                        state = 0;
                    } else {
                        fprintf(stderr,"%s:ERROR: Insufficient room in char array (out).\n", __FUNCTION__);
                        out[0] = 0;
                        return 0;
                    }
                    codePoint = 0;
                 }
            } break;
            default: { 
                out[0] = 0;
                return 0;
            } break;
        } 
        in ++;
    }
    out[len] = 0; /* NULL termination of string. */
    return len;
}

size_t utf8_to_wchar(const char *in, wchar_t *out, size_t outSize) {

    unsigned int codePoint = 0;
    size_t len = 0;
    int state = 0;

    while (*in != 0) {
        unsigned char ch = *in;
        switch (state) {
            case 0: {
                if (ch >= 0xf0) {          // Start of a 4-byte sequence.
                    codePoint = ch & 0x07; // Extract low 3 bits
                    state = 3;
                } else if (ch >= 0xe0) {   // Start of a 3-byte sequence.
                    codePoint = ch & 0x0f; // Extract low 4 bits
                    state = 2;
                } else if (ch >= 0xc0) {   // Start of a 2-byte sequence.
                    codePoint = ch & 0x1f; // Extract low 5 bits
                    state = 1;
                } else if (ch >= 0x80) {   // We should never find a continuation byte in isolation.
                    fprintf(stderr,"%s:ERROR: UTF8 string (in) appears to be corrupted.\n", __FUNCTION__);
                    state = 99;
                } else {
                    codePoint = ch;        // ASCII        
                    if ((outSize-len) > 1) {
                        out[len++] = (wchar_t)codePoint;
                    } else {
                        fprintf(stderr,"%s:ERROR: Insufficient room in wchar_t array (out).\n", __FUNCTION__);
                        state = 99;
                    }
                }
            } break;
            case 1: { // Expecting one continuation byte.
                if ((ch & 0xc0) == 0x80) { // If the next char is a continuation byte ..
                    codePoint = (codePoint << 6) | (ch & 0x3f); // Extract lower 6 bits 
                    state = 0;

                    if (sizeof(wchar_t) == 4) { // wchar_t is UTF-32
                        int32_t temp;
                        if ( ucodepoint_to_utf32(codePoint, &temp) > 0) {
                            if ((outSize-len) > 1) {
                                out[len++] = (wchar_t)temp;
                            } else {
                                fprintf(stderr,"%s:ERROR: Insufficient room in wchar_t array (out).\n", __FUNCTION__);
                                state = 99;
                            }
                        } else {
                            state = 99;
                        }

                    } else if (sizeof(wchar_t) == 2) { // wchar_t is UTF-16
                        int16_t temp[2];
                        size_t count;
                        if (( count = ucodepoint_to_utf16(codePoint, &temp)) > 0) {
                            if (count < (outSize-len)) {
                                memcpy( &out[len], temp, sizeof(int16_t) * count );
                                len += count;
                            } else {
                                fprintf(stderr,"%s:ERROR: Insufficient room in wchar_t array (out).\n", __FUNCTION__);
                                state = 99;
                            }
                        }

                    } else {
                        fprintf(stderr,"%s:ERROR: Unsupported wchar_t size.\n", __FUNCTION__);
                        state = 99;
                    }

                } else {
                    fprintf(stderr,"%s:ERROR: UTF8 string appears to be corrupted.\n", __FUNCTION__);
                    state = 99;
                }
            } break;
            case 2: { /* Expecting two continuation bytes. */
                if ((ch & 0xc0) == 0x80) {
                    codePoint = (codePoint << 6) | (ch & 0x3f);
                    state = 1;
                } else {
                    fprintf(stderr,"%s:ERROR: UTF8 string appears to be corrupted.\n", __FUNCTION__);
                    state = 99;
                }
            } break;
            case 3: { /* Expecting three continuation bytes. */
                if ((ch & 0xc0) == 0x80) {
                    codePoint = (codePoint << 6) | (ch & 0x3f);
                    state = 2;
                } else {
                    fprintf(stderr,"%s:ERROR: UTF8 string appears to be corrupted.\n", __FUNCTION__);
                    state = 99;
                }
            } break;
            default: { /* Error State. */
                out[0] = 0;
                return 0;
            } break;
        }
        in ++;
    }

    /* If we finished in state 0, then we're good. Just
       terminate the string, otherwise we had an error. */
    if (state == 0) {
        out[len] = 0;
        return len;
    } else {
        out[0] = 0;
        return 0;
    }
    return len;
}

size_t wchar_to_utf8(const wchar_t *in, char *out, size_t outSize ) {

    unsigned int codePoint = 0;
    size_t len = 0;

    while ( *in != 0 ) {
        if (*in >= 0xd800 && *in <= 0xdbff)         /* If High-surrogate. */
            codePoint = ((*in - 0xd800) << 10) + 0x10000;
        else {
            if (*in >= 0xdc00 && *in <= 0xdfff) {   /* If Low-surrogate. */
                codePoint |= *in - 0xdc00;
            } else if (*in <= 0x10ffff) {           /* Max Unicode Value */
                codePoint = *in;
            } else {
                fprintf(stderr,"%s:ERROR: Invalid Unicode value.\n", __FUNCTION__);
                out[0] = 0;
                return 0;
            }

            char temp[4];
            size_t count = ucodepoint_to_utf8(codePoint, &temp);
            if (count < (outSize-len)) {
                memcpy( &out[len], temp, sizeof(char) * count );
                len += count;
            } else {
                fprintf(stderr,"%s:ERROR: Insufficient room in char array (out).\n", __FUNCTION__);
                out[0] = 0;
                return 0;
            }
            codePoint = 0;
        }
        in++;
    }
    out[len] = L'\0'; /* NULL termination of string. */
    return len;
}
