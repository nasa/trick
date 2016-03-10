
#ifndef FIELDVALUES_HH
#define FIELDVALUES_HH

#include <string>
#include <map>
#include <stdint.h>

#include "ConstructValues.hh"
#include "clang/Basic/Specifiers.h"

/**

  FieldDescription holds information describing a field found with ICG.  The
  information includes everything about the data member.

  The class also provides print routines for each field in the io_src.

  @author Alexander S. Lin

  @date July 2012

 */

class ClassValues ;

class FieldDescription : public ConstructValues {

    public:

        /* Default the inheritance to false */
        FieldDescription(
         std::string in_container_class ,
         bool access_spec_found ,
         bool inherited ,
         bool virtual_inherited ,
         unsigned int base_class_offset ) ;

        /* Extracts units and io code from a comment */
        void parseComment(std::string) ;

        /* Accessor functions to the data of the class */
        std::string getContainerClass() ;
        void setContainerClass( std::string in_name ) ;
        unsigned int getBaseClassOffset() ;
        void setTypeName( std::string in_val ) ;
        std::string getTypeName() ;
        void setLineNo( unsigned int ) ;
        unsigned int getLineNo() ;
        void setMangledTypeName( std::string in_val ) ;
        std::string getMangledTypeName() ;
        std::string getUnits() ;
        void setIO(unsigned int) ;
        unsigned int getIO() ;
        std::string getDescription() ;
        void setEnumString(std::string) ;
        std::string getEnumString() ;
        void setBitField( bool yes_no ) ;
        void setBitFieldWidth( unsigned int len ) ;
        void setBitFieldStart( unsigned int len ) ;
        void setBitFieldByteOffset( unsigned int len ) ;
        unsigned int getBitFieldWidth() ;
        unsigned int getBitFieldStart() ;
        unsigned int getBitFieldByteOffset() ;
        unsigned int getNumDims() ;
        int getArrayDim(unsigned int dim_num) ;

        bool isBitField() ;
        void setEnum( bool yes_no ) ;
        bool isEnum() ;
        void setRecord( bool yes_no ) ;
        bool isRecord() ;
        void setStatic( bool yes_no ) ;
        bool isStatic() ;
        bool getAccessSpecFound() ;
        bool isInherited() ;
        bool isVirtualInherited() ;
        void setHasType( bool yes_no ) ;
        bool hasType() ;
        void setHasDims( bool yes_no ) ;
        bool hasDims() ;
        void setAccess( clang::AccessSpecifier in_val ) ;
        clang::AccessSpecifier getAccess() ;

        /** Adds an array dimension to the field */
        void addArrayDim( int in_dim ) ;

        friend std::ostream & operator << (std::ostream & os , FieldDescription & cv ) ;

    private:

        /** Line number in current file where field is */
        unsigned int line_no ;

        /** Name of the class this field is in */
        std::string container_class ;

        /** This is copied from the current class we are processing.  It is the class offset to
            be added to field offset */
        unsigned int base_class_offset ;

        /** Name of the type */
        std::string type_name ;

        /** Name of the type */
        std::string mangled_type_name ;


        /** String representing the type enumeration */
        std::string type_enum_string ;

        /** Units specified of the field */
        std::string units ;

        /** io restrictions */
        unsigned int io ;

        /** Comment description */
        std::string description ;

        /** public/protected/private */
        clang::AccessSpecifier access ;

        /** is this field a bitfield */
        bool is_bitfield ;

        /** how many bits is this bitfield */
        unsigned int bitfield_width ;

        /** what bit in the word does this bit start on? */
        unsigned int bitfield_start_bit ;

        /** word offset into struct/class of this bitfield */
        uint64_t bitfield_word_offset ;

        /** Number of fixed and pointer dimensions */
        unsigned int num_dims ;

        /** dimensions sizes */
        int array_sizes[8] ;

        /** does this field follow a public/private/protected keyword */
        bool access_spec_found ;

        /** is this field inherited from parent class */
        bool inherited ;

        /** is this field virtual inherited from parent class */
        bool virtual_inherited ;

        /** is an enumeration  */
        bool is_enum ;

        /** is a record class */
        bool is_record ;

        /** is this field declared static */
        bool is_static ;

        /** Have we resolved the type for this parameter? */
        bool has_type ;

        /** Have we resolved the dimensions for this parameter? */
        bool has_dims ;

        /** map of strings to io numbers.  One copy for all fields */
        static std::map<std::string , unsigned int> io_map ;

        /** Internal function to execute regular expression */
        std::string get_regex_field(std::string input , const char * expr , unsigned int index) ;

} ;

#endif
