#ifndef VSVERSIONIF_HH
#define VSVERSIONIF_HH

#include "../libtc/hostspec.hh"

class VSVersionIF {
  public:
    VSVersionIF(HostSpec::Protocol version): _version(version) {};
    HostSpec::Protocol version() const { return _version; }
  
  private:
    HostSpec::Protocol _version;
};

#endif
