#ifndef _VSSTRUCTS_H_
#define _VSSTRUCTS_H_

#include <string>
#include <cstring>
#include <iostream>
#include <iomanip>
#include <algorithm>
using std::cerr;
using std::endl;

using std::string;

/** \ingroup libtvs
 * Encapsulates the state of the sim */
struct VSState {
    VSState():
        is_frozen(false), is_connected(false), is_synchronous(false),
        is_receiving(false), timestamp(0.0), nparams(0), need_update(false)
    {}

    VSState(bool frozen, bool connected, bool synch, bool receiving,
            double time, int params):
        is_frozen(frozen), is_connected(connected), is_synchronous(synch),
        is_receiving(receiving), timestamp(time), nparams(params), 
        need_update(false)
    {}

    bool is_frozen;      ///< True if sim is frozen; false otherwise
    bool is_connected;   ///< True if connected
    bool is_synchronous; /**< True if sim is sending data synchronously
                              False if sim does not support sychronous data */
    bool is_receiving;   ///< True if value thread is receiving data
    double timestamp;    ///< Timestamp of current update
    int nparams;         ///< Number of values param (not used currently)
    bool need_update;    ///< True if status needs to be sent to the client
};

/** \ingroup libtvs
 * Encapsulates a status or value update */
struct VSMessage {
    enum Message_Type {
        VSNONE = 0,      ///< Unknown type
        VSDATA_UPDATE,   ///< Parameter Values update
        VSSTATUS_UPDATE  ///< Status update
    };

    VSMessage(VSState& thestate): state(thestate),
                 type(VSNONE) {}

    VSState& state;  ///< The current state (send even for value updates)
    int type;        ///< Type of update
};

/** \ingroup libtvs
 * Encapsulates a variable server or Trick file parameter.
 * There are two types of TrickParam. Direct TrickParams are expected to occur
 * on the variable server or in the Trick data file being used. Calculated
 * TrickParams are outputs from transforms acting on direct TrickParams as 
 * inputs. Allocation of TrickParams is somewhat complex due to the way 
 * transforms need to be processed. A TrickParam that is specified outside a
 * transform in an API file is simply taken down as a name, and created and 
 * allocated when its availability is verified in the data stream (e.g. by a
 * DataRiver instance). A TrickParam in a transform on the other hand needs to 
 * be created when the transform is created, which is when the API file is 
 * parsed. However, it does not have its vals array allocated until the 
 * availability of all input parameters for the transform is validated. That is,
 * the TrickParam is really used as a TrickParam spec until the data stream 
 * verification takes place. This is because the data stream owner, has the 
 * responsibility of owning the allocated vals array, and cleaning it up when 
 * it is no longer needed.
 */
struct TrickParam {
    enum ParamType {
        DIRECT = 0,     ///< Value retrieved from variable server/file
        CALCULATED = 1, ///< Value calculated via a transform
        CONSTANT = 2,   ///< Constant value specified in API file
        ONESHOT = 3     ///< Value retrieved only once from variable server
    };

    string name;         ///< Parameter name
    double* vals;        ///< Array of parameter values
    std::string units;   ///< Parameter units
    double incoming_val; /**< Storage for incoming value before it is assigned
                              an indexed location in vals */
    ParamType type;      ///< Parameter type

    TrickParam():
        name(), vals(0), units(), incoming_val(0), _size(0)
    {}

    TrickParam(const string& paramname, const ParamType paramtype,
               const std::string& theunits = "", double initvalue = 0): 
                  name(paramname), vals(0), units(theunits), 
                  incoming_val(initvalue), type(paramtype), _size(0)
    {}

    TrickParam(const char* paramname, const ParamType paramtype,
               const std::string& theunits = "", double initvalue = 0): 
                  name(paramname), vals(0), units(theunits),
                  incoming_val(initvalue), type(paramtype), _size(0)
    {}

    /// Equality operator (required for std::map) -- compare only names
    bool operator==(const TrickParam tp) { return name == tp.name; }

    /// Less than operator (required for std::map) -- compare only names
    bool operator<(const TrickParam& tp) { return name.compare(tp.name) < 0; }

    /// Allocate space for a parameter
    void allocate(unsigned int size) { 
       deallocate();
       vals = new double[size]; 
       _size = size;
    }

    /// Deallocate space for a parameter
    void deallocate() { 
       if (vals) {
           delete [] vals;
           vals = 0;
       }
    }

    /// Reallocate space for a parameter
    void reallocate(const unsigned int newsize)
    {
        double* tmp = new double[newsize];
        if (vals) {
            if (newsize > _size) {
                std::copy(vals, vals + _size, tmp);
            } else {
                std::copy(vals, vals + newsize, tmp);
            }
            deallocate();
            vals = tmp;
            _size = newsize;
        } else {
            allocate(newsize);
        }
    }

  private:
    unsigned int _size;  ///< Allocated space in the vals array
};

#endif
