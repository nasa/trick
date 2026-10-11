/*************************************************************************
PURPOSE: (A variable server variable reference. Refactor of VariableReference
 based on the VariableServerVariable class previously in the webserver.)
**************************************************************************/
#ifndef VARIABLE_REFERENCE_HH
#define VARIABLE_REFERENCE_HH

#include <time.h>
#include <vector>

#include <iostream>
#include <trick/reference.h>

#define MAX_ARRAY_LENGTH 4096

union cv_converter ;
namespace Trick {
    class VariableReference {

    public:
        friend std::ostream& operator<< (std::ostream& s, const Trick::VariableReference& ref);

        VariableReference(std::string var_name);

        // Special constructor to deal with time
        VariableReference(std::string var_name, double* time);

        ~VariableReference();

        std::string getName() const;
        TRICK_TYPE getType() const;

        std::string getBaseUnits() const;
        int setRequestedUnits(std::string new_units);

        // Address resolution without buffer staging
        void* resolveAddress(bool validate_address = false);

        // Address and value accessors (Peeking)
        void* getAddress() const { return _address; }
        template <typename T>
        T* getAddress() const { return static_cast<T*>(_address); }

        template <typename T>
        T getValue() const {
            if (_address != nullptr) {
                return *static_cast<const T*>(_address);
            }
            return T{};
        }

        const void* getStageBuffer() const { return _stage_buffer; }
        template <typename T>
        T getStagedValue() const {
            if (_stage_buffer != nullptr) {
                return *static_cast<const T*>(_stage_buffer);
            }
            return T{};
        }

        const REF2* getRefInfo() const { return _var_info; }
        const ATTRIBUTES* getAttributes() const {
            return (_var_info != nullptr) ? _var_info->attr : nullptr;
        }

        // Variable modification (Poking) using ref_assignment
        int setVariable(double value, const char* units = nullptr);
        int setVariable(float value, const char* units = nullptr);
        int setVariable(long long value, const char* units = nullptr);
        int setVariable(unsigned long long value, const char* units = nullptr);
        int setVariable(long value, const char* units = nullptr);
        int setVariable(unsigned long value, const char* units = nullptr);
        int setVariable(int value, const char* units = nullptr);
        int setVariable(unsigned int value, const char* units = nullptr);
        int setVariable(short value, const char* units = nullptr);
        int setVariable(unsigned short value, const char* units = nullptr);
        int setVariable(char value, const char* units = nullptr);
        int setVariable(unsigned char value, const char* units = nullptr);
        int setVariable(bool value, const char* units = nullptr);
        int setVariable(const char* value, const char* units = nullptr);
        int setVariable(const std::string& value, const char* units = nullptr);
        int setVariable(void* value, const char* units = nullptr);

        // Direct write to memory without ref_assignment overhead
        template <typename T>
        int writeDirect(const T& value) {
            if (_address == nullptr) {
                resolveAddress(false);
            }
            if (_address != nullptr) {
                *static_cast<T*>(_address) = value;
                return 0;
            }
            return -1;
        }

        // These variables have 2 staging buffers that we can swap between to allow for different copy and write-out modes
        // stageValue copies data from the simulation into one of the buffers (stage_buffer)
        // prepareForWrite swaps the pointers for stage_buffer and write_buffer
        // This way we can have data ready for writing, but also be copying out from the sim at the same time
        // stageValue must be called first, and then prepare for write, and then writeValue* can be called.
        int stageValue(bool validate_address = false);
        int prepareForWrite();
        bool isStaged() const;
        bool isWriteReady() const;

        // Write out the value to the given outstream.
        // write_ready must be true
        int getSizeAscii() const;
        int getSizeBinary() const;
        int writeValueAscii( std::ostream& out ) const;
        int writeValueBinary( std::ostream& out , bool byteswap = false) const;
        int writeNameBinary( std::ostream& out, bool byteswap = false) const;
        int writeNameLengthBinary( std::ostream& out, bool byteswap = false) const;
        int writeSizeBinary( std::ostream& out, bool byteswap = false) const;
        int writeTypeBinary( std::ostream& out, bool byteswap = false) const;

        bool validate();
        void tagAsInvalid();

        // Helper method for byteswapping
        static void byteswap_var (char * out, char * in, const VariableReference& ref);

    private:
        VariableReference();
        void byteswap_var(char * out, char * in) const;
        int assign_vdata(V_DATA& v_data, const char* units);

        // Error refs
        static REF2* make_error_ref(std::string in_name);
        static REF2* make_do_not_resolve_ref(std::string in_name);

        static int _bad_ref_int;
        static int _do_not_resolve_bad_ref_int;

        REF2 * _var_info;  
        void * _address;                      // -- address of data copied to buffer
        int    _size;                         // -- size of data copied to buffer
        bool   _deref;                        // -- indicates whether variable is pointer that needs to be dereferenced
        cv_converter * _conversion_factor ;  // ** udunits conversion factor
        TRICK_TYPE _trick_type ;             // -- Trick type of this variable
        bool _used_stl_indexing;             // -- indicates if reference involved STL container indexing

        bool _staged;
        bool _write_ready;

        void *_stage_buffer;
        void *_write_buffer;  

        std::string _base_units;
        std::string _requested_units; 
        std::string _name;
    };

    std::ostream& operator<< (std::ostream& s, const Trick::VariableReference& ref);
}
#endif
