#include <string.h>
#include <stdio.h>
#include <stddef.h>
#include <wchar.h>
#include <ctype.h>
#include <stdint.h>
#include "trick/unicode_utils.h"

/* Author: John M. Penn */

#define ERROR_STATE 99

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

size_t escape_to_ascii(const char *in, char *out, size_t outSize) {
    unsigned int codePoint;
    size_t out_len = 0;
    int state = 0;
    char ascii_elements[11];

    if (in == NULL) {
        fprintf(stderr,"%s:ERROR: UTF8 char-pointer (in) is NULL. No conversion performed.\n", __FUNCTION__);
        return 0;
    }

    if (out != NULL) out[out_len] = 0;

    while ((*in != 0) && (state != ERROR_STATE)) {
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
                    state = ERROR_STATE;
                } else {                   // ASCII        
                    if (ch == '\a') {
                        sprintf(ascii_elements, "\\a");
                    } else if (ch == '\b') {
                        sprintf(ascii_elements, "\\b");
                    } else if (ch == '\f') {
                        sprintf(ascii_elements, "\\f");
                    } else if (ch == '\n') {
                        sprintf(ascii_elements, "\\n");
                    } else if (ch == '\r') {
                        sprintf(ascii_elements, "\\r");
                    } else if (ch == '\t') {
                        sprintf(ascii_elements, "\\t");
                    } else if (ch == '\v') {
                        sprintf(ascii_elements, "\\v");
                    } else if (isprint(ch)) {
                        sprintf(ascii_elements, "%c",ch);
                    } else {
                        sprintf(ascii_elements, "\\x%02x",ch);
                    }
                    size_t n_elements = strlen(ascii_elements);
                    if (out != NULL) {
                        if ((out_len + n_elements) < outSize) {
                            strcat(out, ascii_elements);
                        } else {
                            fprintf(stderr,"%s:ERROR: Insufficient room in (out) array.\n", __FUNCTION__);
                            state = ERROR_STATE;
                        }
                    }
                    out_len += n_elements;
                }
            } break;
            case 1: { // Expecting one continuation byte.
                if ((ch & 0xc0) == 0x80) { // If the next char is a continuation byte ..
                    codePoint = (codePoint << 6) | (ch & 0x3f); // Extract low 6 bits
                    state = 0;
                    if (codePoint <= 0xffff) {
                        sprintf(ascii_elements, "\\u%04x", codePoint); 
                    } else {
                        sprintf(ascii_elements, "\\U%08x", codePoint); 
                    }
                    size_t n_elements = strlen(ascii_elements);
                    if (out != NULL) {
                        if ((out_len + n_elements) < outSize) {
                            strcat(out, ascii_elements);
                        } else {
                            fprintf(stderr,"%s:ERROR: Insufficient room in (out) array.\n", __FUNCTION__);
                            state = ERROR_STATE;
                        }
                    }
                    out_len += n_elements;
                } else {
                    fprintf(stderr,"%s:ERROR: UTF8 string appears to be corrupted.\n", __FUNCTION__);
                    state = ERROR_STATE;
                }
            } break;
            case 2: { /* Expecting two continuation bytes. */
                if ((ch & 0xc0) == 0x80) {
                    codePoint = (codePoint << 6) | (ch & 0x3f);
                    state = 1;
                } else {
                    fprintf(stderr,"%s:ERROR: UTF8 string appears to be corrupted.\n", __FUNCTION__);
                    state = ERROR_STATE;
                }
            } break;
            case 3: { /* Expecting three continuation bytes. */
                if ((ch & 0xc0) == 0x80) {
                    codePoint = (codePoint << 6) | (ch & 0x3f);
                    state = 2;
                } else {
                    fprintf(stderr,"%s:ERROR: UTF8 string appears to be corrupted.\n", __FUNCTION__);
                    state = ERROR_STATE;
                }
            } break;
            default: { 
                state = ERROR_STATE;
            } break;
        }
        in ++;
    }
    /* If we didn't finished in state 0, then we had an error. */
    if (state != 0) {
        out_len = 0;
    } 
    if (out != NULL) out[out_len] = 0; /* NULL termination of string. */
    return out_len;
}

size_t escape_to_ascii_len(const char *in) {
    return escape_to_ascii( in, NULL, (size_t)0);
}

/* Un-escapes ASCII and Unicode escape sequences, and encodes them into UTF-8. */
size_t unescape_to_utf8(const char *in, char *out, size_t outSize) {

    unsigned int codePoint = 0;
    size_t out_len = 0;
    int state = 0;
    int digitsExpected = 0;

    if (in == NULL) {
        fprintf(stderr,"%s:ERROR: char-pointer (in) is NULL. No conversion performed.\n", __FUNCTION__);
        return 0;
    }

    if (out != NULL) out[out_len] = 0;

    while ((*in != 0) && (state != ERROR_STATE )) {
        unsigned char ch = *in;
        switch(state) {
            case 0: { // Normal State
                if (ch >= 0xf0) {        // Start of a 4-byte UTF-8 sequence.
                    if (out != NULL) { out[out_len] = ch; } out_len++; state = 3;
                } else if (ch >= 0xe0) { // Start of a 3-byte UTF-8 sequence.
                    if (out != NULL) { out[out_len] = ch; } out_len++; state = 4;
                } else if (ch >= 0xc0) { // Start of a 2-byte UTF-8 sequence.
                    if (out != NULL) { out[out_len] = ch; } out_len++; state = 5;
                } else if (ch >= 0x80) { // We should never find a UTF-8 continuation byte in isolation.
                    fprintf(stderr,"%s:ERROR: Input string (in) appears to be corrupted.\n", __FUNCTION__);
                    state = ERROR_STATE;
                } else if (ch =='\\') {
                    state = 1;
                } else {
                    if (out != NULL) out[out_len] = ch;
                    out_len++;
                }
            } break;
            case 1: { // Escaped State ( that is: we've found a '\' character.)
                switch(ch) {
                    case '\'':
                    case '\"':
                    case '\?':
                    case '\\': { if (out != NULL) { out[out_len] = ch; } out_len++; state = 0; } break;
                    case 'a': { if (out != NULL) { out[out_len] = '\a'; } out_len++; state = 0; } break;
                    case 'b': { if (out != NULL) { out[out_len] = '\b'; } out_len++; state = 0; } break;
                    case 'f': { if (out != NULL) { out[out_len] = '\f'; } out_len++; state = 0; } break;
                    case 'n': { if (out != NULL) { out[out_len] = '\n'; } out_len++; state = 0; } break;
                    case 'r': { if (out != NULL) { out[out_len] = '\r'; } out_len++; state = 0; } break;
                    case 't': { if (out != NULL) { out[out_len] = '\t'; } out_len++; state = 0; } break;
                    case 'v': { if (out != NULL) { out[out_len] = '\b'; } out_len++; state = 0; } break;
                    case 'x': { digitsExpected = 2; state = 2; } break;
                    case 'u': { digitsExpected = 4; state = 2; } break;
                    case 'U': { digitsExpected = 8; state = 2; } break;
                    default : {
                        state = ERROR_STATE;
                    }
                } // switch ch
            } break;
            case 2: { // Escaped Unicode ( that is: we've found '\x', '\u' or '\U'.)
                 int digit = -1;
                 if (ch >= '0' && ch <= '9') {
                     digit = ch - (int)'0';
                 } else if (ch >= 'A' && ch <= 'F') {
                     digit = ch - (int)'A' + 10;
                 } else if (ch >= 'a' && ch <= 'f') {
                     digit = ch - (int)'a' + 10;
                 } else {
                     fprintf(stderr,"%s:ERROR: Insufficient hexidecimal digits following"
                                    " \\x, \\u, or \\U escape code in char string (in).\n", __FUNCTION__);
                     state = ERROR_STATE;
                 }
                 if (digit >= 0) { 
                     codePoint = codePoint * 16 + digit;
                     digitsExpected -- ;
                     if ( digitsExpected == 0 ) {
                        char utf8_bytes[4];
                        size_t n_elements = ucodepoint_to_utf8(codePoint, &utf8_bytes);
                        state = 0;
                        if (out != NULL) { 
                            if (out_len + n_elements < outSize) {
                                memcpy( &out[out_len], utf8_bytes, sizeof(char) * n_elements );
                            } else {
                                fprintf(stderr,"%s:ERROR: Insufficient room in char array (out).\n", __FUNCTION__);
                                state = ERROR_STATE;
                            }
                        }
                        out_len += n_elements;
                        codePoint = 0;
                     }
                 }
            } break;

            case 3: { /* Expecting 3 UTF-8 continuation bytes. */
                if ((ch & 0xc0) == 0x80) {
                    if (out != NULL) { out[out_len] = ch; } out_len++; state = 4;
                } else {
                    fprintf(stderr,"%s:ERROR: Input (in) appears to be corrupted.\n", __FUNCTION__);
                    state = ERROR_STATE;
                }
            } break;

            case 4: { /* Expecting 2 UTF-8 continuation bytes. */
                if ((ch & 0xc0) == 0x80) {
                    if (out != NULL) { out[out_len] = ch; } out_len++; state = 5;
                } else {
                    fprintf(stderr,"%s:ERROR: Input (in) appears to be corrupted.\n", __FUNCTION__);
                    state = ERROR_STATE;
                }
            } break;

            case 5: { /* Expecting 1 UTF-8 continuation byte. */
                if ((ch & 0xc0) == 0x80) {
                    if (out != NULL) { out[out_len] = ch; } out_len++; state = 0;
                } else {
                    fprintf(stderr,"%s:ERROR: Input (in) appears to be corrupted.\n", __FUNCTION__);
                    state = ERROR_STATE;
                }
            } break;

            default: { 
                state = ERROR_STATE;
            } break;
        } 
        in ++;
    }
    if (state != 0) { /* If we didn't finished in state 0, then we had an error. */
        out_len = 0;
    } 
    if (out != NULL) out[out_len] = 0; /* NULL termination of string. */
    return out_len;
}

size_t unescape_to_utf8_len(const char *in) {
    return unescape_to_utf8( in, NULL, (size_t)0); 
}

size_t utf8_to_wchar(const char *in, wchar_t *out, size_t outSize) {

    unsigned int codePoint = 0;
    size_t out_len = 0;
    int state = 0;

    if (in == NULL) {
        fprintf(stderr,"%s:ERROR: UTF8 char-pointer (in) is NULL. No conversion performed.\n", __FUNCTION__);
        return 0;
    }

    if (out != NULL) out[out_len] = 0;

    while ((*in != 0) && (state != ERROR_STATE)) {
        unsigned char ch = *in;
        switch (state) {
            case 0: {
                if (ch >= 0xf0) {          // Start of a 4-byte UTF-8 sequence.
                    codePoint = ch & 0x07; // Extract low 3 bits
                    state = 3;
                } else if (ch >= 0xe0) {   // Start of a 3-byte UTF-8 sequence.
                    codePoint = ch & 0x0f; // Extract low 4 bits
                    state = 2;
                } else if (ch >= 0xc0) {   // Start of a 2-byte UTF-8 sequence.
                    codePoint = ch & 0x1f; // Extract low 5 bits
                    state = 1;
                } else if (ch >= 0x80) {   // We should never find a UTF-8 continuation byte in isolation.
                    fprintf(stderr,"%s:ERROR: UTF8 string (in) appears to be corrupted.\n", __FUNCTION__);
                    state = ERROR_STATE;
                } else {
                    codePoint = ch;        // ASCII        
                    if (out != NULL) { 
                        if ((out_len + 1) < outSize) {
                            out[out_len] = (wchar_t)codePoint;
                        } else {
                            fprintf(stderr,"%s:ERROR: Insufficient room in wchar_t array (out).\n", __FUNCTION__);
                            state = ERROR_STATE;
                        }
                    }
                    out_len++;
                }
            } break;
            case 1: { /* Expecting one continuation byte. */
                if ((ch & 0xc0) == 0x80) {
                    codePoint = (codePoint << 6) | (ch & 0x3f); // Extract lower 6 bits 
                    state = 0;

                    if (sizeof(wchar_t) == 4) { // wchar_t is UTF-32
                        int32_t utf32_element;
                        if ( ucodepoint_to_utf32(codePoint, &utf32_element) > 0) {
                            if (out != NULL) {
                                if ((out_len + 1) < outSize) {
                                    out[out_len] = (wchar_t)utf32_element;
                                } else {
                                    fprintf(stderr,"%s:ERROR: Insufficient room in wchar_t array (out).\n", __FUNCTION__);
                                    state = ERROR_STATE;
                                }
                            }
                            out_len++;
                        } else {
                            /* ucodepoint_to_utf32() will have, in this case produced an error message. */  
                            state = ERROR_STATE;
                        }
                    } else if (sizeof(wchar_t) == 2) { // wchar_t is UTF-16
                        int16_t utf16_elements[2];
                        size_t n_elements;
                        if (( n_elements = ucodepoint_to_utf16(codePoint, &utf16_elements)) > 0) {
                            if (out != NULL) {
                                if ((out_len + n_elements) < outSize) {
                                    memcpy( &out[out_len], utf16_elements, sizeof(int16_t) * n_elements);
                                } else {
                                    fprintf(stderr,"%s:ERROR: Insufficient room in wchar_t array (out).\n", __FUNCTION__);
                                    state = ERROR_STATE;
                                }
                            }
                            out_len += n_elements;
                        } else {
                            /* ucodepoint_to_utf16() will have, in this case produced an error message. */  
                            state = ERROR_STATE;
                        }
                    } else {
                        fprintf(stderr,"%s:ERROR: Unsupported wchar_t size.\n", __FUNCTION__);
                        state = ERROR_STATE;
                    }

                } else {
                    fprintf(stderr,"%s:ERROR: UTF8 string appears to be corrupted.\n", __FUNCTION__);
                    state = ERROR_STATE;
                }
            } break;
            case 2: { /* Expecting two continuation bytes. */
                if ((ch & 0xc0) == 0x80) {
                    codePoint = (codePoint << 6) | (ch & 0x3f);
                    state = 1;
                } else {
                    fprintf(stderr,"%s:ERROR: UTF8 string appears to be corrupted.\n", __FUNCTION__);
                    state = ERROR_STATE;
                }
            } break;
            case 3: { /* Expecting three continuation bytes. */
                if ((ch & 0xc0) == 0x80) {
                    codePoint = (codePoint << 6) | (ch & 0x3f);
                    state = 2;
                } else {
                    fprintf(stderr,"%s:ERROR: UTF8 string appears to be corrupted.\n", __FUNCTION__);
                    state = ERROR_STATE;
                }
            } break;
            default: {
                state = ERROR_STATE;
            } break;
        }
        in ++;
    }
    if (state != 0) { /* If we didn't finish in state 0, it's an error. */
        out_len = 0;
    } 
    if (out != NULL) out[out_len] = 0; /* NULL termination of string. */
    return out_len;
}

size_t utf8_to_wchar_len(const char *in) {
    return utf8_to_wchar( in, NULL, (size_t)0);
}

size_t wchar_to_utf8(const wchar_t *in, char *out, size_t outSize ) {

    unsigned int codePoint = 0;
    size_t out_len = 0;
    int state = 0;

    if (in == NULL) {
        fprintf(stderr,"%s:ERROR: wchar_t-pointer (in) is NULL. No conversion performed.\n", __FUNCTION__);
        return 0;
    }

    while ((*in != 0) && (state != ERROR_STATE)) {
        if (*in >= 0xd800 && *in <= 0xdbff)         /* If High-surrogate. */
            codePoint = ((*in - 0xd800) << 10) + 0x10000;
        else {
            if (*in >= 0xdc00 && *in <= 0xdfff) {   /* If Low-surrogate. */
                codePoint |= *in - 0xdc00;
            } else if (*in <= 0x10ffff) {           /* Max Unicode Value */
                codePoint = *in;
            } else {
                fprintf(stderr,"%s:ERROR: Invalid Unicode value.\n", __FUNCTION__);
                state = ERROR_STATE;
            }

            if (state != ERROR_STATE) {
                char utf8_elements[4];
                size_t n_elements = ucodepoint_to_utf8(codePoint, &utf8_elements);
                if (out != NULL) {
                    if ((out_len + n_elements) < outSize) {
                        memcpy( &out[out_len], utf8_elements, sizeof(char) * n_elements );
                    } else {
                        fprintf(stderr,"%s:ERROR: Insufficient room in char array (out).\n", __FUNCTION__);
                        state = ERROR_STATE;
                    }
                }
                out_len += n_elements;
                codePoint = 0;
            }
        }
        in++;
    }
    if (state != 0) { /* If we didn't finish in state 0, it's an error. */
        out_len = 0;
    } 
    if (out != NULL) out[out_len] = 0; /* NULL termination of string. */
    return out_len;
}

size_t wchar_to_utf8_len(const wchar_t *in) {
    return wchar_to_utf8( in, NULL, (size_t)0);
}
