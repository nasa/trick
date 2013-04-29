#include "convutils.hh"

#ifndef WIN32
#    include <sys/time.h>
#    include <unistd.h>
#    include <dirent.h>
#else
#    include <windows.h>
#endif

#include <string>
#include <list>
#include <cstdio>
using std::string;
using std::list;

double Utils::get_Time()
{
#ifndef WIN32
    struct timeval tv;
    gettimeofday(&tv, NULL);
    double thetime = tv.tv_sec;
#else
    double t = timeGetTime();
    double thetime = t/1000;
#endif
	return thetime;
}

void Utils::sleep(double microsecs)
{
#ifndef WIN32
    usleep(microsecs);
#else
    double millisecs = microsecs/1000;
    Sleep(millisecs);
#endif
}

list<string> Utils::getDirEntries(const string& dir)
{
	list<string> entries;
#ifndef WIN32
    DIR *dirp;
    struct dirent *dp;
#else
    HANDLE hFind;
    WIN32_FIND_DATA FindFileData;
#endif

#ifndef WIN32
    // Open up dir
    if ((dirp = opendir(dir.c_str())) == NULL) {
	    return entries;
	}

    while ((dp = readdir(dirp)) != NULL) {
        entries.push_back(dp->d_name);
	}
	closedir(dirp);
#else
    string glob = string(dir) + "/*";
	hFind = FindFirstFile(glob.c_str(), &FindFileData);
    if (hFind != INVALID_HANDLE_VALUE) {
        do {
    	    entries.push_back(FindFileData.cFileName);
        } while (FindNextFile(hFind, &FindFileData));
	}
    if (GetLastError() != ERROR_NO_MORE_FILES) {
        fprintf(stderr, "Error while reading directory %s\n", dir.c_str());
    }
	FindClose(hFind);
#endif
	return entries;
}
