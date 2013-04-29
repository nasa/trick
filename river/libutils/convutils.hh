#ifndef CONVUTILS_HH
#define CONVUTILS_HH
#include <list>
#include <string>
using std::list;
using std::string;

class Utils {
  public:
    static double get_Time();
    static void sleep(double microsecs);
	static list<string> getDirEntries(const string& dir);
};
#endif