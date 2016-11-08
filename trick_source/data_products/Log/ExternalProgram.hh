#ifndef EXTERNAL_PROGRAM_HH
#define EXTERNAL_PROGRAM_HH

#include "DataStream.hh"
#include "DataStreamGroup.hh"

class ExternalProgram : public DataStream {

   // This section satisfies inheritance
   public:
        int get(double* timeStamp, double* paramValue);
        int peek(double* timeStamp, double* paramValue);
        int getValueAtTime(double timeStamp, double* paramValue) ;

        void begin();
        int end();
        int step();

   // This section is unique to the external program
   public:
        ExternalProgram( const char* sharedLibName,
                         int numInputs, DataStream** istreams,
                         int numOutputs, int outputParamIdx ) ;
        ~ExternalProgram();

   private:
        char* run_ ;
        char* param_ ;
        double timeStamp_ ;

        // This will be a group of input streams that will be iterated
        // over.  Input will be fed into the external program.
        DataStreamGroup dsg_ ;

        // Input streams which are created from program inputs
        vector < DataStream* >istreams_;

        char *error;

        char *sharedLibName_;
        void *progHandle_;
        double *progInput_;     // Input into external program
        double *progOutput_;    // Output from external program
        int nInputs_;           // Set at instantiation
        int nOutputs_;          // Set at instantiation
        double *input_;         // Sized to nInputs
        double *output_;        // Sized to nOutputs

        // Since this is a "stream", only one external program output
        // will be accessed.  The outputIdx_ is the index of the parameter
        // to access.
        int outputIdx_ ;

        // This is the program that will be dynamically linked.
        // The program is normally created by a developer.
        // The program takes a set of inputs and generates
        // a set of outputs.
        int (*external_program) (double *in, int numIn,
                                 double *out, int numOut);

        // Run the external shared library program and load the
        // output value and time stamp associated with outputIdx
        int getOutputValue( double* timeStamp, double* outputValue ) ;
} ;

#endif
