#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sys/types.h>

#include <errno.h>
#include <ctype.h>
#include <unistd.h>

#include "LogData.hh"
#include "trick_byteswap.h"
#include "../libutils/convutils.hh"

// For DBL_MAX def
#include <cmath>
#include <float.h>

#ifdef WIN32
#include <windows.h>
#endif

/**
 * LogData constructor
 */
LogData::LogData() :
            _varVal(0),
            _sizeVal(0),
            _scaleVal(0),
            _biasVal(0),
            _unitVal(0),
            _maxVal(0),
            _minVal(0),
            _typeVal(0)
{
        unsigned char localByteOrder;

        _open = 0;
        dataByteOrder = 0;
        _currBinExtension = 0;
        _fp = 0;
        _nBinFiles = 0;

        _binFileName = new char[1];
        _binFileName[0] = '\0';

        _binFilePath = new char[1];
        _binFilePath[0] = '\0';

        _currRecord = new char[0];

        _isEOF = 0;
        _currValIdx = 0;
        _convertToDouble = 0;
        _nVars = 0;

        _isEnabledStats = 0;

        _statsMin = 0 ;
        _statsMax = 0 ;

        // Get local machine's byte odor (smelly to be sure)
        TRICK_GET_BYTE_ORDER(localByteOrder)
            if (localByteOrder == (char) (0x00)) {
                // Big Endian
                machineByteOrder = 1;
        } else {
                // Little Endian
                machineByteOrder = 0;
        }
}

/** Destructor for LogData */
LogData::~LogData()
{

        int ii;

        if (_fp) {
                fclose(_fp);
        }

        delete[]_binFilePath;
        delete[]_binFileName;

        for (ii = 0; ii < _nVars; ii++) {
                delete vars[ii];
        }

        if (_currRecord) delete[]_currRecord;
        if (_varVal)     delete[]_varVal;
        if (_sizeVal)    delete[]_sizeVal;
        if (_scaleVal)   delete[]_scaleVal;
        if (_biasVal)    delete[]_biasVal;
        if (_unitVal)    delete[]_unitVal;
        if (_maxVal)     delete[]_maxVal;
        if (_minVal)     delete[]_minVal;
        if (_typeVal)    delete[]_typeVal;

        if ( _statsMin && _statsMax ) {
                delete[] _statsMin;
                delete[] _statsMax;
        }
}

/** Return the number of variable (not values) in binary log file header
 *
 *  If log header contains "a[0-1].b[0-1], x.y[4-5]" there are 2
 *  variables NOT 6. 
 */
int LogData::getNumVars()
{
        return (_nVars);
}

/** Set the number of variables (not values) in binary log file header
 *
 *  If log header contains "a[0-1].b[0-1], x.y[4-5]" there are 2
 *  variables NOT 6. 
 *
 *  This should be used with care!  Currently -only- a parse routine
 *  in LogGroup uses it.  I could have made LogGroup a friend... but
 *  wanted to keep the interface stiff.
 */
void LogData::setNumVars(int nVars)
{
        _nVars = nVars;
}

/** Close a Trick data log 
 * @see rewind() close()
 */
int LogData::close()
{

        // Delete currRecord
        delete[]_currRecord;

        _open = 0;
        fclose(_fp);

        // Return
        return (1);
}

/** Puts next read at beginning of data log 
 *  @see open() close() 
 */
int LogData::rewind()
{

        if (!_open) {
                return (-1);
        }

        _isEOF = 0;

        if (_currBinExtension > 0) {
                // If we have multiple bin files, we must go back to first one
                fclose(_fp);
                _currBinExtension = 0;
                _openCurrBinaryFile();
        } else {
                ::rewind(_fp);
        }

        return (0);
}

/** Open a data log at location specified by path 
 *  Path will normally point to a RUN_ directory
 *  @see rewind() close()
 */
int LogData::open(char *pathToBinaryData)
{

        int i, j;
        int idx;
        int nDims;
        int ret ;

        if (_open) {
                printf("WARNING: Data file already opened.\n");
                return (1);
        }
        // Make sure that number of variables have been set
        if (_nVars == 0) {
                printf("ERROR: The number of variables "
                       "in log file is not set.\n");
                return (-1);
        }
        // Count number of binary files for this log data dude (log*000-?)
        _nBinFiles = _countNumBinaryFiles(pathToBinaryData);

        // Save off binary file path
        delete[]_binFilePath;
        _binFilePath = new char[strlen(pathToBinaryData) + 1];
        strcpy(_binFilePath, pathToBinaryData);

        // Open first binary file (log*.000) 
        ret = _openCurrBinaryFile();
        if ( ret == -1 ) {
                return(-1);
        }

        // Initialize size of record
        _sizeRecord = getSizeRecord();
        if (_sizeRecord < 1) {
                printf("ERROR: Size of record is %d.\n", _sizeRecord);
        }
        // Initialize currRecord
        delete[]_currRecord;
        _currRecord = new char[_sizeRecord];

        // Initialize the value array's size and types 
        // Values are elements in record, each one has a size and type

        // Allocate value attributes
        _nVals = calcNumValRecord();
        _varVal = new int[_nVals];
        _sizeVal = new int[_nVals];
        _scaleVal = new double[_nVals];
        _biasVal = new double[_nVals];
        _unitVal = new double[_nVals];
        _maxVal = new double[_nVals];
        _minVal = new double[_nVals];
        _typeVal = new Var::enumType[_nVals];

        // Allocate variables that hold stats
        _statsMin = new Stats[_nVals];
        _statsMax = new Stats[_nVals];
        for (i = 0; i < _nVals; i++) {
                _statsMin[i].val = DBL_MAX;
                _statsMax[i].val = -DBL_MAX;
        }

        // Initialize value attributes
        idx = 0;
        for (i = 0; i < _nVars; i++) {
                nDims = vars[i]->getNumDims();
                for (j = 0; j < nDims; j++) {
                        _varVal[idx] = i;
                        _sizeVal[idx] = vars[i]->getTypeSize();
                        _scaleVal[idx] = vars[i]->getScaleFactor();
                        _biasVal[idx] = vars[i]->getBias();
                        _unitVal[idx] = 1.0;
                        _maxVal[idx] = vars[i]->getMaxRange();
                        _minVal[idx] = vars[i]->getMinRange();
                        _typeVal[idx] = vars[i]->getType();
                        idx++;
                }
        }

        // Finally set open flag
        _open = 1;

        // Return
        return (1);
}

/** Go through all data and calculate statistics
 */
void LogData::enableStats()
{

        int ii;
        double val, tval;

        // Stats on, just return 
        if (_isEnabledStats) {
                return;
        } else {
                _isEnabledStats = 1;
        }

        int tIdx = getParamIdx("sys.exec.out.time");
        if (tIdx == -1) {
                fprintf(stderr, "ERROR: enableStats() because time is "
                                "something other than sys.exec.out.time\n");
                exit(-1);
        }
        // Mins and maxs 
        rewind();               // Start at top of records 
        while (getNextRecord() != NULL) {

                // Get time stamp and BE SURE to reset pointers
                tval = *((double *) getVal(tIdx));
                _currValPtr = (unsigned char *) _currRecord;
                _currValIdx = 0;
                _bytesRead = 0;

                for (ii = 0; ii < _nVals; ii++) {
                        val = *((double *) getNextVal());       // For speed :)
                        if (_statsMin[ii].val > val) {
                                _statsMin[ii].val = val;
                                _statsMin[ii].tval = tval;
                        }
                        if (_statsMax[ii].val < val) {
                                _statsMax[ii].val = val;
                                _statsMax[ii].tval = tval;
                        }
                }
        }
}

/** Returns min/max time stamps and values calculated by calculateStats() */
void LogData::getMinMaxStats(int paramIdx,
                             double *tmin, double *min,
                             double *tmax, double *max)
{

        if (!_isEnabledStats) {
                enableStats();
        }

        *tmin = _statsMin[paramIdx].tval;
        *min = _statsMin[paramIdx].val;
        *tmax = _statsMax[paramIdx].tval;
        *max = _statsMax[paramIdx].val;
}

/** Count the number of binary files located at path given e.g.
 *  In /user3/vetter/trick_sims/SIM_dog/RUN_test
 *  there may be log_Cat.000 log_Cat.001 .... log_Cat.034
 *  In this case countNumBinaryFiles returns 35
 */
int LogData::_countNumBinaryFiles(char *pathToBinaryData)
{


        int count = 0;
        int ii;                 // Long live Rob!
        int binary;
        int len_binFileName;
        int len_dname;
        int len_extension = 4;  // length .### binary extension
		string name;

        list<string> entries = Utils::getDirEntries(pathToBinaryData);
		for (list<string>::iterator it = entries.begin(); it != entries.end(); it++) {
			    name = *it;
                len_dname = name.length();
                len_binFileName = strlen(_binFileName);
                if (len_dname == len_binFileName + len_extension &&
                    !strncmp(name.c_str(), _binFileName, len_binFileName)) {

                        // We have found bin file name, 
                        // make sure it has a binary extension
                        binary = 1;
                        for (ii = len_dname - 1; ii >= len_dname - 3; ii--) {
                                if (!(name[ii] >= '0' &&
                                      name[ii] <= '9')) {
                                        binary = 0;
                                }
                        }

                        // If this is a binary file, 
                        // 000-999 extension, increment count
                        if (binary) {
                                count++;
                        }
                }
        }

        return (count);
}


/*
 * Used privately to open each subsequent binary file on a the lowest level
 */
int LogData::_openCurrBinaryFile()
{

        char extension[4];
        char *file;
        int len;

        // Construct extension
        if (_currBinExtension < 10) {
                sprintf(extension, "00%d", _currBinExtension);
        } else if (_currBinExtension < 100) {
                sprintf(extension, "0%d", _currBinExtension);
        } else {
                sprintf(extension, "%d", _currBinExtension);
        }

        // Construct binary filename
        len = strlen(_binFilePath) + strlen(_binFileName) +
            strlen(extension) + 3;
        file = new char[len];
        sprintf(file, "%s/%s.%s", _binFilePath, _binFileName, extension);

        // Do sanity check on file
#ifndef WIN32
        if (access(file, F_OK | R_OK) == -1) {
#else
		DWORD fileattrs = GetFileAttributes(file);
		if (fileattrs == INVALID_FILE_ATTRIBUTES) {
#endif
                printf("ERROR: Search for parameter exhausted."
                       "       Or \"%s\" is not readable or doesn't exist. \n",
                       file);
                return(-1);
        }

        // Close previous binary file if not first file to open 
        if (_currBinExtension > 0) {
                fclose(_fp);
        }
        // Open binary file
        _fp = fopen(file, "rb");
        if (_fp == NULL) {
                printf("ERROR: Couldn't open \"%s\" \n", file);
                exit(-1);
        }

        delete[]file;

        return (1);
}

/** Request to convert all data from log file into doubles 
 *  @see convertToDoubleNo()
*/
int LogData::convertToDoubleYes()
{
        _convertToDouble = 1;
        return (1);
}

/** Do NOT convert log data to doubles
 *  @see convertToDoubleYes()
 */
int LogData::convertToDoubleNo()
{
        _convertToDouble = 0;
        return (1);
}

/** Get number of records within a Trick binary log e.g.
 *  
 *  If time, x and y are logged, where time, x & y are doubles
 *  And the Trick binary files contains 29616 bytes, we'd have 
 *  29616/24 = 1,234 records.
 */
int LogData::getNumRecords()
{

        FILE *fpTmp;
        char *file;
        char extension[4];

        int fileOffset;
        int numRecs;
        int totalNumRecs;
        int ii;
        int len;

        totalNumRecs = 0;
        for (ii = 0; ii < _nBinFiles; ii++) {

                // Construct extension
                if (ii < 10) {
                        sprintf(extension, "00%d", ii);
                } else if (ii < 100) {
                        sprintf(extension, "0%d", ii);
                } else {
                        sprintf(extension, "%d", ii);
                }

                // Allocate & construct file name 
                // (+3 for '/' ,  '.' & null terminator)
                len = strlen(_binFilePath) + strlen(_binFileName) +
                    strlen(extension) + 3;
                file = new char[len];
                sprintf(file, "%s/%s.%s", _binFilePath,
                        _binFileName, extension);

                fpTmp = fopen(file, "rb");
                if (fpTmp == NULL) {
                        printf("ERROR: Couldn't open \"%s\" \n", file);
                        exit(-1);
                }

                fseek(fpTmp, 0, SEEK_END);
                fileOffset = ftell(fpTmp);
                numRecs = fileOffset / getSizeRecord();
                totalNumRecs += numRecs;

                fclose(fpTmp);
                delete[]file;
        }

        return (totalNumRecs);
}

/** Are all params read from current record ? */
int LogData::isEOF()
{
        return (_isEOF);
}

/** Return size of record in bytes e.g. 
 *
 *  If a record contains time, x, y, i, j where
 *  time, x, & y are doubles and i,j are ints
 *  You'd have 3*8 + 2*4 = 32 bytes (on most machines)
 */
int LogData::getSizeRecord()
{

        int i;
        int sizeRec;

        sizeRec = 0;
        for (i = 0; i < _nVars; i++) {
                sizeRec = sizeRec + vars[i]->getSize();
        }

        return (sizeRec);
}

/** Calculate the number of values in a record e.g.
 *
 *  A record with "x.y.z[0-2] & a[0-1].b[7]" is
 *  x.y.z[0], x.y.z[1], x.y.z[2], a[0].b[7], a[1].b[7]
 *  This is 5 vals for the record.  Use getSizeRecord
 *  for byte size.
 */
int LogData::calcNumValRecord()
{


        int i;
        int nVals;

        nVals = 0;
        for (i = 0; i < _nVars; i++) {
                nVals = nVals + vars[i]->getNumDims();
        }

        return (nVals);
}

/** Retrieve number of values in a record 
 *  (calculated by calcNumValRecord())
 *  _nVals set once in the open routine
 */
int LogData::getNumValRecord()
{
        return (_nVals);
}

/** Read the next binary record in current binary file 
 * For external data logs we just return a 1 
 **/
char *LogData::getNextRecord()
{

        int ret;
        int sr;

        // Reset variable index to first variable
        _currVarIndex = 0;

        // Reset variable pointer to top of record 
        _currValPtr = (unsigned char *) _currRecord;
        _currValIdx = 0;
        _bytesRead = 0;
        _isEOF = 0;

        // Read in next record
        sr = _sizeRecord;
        ret = fread(_currRecord, sr, 1, _fp);
        if (ret <= 0) {
                // Hit EOF on current bin file
                _currBinExtension++;
                if (_currBinExtension < _nBinFiles) {
                        // Open up next bin file, and get first rec
                        _openCurrBinaryFile();
                        ret = fread(_currRecord, sr, 1, _fp);
                        return (_currRecord);
                } else {
                        // We are at end of log data (no more recs)
                        _currValIdx = 0;
                        _isEOF = 1;
                        return ((char *) NULL);
                }
        } else {
                // Read went OK
                return (_currRecord);
        }
}

/* Very low level routine for advancing value pointer across record */
void *LogData::_advanceCurrValPtr()
{

        _currValPtr = _currValPtr + _sizeVal[_currValIdx];
        _bytesRead = _bytesRead + _sizeVal[_currValIdx];
        _currValIdx++;

        return ((void *) &_currVal);
}

#define DP_ASSIGN_VAL(V, TYPE) \
       if ( _convertToDouble ) { \
               _currVal.d = (double) V ;  \
       } else { \
               _currVal.V =  (TYPE) V ;  \
               _currVal.V *= (TYPE)_scaleVal[_currValIdx] ; \
               _currVal.V *= (TYPE)_unitVal[_currValIdx] ; \
               _currVal.V += (TYPE)_scaleVal[_currValIdx] ; \
               if ( _currVal.V > (TYPE) _maxVal[_currValIdx] ) { \
                       _currVal.V = (TYPE) _maxVal[_currValIdx] ; \
               } \
               if ( _currVal.V < (TYPE) _minVal[_currValIdx] ) { \
                       _currVal.V = (TYPE) _minVal[_currValIdx] ; \
               } \
       }

/**
 * Main function for byte-by-byte grabbing values out of a binary record 
 * TODO: Can I use a Template<> ???
 */
void *LogData::getNextVal()
{

        int idx;
        unsigned char *cp1;

        char c;
        short s;
        int i;
        long l;
        unsigned char uc;
        unsigned short us;
        unsigned int ui;
        unsigned long ul;
        float f;
        double d;
        long long ll;
        unsigned long long ull;

        if (_currRecord == 0) {
                return (0);
        }

        if (_bytesRead >= _sizeRecord) {
                return (0);
        }

        switch (_typeVal[_currValIdx]) {

        case Var::VAR_CHAR:
                cp1 = (unsigned char *) &c;
                cp1[0] = _currValPtr[0];
                if (_convertToDouble) {
                        _currVal.d = (double) c;
                } else {
                        _currVal.c = (char) c;
                }
                break;
        case Var::VAR_SHORT:
                cp1 = (unsigned char *) &s;
                for (idx = 0; idx < 2; idx++)
                        cp1[idx] = _currValPtr[idx];
                if (machineByteOrder != dataByteOrder) {
                        s = trick_byteswap_short(s);
                }
                DP_ASSIGN_VAL(s, short int);
                break;
        case Var::VAR_INT:
                cp1 = (unsigned char *) &i;
                for (idx = 0; idx < 4; idx++) {
                        cp1[idx] = _currValPtr[idx];
                }
                if (machineByteOrder != dataByteOrder) {
                        i = trick_byteswap_int(i);
                }
                DP_ASSIGN_VAL(i, int);
                break;
        case Var::VAR_LONG:
                cp1 = (unsigned char *) &l;
                for (idx = 0; idx < 8; idx++) {
                        cp1[idx] = _currValPtr[idx];
                }
                if (machineByteOrder != dataByteOrder) {
                        l = trick_byteswap_long(l);
                }
                DP_ASSIGN_VAL(l, long);
                break;
        case Var::VAR_UNSIGNED_CHAR:
                cp1 = (unsigned char *) &uc;
                cp1[0] = _currValPtr[0];
                if (_convertToDouble) {
                        _currVal.d = (double) uc;
                } else {
                        _currVal.uc = (unsigned char) uc;
                }
                break;
        case Var::VAR_UNSIGNED_SHORT:
                cp1 = (unsigned char *) &us;
                for (idx = 0; idx < 2; idx++) {
                        cp1[idx] = _currValPtr[idx];
                }
                if (machineByteOrder != dataByteOrder) {
                        us = trick_byteswap_short(us);
                }
                DP_ASSIGN_VAL(us, unsigned short);
                break;
        case Var::VAR_UNSIGNED_INTEGER:
                cp1 = (unsigned char *) &ui;
                for (idx = 0; idx < 4; idx++) {
                        cp1[idx] = _currValPtr[idx];
                }
                if (machineByteOrder != dataByteOrder) {
                        ui = trick_byteswap_int(ui);
                }
                DP_ASSIGN_VAL(ui, unsigned int);
                break;
        case Var::VAR_UNSIGNED_LONG:
                cp1 = (unsigned char *) &ul;
                for (idx = 0; idx < 8; idx++) {
                        cp1[idx] = _currValPtr[idx];
                }
                if (machineByteOrder != dataByteOrder) {
                        ul = trick_byteswap_long(ul);
                }
                DP_ASSIGN_VAL(ul, unsigned long);
                break;
        case Var::VAR_FLOAT:
                cp1 = (unsigned char *) &f;
                for (idx = 0; idx < 4; idx++) {
                        cp1[idx] = _currValPtr[idx];
                }
                if (machineByteOrder != dataByteOrder) {
                        f = trick_byteswap_float(f);
                }
                DP_ASSIGN_VAL(f, float);
                break;
        case Var::VAR_DOUBLE:
                cp1 = (unsigned char *) &d;
                for (idx = 0; idx < 8; idx++) {
                        cp1[idx] = _currValPtr[idx];
                }
                if (machineByteOrder != dataByteOrder) {
                        d = trick_byteswap_double(d);
                }
                _currVal.d = (double) d;
                if (!_convertToDouble) {
                        // If converting, scaling done later 
                        _currVal.d *= _scaleVal[_currValIdx];
                        _currVal.d *= _unitVal[_currValIdx];
                        _currVal.d += _biasVal[_currValIdx];
                        if (_currVal.d > _maxVal[_currValIdx]) {
                                _currVal.d = _maxVal[_currValIdx];
                        }
                        if (_currVal.d < _minVal[_currValIdx]) {
                                _currVal.d = _minVal[_currValIdx];
                        }
                }
                break;
        case Var::VAR_LONG_LONG:
                cp1 = (unsigned char *) &ll;
                if (machineByteOrder != dataByteOrder) {
                        for (idx = 0; idx < 8; idx++) {
                                cp1[7 - idx] = _currValPtr[idx];
                        }
                } else {
                        for (idx = 0; idx < 8; idx++) {
                                cp1[idx] = _currValPtr[idx];
                        }
                }
                DP_ASSIGN_VAL(ll, long long);
                break;
        case Var::VAR_UNSIGNED_LONG_LONG:
                cp1 = (unsigned char *) &ull;
                if (machineByteOrder != dataByteOrder) {
                        for (idx = 0; idx < 8; idx++) {
                                cp1[7 - idx] = _currValPtr[idx];
                        }
                } else {
                        for (idx = 0; idx < 8; idx++) {
                                cp1[idx] = _currValPtr[idx];
                        }
                }

                DP_ASSIGN_VAL(ull, unsigned long long);
                break;
        case Var::VAR_VOID:
                break;
        default:
                printf("ERROR: GetNextVal getting weird type.\n");
                exit(-1);
        }

        // Do scaling etc... if converting to doubles
        if (_convertToDouble) {
                _currVal.d *= _scaleVal[_currValIdx];
                _currVal.d *= _unitVal[_currValIdx];
                _currVal.d += _biasVal[_currValIdx];
                if (_currVal.d > _maxVal[_currValIdx]) {
                        _currVal.d = _maxVal[_currValIdx];
                }
                if (_currVal.d < _minVal[_currValIdx]) {
                        _currVal.d = _minVal[_currValIdx];
                }
        }
        // Now that we have current val, advance pointer
        _advanceCurrValPtr();

        return ((void *) &_currVal);
}

/** Get value in record at given index e.g.
 * 
 *  So if the current record contains a[0-1], b[0-1]
 *  getVal(2) will return b[0]'s value
 */
void *LogData::getVal(int index)
{

        int i;

        // Make sure record exists
        if (_currRecord == 0) {
                return (0);
        }

        // Make sure index is valid
        if (index >= _nVals || index < 0) {
                fprintf(stderr, "ERROR: getVal attempting to grab val "
                        "out of bounds.\n"
                        "       getVal received faulty index \"%d\".\n", index);
                fprintf(stderr, "       %s has only \"%d\" vals per record.\n",
                        _binFileName, getNumValRecord());
                exit(-1);
        }
        // Reset variable pointer to top of current record 
        _currValPtr = (unsigned char *) _currRecord;
        _currValIdx = 0;
        _bytesRead = 0;

        // Now advance to index given
        for (i = 0; i < index; i++) {
                _advanceCurrValPtr();
        }

        // Now return value at index
        return (getNextVal());
}

/** Get C-type for value of current record at given index */
Var::enumType LogData::getType(int idx)
{
        return (_typeVal[idx]);
}

/** Get size in bytes for value of current record at given index */
int LogData::getSize(int idx)
{
        return (_sizeVal[idx]);
}

/**
 * Get index of a parameter in log header e.g.
 *
 * If header has a[0-1].b[3], x[0-1].y[0-1]
 * getParamIdx( "x[1].y[0]" ) returns 4 since
 * x[1].y[0] is 4rth idx into
 * a[0].b[3], a[1].b[3], x[0].y[0], x[0].y[1], x[1].y[0], x[1].y[1]  
 *
 * If this log data is external data just pass back index
 * to external parameter.  No worries about multi-dimensions :)
 */
int LogData::getParamIdx(const char *param)
{

        int i;
        int valIndex;
        int offset;

        valIndex = 0;
        for (i = 0; i < _nVars; i++) {
                if (!vars[i]->compareNames(param)) {
                        /*
                         * We have a match
                         *
                         * So the given parameter was found in the variable
                         * list given in the header file
                         *
                         * We -almost- know which variable index this is,
                         * but we must now get the offset
                         *
                         * Must also make sure that param is in vars[i] space
                         * E.g. a.b[0][1] is in a.b[0-5][1-3] space
                         * but  a.b[0][1] is NOT in a.b[5][6-10] space
                         */

                        offset = vars[i]->getDimOffset(param);
                        if (offset != -1) {
                                valIndex = valIndex + offset;
                                return (valIndex);
                        }
                }
                valIndex = valIndex + vars[i]->getNumDims();
        }

        // Not found
        return (-1);
}

/** Get the unit associated with the parameter as specified in Log header */
Unit *LogData::getUnit(const char *param)
{

        int ii;

        for (ii = 0; ii < _nVars; ii++) {
                if (!vars[ii]->compareNames(param)) {
                        return (vars[ii]->getUnit());
                }
        }

        // Not found, return NULL
        return (0);
}

/** Set scale factor for parameter with given factor */
int LogData::setScaleFactor(int paramIdx, double factor)
{

        if (paramIdx > _nVals || paramIdx < 0) {
                fprintf(stderr, "ERROR: setScaleFactor received paramIdx "
                        "%d out of bounds.\n", paramIdx);
                exit(-1);
        }

        vars[_varVal[paramIdx]]->setScaleFactor(factor);
        _scaleVal[paramIdx] = factor;
        return (1);
}

/** Set bias (offset/shift) for parameter with given value */
int LogData::setBias(int paramIdx, double bias)
{

        if (paramIdx > _nVals || paramIdx < 0) {
                fprintf(stderr, "ERROR: setBias received paramIdx %d "
                        "out of bounds.\n", paramIdx);
                exit(-1);
        }

        vars[_varVal[paramIdx]]->setBias(bias);
        _biasVal[paramIdx] = bias;
        return (1);
}

/** Set maximal value for parameter */
int LogData::setMax(int paramIdx, double max)
{

        if (paramIdx > _nVals || paramIdx < 0) {
                fprintf(stderr, "ERROR: setMax received paramIdx %d"
                        " out of bounds.\n", paramIdx);
                exit(-1);
        }

        vars[_varVal[paramIdx]]->setMaxRange(max);
        _maxVal[paramIdx] = max;
        return (1);
}

/** Set maximal value for parameter */
int LogData::setMin(int paramIdx, double min)
{

        if (paramIdx > _nVals || paramIdx < 0) {
                fprintf(stderr, "ERROR: setMin received paramIdx %d "
                        "out of bounds.\n", paramIdx);
                exit(-1);
        }

        vars[_varVal[paramIdx]]->setMinRange(min);
        _minVal[paramIdx] = min;
        return (1);
}

/** Set unit for parameter 
 *  Internally this will set a scale factor from unit in log
 *  file to unit given as argument.  If the unit passed is
 *  NULL, the scale factor will be set to 1.0.
 */
int LogData::setUnit(int paramIdx, Unit * u)
{

        if (paramIdx > _nVals || paramIdx < 0) {
                fprintf(stderr, "ERROR: setUnit received paramIdx %d "
                        "out of bounds.\n", paramIdx);
                exit(-1);
        }
        // If no unit passed, set scale factor to 1.0 (default)
        if (u == 0) {
                _unitVal[paramIdx] = 1.0;
                return (-1);
        }

        _unitVal[paramIdx] = vars[_varVal[paramIdx]]->getUnit()->convert(u);
        if (_unitVal[paramIdx] == 0) {
                fprintf(stderr, "ERROR: Bad unit conversion "
                        "given to setUnit.\n");
                exit(-1);
        }
        return (1);
}

/** Get the value of a paramter at a given time stamp
 *  Find value around an epsilon of time stamp
 *  return -1 if can't find value with time stamp
 */
int LogData::getValueAtTime( int tIdx, double time,
                             int vIdx, double* value) {

        double tVal ;

        if (vIdx > _nVals || vIdx < 0) {
                fprintf(stderr, "ERROR: getValueAtTime received paramIdx %d "
                        "out of bounds.\n", vIdx);
                exit(-1);
        }
        if ( time < 0 ) {
                fprintf(stderr, "ERROR: getValueAtTime() received "
                        "a time stamp of %lf.\n", time);
                exit(-1);
        }

        // Sequentially look for data associated with time stamp
        // I'll optimize this if it gets more use, or I get more time
        rewind();
        while (getNextRecord() != NULL) {

                // Get time stamp and BE SURE to reset pointers
                tVal = *((double *) getVal(tIdx));
                _currValPtr = (unsigned char *) _currRecord;
                _currValIdx = 0;
                _bytesRead = 0;

                if ( fabs( tVal - time ) < 0.0000001 ) {
                        *value =  *((double*)getVal(vIdx)) ;
                        return( 1 ) ;
                }
        }

        // Never found value associated with time stamp
        return(-1);
}

/** Set the binary file name e.g.
  * log_Ball1, without the 000, 001 extensions...
  */
int LogData::setBinaryFileName(char *fileName)
{

        delete[]_binFileName;
        _binFileName = new char[strlen(fileName) + 1];
        strcpy(_binFileName, fileName);

        return (0);
}

/** Get the binary file name e.g.
  * log_Ball1, without the 000, 001 extensions...
  */
char *LogData::getBinaryFileName()
{
        return (_binFileName);
}

/** Default LogGroup constructor */
LogGroup::LogGroup()
{
        fprintf(stderr, "ERROR: Please use LogGroup::LogGroup(char* headerFile)"
                " constructor.\n");

        exit(0);
}

/** This LogGroup constructor takes a path to the Trick binary data files */
LogGroup::LogGroup(char *path)
{

        int ret;
        int i;

        _nGroups = 0;
        _isEOF = 0;

        _pathToBinaryData = new char[strlen(path) + 1];
        strcpy(_pathToBinaryData, path);
        _getHeaders();
        if (_parseLogHeaders() == -1) {
                fprintf(stderr, "ERROR: Failed parsing log headers "
                                "for \"%s\".\n", path);
        }
        // Make sure there is at least one log file
        if (_nGroups == 0) {
                fprintf(stderr, "ERROR: No log header files found in \"%s\"\n",
                        _pathToBinaryData);
        }
        // Open up data files
        for (i = 0; i < _nGroups; i++) {
                ret = log[i]->open(_pathToBinaryData);
                if ( ret == -1 ) {
                        fprintf(stderr, "ERROR: Failed open() for \"%s\"\n",
                        _pathToBinaryData);
                }
        }
}

/** Default destructor --- broken :( */
LogGroup::~LogGroup()
{

        int ii;

        delete[]_pathToBinaryData;

        for (ii = 0; ii < _nGroups; ii++) {
                delete log[ii];
        }

        if (_headers) {
                for (ii = 0; ii < _nHeaders; ii++) {
                        free(_headers[ii]);
                }
                free(_headers);
        }
}


/** All data will be converted to doubles */
int LogGroup::convertToDoubleYes()
{
        int i;

        for (i = 0; i < _nGroups; i++) {
                log[i]->convertToDoubleYes();
        }

        return (0);
}

/** Get number of records in this log group 
 *  Return the max number of records among all the Trick logs 
 */
int LogGroup::getNumRecords()
{
        int i;
        int ret;
        int max;

        max = 0;
        for (i = 0; i < _nGroups; i++) {
                ret = log[i]->getNumRecords();
                if (ret > max) {
                        max = ret;
                }
        }

        return (max);
}

/** Enable statistics (min/max...) 
 *  This will actually calculate stats on the first pass 
 *  Find absolute min/max value for a param 
 *  This will move the file offsets in the binary files
 *  Maybe we'll add the "ftell()" later!
 */
void LogGroup::getMinMaxStats(const char *param,
                              double *tmin, double *min,
                              double *tmax, double *max)
{

        int xIdx, xLogIdx;
        int yIdx, yLogIdx;
        int ret ;

        // We need to insure all are doubles for these calculations
        convertToDoubleYes();

        ret = getParamIdx("sys.exec.out.time", &xIdx, &xLogIdx,
                    param, &yIdx, &yLogIdx);
        if ( ret == -1 ) {
                fprintf(stderr, "ERROR: Couldn't get min/max statistics "
                                " due to bad log indices.\n");
                exit(-1);
        }

        log[yLogIdx]->getMinMaxStats(yIdx, tmin, min, tmax, max);
}

/** Set scale factor for given paramter and given factor */
int LogGroup::setScaleFactor(int logIdx, int paramIdx, double factor)
{

        log[logIdx]->setScaleFactor(paramIdx, factor);

        return (1);
}


/** Set bias (offset/shift) parameter */
int LogGroup::setBias(int logIdx, int paramIdx, double bias)
{

        log[logIdx]->setBias(paramIdx, bias);

        return (1);
}

/** Set maximum value for parameter */
int LogGroup::setMax(int logIdx, int paramIdx, double max)
{

        log[logIdx]->setMax(paramIdx, max);

        return (1);
}

/** Set minimum value for parameter */
int LogGroup::setMin(int logIdx, int paramIdx, double min)
{

        log[logIdx]->setMin(paramIdx, min);

        return (1);
}

/** Set unit for parameter 
 *  @see LogData::setUnit()
 */
int LogGroup::setUnit(int logIdx, int paramIdx, Unit * u)
{

        log[logIdx]->setUnit(paramIdx, u);

        return (1);
}

/** Set unit for parameter 
 *  @see LogData::setUnit()
 */
int LogGroup::setUnit(const char *paramName, Unit * u)
{

        int xIdx, xLogIdx;
        int yIdx, yLogIdx;
        int ret ;

        ret = getParamIdx("sys.exec.out.time", &xIdx, &xLogIdx,
                    paramName, &yIdx, &yLogIdx);
        if ( ret == -1 ) {
                fprintf(stderr, "ERROR: In setUnit() died with faulty "
                                "parameter name.\n");
                exit(-1);
        }

        log[yLogIdx]->setUnit(yIdx, u);

        return (1);
}

/** Get the value of a paramter at a given time stamp
 *  Return value around an epsilon of time stamp
 */
int LogGroup::getValueAtTime( const char *paramName, 
                              double time, double* value ) {

        int tIdx, tLogIdx;
        int yIdx, yLogIdx;
        double vv ;
        int rr ;
        int ret ;

        // Get indices based on paramName
        ret = getParamIdx("sys.exec.out.time", &tIdx, &tLogIdx,
                    paramName, &yIdx, &yLogIdx);
        if ( ret == -1 ) {
                fprintf(stderr, "ERROR: In getValueAtTime() died with faulty "
                                "parameter name.\n");
                exit(-1);
        }
        
        // Get the value at time
        rr = log[yLogIdx]->getValueAtTime(tIdx, time,
                                          yIdx, &vv) ;
        *value = vv ;

        return(rr); 

}

/** Move all file pointers of all data log files back to start */
int LogGroup::rewind()
{

        int i;

        // Make sure there is at least one log file
        if (_nGroups == 0) {
                fprintf(stderr, "ERROR: No log files specified.\n");
                fprintf(stderr, "       GetNextRecord aborting.\n");
                exit(-1);
        }

        _isEOF = 0;

        // Rewind each log data group
        for (i = 0; i < _nGroups; i++) {
                log[i]->rewind();
        }

        return (0);
}

/** Get next record across data log files by
 *  advancing ALL files in log group to next record
 */
char *LogGroup::getNextRecord()
{

        int i;
        int eof;
        char *record;

        // Make sure there is at least one log file
        if (_nGroups == 0) {
                fprintf(stderr, "ERROR: No log files specified.\n");
                fprintf(stderr, "       GetNextRecord aborting.\n");
                exit(-1);
        }

        // Get next record in each group
        eof = 0;
        for (i = 0; i < _nGroups; i++) {
                if ((record = log[i]->getNextRecord()) == (char*)NULL) {
                        eof++;
                }
        }

        // Return NULL if all groups data is read
        if ( eof >= _nGroups ) {
                _isEOF = 1;
                return ((char *) NULL);
        } else {
                return ((char *) 1);
        }
}

/**
 * Given xVarName & yVarName; return xIdx, yIdx, xLogDataIdx, yLogDataIdx
 *
 * x&y Idx are parameter indices into the x&y LogData files
 *
 * xVarName & yVarName may be in different log data groups, 
 * so it get's a little complicated.  Also there may
 * more than one param with same xVarName (time) or same yVarName!
 *
 * The algorithm here will find first group with yVarName, and then
 * see if xVarName is in same group.  If all possibilities of 
 * getting x & y out same log data group are exhausted, it will
 * then choose y to be last group it found it in, then find
 * first x with xVarName.  Thus x & y will be pulled out of
 * different log data groups.  If x & y not found, it will issue an
 * error and exit.
 */
int LogGroup::getParamIdx(const char *xVarName, int *xIdx, int *xLogDataIdx,
                          const char *yVarName, int *yIdx, int *yLogDataIdx)
{
        int i;
        int yFoundIdx;

        // Search by log data groups for x & y parameters
        *xLogDataIdx = -1;
        *yLogDataIdx = -1;
        yFoundIdx = -1;
        for (i = 0; i < _nGroups; i++) {

                if ((*yIdx = log[i]->getParamIdx(yVarName)) != -1) {
                        *yLogDataIdx = i;
                        yFoundIdx = *yIdx;
                        if ((*xIdx = log[i]->getParamIdx(xVarName)) != -1) {
                                // Yippee.  x & y in same group  
                                *xLogDataIdx = *yLogDataIdx;
                                break;
                        }
                }
        }

        if (*xLogDataIdx == -1) {
                // So x & y not in same group

                if (yFoundIdx != -1) {
                        // Let's see if xVarName is in a different 
                        // log data group than y
                        // Choose first one you find
                        *yIdx = yFoundIdx;
                        for (i = 0; i < _nGroups; i++) {
                                *xIdx = log[i]->getParamIdx(xVarName);
                                if (*xIdx != -1) {
                                        *xLogDataIdx = i;
                                        break;
                                }
                        }
                        if (*xLogDataIdx == -1) {
                                // Never found x parameter
                                fprintf(stderr, "ERROR: Couldn't find x "
                                        "param \"%s\".\n", xVarName);
                                fprintf(stderr, "       Param is not in log "
                                        "header.\n");
                                return(-1);
                        }
                } else {
                        // Never found y parameter
                        fprintf(stderr,
                                "ERROR: Couldn't find y param \"%s\".\n"
                                " Param is not in log header for "
                                "run: \"%s\".\n", 
                                yVarName, getDataDir());
                        return(-1);
                }
        }

        return (0);
}

// Private function for caching a list of headers in a RUN_ dir
int LogGroup::_getHeaders()
{
        unsigned int maxLengthHeaderName;
        int i;

        // Count num headers, and max file name length in RUN dir
        maxLengthHeaderName = 0;
        _nHeaders = 0;
		string name;
		list<string> entries = Utils::getDirEntries(_pathToBinaryData);

		for (list<string>::iterator it = entries.begin(); it != entries.end(); it++) {
			    name = *it;
				if (name.find(".header") != string::npos) {
					    if (!strncmp(name.c_str(), "log_", 4)) {
						        if (name.length() > maxLengthHeaderName) {
                                        maxLengthHeaderName = name.length();
                                }
                                _nHeaders++;
                        }
                }
        }
        maxLengthHeaderName++;  // Add null character onto length

        // Malloc room for header names
        _headers = (char **) malloc((_nHeaders) * sizeof(char *));
        i = 0;
		for (list<string>::iterator it = entries.begin(); it != entries.end(); it++) {
                name = *it;
                if (strstr(name.c_str(), ".header") != NULL) {
                        if (!strncmp(name.c_str(), "log_", 4)) {
                                _headers[i] = (char *) malloc
                                    (maxLengthHeaderName);
                                strcpy(_headers[i], name.c_str());
                                i++;
                        }
                }
        }

        return (0);
}

/** Returns number of Trick Log Data groups in RUN_ dir */
int LogGroup::getNumGroups()
{
        return (_nGroups);
}

/** Returns path to Binary data (RUN_ dir) */
char *LogGroup::getDataDir()
{
        return (_pathToBinaryData);
}

void LogGroup::_setEOF()
{
        _isEOF = 1 ;
}

int LogGroup::isEOF()
{
        int ii ;
        int nGroupsToCheck ;
        int eof ;

        if ( _isEOF ) {
                return( 1 ) ;
        }

        nGroupsToCheck = _nGroups ;

        eof = 1 ;
        for (ii = 0; ii < nGroupsToCheck ; ii++) {
                if ( ! log[ii]->isEOF() ) {
                        eof = 0 ;
                        break ;
                }
        }
        _isEOF = eof ;

        return( eof ) ;
}


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#ifndef WIN32
#include <unistd.h>
#endif

int LogGroup::_parseLogHeaders()
{

        int nVars;
        int lineNum;
        int varIdx;
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
        for (i = 0; i < _nHeaders; i++) {

                // Allocate headerName
                len = strlen(_pathToBinaryData) + strlen(_headers[i]) + 2;
                headerName = new char[len];

                // Full path of log header
                sprintf(headerName, "%s/%s", _pathToBinaryData, _headers[i]);

                // Open log header 
                if ((_fp = fopen(headerName, "r")) == NULL) {
                        fprintf(stderr,
                                "\nERROR:\nCouldn't open file \"%s\" \n",
                                headerName);
                        return (-1);
                }
                // Allocate strings, we know that buffers can't be 
                // longer than file!
                // Since headers are typically small, allocate them at ]
                // the header size 
                fseek(_fp, 0, SEEK_END);
                headerSize = ftell(_fp);
                ::rewind(_fp);
                line = new char[headerSize + 1];        // Not sure about +1 :)
                str1 = new char[headerSize + 1];
                str2 = new char[headerSize + 1];
                str3 = new char[headerSize + 1];
                str4 = new char[headerSize + 1];

                // Parse rest of file
                varIdx = 0;
                lineNum = 0;
                while (fgets(line, headerSize, _fp) != NULL) {

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
                                        currLogData = log[_nGroups];
                                        _nGroups++;
                                        varIdx = 0;

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
                                // Initialize Unit class
                                currVar->setUnit(str3);

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

                fclose(_fp);
                delete[]line;
                delete[]str1;
                delete[]str2;
                delete[]str3;
                delete[]str4;
                delete[]headerName;
        }

        return 1;
}

IterMultiRun::IterMultiRun( LogGroup* lg1, set<int>& logIndices1,
                            LogGroup* lg2, set<int>& logIndices2,
                            double frequency, 
                            double startTime, double stopTime ) {

        // Two iterators for iterating over both runs
        _iter1 = new IterPreserveTime(lg1, logIndices1,
                               frequency, startTime, stopTime);
        _iter2 = new IterPreserveTime(lg2, logIndices2,
                               frequency, startTime, stopTime);

        _startTime = startTime;
        _stopTime = stopTime;

        _logIndices1 = logIndices1;
        _logIndices2 = logIndices2;
}

IterMultiRun::~IterMultiRun()
{
        delete _iter1 ;
        delete _iter2 ;
}

void IterMultiRun::begin() 
{
        _iter1->begin();
        _iter2->begin();
        return ;
}

bool IterMultiRun::end()
{
        if ( _iter1->end() || _iter2->end() ) {
                return( 1 ) ;
        } else {
                return( 0 ) ;
        }
}

void IterMultiRun::operator++(int) {

        double minTime, t1, t2;

        // No more iteration to be done if at end
        if ( end() ) {
                return ;
        }

        // Get all values in all data log groups
        // Also get mininum time stamp from all log data files
        t1 = _iter1->getTime();
        t2 = _iter2->getTime();
        if ( t1 <= t2 ) {
                minTime = t1 ;
        } else {
                minTime = t2 ;
        }

        // Take a step through time (sounds like a song)
        if ( fabs(t1 - minTime) < 0.0000001 ) {
                (*_iter1)++ ;
        }
        if ( fabs(t2 - minTime) < 0.0000001 ) {
                (*_iter2)++ ;
        }

        // Now that a time step has occured 
        // Get the new mininum time stamp
        t1 = _iter1->getTime();
        t2 = _iter2->getTime();
        if ( t1 <= t2 ) {
                minTime = t1 ;
        } else {
                minTime = t2 ;
        }

        // Insure that time stamp remains same across runs
        if ( fabs(t1-t2) > 0.0000001 ) {
                (*this)++ ;
        }

        // If time < start time requested by user, 
        // continue marching through time until start time reached
        if (minTime < _startTime) {
                (*this)++ ;
        }
}

IterPreserveTime::IterPreserveTime( LogGroup* lg, set<int>& logIndices,
                                    double frequency, 
                                    double startTime, double stopTime ) 
{
        unsigned int ii ;

        _lg = lg ; 
        _startTime = startTime ;
        _stopTime = stopTime ;
        _logIndices = logIndices ;
        _frequency = frequency ;
        _frequencyMultiple = 1.0 ;

        // If there is an external program, make sure there is at least
        // one log group other than the external one in logIndices
        if ( logIndices.size() == 1 ) {
                _logIdx = logIndices.begin() ;
        }

        _eof = new bool[logIndices.size()] ;
        for (ii = 0; ii < logIndices.size() ; ii++) {
                _eof[ii] = 0 ;
        }

        // Allocate a time idx for log member
        _timeIdx = new int[logIndices.size()];
        for (ii = 0; ii < logIndices.size(); ii++ ) {
                // TODO: get real time stamp
                // It is safe for now to assume it's 
                // the first param logged
                _timeIdx[ii] = 0 ; 
        }

        _currTime = new double[logIndices.size()] ; 
}

IterPreserveTime::~IterPreserveTime()
{
}


void IterPreserveTime::begin() 
{
        double timeStamp;

        _lg->rewind();

        // Reset frequency
        if ( _frequency == 0.0 ) {
                _nextFrequencyCheck = DBL_MAX;
        } else { 
                if ( _startTime == 0.0 ) {
                        _nextFrequencyCheck = _startTime + _frequency ;
                } else {
                        _nextFrequencyCheck = _startTime ;
                }
        }
        _frequencyMultiple = 1.0 ;

        // Need to initialize by reading in all records
        for (_logIdx = _logIndices.begin() ; _logIdx != _logIndices.end() ;
                _logIdx++) {

                _lg->log[*_logIdx]->getNextRecord() ;
        }

        // If time < start time requested by user, 
        // continue marching through time until start time reached
        timeStamp = 0.0 ;
        while ( timeStamp < _startTime ) {
                (*this)++ ;
                timeStamp = getTime();
        }
}

bool IterPreserveTime::end()
{
        if ( _lg->_isEOF ) {
                return( 1 ) ;
        } else {
                return( 0 ) ;
        }
}

void IterPreserveTime::operator++(int) {

        // I did it this way to avoid recursion
        // The stack got out of hand with recursion
        while ( ! stepInTime() ) {
        }
}

// This returns minimum time stamp across all data logs
//
// If one log has logged more data in time,
// (i.e. one log has 100 seconds of data, another 300 seconds)
// by default getTime() will ignore the log that has expired
// all of its data.  Therefore, the data from the short log
// will not force the longer to be truncated.
//
// In the case of error plotting, you do not want any extra
// data points if the time stamps do not match
double IterPreserveTime::getTime( ) {

        double minTime = DBL_MAX; 
        int ii = 0 ; 

        for (_logIdx = _logIndices.begin() ; _logIdx != _logIndices.end() ; 
                _logIdx++) { 

                if ( ! _lg->log[*_logIdx]->isEOF() ) {
                        _currTime[ii] = *((double *) _lg->log[*_logIdx]->
                                                getVal(_timeIdx[ii])) ;
                } else {
                        _currTime[ii] = DBL_MAX ;
                }
        
                if (_currTime[ii] < minTime) {
                        minTime = _currTime[ii];
                }
                ii++ ;
        }

        return( minTime ) ;
}

// Advance through records in each data log synchronously 
//
// Example: If data log "A" is recorded at 1.0 second, 
// and "B" at 10 seconds.  Both will advance at time zero.
// Then at time=1.0, log "A" will have a record read.  "B" will not be read.
// At time=2.0 through time=9.0 the same will occur. A advances, B stays put. 
// At time=10.0, both A and B will be read from.  Both will advance.
// This keeps the data on the same time stamp on like intervals.
//
// If A has more records than B in time, by default, read all of A's
// data.  E.g. 'A' logged for 300 seconds,  while 'B' 100 seconds, by default
// walk through all of A's data
int IterPreserveTime::stepInTime() {


#define DPLOG_ABS(x) ((x) < 0.0 ? -(x) : (x))

#define NEXTFREQ_EQUALS_MINTIME \
        (DPLOG_ABS(_nextFrequencyCheck - minTime) < 0.000000001)

#define NEXTFREQ_LESSTHAN_MINTIME \
        ((_nextFrequencyCheck - minTime < 0.0) && ! NEXTFREQ_EQUALS_MINTIME)

#define NEXTFREQ_INCR \
        _frequencyMultiple++ ; \
        _nextFrequencyCheck = _frequencyMultiple*_frequency ;


        int ii ;
        double minTime ;
        int isEOF ;
        char* rec ;

        // Get all values in all data log groups
        // Also get mininum time stamp from all log data files
        minTime = getTime(); 

        // Take a step through time (sounds like a song)
        // Check for EOF along the way
        isEOF = 1 ;
        ii = 0 ;
        for (_logIdx = _logIndices.begin() ; _logIdx != _logIndices.end() ;
                _logIdx++) {
                if (_currTime[ii] == minTime ) {

                        rec = _lg->log[*_logIdx]->getNextRecord() ;
                        if (rec) {
                                isEOF = 0 ;
                        }
                }
                ii++ ;
        }
        if (isEOF) {
                _lg->_isEOF = 1 ;
                return(1) ;
        }

        // Now that a time step has occured 
        // Get the new mininum time stamp and associated
        // values from all log data files
        minTime = getTime(); 

        // Step depending on frequency
        if (minTime <= _stopTime) {

                // Handle Frequency (if specified)
                if ( _nextFrequencyCheck != DBL_MAX ) {

                        if ( NEXTFREQ_EQUALS_MINTIME ) { 

                                // Frequency synced with a data point
                                // Stops recursion 
                                NEXTFREQ_INCR ;

                        } else if ( minTime - _nextFrequencyCheck > 0 ) {

                                // Mintime exceeded nextFreqCheck
                                // Frequency must be higher than 
                                // data frequency

                                // Bump nextFrequencyCheck past minTime
                                while ( NEXTFREQ_LESSTHAN_MINTIME ) {

                                        NEXTFREQ_INCR ;
                                }

                                if ( NEXTFREQ_EQUALS_MINTIME ) {

                                       // We synced
                                       NEXTFREQ_INCR ;

                                } else {

                                        // No sync, keep iterating through data
                                        return(0); 
                                }
                        } else {
        
                                // Mintime is less than nextFreq check
                                // Need to iterate again
                                return(0) ;
                        } 
                }
        }

        if (minTime > _stopTime) {
                _lg->_isEOF = 1 ;
                return(1) ;
        }

        // Tells caller that time step did indeed occur
        return(1) ;
}
