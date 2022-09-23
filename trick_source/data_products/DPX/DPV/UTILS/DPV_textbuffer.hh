
#ifndef DPV_TEXTBUF_HH
#define DPV_TEXTBUF_HH

#include <string.h>

class DPV_TextBuffer {

public:
    /**
     * DPV_TextBuffer constructor.
     */
    DPV_TextBuffer();

    /**
     * DPV_TextBuffer destructor.
     */
    ~DPV_TextBuffer();

    void print(const char* format, ...);
    int subst( const char* search_s, const char* replace_s);
    void subst_g( const char* search_s, const char* replace_s);

    const char* getText();
    int readFile( const char* fileName );   /**< Read file into buffer */
    int writeFile( const char* filename );  /**< Write buffer to file */

private:
    char * text_buf;       /**< This is a pointer to the text buffer.
                                It is allocated and realloced as
                                needed by print(). */
    size_t insertion_pos;  /**< This is an index into text_buf where
                                more text should be added by print(). */
    size_t buf_size;       /**< The size (in bytes) of the buffer */

};
#endif
