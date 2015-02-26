
#include <iostream>
#include <sstream>

#include "ConstructValues.hh"

ConstructValues::ConstructValues() {}

void ConstructValues::setName(std::string in_name) {
    name = in_name ;
}

std::string ConstructValues::getName() {
    return name ;
}

void ConstructValues::setFileName(std::string in_name) {
    file_name = in_name ;
}

std::string ConstructValues::getFileName() {
    return file_name ;
}

void ConstructValues::clearNamespacesAndClasses() {
    namespaces.clear() ;
    container_classes.clear() ;
}

void ConstructValues::getNamespacesAndClasses( const clang::DeclContext * Ctx ) {
    // Save container namespaces and classes.
    typedef clang::SmallVector<const clang::DeclContext *, 8> ContextsTy;
    ContextsTy Contexts;
    // Collect contexts.
    while (Ctx && clang::isa<clang::NamedDecl>(Ctx)) {
        Contexts.push_back(Ctx);
        Ctx = Ctx->getParent();
    };
    for (ContextsTy::reverse_iterator I = Contexts.rbegin(), E = Contexts.rend(); I != E; ++I) {
        if (const clang::NamespaceDecl *nd = clang::dyn_cast<clang::NamespaceDecl>(*I)) {
            if (! nd->isAnonymousNamespace()) {
                //std::cout << "namespace " << nd->getIdentifier()->getName().str() << std::endl ;
                addNamespace(nd->getIdentifier()->getName().str()) ;
            }
        } else if (const clang::RecordDecl *rd = clang::dyn_cast<clang::RecordDecl>(*I)) {
            if (rd->getIdentifier()) {
                //std::cout << "in class " << rd->getName().str() << std::endl ;
                addContainerClass(rd->getName().str()) ;
            }
        }
    }
}

void ConstructValues::addNamespace( std::string in_name ) {
    namespaces.push_back(in_name) ;
}

void ConstructValues::addContainerClass( std::string in_name ) {
    container_classes.push_back(in_name) ;
}

std::string ConstructValues::getFullyQualifiedName() {
    std::ostringstream oss ;
    NamespaceIterator ni ;

    for ( ni = namespace_begin() ; ni != namespace_end() ; ni++ ) {
        oss << (*ni) << "::" ;
    }
    for ( ni = container_class_begin() ; ni != container_class_end() ; ni++ ) {
        oss << (*ni) << "::" ;
    }
    oss << name ;
    return oss.str() ;
}
