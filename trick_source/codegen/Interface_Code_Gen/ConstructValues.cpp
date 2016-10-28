
#include <iostream>
#include <sstream>
#include <clang/AST/DeclTemplate.h>

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
                std::string temp_name = nd->getIdentifier()->getName().str() ;
                if ( temp_name.compare("std") and temp_name.compare("__1")) {
                    addNamespace(nd->getIdentifier()->getName().str()) ;
                }
            }
        } else if (const clang::RecordDecl *rd = clang::dyn_cast<clang::RecordDecl>(*I)) {
            if (rd->getIdentifier()) {
                if (const clang::ClassTemplateSpecializationDecl *td = clang::dyn_cast<clang::ClassTemplateSpecializationDecl>(*I)) {
                    std::string text;
                    llvm::raw_string_ostream stream(text);
                    stream << td->getName().str() << "<";
                    const clang::TemplateArgumentList& arguments = td->getTemplateArgs();
                    unsigned end = arguments.size() - 1;
                    for (unsigned i = 0; i < end; ++i) {
                        arguments[i].print(printingPolicy, stream);
                        stream << ", ";
                    }
                    arguments[end].print(printingPolicy, stream);
                    stream << ">";
                    addContainerClass(stream.str());
                }
                else {
                    addContainerClass(rd->getName().str()) ;
                }
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

std::string ConstructValues::getFullyQualifiedName(const std::string& delimiter) {
    std::ostringstream oss;
    printNamespaces(oss, delimiter);
    printContainerClasses(oss, delimiter);
    oss << name;
    return oss.str();
}

void ConstructValues::printOpenNamespaceBlocks(std::ostream& ostream) {
    for (auto name : namespaces) {
        ostream << "namespace " << name << " {" << std::endl;
    }
}

void ConstructValues::printCloseNamespaceBlocks(std::ostream& ostream) {
    for (auto name : namespaces) {
        ostream << "}" << std::endl;
    }
}

void ConstructValues::printNamespaces(std::ostream& ostream, const std::string& delimiter) {
    for (auto name : namespaces) {
        ostream << name << delimiter;
    }
}

void ConstructValues::printContainerClasses(std::ostream& ostream, const std::string& delimiter) {
    for (auto clazz : container_classes) {
        ostream << clazz << delimiter;
    }
}

void ConstructValues::printNamespacesAndContainerClasses(std::ostream& ostream, const std::string& delimiter) {
    printNamespaces(ostream, delimiter);
    printContainerClasses(ostream, delimiter);
}

std::string ConstructValues::getNamespacesAndContainerClasses(const std::string& delimiter) {
    std::string result = "";
    for (auto name : namespaces) {
        result += name + delimiter;
    }
    for (auto clazz : container_classes) {
        result += clazz + delimiter;
    }
    return result;
}

std::string ConstructValues::getFullyQualifiedTypeName(const std::string& delimiter) {
    std::ostringstream oss ;
    printNamespacesAndContainerClasses(oss, delimiter) ;
    oss << name ;
    return oss.str() ;
}
