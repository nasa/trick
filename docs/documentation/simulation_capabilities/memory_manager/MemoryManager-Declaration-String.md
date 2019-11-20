### Memory Manager Declaration
A declaration provides a data type description of a chunk of memory.

A MemoryManager declaration consists of four parts from left to right:
1. One type specifier
2. Zero or more asterisks (pointers)
3. Zero or one (variable) names
4. Zero or more bracketed integers 

#### Intrinsic Type Specifiers
"char", "unsigned char", "short", "unsigned short", "int", "unsigned int", "long",
"unsigned", "float", "double", "long long", "unsigned long long", "bool", "wchar_t"

#### User Defined Type Specifiers

```
<user-defined-type> ::= NAME
                      | <user-defined-type> ":" ":" NAME

NAME ::=  [_a-zA-Z][_a-zA-Z0-9:]*
