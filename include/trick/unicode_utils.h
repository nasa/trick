#ifndef UNITCODE_UTILS_H
#define UNITCODE_UTILS_H
#include <stddef.h>

/* Maintainer: John M. Penn */

#ifdef __cplusplus
extern "C" {
#endif

/* Convert Unicode codepoint to UTF-32. Validates that it's a legal unicode value.
   Returns 1, if successful, 0 otherwise. */
size_t ucodepoint_to_utf32(unsigned int codePoint, int32_t *out);

/* Convert Unicode codepoint to UTF-16.
   Returns the number of UTF-16 elements (1..2) necessary to represent the codepoint,
   or 0 on failure.
 */ 
size_t ucodepoint_to_utf16(unsigned int codePoint, int16_t (*out)[2]);

/* Convert Unicode codepoint to UTF-8.
   Returns the number of UTF_8 elements (1..4) )necessary to represent the codepoint,
   or 0 on failure.
 */ 
size_t ucodepoint_to_utf8(unsigned int codePoint, char (*out)[4]);



/* Un-escape C escape sequences, including \u and \U Unicode escape sequences,
   in an ASCII character array, producing a UTF-8 character array. Return the
   number of elements in the character string.
*/
size_t ascii_to_utf8(const char *in, char *out, size_t outSize); 

/* Escape ('\' escape codes) all unicode and non-printable ASCII characters
   in a UTF-8 character string. Return the number of elements in the character string.
*/
size_t utf8_to_printable_ascii(const char *in, char *out, size_t outSize); 

/* Convert a UTF-8 character array to a wchar_t array. Supports 16, and 32 bit wchar_t.
   Return the number of elements in the wchar_t string. */
size_t utf8_to_wchar(const char *in, wchar_t *out, size_t outSize);

/* Convert wchar_t character array to UTF-8. Return the number of elements in
   the character (utf-8) string.*/
size_t wchar_to_utf8(const wchar_t *in, char *out, size_t outSize);

#ifdef __cplusplus
}
#endif
#endif
