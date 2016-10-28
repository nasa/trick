
#ifndef CONSTRUCTVALUES_HH
#define CONSTRUCTVALUES_HH

#include <string>
#include <vector>
#include <utility>

#include "clang/AST/Decl.h"
#include <clang/AST/PrettyPrinter.h>
#include <clang/Basic/LangOptions.h>

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

        const std::vector<std::string>& getNamespaces() {
            return namespaces;
        }


        void addContainerClass(std::string in_name) ;

        const std::vector<std::string>& getContainerClasses() {
            return container_classes;
        }

        std::string getFullyQualifiedName(const std::string& delimiter = "::") ;

        void printOpenNamespaceBlocks(std::ostream& ostream);
        void printCloseNamespaceBlocks(std::ostream& ostream);
        void printNamespaces(std::ostream& ostream, const std::string& delimiter = "::") ;
        void printContainerClasses(std::ostream& ostream, const std::string& delimiter = "::") ;
        void printNamespacesAndContainerClasses(std::ostream& ostream, const std::string& delimiter = "::") ;
        std::string getNamespacesAndContainerClasses(const std::string& delimiter = "::") ;
        std::string getFullyQualifiedTypeName(const std::string& delimiter = "::")  ;

    protected:

        /** Name of the construct */
        std::string name ;

        /** List of namespaces this class is contained within */
        std::vector<std::string> namespaces ;

        /** List of container classes this construct is contained within */
        std::vector<std::string> container_classes ;

        /** File where construct is defined */
        std::string file_name ;

        const clang::PrintingPolicy printingPolicy = clang::PrintingPolicy(clang::LangOptions());

} ;

#endif
