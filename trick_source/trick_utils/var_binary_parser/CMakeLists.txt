# libtrick_var_binary_parser: parses the variable server's binary message format.
trick_add_library(trick_var_binary_parser STATIC
    EXPORT_NAME var_binary_parser
    SOURCES
        src/var_binary_parser.cc
)
