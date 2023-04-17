# define Python user-defined exceptions
class RequiredParamException(Exception):
    "Raised when a parameter from a YAML file must exist but doesn't"
    pass
class IncorrectYAMLException(Exception):
    "Raised when a YAML file does not meet expected format"
    pass
