#include <stdlib.h>
#include <iostream>
#include <udunits2.h>
#include <math.h> // for fpclassify
#include <iomanip> // for setprecision
#include <string.h>
#include <sstream>

#include "trick/VariableReference.hh"
#include "trick/memorymanager_c_intf.h"
#include "trick/wcs_ext.h"
#include "trick/map_trick_units_to_udunits.hh"
#include "trick/message_proto.h"
#include "trick/message_type.h"
#include "trick/UdUnits.hh"
#include "trick/bitfield_proto.h"
#include "trick/trick_byteswap.h"
// #include "trick/tc_proto.h"


// Static variables to be addresses that are known to be the error ref address
int Trick::VariableReference::bad_ref_int = 0 ;
int Trick::VariableReference::do_not_resolve_bad_ref_int = 0 ;

REF2* Trick::VariableReference::make_error_ref(std::string in_name) {
    REF2* new_ref;
    new_ref = (REF2*)calloc(1, sizeof(REF2));
    new_ref->reference = strdup(in_name.c_str()) ;
    new_ref->units = NULL ;
    new_ref->address = (char *)&bad_ref_int ;
    new_ref->attr = (ATTRIBUTES*)calloc(1, sizeof(ATTRIBUTES)) ;
    new_ref->attr->type = TRICK_NUMBER_OF_TYPES ;
    new_ref->attr->units = (char *)"--" ;
    new_ref->attr->size = sizeof(int) ;
    return new_ref;
}

REF2* Trick::VariableReference::make_do_not_resolve_ref(std::string in_name) {
    REF2* new_ref;
    new_ref = (REF2*)calloc(1, sizeof(REF2));
    new_ref->reference = strdup(in_name.c_str()) ;
    new_ref->units = NULL ;
    new_ref->address = (char *)&do_not_resolve_bad_ref_int ;
    new_ref->attr = (ATTRIBUTES*)calloc(1, sizeof(ATTRIBUTES)) ;
    new_ref->attr->type = TRICK_NUMBER_OF_TYPES ;
    new_ref->attr->units = (char *)"--" ;
    new_ref->attr->size = sizeof(int) ;
    return new_ref;
}

// Helper function to deal with time variable
REF2* make_time_ref(double * time) {
    REF2* new_ref;
    new_ref = (REF2*)calloc(1, sizeof(REF2));
    new_ref->reference = strdup("time") ;
    new_ref->units = strdup("s") ;
    new_ref->address = (char *)time ;
    new_ref->attr = (ATTRIBUTES*)calloc(1, sizeof(ATTRIBUTES)) ;
    new_ref->attr->type = TRICK_DOUBLE ;
    new_ref->attr->units = strdup("s") ;
    new_ref->attr->size = sizeof(double) ;
    return new_ref;
}

Trick::VariableReference::VariableReference(std::string var_name, double* time) : staged(false), write_ready(false) {
    if (var_name != "time") {
        ASSERT(0);
    }

    var_info = make_time_ref(time);

    // Set up member variables
    address = var_info->address;
    size = var_info->attr->size ;
    deref = false;

    // Deal with weirdness around string vs wstring
    trick_type = var_info->attr->type ;

    // Allocate stage and write buffers
    stage_buffer = calloc(size, 1) ;
    write_buffer = calloc(size, 1) ;

    conversion_factor = cv_get_trivial();
}

Trick::VariableReference::VariableReference(std::string var_name) : staged(false), write_ready(false) {

    if (var_name == "time") {
        ASSERT(0);
    } else {
        // get variable attributes from memory manager
        var_info = ref_attributes(var_name.c_str());
    }

    // Handle error cases
    if ( var_info == NULL ) {
        // TODO: ERROR LOGGER sendErrorMessage("Variable Server could not find variable %s.\n", var_name);
        // PRINTF IS NOT AN ERROR LOGGER @me
        printf("Variable Server could not find variable %s.\n", var_name.c_str());
        var_info = make_error_ref(var_name);
    } else if ( var_info->attr ) {
        if ( var_info->attr->type == TRICK_STRUCTURED ) {
            // sendErrorMessage("Variable Server: var_add cant add \"%s\" because its a composite variable.\n", var_name);
            printf("Variable Server: var_add cant add \"%s\" because its a composite variable.\n", var_name.c_str());

            free(var_info);
            var_info = make_do_not_resolve_ref(var_name);

        } else if ( var_info->attr->type == TRICK_STL ) {
            // sendErrorMessage("Variable Server: var_add cant add \"%s\" because its an STL variable.\n", var_name);
            printf("Variable Server: var_add cant add \"%s\" because its an STL variable.\n", var_name.c_str());

            free(var_info);
            var_info = make_do_not_resolve_ref(var_name);
        }
    } else {
        // sendErrorMessage("Variable Server: BAD MOJO - Missing ATTRIBUTES.");
        printf("Variable Server: BAD MOJO - Missing ATTRIBUTES.");

        free(var_info);
        var_info = make_error_ref(var_name);
    }

    // Set up member variables
    var_info->units = NULL;
    address = var_info->address;
    size = var_info->attr->size ;
    deref = false;

    // Deal with weirdness around string vs wstring
    trick_type = var_info->attr->type ;

    if ( var_info->num_index == var_info->attr->num_index ) {
        // single value - nothing else necessary
    } else if ( var_info->attr->index[var_info->attr->num_index - 1].size != 0 ) {
        // Constrained array
        for ( int i = var_info->attr->num_index-1;  i > var_info->num_index-1 ; i-- ) {
            size *= var_info->attr->index[i].size ;
        }
    } else {
        // Unconstrained array
        if ((var_info->attr->num_index - var_info->num_index) > 1 ) {
            // TODO: ERROR LOGGER
            printf("Variable Server Error: var_add(%s) requests more than one dimension of dynamic array.\n", var_info->reference);
            printf("Data is not contiguous so returned values are unpredictable.\n") ;
        }
        if ( var_info->attr->type == TRICK_CHARACTER ) {
            trick_type = TRICK_STRING ;
            deref = true;
        } else if ( var_info->attr->type == TRICK_WCHAR ) {
            trick_type = TRICK_WSTRING ;
            deref = true;
        } else {
            deref = true ;
            size *= get_size((char*)address) ;
        }
    }
    // handle strings: set a max buffer size, the copy size may vary so will be set in copy_sim_data
    if (( trick_type == TRICK_STRING ) || ( trick_type == TRICK_WSTRING )) {
        size = MAX_ARRAY_LENGTH ;
    }

    // Allocate stage and write buffers
    stage_buffer = calloc(size, 1) ;
    write_buffer = calloc(size, 1) ;

    conversion_factor = cv_get_trivial();

    // Done!
}

Trick::VariableReference::~VariableReference() {
    if (var_info != NULL) {
        free( var_info );
        var_info = NULL;
    }
    if (stage_buffer != NULL) {
        free (stage_buffer);
        stage_buffer = NULL;
    }
    if (write_buffer != NULL) {
        free (write_buffer);
        write_buffer = NULL;
    }
    if (conversion_factor != NULL) {
        cv_free(conversion_factor);
    }
}

const char* Trick::VariableReference::getName() const {
    return var_info->reference;
}

int Trick::VariableReference::getSizeBinary() const {
    return size;
}

TRICK_TYPE Trick::VariableReference::getType() const {
    return trick_type;
}

const char* Trick::VariableReference::getBaseUnits() const {
    return var_info->attr->units;
}

int Trick::VariableReference::setRequestedUnits(std::string units_name) {
    // Some error logging lambdas - these should probably go somewhere else
    // But I do kinda like them
    auto publish = [](MESSAGE_TYPE type, const std::string& message) {
        std::ostringstream oss;
        oss << "Variable Server: " << message << std::endl;
        message_publish(type, oss.str().c_str());
    };

    auto publishError = [&](const std::string& units) {
        std::ostringstream oss;
        oss << "units error for [" << getName() << "] [" << units << "]";
        publish(MSG_ERROR, oss.str());
    };

    // If the units_name parameter is "xx", set it to the current units.
    if (!units_name.compare("xx")) {
        units_name = getBaseUnits();
    }

    // if unitless ('--') then do not convert to udunits 
    if(units_name.compare("--")) {
        // Check to see if this is an old style Trick unit that needs to be converted to new udunits
        std::string new_units = map_trick_units_to_udunits(units_name) ;
        // Warn if a conversion has taken place
        if ( units_name.compare(new_units) ) {
            // TODO: MAKE BETTER SYSTEM FOR ERROR LOGGING
            std::ostringstream oss;
            oss << "[" << getName() << "] old-style units converted from ["
                << units_name << "] to [" << new_units << "]";
            publish(MSG_WARNING, oss.str());
        }

        // Interpret base unit
        ut_unit * from = ut_parse(Trick::UdUnits::get_u_system(), getBaseUnits(), UT_ASCII) ;
        if ( !from ) {
            std::cout << "Error in interpreting base units" << std::endl;
            publishError(getBaseUnits());
            ut_free(from) ;
            return -1 ;
        }

        // Interpret requested unit
        ut_unit * to = ut_parse(Trick::UdUnits::get_u_system(), new_units.c_str(), UT_ASCII) ;
        if ( !to ) {
            std::cout << "Error in interpreting requested units" << std::endl;
            publishError(new_units);
            ut_free(from) ;
            ut_free(to) ;
            return -1 ;
        }

        // Create a converter from the base to the requested
        auto new_conversion_factor = ut_get_converter(from, to) ;
        ut_free(from) ;
        ut_free(to) ;
        if ( !new_conversion_factor ) {
            std::ostringstream oss;
            oss << "[" << getName() << "] cannot convert units from [" << getBaseUnits()
                << "] to [" << new_units << "]";
            publish(MSG_ERROR, oss.str());
            return -1 ;
        } else {
            conversion_factor = new_conversion_factor;
        }

        // Don't memory leak the old units!
        if (var_info->units != NULL)  {
            free(var_info->units);
        }
    
        // Set the requested units. This will cause the unit string to be printed in write_value_ascii
        var_info->units = strdup(new_units.c_str());;
    }
    return 0;
}

int Trick::VariableReference::stageValue(bool validate_address) {
    write_ready = false;

    // Copy <size> bytes from <address> to staging_point.

    // Try to recreate connection if it has been broken
    if (var_info->address == &bad_ref_int) {
        REF2 *new_ref = ref_attributes(var_info->reference);
        if (new_ref != NULL) {
            var_info = new_ref;
            address = var_info->address;
        }
    }

    // if there's a pointer somewhere in the address path, follow it in case pointer changed
    if ( var_info->pointer_present == 1 ) {
        address = follow_address_path(var_info) ;
        if (address == NULL) {
            tagAsInvalid();
        } else if ( validate_address ) {
            validate();
        } else {
            var_info->address = address ;
        }
    }

    // if this variable is a string we need to get the raw character string out of it.
    if (( trick_type == TRICK_STRING ) && !deref) {
        std::string * str_ptr = (std::string *)var_info->address ;
        // Get a pointer to the internal character array
        address = (void *)(str_ptr->c_str()) ;
    }

    // if this variable itself is a pointer, dereference it
    if ( deref ) {
        address = *(void**)var_info->address ;
    }

    // handle c++ string and char*
    if ( trick_type == TRICK_STRING ) {
        if (address == NULL) {
            size = 0 ;
        } else {
            size = strlen((char*)address) + 1 ;
        }
    }
    // handle c++ wstring and wchar_t*
    if ( trick_type == TRICK_WSTRING ) {
        if (address == NULL) {
            size = 0 ;
        } else {
            size = wcslen((wchar_t *)address) * sizeof(wchar_t);
        }
    }
    if(address != NULL) {
        memcpy( stage_buffer , address , size ) ;
    }

    staged = true;
    return 0;
}

bool Trick::VariableReference::validate() {
    // The address is not NULL.
    // Should be called by VariableServer Session if validateAddress is on.
    // check the memory manager if the address falls into
    // any of the memory blocks it knows of.  Don't do this if we have a std::string or
    // wstring type, or we already are pointing to a bad ref.
    if ( (trick_type != TRICK_STRING) and
            (trick_type != TRICK_WSTRING) and
            (var_info->address != &bad_ref_int) and
            (get_alloc_info_of(address) == NULL) ) {
        
        // This variable is broken, make it into an error ref
        tagAsInvalid();
        return false;
    }

    // Everything is fine
    return true;
}

static void write_escaped_string( std::ostream& os, const char* s) {
    for (int ii=0 ; ii<strlen(s) ; ii++) {
        if (isprint(s[ii])) {
            os << s[ii];
        } else {
            switch ((s)[ii]) {
                case '\n': os << "\\n"; break;
                case '\t': os << "\\t"; break;
                case '\b': os << "\\b"; break;
                case '\a': os << "\\a"; break;
                case '\f': os << "\\f"; break;
                case '\r': os << "\\n"; break;
                case '\v': os << "\\v"; break;
                case '\"': os << "\\\""; break;
                default  : {
                    // Replicating behavior from original vs_format_ascii
                    char temp_s[6];
                    sprintf(temp_s, "\\x%02x", s[ii]);
                    os << temp_s ; 
                    break;
                }
            }
        }
    }
}

int Trick::VariableReference::getSizeAscii() const {
    std::stringstream ss;
    writeValueAscii(ss);
    return ss.str().length();
}


int Trick::VariableReference::writeValueAscii( std::ostream& out ) const {
    // This is copied and modified from vs_format_ascii
    // There's a lot here that doesn't make sense to me that I need to come back to
    // There seems to be a huge buffer overflow issue in the original.
    // Only strings are checked for length, arrays aren't
    // But using a stream instead should make that better
    // The way that arrays are handled seems weird.

    if (!isWriteReady()) {
        return -1;
    }

    int bytes_written = 0;
    void * buf_ptr = write_buffer ;
    while (bytes_written < size) {
        bytes_written += var_info->attr->size ;

        switch (trick_type) {

        case TRICK_CHARACTER:
            if (var_info->attr->num_index == var_info->num_index) {
                // Single char
                out << (int)cv_convert_double(conversion_factor, *(char *)buf_ptr);
            } else {
                // All but last dim specified, leaves a char array 
                write_escaped_string(out, (const char *) buf_ptr);
                bytes_written = size ;
            }
            break;
        case TRICK_UNSIGNED_CHARACTER:
            if (var_info->attr->num_index == var_info->num_index) {
                // Single char
                out << (unsigned int)cv_convert_double(conversion_factor,*(unsigned char *)buf_ptr);
            } else {
                // All but last dim specified, leaves a char array 
                write_escaped_string(out, (const char *) buf_ptr);
                bytes_written = size ;
            }
            break;

        case TRICK_WCHAR:{
                if (var_info->attr->num_index == var_info->num_index) {
                    out << *(wchar_t *) buf_ptr;
                } else {
                    // convert wide char string char string
                    size_t len = wcs_to_ncs_len((wchar_t *)buf_ptr) + 1 ;

                    char temp_buf[len];
                    wcs_to_ncs((wchar_t *) buf_ptr, temp_buf, len);
                    out << temp_buf;
                    bytes_written = size ;
                }
            }
            break;

        case TRICK_STRING:
            if ((char *) buf_ptr != NULL) {
                write_escaped_string(out, (const char *) buf_ptr);
                bytes_written = size ;
            } else {
                out << '\0';
            }
            break;

        case TRICK_WSTRING:
            if ((wchar_t *) buf_ptr != NULL) {
                // convert wide char string char string
                size_t len = wcs_to_ncs_len( (wchar_t *)buf_ptr) + 1 ;

                char temp_buf[len];
                wcs_to_ncs(  (wchar_t *) buf_ptr, temp_buf, len);
                out << temp_buf;
                bytes_written = size ;
            } else {
                out << '\0';
            }
            break;
        case TRICK_SHORT:
            out << (short)cv_convert_double(conversion_factor,*(short *)buf_ptr);
            break;

        case TRICK_UNSIGNED_SHORT:
            out << (unsigned short)cv_convert_double(conversion_factor,*(unsigned short *)buf_ptr);
            break;

        case TRICK_INTEGER:
        case TRICK_ENUMERATED:
            out << (int)cv_convert_double(conversion_factor,*(int *)buf_ptr);
            break;

        case TRICK_BOOLEAN:
            out << (int)cv_convert_double(conversion_factor,*(bool *)buf_ptr);
            break;

        case TRICK_BITFIELD:
            out << (GET_BITFIELD(buf_ptr, var_info->attr->size, var_info->attr->index[0].start, var_info->attr->index[0].size));
            break;

        case TRICK_UNSIGNED_BITFIELD:
            out << (GET_UNSIGNED_BITFIELD(buf_ptr, var_info->attr->size, var_info->attr->index[0].start, var_info->attr->index[0].size));
            break;
            
        case TRICK_UNSIGNED_INTEGER:
            out << (unsigned int)cv_convert_double(conversion_factor,*(unsigned int *)buf_ptr);
            break;

        case TRICK_LONG: {
            long l = *(long *)buf_ptr;
            if (conversion_factor != cv_get_trivial()) {
                l = (long)cv_convert_double(conversion_factor, l);
            }
            out << l;
            break;
        }

        case TRICK_UNSIGNED_LONG: {
            unsigned long ul = *(unsigned long *)buf_ptr;
            if (conversion_factor != cv_get_trivial()) {
                ul = (unsigned long)cv_convert_double(conversion_factor, ul);
            }
            out << ul;
            break;
        }

        case TRICK_FLOAT:
            out << std::setprecision(8) << cv_convert_float(conversion_factor,*(float *)buf_ptr);
            break;

        case TRICK_DOUBLE:
            out << std::setprecision(16) << cv_convert_double(conversion_factor,*(double *)buf_ptr);
            break;

        case TRICK_LONG_LONG: {
            long long ll = *(long long *)buf_ptr;
            if (conversion_factor != cv_get_trivial()) {
                ll = (long long)cv_convert_double(conversion_factor, ll);
            }
            out << ll;
            break;
        }

        case TRICK_UNSIGNED_LONG_LONG: {
            unsigned long long ull = *(unsigned long long *)buf_ptr;
            if (conversion_factor != cv_get_trivial()) {
                ull = (unsigned long long)cv_convert_double(conversion_factor, ull);
            }
            out << ull;
            break;
        }

        case TRICK_NUMBER_OF_TYPES:
            out << "BAD_REF";
            break;

        default:{

            break;
        }
        } // end switch

        if (bytes_written < size) {
        // if returning an array, continue array as comma separated values
            out << ",";
            buf_ptr = (void*) ((long)buf_ptr + var_info->attr->size) ;
        }
    } //end while

    if (var_info->units) {
        if ( var_info->attr->mods & TRICK_MODS_UNITSDASHDASH ) {
            out << " {--}";
        } else {
            out << " {" << var_info->units << "}";
        }
    }

    return 0;
}

void Trick::VariableReference::tagAsInvalid () {
    std::string save_name(getName()) ;
    free(var_info) ;
    var_info = make_error_ref(save_name) ;
    address = var_info->address ;
}


int Trick::VariableReference::prepareForWrite() {
    if (!staged) {
        return 1;
    }

    void * temp_p = stage_buffer;
    stage_buffer = write_buffer;
    write_buffer = temp_p;

    staged = false;
    write_ready = true;
    return 0;
}

bool Trick::VariableReference::isStaged() const {
    return staged;
}

bool Trick::VariableReference::isWriteReady() const {
    return write_ready;
}

int Trick::VariableReference::writeTypeBinary( std::ostream& out, bool byteswap ) const {
    int local_type = trick_type;
    if (byteswap) {
        local_type = trick_byteswap_int(local_type);
    }
    out.write(const_cast<const char *>(reinterpret_cast<char *>(&local_type)), sizeof(int));

    return 0;
}

int Trick::VariableReference::writeSizeBinary( std::ostream& out, bool byteswap ) const {
    int local_size = size;
    if (byteswap) {
        local_size = trick_byteswap_int(local_size);
    }
    out.write(const_cast<const char *>(reinterpret_cast<char *>(&local_size)), sizeof(int));

    return 0;
}

int Trick::VariableReference::writeNameBinary( std::ostream& out, bool byteswap ) const {
    const char * name = getName();

    int name_size = strlen(name);
    if (byteswap) {
        name_size = trick_byteswap_int(name_size);
    }

    out.write(const_cast<const char *>(reinterpret_cast<char *>(&name_size)), sizeof(int));
    out.write(name, strlen(name));

    return 0;
}

void Trick::VariableReference::byteswap_var (char * out, char * in) const {
    byteswap_var(out, in, *this);
}


void Trick::VariableReference::byteswap_var (char * out, char * in, const VariableReference& ref) {
    ATTRIBUTES * attr = ref.var_info->attr;
    int array_size = 1;

    // Determine how many elements are in this array if it is an array
    for (int j = 0; j < ref.var_info->attr->num_index; j++) {
        array_size *= attr->index[j].size;
    }

    switch (attr->size) {
        case 1:
            // If these are just characters, no need to byteswap
            for (int j = 0; j < array_size; j++) {
                out[j] = in[j];
            }
            break;

        case 2: {
            short * short_in = reinterpret_cast<short *> (in);
            short * short_out = reinterpret_cast<short *> (out);

            for (int j = 0; j < array_size; j++) {
                short_out[j] = trick_byteswap_short(short_in[j]);
            }
            break;
        }

        case 4: {
            int * int_in = reinterpret_cast<int *> (in);
            int * int_out = reinterpret_cast<int *> (out);

            for (int j = 0; j < array_size; j++) {
                int_out[j] = trick_byteswap_int(int_in[j]);
            }
            break;
        }
        case 8: {
            // We don't actually care if this is double or long, just that it's the right size
            double * double_in = reinterpret_cast<double *> (in);
            double * double_out = reinterpret_cast<double *> (out);

            for (int j = 0; j < array_size; j++) {
                double_out[j] = trick_byteswap_double(double_in[j]);
            }
            break;
        }
    }
}



int Trick::VariableReference::writeValueBinary( std::ostream& out, bool byteswap ) const {

    char buf[20480];
    int temp_i ;
    unsigned int temp_ui ;

    // int offset = 0;

    switch ( var_info->attr->type ) {
        case TRICK_BITFIELD:
            temp_i = GET_BITFIELD(address , var_info->attr->size ,
                var_info->attr->index[0].start, var_info->attr->index[0].size) ;
            memcpy(buf, &temp_i , (size_t)size) ;
        break ;
        case TRICK_UNSIGNED_BITFIELD:
            temp_ui = GET_UNSIGNED_BITFIELD(address , var_info->attr->size ,
                    var_info->attr->index[0].start, var_info->attr->index[0].size) ;
            memcpy(buf , &temp_ui , (size_t)size) ;
        break ;
        case TRICK_NUMBER_OF_TYPES:
            // TRICK_NUMBER_OF_TYPES is an error case
            temp_i = 0 ;
            memcpy(buf , &temp_i , (size_t)size) ;
        break ;
        default:
            if (byteswap)
                byteswap_var(buf, (char *) address);
            else
                memcpy(buf , address , (size_t)size) ;
        break ;
    }

    out.write(buf, size);
}  

std::ostream& Trick::operator<< (std::ostream& s, const Trick::VariableReference& ref) {

    s << "      \"" << ref.getName() << "\"";
    return s;
}