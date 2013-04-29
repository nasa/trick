#ifndef _VSUSERIF_H_
#define _VSUSERIF_H_

#include "vsstructs.hh"

#include <iostream>
using std::cerr;
using std::endl;

/** \ingroup libtvs
 * Abstract notification interface for vsClientImpl users */
class VSUserIF {
  public:
    /// Callback function for updates
    virtual void handler(VSMessage&) //= 0;
    { cerr << "PURE HANDLER" << endl; }
    virtual ~VSUserIF() {}
};

#endif
