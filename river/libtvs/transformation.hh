#ifndef __TRANSFORMATION_HH
#define __TRANSFORMATION_HH

#include <vector>
#include <map>
#include <string>
#include <algorithm>
using std::vector;
using std::map;
using std::string;

#include "vsstructs.hh"
#include "../libutils/validatedobject.hh"

/** \ingroup libtvs
 * Encapsulates a set of input or output parameters used in a transform.
 * Values supports "aliasing" parameters for ease of use and genericity. That
 * is, rather than having to hardcode references to a specific variable server
 * parameter into a transform, the parameter can be given an alias in the API
 * file, and referenced by alias in the transform code. For example, rather 
 * than:
 *    void degs2rads(Values& inputs, Values& output) {
 *        outputs["my.specific.outputinrads"] 
 *                       = inputs["my.specific.param.name[3]"]/180*M_PI;
 *    }
 * we can code:
 *    void degs2rads(Values& inputs, Values& output) {
 *        outputs["rads"] = inputs["degs"]/180*M_PI;
 *    }
 * Not only is the transform shorter to write and more comprehensible With the 
 * alias, but now degs2rads can be used any time a degrees to radians 
 * conversion is needed -- the input parameter just needs to be aliased as
 * "degs" in the API file, and the output parameter aliased as "rads".
 */
class Values {
  public:
    Values() {}
    /// Get list of param names
    const vector<string>& names() { return _names; } 
   
    /// Add a parameter to the list
    void addParam(const TrickParam& param, const string& alias);

    /// Get a parameter value by name
    double& operator[](const string& name);

    /// Get a parameter's units by name
    string getUnits(const string& name);

    /// Set a parameter's units by name
    void setUnits(const string& name, const string& units);

    /// Check whether a name is currently in the list of parameters
    bool validName(const string& name) { 
        return (_values.count(name) == 1) || (_aliases.count(name) == 1); 
    }
    
    /// Get a whole param by name
    TrickParam& getParam(const string& name);

    /// For all params, copy the time-indexed value to the incoming value
    void setCurrent(const int index);

    /// For all params, copy the incoming value to the time-indexed value
    void setFromCurrent(const int index);

  private:
    /// Find a param's proper name (not alias)
    string _resolveName(const string& incomingName);
    map<string, TrickParam> _values; ///< Name to param mapping
    map<string, string> _aliases;    ///< Alias to name mapping
    vector<string> _names;           ///< List of names (excluding aliases)
   
};

/** \ingroup libtvs
 * Contains input and output parameters, and a transformation function
 * that maps inputs to outputs.
 * The process of invoking a transform is slightly unusual because the 
 * transform does not own the inputs or outputs. At construction time, the 
 * transform is given a function to invoke. Input and output parameters are 
 * then added by reference as TrickParams. Each TrickParam has an "incoming"
 * value, and an array of timestamp-referenced values (see DataRiver for
 * details). The transform is invoked with an index, which indicates which 
 * timestamp-referenced entry to use for the output. Though this system may
 * seem cumbersome, it is actually very efficient since it means that no
 * additional space is needed to store calculations, and values are instantly
 * available without having to be copied over.
 *
 * Transformations are constructed using a function name which may be loaded 
 * from a .so by specifying a libname, or may be defined as global in another 
 * module without being visible to the calling party, or may be a visible 
 * global function. In all cases, the code containing the function should be 
 * compiled with -rdynamic to ensure that the linker exports the symbol.
 *
 * A valid transform function needs to have a signature that matches the
 * Transform typedef.
 */
class Transformation: public ValidatedObject {
  public:

    typedef void (*Transform)(Values& ins, Values& outs);

    /// Transformation using a function name
    Transformation(const string& funcname, const string& libname = "");

    /// Transformation using a global function
    Transformation(Transform usertransform);

    /// Close open dl
    void close();

    /// Invoke the transform
    void callTransform(const int index);

    /// Add an input parameter
    void addInparam(const TrickParam& param, const string alias = "");

    /// Add an output parameter
    void addOutparam(const TrickParam& param, const string alias = "");

    /// Retrieve an input parameter
    TrickParam& getInparam(const string& name);

    /// Retrieve an output parameter
    TrickParam& getOutparam(const string& name);

    /// Retrieve all input parameter names
    const vector<string>& inparams() { return _inputs.names(); }

    /// Retrieve all output parameter names
    const vector<string>& outparams() { return _outputs.names(); }

    /// Retrieve the name of the transform function
    const string funcname() { return _funcname; }

  private:
    void _openTransformation(const string& libname);

    Values _inputs;         ///< Input parameters
    Values _outputs;        ///< Output parameters
    string _funcname;       ///< Transform function name
    Transform _transform;   ///< Transform function
    void* _progHandle;      ///< Handle to opened .so (NULL if not used)
};
#endif
