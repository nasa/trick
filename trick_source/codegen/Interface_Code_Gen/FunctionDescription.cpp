#include "FunctionDescription.hh"


FunctionDescription::FunctionDescription() :
    container_class(container_class),
    is_inherited(false),
    is_virtual_inherited(false),
    is_static(false),
    is_constructor(false),
    is_default_constructor(false)
{

}



FunctionDescription::FunctionDescription(std::string& container_class) :
    container_class(container_class),
    is_inherited(false),
    is_virtual_inherited(false),
    is_static(false),
    is_constructor(false),
    is_default_constructor(false)
{

}

FunctionDescription::~FunctionDescription()
{

}


void FunctionDescription::setContainerClass(std::string& container_class)
{
    this->container_class = container_class;
}

void FunctionDescription::setInherited(bool inherited)
{
    this->is_inherited = inherited;
}

void FunctionDescription::setVirtualInherited(bool virtual_inherited)
{
    this->is_virtual_inherited = virtual_inherited;
}

void FunctionDescription::setStatic(bool is_static)
{
    this->is_static = is_static;
}

void FunctionDescription::setIsConstructor(bool is_constructor)
{
    this->is_constructor = is_constructor;
}

void FunctionDescription::setReturnType(std::string return_type_name)
{
    this->return_type_name = return_type_name;
}

std::string FunctionDescription::getMangledName()
{
    std::string base = this->function_name;

    for(auto& arg : this->function_args)
    {   
        base += "_" + removePointerAndReference(arg.typeName);
    }

    return base;
}

void FunctionDescription::addFunctionArgument(FunctionArgument arg)
{
    function_args.emplace_back(arg);
}


bool FunctionDescription::getIsPublic() const
{

    return this->access ==  clang::AccessSpecifier::AS_public;

}


std::string FunctionDescription::removePointerAndReference(const std::string& typeStr)
{
    std::string result = typeStr;
    // Remove all '*' characters
    result.erase(std::remove(result.begin(), result.end(), '*'), result.end());

    // Remove all '&' characters
    result.erase(std::remove(result.begin(), result.end(), '&'), result.end());

    result.erase(std::remove(result.begin(), result.end(), ' '), result.end());

    return result;

}