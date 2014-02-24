#include "sniffline.hh"
#include "../libtc/tokenizer.hh"
#include <string>
#include <sstream>
#include <vector>
using std::string;
using std::vector;
using std::stringstream;

#include <iostream>
using std::cerr;
using std::endl;

SniffLine::SniffLine(const string& line): _isvalid(true)
{
    vector<string> words = Tokenizer::tokenize(line, "\t ");
    if (_isRemote(line)) {
        if (words.size() >= 5) {
            stringstream strm;
            _sniffspec = words[0];
            _host      = words[1];
            _port      = words[2];
            _user      = words[3];
            _process   = words[4];
            _version   = words[8];
        } else {
            _isvalid = false;
        }
    } else {
        if (words.size() >= 4) {
            stringstream strm;
            _sniffspec = "";
            _host      = words[0];
            _port      = words[1];
            _user      = words[2];
            _process   = words[3];
            _version   = words[7];
        } else {
            _isvalid = false;
        }
    }
}

string SniffLine::serialize()
{
    string result;
    if (_sniffspec != "") {
        result += _sniffspec + " ";
    }
    result += _host + " " + _port + " " + _version + " " + _user 
              + " " + _process;
    return result;
}
