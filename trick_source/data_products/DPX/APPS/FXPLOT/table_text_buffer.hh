#ifndef TRICK_FXPLOT_TABLE_TEXT_BUFFER_HH
#define TRICK_FXPLOT_TABLE_TEXT_BUFFER_HH

#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <limits>

// Append formatted table text without limiting a row to a fixed-size scratch buffer.
static char* twprint(char* text_buf, size_t *text_buf_size, size_t *insertion_pos,
                     const char* format, ...) {
    const size_t allocation_step = 0xFFFF;
    const size_t max_size = std::numeric_limits<size_t>::max();
    va_list args;
    va_list sizing_args;
    va_start(args, format);
    va_copy(sizing_args, args);
    int formatted_len = vsnprintf(NULL, 0, format, sizing_args);
    va_end(sizing_args);
    if (formatted_len < 0) {
        va_end(args);
        return text_buf;
    }

    size_t message_len = static_cast<size_t>(formatted_len);
    if (*insertion_pos > max_size - message_len - 1) {
        va_end(args);
        fprintf(stderr, "OUT_OF_MEMORY in twprint.\n");
        exit(1);
    }
    size_t required_size = *insertion_pos + message_len + 1;
    if (text_buf == NULL) {
        *text_buf_size = 0;
    }
    if (required_size > *text_buf_size) {
        size_t new_size = *text_buf_size;
        while (new_size < required_size) {
            if (new_size > max_size - allocation_step) {
                new_size = required_size;
                break;
            }
            new_size += allocation_step;
        }
        char *grown = static_cast<char *>(realloc(text_buf, new_size));
        if (grown == NULL) {
            va_end(args);
            fprintf(stderr, "OUT_OF_MEMORY in twprint.\n");
            exit(1);
        }
        // Keep unused capacity zeroed, as it is also used by the table save callback.
        memset(grown + *text_buf_size, 0, new_size - *text_buf_size);
        text_buf = grown;
        *text_buf_size = new_size;
    }

    int written = vsnprintf(text_buf + *insertion_pos, *text_buf_size - *insertion_pos, format, args);
    va_end(args);
    if (written < 0) {
        text_buf[*insertion_pos] = '\0';
    } else {
        *insertion_pos += strlen(text_buf + *insertion_pos);
    }
    return text_buf;
}

#endif
