
/**
    Class to access Trick logged data
    @author Keith Vetter
    @version May 2002
*/

#ifndef _LOG_H_
#define _LOG_H_

#include <stdio.h>

#ifdef __APPLE__
#include "darwin_dl.h"
#else
#include <dlfcn.h>              // Dynamically linked external programs
#endif

#include "Var/var.h"
#include "trick_byteswap.h"

#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <string>
using namespace std;


/**
 * Low level access to individual log binary data files
 */
class LogData {

      friend class IterPreserveTime ;

      public:

        LogData();
        ~LogData();
        int getSizeRecord();
        int getNumRecords();
        int calcNumValRecord();
        int getNumValRecord();
        int open(char *pathToBinaryData);
        int close();
        int rewind();
        int isEOF();
        char *getNextRecord();
        void *getNextVal();
        void *getVal(int);
        int convertToDoubleYes();
        int convertToDoubleNo();
        int printCurrRecord();
        int getParamIdx(const char *param);
         Var::enumType getType(int vIdx);
        int getSize(int vIdx);          // TODO: getValByteSize
        std::string getUnit(char *param);     // Get unit for associated param
        char *getBinaryFileName();

        int setBinaryFileName(char *fileName);
        int setScaleFactor(int paramIdx, double factor);
        int setBias(int paramIdx, double bias);
        int setMax(int paramIdx, double max);
        int setMin(int paramIdx, double min);
        int setUnit(int paramIdx, std::string u);

        // Get value of a parameter at a given time stamp
        int getValueAtTime( int timeIdx, double time,
                            int valIdx,  double* value );

        int getNumVars();
        void setNumVars(int n);

        /* Variable is variable as seen in log file
         * E.g. Variable x[2].y[0-2]
         */
         vector < Var * >vars;

        /* Endian that data was logged in
         * 0: little endian 1: big endian
         */
        int dataByteOrder;

        /* Byte order of machine running program
         * 0: little endian 1: big endian
         */
        int machineByteOrder;

        // Stats (Min/Max...)
        void getMinMaxStats(int paramIdx,
                            double *timeMin, double *min,
                            double *timeMax, double *max);

      private:

        FILE * fp_;             // File pointer to current binary file
        char *binFilePath_;     // Path to binary file
        char *binFileName_;     // Root binary file name
        char *currRecord_;      // Stores current record
        int sizeRecord_;        // Holds current size of record
        int convertToDouble_;   // 1: All parms converted to double

        void *advanceCurrValPtr();      // Advance val ptr in curr record

        int openCurrBinaryFile();       // Open log*000, 001, 002 bin files
        int countNumBinaryFiles(char *pathToBinaryData);

        /*
         * Then following is a series of variables that define the
         * attributes of each value in a record
         * The arrays are sized in LogData::InitValSizeAndType()
         */

        union {                 // Current value from record
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
        } currVal_;

        int nVals_;             // Number of values in a record
        int currValIdx_;        // Idx of current value in record
        unsigned char *currValPtr_;     // Curr read loc in curr rec
        int *varVal_;           // Variable index for each val
        int *sizeVal_;          // Byte size each value in record
        double *scaleVal_;      // Scale factor for variables
        double *biasVal_;       // Bias for variables
        double *unitVal_;       // Unit scale factor variables
        double *maxVal_;        // Max value for variables
        double *minVal_;        // Max value for variables
         Var::enumType * typeVal_;      // Type for each value in record


        int nVars_;
        int currVarIndex_;      // Idx curr var in a list records
        int bytesRead_;         // Bytes read reading a record
        int endOfRecord_;       // Read all params from curr rec
        int isEOF_ ;            // End of file? 0|1
        int currBinExtension_;  // Binary file extension (e.g. 000)
        int nBinFiles_;         // Num binary files in this log
        int open_;              // Log open for reading?

        /*
         * For grabbing stats out of the entire data file
         */

        typedef struct {
                double tval;    // time stamp
                double val;     // value
        } Stats;

        void enableStats();
        int isEnabledStats_;

        // Holds stats foreach param
        Stats* statsMin_;
        Stats* statsMax_;
};


/**
 * Highlevel "group" access to individual Trick logged binary data
 * LogGroup is built on top of the LogData class
 */
class LogGroup {

      friend class IterPreserveTime ;

      public:

        LogGroup();             // Not to be used
        ~LogGroup();
        LogGroup(char *pathToBinaryData);

        // For iterating over recs
        void begin() ;
        bool end() ;

        int isEOF();            // Finished reading data?
        int getNumRecords();    // Max num of records across data logs
        char *getNextRecord();
        int getNumGroups();
        int rewind();           // Start reading records all over
         vector < LogData * >log;       // Log Data Classes
        int convertToDoubleYes();       // Convert all data to doubles

        // Using x-y pair find data group and index for x & y
        int getParamIdx(const char *xVarName,
                        int *xIdx, int *xLogDataIdx,
                        const char *yVarName, int *yIdx, int *yLogDataIdx);

        // Find absolute min/max value for a parameter within a log group
        void getMinMaxStats(const char *paramName,
                            double *timeMin, double *min,
                            double *timeMax, double *max);

        // Get value of a parameter at a given time stamp
        int getValueAtTime( const char *paramName,
                            double time, double* value);

        int setScaleFactor(int logIdx, int paramIdx, double factor);
        int setBias(int logIdx, int paramIdx, double bias);
        int setUnit(int logIdx, int paramIdx, std::string u);
        int setUnit(const char *paramName, std::string u);
        int setMax(int logIdx, int paramIdx, double max);
        int setMin(int logIdx, int paramIdx, double min);

        char *getDataDir();

      private:
         FILE * fp_;
        int parseLogHeaders();
        bool is04BinaryHeader_(const char*);
        int open(char *);          // Open log groups for reading given bin file

        int getHeaders();          // Get list of headers in binary data area
        char **headers_;           // log header names
        int nHeaders_;

        char *pathToBinaryData_;        // Where headers & Data reside

        int nGroups_;

        int isEOF_;

        void setEOF_();
};

/**
 * Iterate over each log member keeping preserving the same
 * time stamp over the entire group
 */
class IterPreserveTime {

      public:

        IterPreserveTime( LogGroup* lg, set<int>& logIndices,
                          double freq, double startTime, double stoptime);
        ~IterPreserveTime();

        void begin();
        bool end();
        void operator++(int) ;

        double getTime() ;
        int   stepInTime() ;

      private:

        LogGroup* lg_ ;

        // The iterator needs to know the log members that it is going
        // to need to iterate over.  I could just do iterate over -all-
        // each time a getNextRecord() occurs.  But this can waste a
        // lot of time when there is a mix of groups
        // with high/low frequencies.
        // (e.g Maybe log_frame is logged at 0.0001 and log_ball is logged
        // at 0.1 --- you don't want to have to spin through frame data
        // when you aren't even viewing it.)
        set<int> logIndices_ ;
        set<int>::const_iterator logIdx_ ;

        int* timeIdx_ ;         // Time idx for each log member
        bool* eof_ ;            // EOF status for each log member
        double* currTime_ ;     // Holds curr time for each log member
        double frequency_ ;     // Time frequency to step through data
        double startTime_ ;     // Start time to begin iterating through data
        double stopTime_ ;      // Stop time to stop iterating through data

        double nextFrequencyCheck_ ;
        double frequencyMultiple_ ; // Used to calc next freq check
};

/**
 * Iterate over runs (right now only 2 supported)
 * keeping time stamp same while stepping
 */
class IterMultiRun {

      public:

        IterMultiRun( LogGroup* lg1, set<int>& logIndices1,
                      LogGroup* lg2, set<int>& logIndices2,
                      double freq, double startTime, double stoptime);
        ~IterMultiRun();

        void begin();
        bool end();
        void operator++(int) ;

      private:

        LogGroup* lg1_ ;
        LogGroup* lg2_ ;

        // This iterator will use IterPreserveTime iterators
        // for cycling over the runs
        set<int> logIndices1_ ;
        set<int> logIndices2_ ;
        IterPreserveTime* iter1_ ;
        IterPreserveTime* iter2_ ;

        double startTime_ ;     // Start time to begin iterating through data
        double stopTime_ ;      // Stop time to stop iterating through data
};

class LogMultiGroup {

      friend class IterOverLogGroups ;

      public:

        LogMultiGroup();             // Not to be used
        LogMultiGroup(vector <string>& runDirs) ;
        ~LogMultiGroup();

        // Convert all data to doubles
        int convertToDoubleYes();

        // Set factors for scaling, biasing...
        int setScaleFactor(const char* paramName, double factor);
        int setBias(const char* paramName, double bias);
        int setUnit(const char* paramName, std::string u);
        int setMax(const char* paramName, double max);
        int setMin(const char* paramName, double min);

      private:

        // This contains accessible log groups
         vector < LogGroup* >logGroups_;


        int open_(char *);       // Open log groups for reading given bin file
        int nGroups_;
        int isEOF_;
} ;

/**
 * Iterate over each log group keeping preserving the same
 * time stamp over the entire set of groups
 */
class IterOverLogGroups {

      public:

        IterOverLogGroups( LogMultiGroup* lmg,
                                            vector<string>& runDirs,
                                            vector<string>& paramNames );

        ~IterOverLogGroups();

        // Get a value by run name and parameter name
        double getVal(string runName, string paramName);

        // For loop functions
        void begin() ;
        bool end() ;
        void operator++(int) ;

      private:

        LogMultiGroup* lmg_ ;

        // The iterator needs to know the individual log groups that it is going
        // to need to iterate over.  I could just do iterate over -all-
        // each time a getNextRecord() occurs.  But this can waste a
        // lot of time when you are only wanting to iterate over two runs
        // for an error plot or something similar.
        vector< LogGroup* > iterLogGroups_ ;

        // For each parameter (e.g. ball.state.out.pos)
        // has a map to a list of data log/var idx pairs
        // that defines where it is in its log group
        typedef struct {
                int logIdx ;
                int varIdx ;
        } ParamIdx ;
        map<string, ParamIdx*> paramMap_ ;

        // For the entire multi group, each run
        // has a param->idx mapping
        // So we have a list of hashes :-) @%
        //
        // The way I use this is like so:
        // logIdx = runs_[1]["ball.state.out.pos"]->logIdx ;
        // This simply means that in run 1's data set, parameter
        // with name "ball.state.out.pos" resides in logIdx.
        vector< map< string, ParamIdx* > > runs_ ;

        // Iterator for array of LogGroups in LogMultiGroup
        vector< LogGroup* >::const_iterator lgIterator_;

        // Each log group requires an iterator
        vector < IterPreserveTime* > listLogGroupIterators_  ;

        // Map run dir names to log group indices and
        // log groups
        map < string, int > runDirName2runIdx_ ;
        map < string, LogGroup* > runDirName2lg_ ;

        int* timeIdx_ ;         // Time idx for each log member
        bool* eof_ ;            // EOF status for each log member
        double* currTime_ ;     // Holds curr time for each log member
        double frequency_ ;     // Time frequency to step through data
        double startTime_ ;     // Start time to begin iterating through data
        double stopTime_ ;      // Stop time to stop iterating through data

        double nextFrequencyCheck_ ;
};

#endif
