
#include <iostream>
#include <algorithm>
#include <sstream>
#include <set>

#include <libgen.h>
#include <regex.h>
#include <string.h>

#include "FieldDescription.hh"
#include "Utilities.hh"

// Provides units check capability.
// Found in "${TRICK_HOME}/trick_source/trick_utils/units/include"
#include "units_conv.h"

FieldDescription::FieldDescription(
 std::string in_container_class ,
 bool in_access_spec_found ,
 bool in_inherited ,
 bool in_virtual_inherited ,
 unsigned int in_base_class_offset) :
  container_class(in_container_class) ,
  base_class_offset(in_base_class_offset) ,
  access_spec_found(in_access_spec_found) ,
  inherited(in_inherited) ,
  virtual_inherited(in_virtual_inherited) ,
  units("--") ,
  line_no(0) ,
  io(15) ,
  type_enum_string("TRICK_VOID") ,
  is_bitfield(0) ,
  bitfield_width(0) ,
  bitfield_start_bit(0) ,
  bitfield_word_offset(0) ,
  is_enum(0) ,
  is_record(0) ,
  is_static(0) ,
  has_type(0) ,
  has_dims(0) ,
  num_dims(0) ,
  array_sizes() {} ;


std::map<std::string , unsigned int > create_io_map() {
    std::map<std::string , unsigned int > io_map ;
    io_map[""] = 3 ;
    io_map["o"] = 1 ;
    io_map["*o"] = 1 ;
    io_map["*i"] = 2 ;
    io_map["i"] = 2 ;
    io_map["*oi"] = 3 ;
    io_map["oi"] = 3 ;
    io_map["*io"] = 3 ;
    io_map["io"] = 3 ;
    io_map["**"] = 0 ;
    // The "--" is a bad value, but we'll take it for now.
    // TODO: Make this an error instead of being nice!
    io_map["--"] = 4 ;
    return io_map ;
}

std::map<std::string , unsigned int > FieldDescription::io_map = create_io_map() ;

std::string FieldDescription::get_regex_field(std::string input , const char * expr , unsigned int index) {
    int ret ;
    regex_t reg_expr ;
    regmatch_t pmatch[10] ;
    memset(pmatch , 0 , sizeof(pmatch)) ;
    ret = regcomp( &reg_expr , expr , REG_EXTENDED ) ;
    //std::cout << "regcomp ret = " << ret << std::endl ;
    ret = regexec( &reg_expr , input.c_str() , 10 , pmatch , 0 ) ;
    //std::cout << "regexec ret = " << ret << std::endl ;
    regfree(&reg_expr) ;
    if ( ret == 0 ) {
        //std::cout << "pmatch range = " << pmatch[index].rm_so << " " << pmatch[index].rm_eo << std::endl ;
        if ( pmatch[index].rm_so != -1 ) {
            return input.substr(pmatch[index].rm_so , pmatch[index].rm_eo - pmatch[index].rm_so ) ;
        }
    } else {
        //char error_msg[1024] ;
        //regerror( ret , &reg_expr , (char *)error_msg , 1024 ) ;
        //std::cout << error_msg << std::endl ;
    }
    return std::string() ;
}

// global set of all units processed. hopefully saves time from reparsing same units strings over and over
std::set< std::string > valid_units ;

void FieldDescription::parseComment(std::string comment) {
    std::string ret_str ;

    bool units_found = false ;
    bool io_found = false ;
    bool chkpnt_io_found = false ;
    unsigned int chkpnt_io ;

    if ( comment.empty() ) {
        // If the comment is empty default all I/O enabled.
        io = 15 ;
        return ;
    }

    //std::cout << "comment before " << comment << std::endl ;

    // remove open comment chars
    comment = get_regex_field(comment , "^(//|/\\*)(.*)" , 2) ;
    //std::cout << "1. " << comment << std::endl ;

    // remove optional doxygen comment chars
    // Note: I had to use [ \t\n\r] for \s because the Mac don't understand!
    comment = get_regex_field(comment , "^((\\*|!)<)?[ \t\n\r]*(.*)" , 3) ;
    //std::cout << "2. " << comment << std::endl ;

    // remove optional doxygen keyword
    comment = get_regex_field(comment , "(\\\\\\w+[ \t\n\r]*)?(.*)" , 2) ;
    //std::cout << "3. " << comment << std::endl ;

    ret_str = get_regex_field(comment , "@?trick_chkpnt_io[\\({]([^\\)}]+)[\\)}]" , 1) ;
    if ( ! ret_str.empty()) {
        chkpnt_io = io_map[ret_str] ;
        //std::cout << "go for trick_chkpnt_io " <<  io << std::endl ;
        chkpnt_io_found = true ;
        comment = get_regex_field(comment , "(.*)@?trick_chkpnt_io[\\({]([^\\)}]+)[\\)}]" , 1) +
         get_regex_field(comment , "@?trick_chkpnt_io[\\({]([^\\)}]+)[\\)}](.*)" , 2) ;
    }

    ret_str = get_regex_field(comment , "@?trick_io[\\({]([^\\)}]+)[\\)}]" , 1) ;
    if ( ! ret_str.empty()) {
        io = io_map[ret_str] ;
        //std::cout << "go for trick_io " <<  io << std::endl ;
        io_found = true ;
        comment = get_regex_field(comment , "(.*)@?trick_io[\\({]([^\\)}]+)[\\)}]" , 1) +
         get_regex_field(comment , "@?trick_io[\\({]([^\\)}]+)[\\)}](.*)" , 2) ;
    }

    //std::cout << "4. " << comment << std::endl ;
    ret_str = get_regex_field(comment , "@?trick_units[\\({]([^\\)}]+)[\\)}]" , 1) ;
    if ( ! ret_str.empty()) {
        units = ret_str ;
        //std::cout << "go for trick_units " <<  units << std::endl ;
        units_found = true ;
        comment = get_regex_field(comment , "(.*)@?trick_units[\\({]([^\\)}]+)[\\)}]" , 1) +
         get_regex_field(comment , "@?trick_units[\\({]([^\\)}]+)[\\)}](.*)" , 2) ;
    }

    if ( ! io_found ) {
        // Note: I had to use [ \t\n\r] for \s because the Mac don't understand!
        ret_str = get_regex_field(comment , "^[ \t\n\r]*(\\*io|\\*oi|\\*i|\\*o|\\*\\*)" , 1) ;
        //std::cout << "3. " << ret_str << std::endl ;
        if ( ! ret_str.empty()) {
            io = io_map[ret_str] ;
            //std::cout << "stand-alone io " <<  io << std::endl ;
            io_found = true ;
            comment = get_regex_field(comment , "^[ \t\n\r]*(\\*io|\\*oi|\\*i|\\*o|\\*\\*)[ \t\n\r]*(.*)" , 2) ;
        }
    }

    //std::cout << "3. " << comment << std::endl ;
    if ( ! units_found ) {
        ret_str = get_regex_field(comment , "^[ \t\n\r]*\\(([^\\)]*)\\)" , 1) ;
        if ( ! ret_str.empty()) {
            units = ret_str ;
            //std::cout << "stand-alone units " << units << std::endl ;
            units_found = true ;
            comment = get_regex_field(comment , "^[ \t\n\r]*\\(([^\\)]*)\\)(.*)" , 2) ;
        } else {
            ret_str = get_regex_field(comment , "^[ \t\n\r]*([^ \t\n\r)]*)" , 1) ;
            if ( ! ret_str.empty()) {
                units = ret_str ;
                //std::cout << "stand-alone units " << units << " " << comment << std::endl ;
                units_found = true ;
                comment = get_regex_field(comment , "^[ \t\n\r]*([^ \t\n\r)]*)(.*)" , 2) ;
            }
        }
    }

    // Test if we have valid units.  We need to have found a units string and an io spec not zero
    // Possible todo is to create a map of valid units so we don't have to retest each string.
    if ( units_found and io != 0 and (valid_units.find(units) == valid_units.end())) {
        Units_t * test_units = new_units(units.c_str()) ;
        if ( test_units == NULL ) {
            // If the units are invalid write an error message and change the units to "--"
            std::cout << "\033[31mBad units specification (" << units << "): " << file_name << ":" << line_no
             << "\033[0m" << std::endl ;
            units = "--" ;
        } else {
            // If the units are valid, free the memory allocated by new_units.
            CONV_FREE(test_units) ;
            valid_units.insert(units) ;
        }
    }

    if ( io == 4 ) {
        std::cout << "\033[33mWarning: " << file_name << ": line " << line_no << ": " <<
         "\"--\" is not a valid trick_io value. Setting to *io (3)\033[0m" << std::endl ;
        io = 3 ;
    }

    if ( chkpnt_io_found == true ) {
        // If a checkpoint I/O spec is found add it to the io field.
        io |= (chkpnt_io << 2 ) ;
    } else {
        // else duplicated the io field to the chkpnt io field.
        io |= (io << 2 ) ;
    }

    // The rest of the comment is the description of the variable.

    // remove the c comment end marker.
    comment = get_regex_field(comment , "(.*)\\*/" , 1) ;

    // posix c regular expressions are terrible. the regexes above will leave "@" signs because
    // the regular expressions are so greedy.
    comment = get_regex_field(comment , "^[ \t\n\r@]+(.*)" , 1) ;

    // remove leading and trailing whitespace
    comment = trim(comment) ;

    // escape special characters, convert tabs and newlines to spaces, remove multiple spaces.
    std::ostringstream ss ;
    bool is_space = false ;
    for (std::string::iterator it = comment.begin(); it != comment.end(); it++) {
        switch (*it) {
            case '\\': ss << "\\\\"; is_space = false ; break;
            case '"': ss << "\\\""; is_space = false ; break;
            case '\b': ss << "\\b"; is_space = false ; break;
            case '\f': ss << "\\f"; is_space = false ; break;
            case '\n':
            case '\r':
            case '\t':
            case ' ': if ( ! is_space ) ss << " "; is_space = true ; break;
            // -110 to -108 (signed char) are the Windows quotation marks.  We ASCII-fy them.
            case -110 : ss << "'" ; break;
            case -109 : ss << "\\\"" ; break;
            case -108 : ss << "\\\"" ; break;
            default: ss << *it; is_space = false ; break;
        }
    }

    description = ss.str() ;

}

std::string FieldDescription::getContainerClass() {
    return container_class ;
}

void FieldDescription::setContainerClass(std::string in_name ) {
    container_class = in_name ;
}

unsigned int FieldDescription::getBaseClassOffset() {
    return base_class_offset ;
}

void FieldDescription::setTypeName( std::string in_val ) {
    type_name = in_val ;
}

std::string FieldDescription::getTypeName() {
    return type_name ;
}

void FieldDescription::setLineNo( unsigned int in_val ) {
    line_no = in_val ;
}

unsigned int FieldDescription::getLineNo() {
    return line_no ;
}

void FieldDescription::setMangledTypeName( std::string in_val ) {
    mangled_type_name = in_val ;
}

std::string FieldDescription::getMangledTypeName() {
    if ( mangled_type_name.empty() ) {
        return type_name ;
    }
    return mangled_type_name ;
}

std::string FieldDescription::getUnits() {
    return units ;
}

void FieldDescription::setIO(unsigned int in_io) {
    io = in_io ;
}

unsigned int FieldDescription::getIO() {
    return io ;
}

std::string FieldDescription::getDescription() {
    return description ;
}

bool FieldDescription::getAccessSpecFound() {
    return access_spec_found ;
}

bool FieldDescription::isInherited() {
    return inherited ;
}

bool FieldDescription::isVirtualInherited() {
    return virtual_inherited ;
}

void FieldDescription::setAccess( clang::AccessSpecifier in_val ) {
    access = in_val ;
}

clang::AccessSpecifier FieldDescription::getAccess() {
    return access ;
}

void FieldDescription::setEnumString(std::string in_str) {
    type_enum_string = in_str ;
}

std::string FieldDescription::getEnumString() {
    return type_enum_string ;
}

void FieldDescription::setBitField(bool yes_no) {
    is_bitfield = yes_no ;
}

bool FieldDescription::isBitField() {
    return is_bitfield ;
}

void FieldDescription::setBitFieldWidth(unsigned int len) {
    bitfield_width = len ;
}

void FieldDescription::setBitFieldStart(unsigned int sb) {
    bitfield_start_bit = sb ;
}

void FieldDescription::setBitFieldByteOffset(unsigned int wo) {
    bitfield_word_offset = wo ;
}

unsigned int FieldDescription::getBitFieldWidth() {
    return bitfield_width ;
}

unsigned int FieldDescription::getBitFieldStart() {
    return bitfield_start_bit ;
}

unsigned int FieldDescription::getBitFieldByteOffset() {
    return bitfield_word_offset ;
}

void FieldDescription::setEnum(bool yes_no) {
    is_enum = yes_no ;
}

bool FieldDescription::isEnum() {
    return is_enum ;
}

void FieldDescription::setRecord(bool yes_no) {
    is_record = yes_no ;
}

bool FieldDescription::isRecord() {
    return is_record ;
}

void FieldDescription::setStatic(bool yes_no) {
    is_static = yes_no ;
}

bool FieldDescription::isStatic() {
    return is_static ;
}

void FieldDescription::setHasType( bool yes_no ) {
    has_type = yes_no ;
}

bool FieldDescription::hasType() {
    return has_type ;
}

void FieldDescription::setHasDims( bool yes_no ) {
    has_dims = yes_no ;
}

bool FieldDescription::hasDims() {
    return has_dims ;
}

unsigned int FieldDescription::getNumDims() {
    return num_dims ;
}

int FieldDescription::getArrayDim(unsigned int dim_num) {
    if ( dim_num < 8 ) {
        return array_sizes[dim_num] ;
    }
    return 0 ;
}

void FieldDescription::addArrayDim( int in_dim ) {
    array_sizes[num_dims++] = in_dim ;
}

std::ostream & operator << (std::ostream & os , FieldDescription & fdes ) {
    os << "    name = " << fdes.name << std::endl ;
    os << "    file_name = " << fdes.file_name << std::endl ;
    os << "    namespaces =" ;
    ConstructValues::NamespaceIterator it ;
    for ( it = fdes.namespace_begin() ; it != fdes.namespace_end() ; it++ ) {
        os << " " << *it ;
    }
    os << std::endl ;
    os << "    parent classes =" ;
    for ( it = fdes.container_class_begin() ; it != fdes.container_class_end() ; it++ ) {
        os << " " << *it ;
    }
    os << std::endl ;
    os << "    line_no = " << fdes.line_no << std::endl ;
    os << "    container_class = " << fdes.container_class << std::endl ;
    os << "    type_name = " << fdes.type_name << std::endl ;
    os << "    mangled_type_name = " << fdes.mangled_type_name << std::endl ;
    os << "    type_enum_string = " << fdes.type_enum_string << std::endl ;
    os << "    units = " << fdes.units << std::endl ;
    os << "    io = " << fdes.io << std::endl ;
    os << "    description = " << fdes.description << std::endl ;
    os << "    access = " << fdes.access << std::endl ;
    os << "    access_spec_found = " << fdes.access_spec_found << std::endl ;
    os << "    is_bitfield = " << fdes.is_bitfield << std::endl ;
    os << "    bitfield_width = " << fdes.bitfield_width << std::endl ;
    os << "    bitfield_start_bit = " << fdes.bitfield_start_bit << std::endl ;
    os << "    bitfield_word_offset = " << fdes.bitfield_word_offset << std::endl ;
    os << "    num_dims = " << fdes.num_dims << std::endl ;
    os << "    array_sizes =" ;
    for( unsigned int ii ; ii < 8 ; ii++ ) {
        os << " " << fdes.array_sizes[ii] ;
    }
    os << std::endl ;
    os << "    is_enum = " << fdes.is_enum << std::endl ;
    os << "    is_record = " << fdes.is_record << std::endl ;
    os << "    is_static = " << fdes.is_static << std::endl ;
    os << "    has_type = " << fdes.has_type << std::endl ;

    return os ;
}
