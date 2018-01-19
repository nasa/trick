#include "Varlist_Formatter.hh"
/* ================================================================================
 * CLASS: Varlist_Formatter isa LogFormatter.
 * ================================================================================
 */

    void Varlist_Formatter::writeColumnLabel(FILE* out_fp, const char* name, const char* units) {
        fprintf(out_fp,"%s {%s}", name, units);
    }
    void Varlist_Formatter::writeColumnLabelSeparator(FILE* out_fp)  { fprintf(out_fp,"\n"); }
    const char* Varlist_Formatter::extension() { return ".varlist"; };
