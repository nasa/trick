#ifndef VALIDATEDOBJECT_HH
#define VALIDATEDOBJECT_HH
#include <string>

/** \ingroup libutils
 * This class provides a uniform interface for reporting problems without using
 * exceptions. The primary intended use is to address problems with exception
 * catching on Windows, but it can be used more generally. In particular, 
 * during object construction there is no return value, and it is inconvenient 
 * to pass in an extra string parameter to the constructor just in case there
 * is a problem. Inheriting from this class allows the construction to fail in
 * a testable way (i.e. the client can call isValid to determine whether the
 * constructed object is good or not), and if the construction fails, the
 * constructed object can make a failure reason available.
 *
 * For classes with complex construction needs, this interface also allows the
 * constructed object to build the error string over time, providing more detail
 * on the failure point.
 */
class ValidatedObject {
  public:
    ValidatedObject(): _isValid(true) {}
        /// Pure virtual destructor to enforce the abstract nature of the class.
    virtual ~ValidatedObject() = 0; 
    bool isValid() { return _isValid; }
    std::string errorString() { return _error; }

  protected:
    void setError(const std::string& error) { _error = error; _isValid = false; }
  private:
    bool _isValid;
    std::string _error;
};

#endif
