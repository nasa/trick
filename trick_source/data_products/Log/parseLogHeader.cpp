/**
    Used to parse a Trick log header file, and load info for Log Classes
    @author Keith Vetter
    @version May 2002
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <unistd.h>

#include "log.h"

#include "trick/units_conv.h"
#include "trick/map_trick_units_to_udunits.hh"

/* A private function for parsing log header files.
 * This is responsible for initialize the Log classes as well
 */
int LogGroup::parseLogHeaders()
{

        int nVars;
        int lineNum;
        char *line;
        char *str1;
        char *str2;
        char *str3;
        char *str4;
        char *headerName;
        int i;
        int headerSize;
        int len;

        LogData *currLogData;
        Var *currVar;


        // Foreach log header
        for (i = 0; i < nHeaders_; i++) {

                // Allocate headerName
                len = strlen(pathToBinaryData_) + strlen(headers_[i]) + 2;
                headerName = new char[len];

                // Full path of log header
                sprintf(headerName, "%s/%s", pathToBinaryData_, headers_[i]);

                // Open log header
                if ((fp_ = fopen(headerName, "r")) == NULL) {
                        fprintf(stderr,
                                "\nERROR:\nCouldn't open file \"%s\" \n",
                                headerName);
                        return (-1);
                }
                // Allocate strings, we know that buffers can't be
                // longer than file!
                // Since headers are typically small, allocate them at ]
                // the header size
                fseek(fp_, 0, SEEK_END);
                headerSize = ftell(fp_);
                ::rewind(fp_);
                line = new char[headerSize + 1];        // Not sure about +1 :)
                str1 = new char[headerSize + 1];
                str2 = new char[headerSize + 1];
                str3 = new char[headerSize + 1];
                str4 = new char[headerSize + 1];

                // Parse rest of file
                lineNum = 0;
                while (fgets(line, headerSize, fp_) != NULL) {

                        lineNum++;

                        /* <log_file_name> <data_type> <units>    <param_name>
                           or
                           <log_file_name>  byte_order    is   [lit|big]_endian
                         */
                        if ( sscanf(line, "%s %s %s %s", str1, str2, str3, str4) == 4 ) {

                                // Byte order statement and top of a log group?
                                if (!strcmp(str2, "byte_order")) {

                                        LogData *ld = new LogData;
                                        log.push_back(ld);
                                        currLogData = log[nGroups_];
                                        nGroups_++;

                                        // New binary file
                                        currLogData->setBinaryFileName(str1);

                                        // Set byte order
                                        if (!strcmp(str4, "big_endian")) {
                                                currLogData->dataByteOrder = 1;
                                        } else {
                                                currLogData->dataByteOrder = 0;
                                        }

                                        continue;
                                }
                                // Check for binary file name mismatches
                                if (strcmp(str1, currLogData->getBinaryFileName())) {
                                        printf("ERROR: Parsing log header \"%s\".\n"
                                               "       Line %d. Binary file name "
                                               "mismatch with \"%s\".\n"
                                               "       May need little/big endian "
                                               "specification.\n\n",
                                               headerName, lineNum,
                                               currLogData->getBinaryFileName());
                                        return (-1);
                                }
                                // New variable
                                Var *v = new Var;
                                currLogData->vars.push_back(v);
                                nVars = currLogData->getNumVars();
                                currVar = currLogData->vars[nVars];
                                nVars++;
                                currLogData->setNumVars(nVars);

                                // Set Type
                                if (currVar->setType(str2) < 0) {
                                        printf("ERROR: In log header \"%s\".\n"
                                               "       Line %d. Type \"%s\" is "
                                               "not supported.\n",
                                               headerName, lineNum, str2);
                                        return (-1);
                                }

                                // For Release-07 we need to convert the old unit specs by
                                // adding explicit asterisk for multiplication. )
                                {
                                        char new_units_spec[100];
                                        new_units_spec[0] = 0;
                                        if ( convert_units_spec (str3, new_units_spec) != 0 ) {
                                                printf (" ERROR: Attempt to convert old units-spec, \"%s\" failed.\n\n",str3);
                                                return (-1);
                                        }
                                        delete [] str3;
                                        len = strlen(new_units_spec);
                                        str3 = new char[len + 1] ;
                                        strcpy(str3, new_units_spec);
                                }

                                // Initialize Unit class
                                if ( ! strcmp(str3,"--")) {
                                    currVar->setUnit(str3);
                                } else {
                                    currVar->setUnit(map_trick_units_to_udunits(str3));
                                }

                                // Set Var Name
                                currVar->setVarName(str4) ;
                                if (currVar->getSize() < 0) {
                                        printf("ERROR: In log header \"%s\".\n"
                                               "       Line %d. Problem with var "
                                               "name \"%s\" . \n",
                                               headerName, lineNum, str4);
                                        return (-1);
                                }
                        }
                }

                fclose(fp_);
                delete[]line;
                delete[]str1;
                delete[]str2;
                delete[]str3;
                delete[]str4;
                delete[]headerName;
        }

        return 1;
}
