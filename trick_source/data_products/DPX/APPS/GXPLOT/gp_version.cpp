#include "gp_version.hh"

#include <iostream>
#include <set>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <string>

using namespace std;

/*
 * Exit with a clear error if gnuplot is not on PATH; otherwise later checks
 * silently misreport (popen succeeds even when the command does not exist).
 */
static void RequireGnuplot()
{
    static bool checked = false;
    if (checked)
    {
        return;
    }
    checked = true;
    if (system("command -v gnuplot > /dev/null 2>&1") != 0)
    {
        cerr << "ERROR: gnuplot not found. Please install gnuplot "
             << "and make sure it is on your PATH." << endl;
        exit(-1);
    }
}

double GnuplotVersion() {
    FILE* pipe;
    char buf[256];
    double version = -1.0;
    int ret;

    buf[0] = '\0';

    RequireGnuplot();

    if ((pipe = popen("gnuplot --version 2> /dev/null", "r")) != NULL)
    {
        fgets(buf, 255, pipe);

        // In gnuplot 3.x, the gnuplot --version command
        // causes an error, so return 3.0 if buf[0] != 'g'
        if (buf[0] != 'g')
        {
            pclose(pipe);
            return (3.0);
        }
        // Okay in 4.0
        // %> gnuplot --version
        // %> gnuplot 4.0 patchlevel 0
        // For now assume it will stay like this
        ret = sscanf(buf, "%*s %lf", &version);
        if (ret == 1)
        {
            pclose(pipe);
            return (version);
        }
        else
        {
            cerr << "ERROR: getGnuplotVersion() : Died while "
                 << "\" scanning gnuplot --version\".  Aborting." << endl;
            exit(-1);
        }
        } else {
                cerr << "ERROR: getGnuplotVersion() : Died while running "
                     << "\"gnuplot --version\".  Aborting."
                     << endl ;
                pclose(pipe);
                exit(-1);
        }
        // This just makes the compiler happy,
        // it will not be executed
        return( version ) ;
}

static const std::set<std::string>& GnuplotTerminals()
{
    static std::set<std::string> terminals;
    static bool initialized = false;

    if (!initialized)
    {
        initialized = true;
        RequireGnuplot();
        FILE* pipe;
        char buf[512];
        if ((pipe = popen("gnuplot -e 'set terminal' 2>&1 < /dev/null", "r")) != NULL)
        {
            while (fgets(buf, sizeof(buf), pipe) != NULL)
            {
                // Terminal lines look like: "             x11  X11 Window System ..."
                if (buf[0] != ' ' && buf[0] != '\t')
                {
                    continue;
                }
                std::istringstream iss(buf);
                std::string name;
                if (iss >> name)
                {
                    terminals.insert(name);
                }
            }
            pclose(pipe);
        }
    }
    return terminals;
}

bool GnuplotHasTerminal(const char* term)
{
    const std::set<std::string>& terminals = GnuplotTerminals();
    // If the terminal list could not be determined, assume the terminal exists (historical behavior).
    return terminals.empty() || terminals.count(term) > 0;
}

std::string GnuplotInteractiveTerminal()
{
    if (GnuplotTerminals().empty())
    {
        // Using static flag to warn only once
        // This flag is in the block scope
        static bool warned = false;
        if (!warned)
        {
            warned = true;
            cerr << "WARNING: could not determine available gnuplot terminals; "
                 << "assuming x11." << endl;
        }
        return "x11";
    }
#if defined(__APPLE__)
    // On macOS prefer qt/wxt, which need no extra app; x11 requires XQuartz, aqua requires AquaTerm.
    static const char* candidates[] = { "qt", "wxt", "x11", "aqua" };
#else
    // x11 first preserves -geometry and -xrm color support.
    static const char* candidates[] = { "x11", "qt", "wxt" };
#endif
    for (size_t ii = 0; ii < sizeof(candidates) / sizeof(candidates[0]); ii++)
    {
        if (GnuplotHasTerminal(candidates[ii]))
        {
            return candidates[ii];
        }
    }
    // Using static flag to warn only once
    // This flag is in the function scope
    static bool warned = false;
    if (!warned)
    {
        warned = true;
        cerr << "WARNING: gnuplot has no interactive terminal (";
        for (size_t ii = 0; ii < sizeof(candidates) / sizeof(candidates[0]); ii++)
        {
            cerr << (ii ? ", " : "") << candidates[ii];
        }
        cerr << "); plots cannot be displayed on screen." << endl;
    }
    return "x11";
}
