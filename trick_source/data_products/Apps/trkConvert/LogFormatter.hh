#ifndef LOG_FORMATTER
#define LOG_FORMATTER
#include <stdio.h>
#include <stdint.h>

class LogFormatter {

    public:
    virtual void writeHeader(FILE* out_fp,
                     int version,
                     int endianness) = 0;

    virtual void writeColumnLabel(FILE* out_fp,
                                const char* name,
                                const char* units) = 0;

    virtual void writeColumnLabelSeparator(FILE* out_fp) = 0;
    virtual void writeDatumSeparator( FILE* out_fp) = 0;
    virtual void writeRecordSeparator(FILE* out_fp) = 0;
    virtual void writeDatum(FILE* out_fp, int8_t   datum) = 0;
    virtual void writeDatum(FILE* out_fp, uint8_t  datum) = 0;
    virtual void writeDatum(FILE* out_fp, int16_t  datum) = 0;
    virtual void writeDatum(FILE* out_fp, uint16_t datum) = 0;
    virtual void writeDatum(FILE* out_fp, int32_t  datum) = 0;
    virtual void writeDatum(FILE* out_fp, uint32_t datum) = 0;
    virtual void writeDatum(FILE* out_fp, int64_t  datum) = 0;
    virtual void writeDatum(FILE* out_fp, uint64_t datum) = 0;
    virtual void writeDatum(FILE* out_fp, float    datum) = 0;
    virtual void writeDatum(FILE* out_fp, double   datum) = 0;
    virtual const char* extension() = 0;
};
#endif
