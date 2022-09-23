#ifndef TRK_DATA_LOG_HH
#define TRK_DATA_LOG_HH

#include <stdio.h> // FILE
#include <stdint.h> // Requires C99

#include <string>
#include <vector>

#include "LogFormatter.hh"
#include "ParamDescription.hh"

class TRK_DataLog {

  public:
    static const int LittleEndian;
    static const int BigEndian;

    std::vector<ParamDescription*> paramDescriptions;
    std::vector<int> paramOffsets;
    std::vector<bool> paramSelected;

    // Constructors
    TRK_DataLog(){}
    TRK_DataLog(std::string fileName);

    std::string getFileName() const;
    int  parameterCount() const;
    const char* parameterName(unsigned int n) const;
    const char* parameterUnits(unsigned int n) const;
    const char* parameterType(unsigned int n) const;

    void selectAllParameters();
    void selectParameter(unsigned int index);
    void selectParameter(const char * paramName); 
    void deselectParameter(unsigned int index);
    void formattedWrite(FILE* out_fp, LogFormatter* formatter);

  private:
    std::string fileName;
    FILE* in_fp;
    int version;
    int endianness;
    uint32_t N_params;
    fpos_t dataPosition;
    int dataRecordSize;
    char* dataRecord;
};
#endif
