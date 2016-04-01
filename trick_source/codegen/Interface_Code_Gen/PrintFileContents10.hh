
#ifndef PRINTFILECONTENTS10_HH
#define PRINTFILECONTENTS10_HH

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

class PrintFileContents10 : public PrintFileContentsBase {

    public:

        PrintFileContents10() ;

        /** Prints the io_src header information */
        virtual void printIOHeader(std::ofstream & outfile , std::string header_file_name) ;

        /** Prints all io_src code for incoming class */
        virtual void printClass(std::ofstream & outfile , ClassValues * cv) ;

        /** Prints all io_src code for incoming enum */
        virtual void printEnum(std::ofstream & outfile , EnumValues * ev) ;

        /** Prints all classes for global map */
        virtual void printClassMapHeader(std::ofstream & out, std::string function_name ) ;
        virtual void printClassMap(std::ofstream & outfile , ClassValues * cv) ;
        virtual void printClassMapFooter(std::ofstream & out) ;

        /** Prints all enums for global map */
        virtual void printEnumMapHeader(std::ofstream & out, std::string function_name ) ;
        virtual void printEnumMap(std::ofstream & out, EnumValues * ev) ;
        virtual void printEnumMapFooter(std::ofstream & out) ;

    private:

        /** Prints enumeration attributes */
        void print_enum_attr(std::ofstream & outfile , EnumValues * in_enum) ;

        /** Prints attributes for a field */
        void print_field_attr(std::ofstream & outfile ,  FieldDescription * fdes ) ;

        /** Prints class attributes */
        void print_class_attr(std::ofstream & outfile , ClassValues * in_class) ;

        /** Prints init_attr function for each class */
        void print_field_init_attr_stmts(std::ofstream & outfile , FieldDescription * fdes , ClassValues * cv ) ;

        /** Prints add_attr_info statements for each inherited class */
        void print_inherited_add_attr_info(std::ofstream & outfile , ClassValues * cv ) ;

        /** Prints init_attr function for each class */
        void print_init_attr_func(std::ofstream & outfile , ClassValues * c ) ;

        /** Prints the io_src_sizeof function for enumerations */
        void print_enum_io_src_sizeof(std::ofstream & outfile , EnumValues * ev ) ;

        /** Prints the C linkage init_attr function */
        void print_init_attr_c_intf(std::ofstream & outfile , ClassValues * cv ) ;

        /** Prints the io_src_sizeof function */
        void print_io_src_sizeof(std::ofstream & outfile , ClassValues * cv ) ;

        /** Prints the io_src_allocate function */
        void print_io_src_allocate(std::ofstream & outfile , ClassValues * cv ) ;

        /** Prints the io_src_destruct function */
        void print_io_src_destruct(std::ofstream & outfile , ClassValues * cv ) ;

        /** Prints the io_src_delete function */
        void print_io_src_delete(std::ofstream & outfile , ClassValues * cv ) ;

        /** Prints stl helper function prototypes */
        void print_stl_helper_proto(std::ofstream & outfile , ClassValues * in_class) ;

        /** Prints stl helper function */
        void print_stl_helper(std::ofstream & outfile , ClassValues * in_class) ;

        /** Prints stl checkpoint function */
        void print_checkpoint_stl(std::ofstream & outfile , FieldDescription * fdes , ClassValues * in_class) ;

        /** Prints stl post_checkpoint function */
        void print_post_checkpoint_stl(std::ofstream & outfile , FieldDescription * fdes , ClassValues * in_class) ;

        /** Prints stl restart function */
        void print_restore_stl(std::ofstream & outfile , FieldDescription * fdes , ClassValues * in_class) ;

        /** Prints stl clear function */
        void print_clear_stl(std::ofstream & outfile , FieldDescription * fdes , ClassValues * in_class) ;
} ;

#endif
