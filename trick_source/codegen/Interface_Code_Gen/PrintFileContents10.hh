
#ifndef PRINTFILECONTENTS10_HH
#define PRINTFILECONTENTS10_HH

#include <iostream>
#include <map>
#include <set>
#include <string>
#include <vector>

#include "PrintFileContentsBase.hh"

/**

  This class prints Trick 10 style io_src code. The print command is called when
  the container class has added all of the classes and enumerations to this class
  to print.

  @author Alexander S. Lin

  @date July 2012

 */

class PrintFileContents10 : public PrintFileContentsBase
{
public:
    PrintFileContents10();

    /** Prints the io_src header information */
    virtual void printIOHeader(std::ostream & outfile, std::string header_file_name);

    /** Prints all io_src code for incoming class */
    virtual void printClass(std::ostream & outfile, ClassValues * cv);

    /** Prints all io_src code for incoming enum */
    virtual void printEnum(std::ostream & outfile, EnumValues * ev);

    /** Prints all classes for global map */
    virtual void printClassMapHeader(std::ostream & out, std::string function_name);
    virtual void printClassMap(std::ostream & outfile, ClassValues * cv);
    virtual void printClassMapFooter(std::ostream & out);

    /** Prints all enums for global map */
    virtual void printEnumMapHeader(std::ostream & out, std::string function_name);
    virtual void printEnumMap(std::ostream & out, EnumValues * ev);
    virtual void printEnumMapFooter(std::ostream & out);

    /** stores template argument header dependencies to be printed */
    virtual void addTemplateArgumentHeaderDependency(const std::string & header, const std::string & dependency);

private:
    /** Prints enumeration attributes */
    void print_enum_attr(std::ostream & outfile, EnumValues * in_enum);

    /** Prints attributes for a field */
    void print_field_attr(std::ostream & outfile, FieldDescription & fdes, ClassValues * cv = NULL);

    /** Prints forward declarations for STL accessor functions */
    void print_stl_declarations(std::ostream & outfile, ClassValues * in_class);

    /** Prints class attributes */
    void print_class_attr(std::ostream & outfile, ClassValues * in_class);

    /** Prints init_attr function for each class */
    void print_field_init_attr_stmts(std::ostream & outfile,
                                     FieldDescription * fdes,
                                     ClassValues * cv,
                                     unsigned int index);

    /** Prints add_attr_info statements for each inherited class */
    void print_inherited_add_attr_info(std::ostream & outfile, ClassValues * cv);

    /** Prints init_attr function for each class */
    void print_init_attr_func(std::ostream & outfile, ClassValues * c);

    /** Prints the io_src_sizeof function for enumerations */
    void print_enum_io_src_sizeof(std::ostream & outfile, EnumValues * ev);

    /** Prints the C linkage init_attr function */
    void print_init_attr_c_intf(std::ostream & outfile, ClassValues * cv);

    /** Prints the io_src_sizeof function */
    void print_io_src_sizeof(std::ostream & outfile, ClassValues * cv);

    /** Prints the io_src_allocate function */
    void print_io_src_allocate(std::ostream & outfile, ClassValues * cv);

    /** Prints the io_src_destruct function */
    void print_io_src_destruct(std::ostream & outfile, ClassValues * cv);

    /** Prints the io_src_delete function */
    void print_io_src_delete(std::ostream & outfile, ClassValues * cv);

    /** Prints stl helper function */
    void print_stl_helper(std::ostream & outfile, ClassValues * in_class);

    /** Prints stl checkpoint function */
    void print_checkpoint_stl(std::ostream & outfile, FieldDescription * fdes, ClassValues * in_class);

    /** Prints stl post_checkpoint function */
    void print_post_checkpoint_stl(std::ostream & outfile, FieldDescription * fdes, ClassValues * in_class);

    /** Prints stl restart function */
    void print_restore_stl(std::ostream & outfile, FieldDescription * fdes, ClassValues * in_class);

    /** Prints stl clear function */
    void print_clear_stl(std::ostream & outfile, FieldDescription * fdes, ClassValues * in_class);

    /** Prints stl get_size function */
    void print_get_stl_size(std::ostream & outfile, FieldDescription * fdes, ClassValues * in_class);

    /** Prints stl get_element function */
    void print_get_stl_element(std::ostream & outfile, FieldDescription * fdes, ClassValues * in_class);

    /** Prints stl set_element function (for vector<bool> write-back) */
    void print_set_stl_element(std::ostream & outfile, FieldDescription * fdes, ClassValues * in_class);

    void printStlFunction(const std::string & name,
                          const std::string & parameters,
                          const std::string & call,
                          std::ostream & ostream,
                          FieldDescription & fieldDescription,
                          ClassValues & classValues,
                          const std::string & returnType = "void");

    /** Prints #include statements requried by tempalte arguments for the specified header */
    void print_template_argument_header_dependencies(std::ostream & outfile, std::string header_file_name);

    /** Struct contains the header path and whether it's printed already */
    struct HeaderInfo
    {
        std::string header_path;
        bool printed;

        HeaderInfo(const std::string & path)
            : header_path(path),
              printed(false)
        {
        }

        // comparison operator for std::set
        bool operator<(const HeaderInfo & other) const
        {
            return header_path < other.header_path;
        }
    };

    /** The key is the header that have template argument header dependencies */
    std::map<std::string, std::set<HeaderInfo>> template_argument_header_dependencies;
};

#endif
