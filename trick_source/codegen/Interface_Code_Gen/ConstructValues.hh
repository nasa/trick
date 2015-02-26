
/*
 * $Id:$
 */

#ifndef _CONSTRUCTVALUES_HH_
#define _CONSTRUCTVALUES_HH_

#include <string>
#include <vector>
#include <utility>

#include "clang/AST/Decl.h"

/**

  ConstructValues is a base class that contains information common to both
  classes and enumerations.  The information includes the name and location
  of the construct, as well as the container namespaces and classes.

  @author Alexander S. Lin

  @date July 2012

 */

class ConstructValues {
    public:

        ConstructValues() ;

        void setName(std::string in_name) ;
        std::string getName() ;

        void setFileName(std::string in_name) ;
        std::string getFileName() ;

        /** Clears current namespaces and classes */
        void clearNamespacesAndClasses() ;

        /** Gets all of the container namespaces and classes this construct resides in */
        void getNamespacesAndClasses( const clang::DeclContext * Ctx ) ;

        void addNamespace(std::string in_name) ;

        typedef std::vector< std::string >::iterator NamespaceIterator ;
        NamespaceIterator namespace_begin() { return namespaces.begin() ; } ;
        NamespaceIterator namespace_end() { return namespaces.end() ; } ;

        void addContainerClass(std::string in_name) ;

        typedef std::vector< std::string >::iterator ContainerClassIterator ;
        ContainerClassIterator container_class_begin() { return container_classes.begin() ; } ;
        ContainerClassIterator container_class_end() { return container_classes.end() ; } ;

        std::string getFullyQualifiedName() ;

    protected:

        /** Name of the construct */
        std::string name ;

        /** List of namespaces this class is contained within */
        std::vector<std::string> namespaces ;

        /** List of container classes this construct is contained within */
        std::vector<std::string> container_classes ;

        /** File where construct is defined */
        std::string file_name ;

} ;

#endif
