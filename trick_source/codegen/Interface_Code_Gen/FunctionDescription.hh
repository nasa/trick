#ifndef __FUNCTION_DESCRIPTION_HH__
#define __FUNCTION_DESCRIPTION_HH__

#include <string>
#include <vector>
#include <utility>

#include "clang/Basic/Specifiers.h"

struct FunctionArgument
{
    std::string typeName;
    std::string argName;
    bool isPointer;
    bool isReference;
    bool isConst;
};


class FunctionDescription
{
    public:
    FunctionDescription();
    FunctionDescription(std::string& container_class);
    ~FunctionDescription();


    void setReturnType(std::string return_type_name);
    
    void setContainerClass(std::string& container_class);
    void setInherited(bool inherited);
    void setVirtualInherited(bool virtual_inherited);
    void setStatic(bool is_static);
    void setIsConstructor(bool is_constructor);
    
    
    std::string getMangledName();

    void addFunctionArgument(FunctionArgument arg);
    
    std::string getReturnType() const { return return_type_name; }

    inline std::vector<FunctionArgument>& getFunctionArgs();

    inline void setFunctionName(std::string in_function_name);
    inline void setAccess(clang::AccessSpecifier in_access);
    inline void setIsDefaultConstructor(bool is_default);

    inline bool getIsDefaultConstructor() const;
    inline std::string getFunctionName() const;
    inline std::string getContainerClass() const;

    bool getIsPublic() const;

    private:
    /** Line number in current file where field is */
    unsigned int line_no ;

    /** Name of the class this field is in */
    std::string container_class ;

    std::string return_type_name ;

    std::string function_name;

    std::vector<FunctionArgument> function_args;



    /** public/protected/private */
    clang::AccessSpecifier access ;

    /** is this function inherited from parent class */
    bool is_inherited ;

    /** is this function virtual inherited from parent class */
    bool is_virtual_inherited ;

    /** is this field declared static */
    bool is_static ;

    bool is_constructor ;

    bool is_default_constructor ;


    private:

    std::string removePointerAndReference(const std::string& typeStr);
};


inline std::string FunctionDescription::getContainerClass() const {return container_class; }

inline std::vector<FunctionArgument>& FunctionDescription::getFunctionArgs() { return function_args; }

inline void FunctionDescription::setFunctionName(std::string in_function_name) { function_name = in_function_name; }

inline std::string FunctionDescription::getFunctionName() const { return function_name; }

inline void FunctionDescription::setAccess(clang::AccessSpecifier in_access) { access = in_access;}

inline bool FunctionDescription::getIsDefaultConstructor() const {return is_default_constructor;}

inline void FunctionDescription::setIsDefaultConstructor(bool is_default) { is_default_constructor = is_default; }

#endif // __FUNCTION_DESCRIPTION_HH__