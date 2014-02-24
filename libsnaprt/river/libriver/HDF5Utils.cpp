#include "HDF5Utils.hh"
#include <string>
using std::string;

#include <iostream>
using std::cout;
using std::endl;

HDFFile::HDFFile(const char* filename, unsigned flags)
{
    if ( (_hid = H5Fopen ( filename, flags, H5P_DEFAULT )) < 0 ) {
        throw Exception("Unable to open file " + string(filename));
    }
}

HDFFile::~HDFFile()
{
    H5Fclose(_hid);
}

HDFGroup::HDFGroup(hid_t id, const char* name)
{
    if ( (_hid = H5Gopen ( id, name, H5P_DEFAULT )) < 0 ) {
        throw Exception("Unable to open group " + string(name));
    }
}

HDFGroup::~HDFGroup()
{
    H5Gclose(_hid);
}

HDFPT::HDFPT(hid_t id, const char* name)
{
    if ( (_hid = H5PTopen ( id, name)) < 0 ) {
        throw Exception("Unable to open table " + string(name));
    }
}

HDFPT::~HDFPT()
{
    H5PTclose(_hid);
}
