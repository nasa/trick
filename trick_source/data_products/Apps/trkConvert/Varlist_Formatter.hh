#ifndef VARLIST_FORMATTER
#define VARLIST_FORMATTER

#include "LogFormatter.hh"
/* ================================================================================
 * CLASS: Varlist_Formatter isa LogFormatter.
 * ================================================================================
 */
class Varlist_Formatter: public LogFormatter {
    public:
    void writeHeader(FILE* out_fp, int version, int endianness);
    void writeColumnLabel(FILE* out_fp, const char* name, const char* units);
    void writeColumnLabelSeparator(FILE* out_fp);
    void writeDatumSeparator(FILE* out_fp);
    void writeRecordSeparator(FILE* out_fp);
    void writeDatum(FILE* out_fp, int8_t datum);
    void writeDatum(FILE* out_fp, uint8_t datum);
    void writeDatum(FILE* out_fp, int16_t datum);
    void writeDatum(FILE* out_fp, uint16_t datum);
    void writeDatum(FILE* out_fp, int32_t datum);
    void writeDatum(FILE* out_fp, uint32_t datum);
    void writeDatum(FILE* out_fp, int64_t datum);
    void writeDatum(FILE* out_fp, uint64_t datum);
    void writeDatum(FILE* out_fp, float datum);
    void writeDatum(FILE* out_fp, double datum);
    const char* extension();
};
#endif
