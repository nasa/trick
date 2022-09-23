#include "data_products/Log/DataStream.hh"
#include "data_products/Log/DataStreamFactory.hh"

#include <iostream>
#include <string>

/**
 * Provides a means to inactively exercise a DataStream class object.
 */
class DS_exercisor {
public:
  DS_exercisor(DataStream* ds)
    : testds(ds) {}
  ~DS_exercisor() {}
  int run();

private:
  DataStream* testds;
};
