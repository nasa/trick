
#ifndef _PRINTFILECONTENTSBASE_HH_
#define _PRINTFILECONTENTSBASE_HH_

#include <string>
#include <vector>
#include <map>
#include <set>

class ConstructValues ;
class ClassValues ;
class EnumValues ;
class FieldDescription ;

/**

  This class prints Trick 10 style io_src code.  Given a vector of class information,
  this class determines the io_src code file names, excluding files in system directories,
  and writes out the attributes.

  @author Alexander S. Lin

  @date July 2012

 */

class PrintFileContentsBase {
    public:
        PrintFileContentsBase() ;

        virtual void printIOHeader(std::ofstream & out, std::string header_file_name) = 0 ;
        virtual void printClass(std::ofstream & out, ClassValues * cv) = 0 ;
        virtual void printEnum(std::ofstream & out, EnumValues * ev) = 0 ;

        // these routines provide default empty implementation
        virtual void printClassMapHeader(std::ofstream & out, std::string function_name ) ;
        virtual void printClassMap(std::ofstream & out, ClassValues * cv) ;
        virtual void printClassMapFooter(std::ofstream & out) ;

        virtual void printEnumMapHeader(std::ofstream & out, std::string function_name ) ;
        virtual void printEnumMap(std::ofstream & out, EnumValues * ev) ;
        virtual void printEnumMapFooter(std::ofstream & out) ;

    protected:
        /** Prints the io_src_allocate function */
        virtual void print_units_map(std::ofstream & out, ClassValues * cv) ;

        /** Prints "extern \"C\" {" */
        void print_open_extern_c(std::ofstream & out) ;

        /** Prints "} // extern C" */
        void print_close_extern_c(std::ofstream & out) ;

        /* internal function determines if a particular field is printable */
        bool determinePrintAttr(ClassValues * c , FieldDescription *fdes) ;

        /** Prints namespace containers of a class delimited by delim */
        void printNamespaces(std::ofstream & out, ConstructValues * c , const char * delim ) ;

        /** Prints namespace open block */
        void printOpenNamespaceBlocks( std::ofstream & out, ClassValues * c ) ;

        /** Prints namespace close block */
        void printCloseNamespaceBlocks( std::ofstream & out, ClassValues * c ) ;

        /* Note: There are no open/close namespace blocks for enums.  The
           enum attributes do not have anything that needs to be enclosed in
           namespaces */

        /** Prints class containers of a class delimited by delim */
        void printContainerClasses(std::ofstream & out, ConstructValues * c , const char * delim ) ;

} ;

#endif
