
#ifndef PRINTFILECONTENTSMIN_HH
#define PRINTFILECONTENTSMIN_HH

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <set>

#include "PrintFileContentsBase.hh"

/**

  This class prints Trick 10 style io_src code. The print command is called when
  the container class has added all of the classes and enumerations to this class
  to print.

  @author Alexander S. Lin

  @date July 2012

 */

class PrintFileContentsMin : public PrintFileContentsBase {

    public:

        PrintFileContentsMin() ;

        /** create io_src file name */
        virtual std::string createIOSrcFileName(std::string base_file_name) ;

        /** Prints the io_src header information */
        virtual void printIOHeader(std::ostream & outfile , std::string header_file_name) ;

        /** Prints all io_src code for incoming class */
        virtual void printClass(std::ostream & outfile , ClassValues * cv) ;

        /** Prints all io_src code for incoming enum */
        virtual void printEnum(std::ostream & outfile , EnumValues * ev) ;

    private:

        /** Prints enumeration attributes */
        void print_enum_attr(std::ostream & outfile , EnumValues * in_enum) ;

        /** Prints attributes for a field */
        void print_field_attr(std::ostream & outfile , FieldDescription & fdes ) ;

        /** Prints class attributes */
        void print_class_attr(std::ostream & outfile , ClassValues * in_class) ;

} ;

#endif
