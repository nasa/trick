
#ifndef DATASTREAMGROUP_HH
#define DATASTREAMGROUP_HH

#include <iostream>
#include <vector>
#include <map>
using namespace std;

#include "DataStream.hh"

class DataStreamGroup {

      public:

        DataStreamGroup() ;
        ~DataStreamGroup();

        void add(DataStream* d);
        void clear();

        void setStartTime(double time);
        void setStopTime(double time);
        void setFrequency(double freq);
        void setPreserveTimeOn();  // Default
        void setPreserveTimeOff();

        int matchTimeStamps();
        void setTimeMatchTolerance(const double tolerance);
        int getLastRead(DataStream *, double * time , double * value);

        void begin();
        bool end();
        void step();

        double getTime();

      private:

        struct LastRead {
                double time ;
                double value ;
        } ;

        vector < DataStream* >dataStreams_;
        vector < double > currTime_ ;
        map    < DataStream * , struct LastRead > lastRead_ ;

        bool preserveTime_ ;    // While iterating, step() insures all time
                                // stamps are same.  step() will skip points
                                // where time stamps don't match

        double frequency_ ;     // Time frequency to step through data
        double startTime_ ;     // Start time to begin iterating through data
        double stopTime_ ;      // Stop time to stop iterating through data
        double nextFrequencyCheck_ ;
        double frequencyMultiple_ ;
        double timeMatchTolerance_ ; // When comparing time stamps, if time
                                          // stamps are within this tolerance,
                                          // assume they are equivalent

        int stepInTime_();

        bool isEOF_ ;

} ;

#endif
