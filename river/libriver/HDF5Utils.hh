#ifndef HDF5UTILS_HH
#define HDF5UTILS_HH

#include "hdf5/hdf5.h"
#include "hdf5/hdf5_hl.h"
#include "../libutils/exception.hh"

class HDFFile {
  public:
    HDFFile(const char* filename, unsigned flags);
    ~HDFFile();
    hid_t hid() { return _hid; }
  private:
    hid_t _hid;
};

class HDFGroup {
  public:
    HDFGroup(hid_t id, const char* name);
    ~HDFGroup();
    hid_t hid() { return _hid; }
  private:
    hid_t _hid;
};

class HDFPT {
  public:
    HDFPT(hid_t id, const char* name);
    ~HDFPT();
    hid_t hid() { return _hid; }
  private:
    hid_t _hid;
};

#endif
