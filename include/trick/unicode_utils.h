#ifndef UNITCODE_UTILS_H
#define UNITCODE_UTILS_H
#include <stddef.h>

/* Author: John M. Penn */

#ifdef __cplusplus
extern "C" {
#endif

/* Convert Unicode codepoint to UTF-32. Validates that it's a legal unicode value.
   Returns 1, if successful, 0 otherwise.
 */
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

/* Escape ('\' escape codes) all unicode and non-printable ASCII characters
   in a UTF-8 character string to an all-ASCII representation.
   Returns the number of elements in the character string, or 0 on failure.
 */
size_t escape_to_ascii(const char *in, char *out, size_t outSize); 

/* Return the length of the array that would be produced if it were converted,
   or 0 on failure.
 */
size_t escape_to_ascii_len(const char *in);

/* Un-escape C-language escape sequences, including \u and \U Unicode escape sequences,
   in an ASCII character array, producing a UTF-8 character array.
   Returns the number of elements in the character string, or 0 on failure.
*/
size_t unescape_to_utf8(const char *in, char *out, size_t outSize); 

/* Return the length of the array that would be produced if it were converted,
   or 0 on failure.
 */
size_t unescape_to_utf8_len(const char *in);

/* Convert a UTF-8 character array to a wchar_t array. Supports 16, and 32 bit wchar_t.
   Returns the number of elements in the wchar_t string, or 0 on failure. */
size_t utf8_to_wchar(const char *in, wchar_t *out, size_t outSize);

/* Return the length of the array that would be produced if in were converted,
   or 0 on failure.
 */
size_t utf8_to_wchar_len(const char *in);

/* Convert wchar_t character array to UTF-8.
   Returns the number of elements in the character (utf-8) string,
   or 0 on failure.
*/
size_t wchar_to_utf8(const wchar_t *in, char *out, size_t outSize);

/* Return the length of the array that would be produced if in were converted, or 0 on failure. */
size_t wchar_to_utf8_len(const wchar_t *in);

#ifdef __cplusplus
}
#endif
#endif
