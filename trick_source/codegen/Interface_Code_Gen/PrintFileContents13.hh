
#ifndef PRINTFILECONTENTS13_HH
#define PRINTFILECONTENTS13_HH

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <set>

#include "PrintFileContentsBase.hh"

/**

  This class prints Trick 13 style io_src code. The print command is called when
  the container class has added all of the classes and enumerations to this class
  to print.

  @author somebody

  @date July 2012

 */

class PrintFileContents13 : public PrintFileContentsBase {

    public:

        PrintFileContents13() ;

        /** Prints the io_src header information */
        virtual void printIOHeader(std::ostream & ostream , std::string header_file_name) ;

        std::string enumeration_identifier( EnumValues * e );
        /** Prints enumeration attributes */
        void print_enum_attr(std::ostream & ostream , EnumValues * in_enum) ;


        std::string bit_field_getter_name(ClassValues * c, FieldDescription * f);
        std::string bit_field_setter_name(ClassValues * c, FieldDescription * f);

        std::string class_identifier( ClassValues * c ) ;

        /** Prints attributes for a field */
        void print_field_attr(std::ostream & ostream , ClassValues * c, FieldDescription * fv ) ;

        /** Prints class attributes */
        void print_class_attr(std::ostream & ostream , ClassValues * in_class) ;

        /** Prints attributes for a class */
        virtual void printClass(std::ostream & ostream , ClassValues * cv) ;

        /** Prints attributes for an enum */
        virtual void printEnum(std::ostream & ostream , EnumValues * ev) ;

} ;

#endif
