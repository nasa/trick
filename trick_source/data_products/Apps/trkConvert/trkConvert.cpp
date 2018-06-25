#include <stdio.h>
#include <iostream>
#include <string>
#include "LogFormatter.hh"
#include "CSV_Formatter.hh"
#include "Varlist_Formatter.hh"
#include "TRK_DataLog.hh"

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

    TRK_DataLog* datalog = new TRK_DataLog(trkFilePath.c_str());
    datalog->selectAllParameters();

    if (( fp = fopen(outputName.c_str(), "w") ) != NULL) {
        datalog->formattedWrite(fp, logFormatter);
        return 0;
    }
    return 1;
}
