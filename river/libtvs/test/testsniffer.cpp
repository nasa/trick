#include "../sniffer.hh"
#include "../../libtc/tokenizer.hh"
#include "../../libutils/convutils.hh"
#include <iostream>
#include <string>
#include <vector>
#include <list>
using std::cout;
using std::endl;
using std::string;
using std::vector;
using std::list;

#include <sstream>
using std::stringstream;

void testparse(Sniffer& s, string line)
{
    stringstream strm;
    int port;
    vector<string> words;
    string delimiters = "[]: ";
    words = Tokenizer::tokenize(line, delimiters);
    for (unsigned int ii = 0; ii < words.size(); ii++) {
        if (ii % 2) {
            strm << words[ii];
            strm >> port;
            cout << port << "|";
        } else {
            cout << words[ii] << "|";
        }
    }
    cout << endl;
}

int main(int argc, char* argv[])
{
    list<string> hostnport;
    hostnport.push_back("localhost:9796");
    Sniffer s(hostnport);
    while (1) {
        vector<string> list = s.get_list();
        string serial_list = s.get_serialized_list();
//        cout << "========================" << endl;
//        for (unsigned int ii = 0; ii < list.size(); ii++) {
//            cout << "Item " << ii << ": " << list[ii] << endl;
//        }
        cout << "Serial: " << serial_list;
        Utils::sleep(5000000);
    }
}
