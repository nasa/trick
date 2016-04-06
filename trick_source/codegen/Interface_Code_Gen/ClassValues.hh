
#ifndef CLASSVALUES_HH
#define CLASSVALUES_HH

#include <string>
#include <vector>
#include <set>
#include <map>
#include <set>
#include "ConstructValues.hh"

class FieldDescription ;

/**

  ClassValues holds information describing a class found with ICG.  The
  information includes the name and location of the class, properties of the
  class, and a vector list of the member data contained in the class.

  The class also provides print routines for each function in the io_src.

  @author Alexander S. Lin

  @date July 2012

 */

class ClassValues : public ConstructValues {
    public:
        ClassValues(bool inherit , bool virtual_inherit) ;

        ~ClassValues() ;

        /** Appends a single field to field_descripts */
        void addFieldDescription(FieldDescription * in_fdes) ;

        /** Appends a vector of fields to field_descripts.
            A vector comes from adding all inherited fields at once */
        void addInheritedFieldDescriptions(std::vector<FieldDescription *>, unsigned int class_offset) ;

        /** Gets the list of fields in this class */
        std::vector<FieldDescription *> getFieldDescription() ;

        void clearFieldDescription() ;

        typedef std::vector< FieldDescription * >::iterator FieldIterator ;
        FieldIterator field_begin() { return field_descripts.begin() ; } ;
        FieldIterator field_end() { return field_descripts.end() ; } ;

        /** Appends an inherited class name to the list this class inherits from */
        void addInheritedClass( ClassValues * in_cv ) ;

        void saveInheritAncestry( ClassValues * in_cv ) ;
        void setContainerClassForFields() ;
        void clearAmbiguousVariables() ;

        void clearInheritedClass() ;

        typedef std::vector< ClassValues * >::iterator InheritedClassesIterator ;
        InheritedClassesIterator inherit_classes_begin() { return inherited_classes.begin() ; } ;
        InheritedClassesIterator inherit_classes_end() { return inherited_classes.end() ; } ;
        unsigned int getNumInheritedClasses() { return inherited_classes.size() ; } ;

        void setInherited(bool in_inh) ;
        bool isInherited() ;
        void setVirtualInherited(bool in_inh) ;
        bool isVirtualInherited() ;
        void setHasInitAttrFriend(bool in_val) ;
        bool getHasInitAttrFriend() ;
        void setPOD(bool in_val) ;
        bool isPOD() ;
        void setAbstract(bool in_val) ;
        bool isAbstract() ;
        void setHasDefaultConstructor(bool in_val) ;
        bool getHasDefaultConstructor() ;
        void setHasPublicDestructor(bool in_val) ;
        bool getHasPublicDestructor() ;
        void setMangledTypeName( std::string in_val ) ;
        bool getInvadePrivacy() ;
        void setInvadePrivacy( bool in_val ) ;
        std::string getMangledTypeName() ;
        std::string getFullyQualifiedMangledTypeName() ;

        void print_namespaces(std::ostream & os, const char * delimiter) ;

        friend std::ostream & operator << (std::ostream & os , ClassValues & cv ) ;

    private:
        /** List of fields (data members) contained in the class */
        std::vector< FieldDescription * > field_descripts ;

        std::map< std::string , FieldDescription * > field_name_to_info_map ;
        std::set< std::string > field_names_to_qualify ;

        /** List of classes we inherit from */
        std::vector< ClassValues * > inherited_classes ;

        /** Map of all inherited classes. Counts how many times a class is inherited. */
        std::map< std::string , unsigned int > all_inherited_class_names_map ;

        /** Is this class being processed as an inherited class */
        bool inherited ;

        /** Is this class being processed as an virtual inherited class */
        bool virtual_inherited ;

        /** Does this class a "friend class init_attr<class_name>" statement */
        bool has_init_attr_friend ;

        /** Is this class plain old data? */
        bool is_pod ;

        /** Is this class abstract? */
        bool is_abstract ;

        /** Does this class have a default constructor? */
        bool has_default_constructor ;

        /** Does this class have a public destructor? */
        bool has_public_destructor ;

        /** Mangled type name. Templates will have a mangled_type_name.  */
        std::string mangled_type_name ;
} ;

#endif
