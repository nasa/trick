#include "var_binary_parser.hh"
#include <iostream>

int main() {
    ParsedBinaryMessage message;
    Var variable;

    std::vector<unsigned char> bytes = {0x00, 0x00, 0x00, 0xa};
    variable.value_bytes = bytes;

    std::cout << variable.getValue<int>() << std::endl;


}