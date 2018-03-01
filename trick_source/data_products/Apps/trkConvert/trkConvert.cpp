#include <stdio.h>
#include <unistd.h>
#include <stdint.h> // Requires C99
#include <vector>
#include <iostream>
#include <exception>
#include <string>
#include <stdlib.h>
#include <cstring>

#include "LogFormatter.hh"
#include "CSV_Formatter.hh"
#include "Varlist_Formatter.hh"

typedef enum {
    TRICK_VOID               =   0, /* No type */
    TRICK_CHARACTER          =   1, /* (char) */
    TRICK_UNSIGNED_CHARACTER =   2, /* (unsigned char) */
    TRICK_STRING             =   3, /* (char *) */
    TRICK_SHORT              =   4, /* (short) */
    TRICK_UNSIGNED_SHORT     =   5, /* (unsigned short) */
    TRICK_INTEGER            =   6, /* (int) */
    TRICK_UNSIGNED_INTEGER   =   7, /* (unsigned int) */
    TRICK_LONG               =   8, /* (long) */
    TRICK_UNSIGNED_LONG      =   9, /* (unsigned long) */
    TRICK_FLOAT              =  10, /* (float) */
    TRICK_DOUBLE             =  11, /* (double) */
    TRICK_BITFIELD           =  12, /* (signed int : 1) */
    TRICK_UNSIGNED_BITFIELD  =  13, /* (unsigned int : 1) */
    TRICK_LONG_LONG          =  14, /* (long long) */
    TRICK_UNSIGNED_LONG_LONG =  15, /* (long long) */
    TRICK_FILE_PTR           =  16, /* (file *) */
    TRICK_BOOLEAN            =  17, /* (C++ boolean) */
    TRICK_WCHAR              =  18, /* (wchar_t) */
    TRICK_WSTRING            =  19, /* (wchar_t *) */
    TRICK_VOID_PTR           =  20, /* an arbitrary address */
    TRICK_ENUMERATED         =  21, /* User defined type (enumeration) */
    TRICK_STRUCTURED         =  22, /* User defined type (struct/class) */
    TRICK_OPAQUE_TYPE        =  23, /* User defined type (where type details are as yet unknown) */
    TRICK_STL                =  24, /* Standard template library type */
    TRICK_NUMBER_OF_TYPES
} TRICK_TYPE ;

class ReadException: public std::exception {
  virtual const char* what() const throw() {
    return "fread() failed.";
  }
} readException;

/* ================================================================================
 * CLASS: ParamDescription
 * ================================================================================
 */
class ParamDescription {
    public:
    char* parameterName;
    char* unitsName;
    int32_t dataType;
    int32_t dataSize;

    ParamDescription(){}
    ParamDescription(FILE* fp);
};

ParamDescription::ParamDescription(FILE* in_fp){

    int32_t nameStringLength;
    if ( fread( &nameStringLength, 1, 4, in_fp) != 4) throw readException;

    parameterName = new char[nameStringLength+1];
    if ( fread( parameterName, 1, nameStringLength, in_fp) != (size_t)nameStringLength) throw readException;
    parameterName[nameStringLength] = 0;

    int32_t unitsStringLength;
    if ( fread( &unitsStringLength, 1, 4 , in_fp) != 4) throw readException;

    unitsName = new char[unitsStringLength+1];
    if ( fread( unitsName, 1, unitsStringLength, in_fp) != (size_t)unitsStringLength) throw readException;
    unitsName[unitsStringLength] = 0;

    if ( fread( &dataType, 1, 4, in_fp) != 4) throw readException;
    if ( fread( &dataSize, 1, 4, in_fp) != 4) throw readException;
}

/* ================================================================================
 * CLASS: DataLog
 * ================================================================================
 */
class DataLog {

    public:
    std::string fileName;
    FILE* in_fp;
    int version;
    int endianness;
    uint32_t N_params;
    fpos_t dataPosition;
    int dataRecordSize;
    char* dataRecord;

    static const int LittleEndian;
    static const int BigEndian;

    std::vector<ParamDescription*> paramDescriptions;
    std::vector<int> paramOffsets;
    std::vector<bool> paramSelected;

    DataLog(){}
    DataLog(std::string fileName);
    void selectAllParameters();
    void selectParameter(unsigned int index);
    void selectParameter(const char * paramName); 
    void deselectParameter(unsigned int index);
    void formattedWrite(FILE* out_fp, LogFormatter* formatter);
};

const int DataLog::LittleEndian = 1;
const int DataLog::BigEndian    = 2;

DataLog::DataLog(std::string file_name) {

    fileName = file_name;
    in_fp = fopen(fileName.c_str(), "rb");

    if (in_fp != NULL) {

        char trick_header_string[11];
        if (fread( trick_header_string, 1, 10, in_fp) != 10) throw readException;
        trick_header_string[10] = 0;

        if (!strncmp( trick_header_string, "Trick-", 6)) {
             
            char version_txt[3];   
            memcpy(version_txt, &trick_header_string[6], 2) ;
            version_txt[2] = 0;
            version = atoi(version_txt);

            char endian_txt[2];   
            memcpy(endian_txt, &trick_header_string[9], 1) ;
            endian_txt[1] = 0;

            if (!strncmp( endian_txt, "L", 1)) {
                endianness = LittleEndian;
            } else if (!strncmp( endian_txt, "B", 1)) {
                endianness = BigEndian;
            } else {
                fprintf (stderr, "Trick header error. Endianness should be \"L\" or \"B\".");
            }

            if (fread( &N_params, 1, 4, in_fp) != 4) throw readException;

            dataRecordSize = 0;
            for (int ii = 0 ; ii < (int)N_params ; ii++ ) {
                ParamDescription* paramDescription = new ParamDescription(in_fp);
                paramDescriptions.push_back(paramDescription);
                paramOffsets.push_back(dataRecordSize);
                paramSelected.push_back(false);

                dataRecordSize += paramDescription->dataSize;
            }
            dataRecord = new char[dataRecordSize];

            // Time should always be selected.
            paramSelected[0] = true;

            if (fgetpos(in_fp, &dataPosition) != 0) {
                fprintf (stderr, "fgetpos failure.\n");
            }
        } else {
            std::cerr << "File \"" << fileName << "\" isn't a Trick binary log file." << std::endl;
        }
    } else {
        std::cerr << "File \"" << fileName << "\" failed to open." << std::endl;
    }
}

void DataLog::selectAllParameters() {
    for (int ii = 1 ; ii < (int)N_params ; ii++ ) {
        paramSelected[ii] = true;
    }
}

void DataLog::selectParameter(unsigned int index) {
    if ((index > 0) && (index < N_params)) {
        paramSelected[index] = true;
    }
}

void DataLog::selectParameter(const char * paramName) {
    bool found = false;
    int ii = 1;
    while ((ii < (int)N_params) && (found == false)) {
        if ( !strcmp( paramName, paramDescriptions[ii]->parameterName )) {
            paramSelected[ii] = true;
            found = true;
        }
        ii ++;
    }
}

void DataLog::deselectParameter(unsigned int index) {
    if ((index > 0) && (index < N_params)) {
        paramSelected[index] = false;
    }
}

void DataLog::formattedWrite(FILE* out_fp, LogFormatter* formatter) {

    formatter->writeHeader(out_fp, version, endianness);
    formatter->writeColumnLabel(out_fp, paramDescriptions[0]->parameterName, paramDescriptions[0]->unitsName); 
    for (int ii = 1; ii < (int)N_params ; ii++) {
        if (paramSelected[ii])
            formatter->writeColumnLabelSeparator(out_fp);
            formatter->writeColumnLabel(out_fp, paramDescriptions[ii]->parameterName, paramDescriptions[ii]->unitsName); 
    }

    if ( fsetpos(in_fp, &dataPosition) != 0 ) {
       fprintf(stderr,"fsetpos() error.\n");
    }
    while ( fread( dataRecord, 1, dataRecordSize, in_fp) == (size_t)dataRecordSize) {
        formatter->writeRecordSeparator(out_fp);
        formatter->writeDatum(out_fp, *(double*)&dataRecord[0]); // Time is always selected.
        for (int ii = 1 ; ii < (int)N_params ; ii++ ) {
            if (paramSelected[ii]) {
                formatter->writeDatumSeparator(out_fp);
                switch (paramDescriptions[ii]->dataType) {
                    case TRICK_CHARACTER: { /*1*/
                        formatter->writeDatum(out_fp, *(int8_t*)&dataRecord[paramOffsets[ii]]);
                    } break;
                    case TRICK_UNSIGNED_CHARACTER: { /*2*/
                        formatter->writeDatum(out_fp, *(uint8_t*)&dataRecord[paramOffsets[ii]]) ;
                    } break;
                    case TRICK_SHORT: { /*4*/
                        formatter->writeDatum(out_fp, *(int16_t*)&dataRecord[paramOffsets[ii]]);
                    } break;
                    case TRICK_UNSIGNED_SHORT: { /*5*/
                        formatter->writeDatum(out_fp, *(uint16_t*)&dataRecord[paramOffsets[ii]]);
                    } break;
                    case TRICK_INTEGER: {/*6*/
                        formatter->writeDatum(out_fp, *(int32_t*)&dataRecord[paramOffsets[ii]]);
                    } break;
                    case TRICK_UNSIGNED_INTEGER: {/*7*/
                        formatter->writeDatum(out_fp, *(uint32_t*)&dataRecord[paramOffsets[ii]]);
                    } break;
                    case TRICK_LONG: { /*8*/
                        formatter->writeDatum(out_fp, *(int64_t*)&dataRecord[paramOffsets[ii]]);
                    } break;
                    case TRICK_UNSIGNED_LONG: { /*9*/
                        formatter->writeDatum(out_fp, *(uint64_t*)&dataRecord[paramOffsets[ii]]);
                    } break;
                    case TRICK_FLOAT: {/*10*/
                        formatter->writeDatum(out_fp, *(float*)&dataRecord[paramOffsets[ii]]);
                    } break;
                    case TRICK_DOUBLE: {/*11*/
                        formatter->writeDatum(out_fp, *(double*)&dataRecord[paramOffsets[ii]]);
                    } break;
                    case TRICK_LONG_LONG: { /*14*/
                        formatter->writeDatum(out_fp, *(int64_t*)&dataRecord[paramOffsets[ii]]);
                    } break;
                    case TRICK_UNSIGNED_LONG_LONG: { /*15*/
                        formatter->writeDatum(out_fp, *(uint64_t*)&dataRecord[paramOffsets[ii]]);
                    } break;
                    case TRICK_BOOLEAN: { /*17*/
                        formatter->writeDatum(out_fp, *(uint8_t*)&dataRecord[paramOffsets[ii]]);
                    } break;
                    default: {
                        fprintf(stdout, "Unknown Data Type [%d] of size [%d] bytes.",
                            paramDescriptions[ii]->dataType,
                            paramDescriptions[ii]->dataSize);
                        fflush(stdout);
                        exit(1);
                    }
                }
            }
        }
    }
    formatter->writeRecordSeparator(out_fp);
}

static const char *usage_doc[] = {
"----------------------------------------------------------------------------",
" trkConvert -                                                               ",
"                                                                            ",
" USAGE:  trkConvert -help                                                   ",
"         trkConvert [-csv|-varlist] [-o <outfile>] <trk_file_name>          ",
" Options:                                                                   ",
"     -help                Print this message and exit.                      ",
"     -csv  (the default)  Generates a comma-separated value (CSV) file from ",
"                          a Trick binary data file. CSV files are a common  ",
"                          means of sharing data between applications.       ",
"     -varlist             Generates a list of the names of the variables    ",
"                          the are recorded in the  Trick binary data file.  ",
"----------------------------------------------------------------------------"};
#define N_USAGE_LINES (sizeof(usage_doc)/sizeof(usage_doc[0]))

void print_doc(char *doc[], int nlines) {
    int i;
    for (i=0; i < nlines; i++) {
        std::cerr << doc[i] << '\n';
    }
    std::cerr.flush();
}

void usage() {
    print_doc((char **)usage_doc,N_USAGE_LINES);
}

int main(int argc, char* argv[]) {

    std::string programName = argv[0];
    std::string trkFilePath;
    std::string trkBaseName;
    std::string outputName;
    FILE *fp;

    CSV_Formatter csv_formatter;
    LogFormatter* logFormatter = &csv_formatter; // default formatter.
    Varlist_Formatter varlist_formatter;

    if (argc <= 1 ) {
        std::cerr << programName << ": No arguments were supplied.\n";
        std::cerr.flush();
        usage();
        exit(1);
    } else {
        int i = 1;
        std::string arg;
        while ( i < argc ) {
            arg = argv[i];

            if (arg.find("-") == 0) {
                if (arg == "-help" | arg == "--help" ) {
                    usage();
                    exit(0);
                } else if (arg == "-csv") {
                   logFormatter = &csv_formatter;
                } else if (arg == "-varlist") {
                   logFormatter = &varlist_formatter;
                } else if (arg == "-o") {
                    i++;
                    if (i<argc) {
                       arg = argv[i];
                       outputName = arg;
                    } else {
                        std::cerr << programName << ": -o option requires a filename." << std::endl;
                        usage();
                        exit(1);
                    }
                } else {
                    std::cerr << programName << ": Invalid option \"" << arg << "\"." << std::endl;
                    usage();
                    exit(1);
                }
            } else if (arg.substr(arg.find_last_of(".")) == ".trk") {
                size_t pos;
                std::string trkFileName;
                trkFilePath = arg;
                if ((pos = trkFilePath.find_last_of("/")) != std::string::npos) {
                    trkFileName = trkFilePath.substr(pos+1);
                } else {
                    trkFileName = trkFilePath;
                }
                trkBaseName = trkFileName.substr(0, trkFileName.length()-4);
            } else {
                std::cerr << programName << ": Invalid argument \"" << arg << "\"." << std::endl;
                usage();
                exit(1);
            }
            i++;
        }
    }

    if (trkFilePath.empty()) {
        std::cerr << programName << ": Missing .trk filename." << std::endl;
        usage();
        exit(1);
    }

    if (outputName.empty()) {

        outputName = trkBaseName + logFormatter->extension();
    }

    std::cout << programName << ": Input  = \"" << trkFilePath << "\"." << std::endl;
    std::cout << programName << ": Output = \"" << outputName << "\"." << std::endl;

    DataLog* datalog = new DataLog(trkFilePath);
    datalog->selectAllParameters();

    if (( fp = fopen(outputName.c_str(), "w") ) != NULL) {
        datalog->formattedWrite(fp, logFormatter);
        return 0;
    }
    return 1;
}
