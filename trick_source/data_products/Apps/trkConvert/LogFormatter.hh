#ifndef LOG_FORMATTER
#define LOG_FORMATTER
#include <stdio.h>
#include <stdint.h>

class LogFormatter {

    public:
    void writeHeader(FILE* out_fp __attribute__((unused)),
                     int version __attribute__((unused)),
                     int endianness __attribute__((unused))) {};

    void writeColumnLabel(FILE* out_fp __attribute__((unused)),
                                const char* name __attribute__((unused)),
                                const char* units __attribute__((unused))) {};

    void writeColumnLabelSeparator(FILE* out_fp __attribute__((unused))) {};
    void writeDatumSeparator( FILE* out_fp __attribute__((unused))) {};
    void writeRecordSeparator(FILE* out_fp __attribute__((unused))) {};
    void writeDatum(FILE* out_fp __attribute__((unused)), int8_t   datum __attribute__((unused))) {};
    void writeDatum(FILE* out_fp __attribute__((unused)), uint8_t  datum __attribute__((unused))) {};
    void writeDatum(FILE* out_fp __attribute__((unused)), int16_t  datum __attribute__((unused))) {};
    void writeDatum(FILE* out_fp __attribute__((unused)), uint16_t datum __attribute__((unused))) {};
    void writeDatum(FILE* out_fp __attribute__((unused)), int32_t  datum __attribute__((unused))) {};
    void writeDatum(FILE* out_fp __attribute__((unused)), uint32_t datum __attribute__((unused))) {};
    void writeDatum(FILE* out_fp __attribute__((unused)), int64_t  datum __attribute__((unused))) {};
    void writeDatum(FILE* out_fp __attribute__((unused)), uint64_t datum __attribute__((unused))) {};
    void writeDatum(FILE* out_fp __attribute__((unused)), float    datum __attribute__((unused))) {};
    void writeDatum(FILE* out_fp __attribute__((unused)), double   datum __attribute__((unused))) {};
    virtual const char* extension() = 0;
};
#endif
