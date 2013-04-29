/*******************************************************************************
                                                                              
 Trick Simulation Environment Software                                       
                                                                              
 Copyright (c) 1996,1997 LinCom Corporation, Houston, TX                      
 All rights reserved.                                                         
                                                                              
 Copyrighted by LinCom Corporation and proprietary to it. Any unauthorized    
 use of Trick Software including source code, object code or executables is   
 strictly prohibited and LinCom assumes no liability for such actions or      
 results thereof.                                                             
                                                                              
 Trick Software has been developed under NASA Government Contracts and        
 access to it may be granted for Government work by the following contact:    
                                                                              
 Contact: Charles Gott, Branch Chief                                          
          Simulation and Graphics Branch                                      
          Automation, Robotics, & Simulation Division                         
          NASA, Johnson Space Center, Houston, TX                             
                                                                             
*******************************************************************************/
/** \ingroup libdp
    Class to access Trick logged data 
    @author Keith Vetter
    @version May 2002
*/

#ifndef _LOG_H_
#define _LOG_H_

#include <stdio.h>

#ifndef WIN32
#include <dlfcn.h>              // Dynamically linked external programs
#endif

#include "var.hh"
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
        Unit *getUnit(const char *param);     // Get unit for associated param
        char *getBinaryFileName();

        int setBinaryFileName(char *fileName);
        int setScaleFactor(int paramIdx, double factor);
        int setBias(int paramIdx, double bias);
        int setMax(int paramIdx, double max);
        int setMin(int paramIdx, double min);
        int setUnit(int paramIdx, Unit * u);

        /// Get value of a parameter at a given time stamp
        int getValueAtTime( int timeIdx, double time,
                            int valIdx,  double* value );

        int getNumVars();       
        void setNumVars(int n); 

        /** Variable is variable as seen in log file 
         * E.g. Variable x[2].y[0-2]
         */
         vector < Var * >vars;
         Var* getVar(const unsigned int index) { return vars[index]; }

        /** Endian that data was logged in.
         * 0: little endian 1: big endian 
         */
        int dataByteOrder;

        /** Byte order of machine running program.
         * 0: little endian 1: big endian
         */
        int machineByteOrder;

        // Stats (Min/Max...)
        void getMinMaxStats(int paramIdx,
                            double *timeMin, double *min,
                            double *timeMax, double *max);

      private:

        FILE * _fp;             ///< File pointer to current binary file
        char *_binFilePath;     ///< Path to binary file
        char *_binFileName;     ///< Root binary file name
        char *_currRecord;      ///< Stores current record 
        int _sizeRecord;        ///< Holds current size of record
        int _convertToDouble;   ///< 1: All parms are converted to double

        void *_advanceCurrValPtr();      ///< Advance val ptr in curr record

        int _openCurrBinaryFile();       ///< Open log*000, 001, 002 bin files
        int _countNumBinaryFiles(char *pathToBinaryData);

        /*
         * Then following is a series of variables that define the 
         * attributes of each value in a record
         * The arrays are sized in LogData::InitValSizeAndType()
         */

        union {                 ///< Current value from record
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
        } _currVal;

        int _nVals;             ///< Number of values in a record
        int _currValIdx;        ///< Idx of current value in record 
        unsigned char *_currValPtr;     ///< Curr read loc in curr rec
        int *_varVal;           ///< Variable index for each val 
        int *_sizeVal;          ///< Byte size each value in record
        double *_scaleVal;      ///< Scale factor for variables 
        double *_biasVal;       ///< Bias for variables
        double *_unitVal;       ///< Unit scale factor variables
        double *_maxVal;        ///< Max value for variables
        double *_minVal;        ///< Max value for variables
         Var::enumType * _typeVal;      ///< Type for each value in record


        int _nVars;
        int _currVarIndex;      ///< Idx curr var in a list records
        int _bytesRead;         ///< Bytes read reading a record
        int _endOfRecord;       ///< Read all params from curr rec
        int _isEOF;            ///< End of file? 0|1
        int _currBinExtension;  ///< Binary file extension (e.g. 000)
        int _nBinFiles;         ///< Num binary files in this log
        int _open;              ///< Log open for reading?

        /**
         * For grabbing stats out of the entire data file
         */
        typedef struct {
                double tval;    ///< time stamp
                double val;     ///< value
        } Stats;

        void enableStats();
        int _isEnabledStats;

        // Holds stats foreach param
        Stats* _statsMin;
        Stats* _statsMax;
};


/**
 * Highlevel "group" access to individual Trick logged binary data
 * LogGroup is built on top of the LogData class
 */
class LogGroup {

      friend class IterPreserveTime ;

      public:

        LogGroup();             ///< Not to be used
        ~LogGroup();
        LogGroup(char *pathToBinaryData); ///< The real constructor

        // For iterating over recs
        void begin() ;
        bool end() ;

        int isEOF();            ///< Finished reading data?
        int getNumRecords();    ///< Max num of records across data logs
        char *getNextRecord();
        int getNumGroups();
        int rewind();           ///< Start reading records all over
         vector < LogData * >log;       ///< Log Data Classes
        int convertToDoubleYes();       ///< Convert all data to doubles

        /// Using x-y pair find data group and index for x & y
        int getParamIdx(const char *xVarName,
                        int *xIdx, int *xLogDataIdx,
                        const char *yVarName, int *yIdx, int *yLogDataIdx);

        /// Find absolute min/max value for a parameter within a log group
        void getMinMaxStats(const char *paramName,
                            double *timeMin, double *min,
                            double *timeMax, double *max);

        /// Get value of a parameter at a given time stamp
        int getValueAtTime( const char *paramName, 
                            double time, double* value);

        int setScaleFactor(int logIdx, int paramIdx, double factor);
        int setBias(int logIdx, int paramIdx, double bias);
        int setUnit(int logIdx, int paramIdx, Unit * u);
        int setUnit(const char *paramName, Unit * u);
        int setMax(int logIdx, int paramIdx, double max);
        int setMin(int logIdx, int paramIdx, double min);

        char *getDataDir();

      private:
         FILE * _fp;
        int _parseLogHeaders();

        int _getHeaders();      ///< Get list of headers in binary data area
        char **_headers;        ///< log header names
        int _nHeaders;

        char *_pathToBinaryData;        ///< Where headers & Data reside

        int _nGroups;

        int _isEOF;

        void _setEOF();
};

/**
 * Iterate over each log member keeping preserving the same
 * time stamp over the entire group
 */
class IterPreserveTime {

      public:
        
        IterPreserveTime( LogGroup* lg, 
                                            set<int>& logIndices,
                                            double freq, 
                                            double startTime, double stoptime);
        ~IterPreserveTime();

        void begin();
        bool end();
        void operator++(int) ;

        double getTime() ;
        int   stepInTime() ;

      private:

        LogGroup* _lg;

        /** Set of log indices to iterator over.
         * The iterator needs to know the log members that it is going
         * to need to iterate over.  We could just do iterate over -all- 
         * each time a getNextRecord() occurs.  But this can waste a 
         * lot of time when there is a mix of groups 
         * with hilow frequencies. 
         * (e.g Maybe log_frame is logged at 0.0001 and log_ball is logged
         * at 0.1 --- you don't want to have to spin through frame data
         * when you aren't even viewing it.)
         */
        set<int> _logIndices;
        set<int>::const_iterator _logIdx;

        int* _timeIdx;         ///< Time idx for each log member
        bool* _eof;            ///< EOF status for each log member 
        double* _currTime;     ///< Holds curr time for each log member
        double _frequency;     ///< Time frequency to step through data
        double _startTime;     ///< Start time to begin iterating through data
        double _stopTime;      ///< Stop time to stop iterating through data 

        double _nextFrequencyCheck;
        double _frequencyMultiple; ///< Used to calc next freq check
};

/**
 * Iterate over runs (right now only 2 supported) 
 * keeping time stamp same while stepping 
 */
class IterMultiRun {

      public:
        

        IterMultiRun( LogGroup* lg1, set<int>& logIndices1,
                                    LogGroup* lg2, set<int>& logIndices2,
                                    double freq, 
                                    double startTime, double stoptime);
        IterMultiRun();
        ~IterMultiRun();


        void begin();
        bool end();
        void operator++(int) ;

      private:

        LogGroup* _lg1;
        LogGroup* _lg2;

        /** This iterator will use IterPreserveTime iterators
         * for cycling over the runs
         */
        set<int> _logIndices1;
        set<int> _logIndices2;
        IterPreserveTime* _iter1;
        IterPreserveTime* _iter2;

        double _startTime;     ///< Start time to begin iterating through data
        double _stopTime;      ///< Stop time to stop iterating through data 
};

class LogMultiGroup {

      friend class IterOverLogGroups ;

      public:

        LogMultiGroup();             ///< Not to be used
        LogMultiGroup(vector <string>& runDirs) ;
        ~LogMultiGroup();            ///< The real constructor

        /// Convert all data to doubles
        int convertToDoubleYes();

        /// Set factors for scaling, biasing... 
        int setScaleFactor(const char* paramName, double factor);
        int setBias(const char* paramName, double bias);
        int setUnit(const char* paramName, Unit * u);
        int setMax(const char* paramName, double max);
        int setMin(const char* paramName, double min);

      private:

        /// Contains accessible log groups
         vector < LogGroup* >_logGroups;


        int _open(char *);     ///< Open log groups for reading given bin file
        int _nGroups;
        int _isEOF;
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

        IterOverLogGroups();
        ~IterOverLogGroups();

        /// Get a value by run name and parameter name
        double getVal(string runName, string paramName);

        /// For loop functions
        void begin() ;
        bool end() ;
        void operator++(int) ;

      private:

        LogMultiGroup* _lmg;

        /** Set of log indices to iterator over.
         * The iterator needs to know the individual log groups that it is going
         * to need to iterate over.  I could just do iterate over -all- 
         * each time a getNextRecord() occurs.  But this can waste a 
         * lot of time when you are only wanting to iterate over two runs
         * for an error plot or something similar. 
         */
        vector< LogGroup* > _iterLogGroups;

        // For each parameter (e.g. ball.state.out.pos) ParamIdx
        // has a map to a list of data log/var idx pairs
        // that defines where it is in its log group
        typedef struct {
                int logIdx ;
                int varIdx ;
        } ParamIdx ;
        map<string, ParamIdx*> _paramMap; 

        /** For the entire multi group, each run
         * has a param->idx mapping
         * So we have a list of hashes :-) @%
         *
         * The way I use this is like so:
         * logIdx = runs_[1]["ball.state.out.pos"]->logIdx ;
         * This simply means that in run 1's data set, parameter
         * with name "ball.state.out.pos" resides in logIdx.
        */
        vector< map< string, ParamIdx* > > _runs; 

        /// Iterator for array of LogGroups in LogMultiGroup
        vector< LogGroup* >::const_iterator _lgIterator;

        /// Each log group requires an iterator
        vector < IterPreserveTime* > _listLogGroupIterators ;

        // Map run dir names to log group indices and
        // log groups
        map < string, int > _runDirName2runIdx;
        map < string, LogGroup* > _runDirName2lg;

        int* _timeIdx;         ///< Time idx for each log member
        bool* _eof;            ///< EOF status for each log member 
        double* _currTime;     ///< Holds curr time for each log member
        double _frequency;     ///< Time frequency to step through data
        double _startTime;     ///< Start time to begin iterating through data
        double _stopTime;      ///< Stop time to stop iterating through data 

        double _nextFrequencyCheck;
};

#endif
