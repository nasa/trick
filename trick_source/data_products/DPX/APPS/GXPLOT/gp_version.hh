
#ifndef GP_VERSION_HH
#define GP_VERSION_HH

#include <string>

double GnuplotVersion();

/*
 * True if the installed gnuplot was built with the given terminal (e.g. "x11").
 * Also true for any terminal if gnuplot's terminal list could not be determined,
 * so callers fall back to historical (x11) behavior.
 */
bool GnuplotHasTerminal(const char* term);

/*
 * Interactive terminal to use for on-screen plots: the first available of
 * "qt", "wxt", "x11", "aqua" on macOS, or "x11", "qt", "wxt" elsewhere.
 * Returns "x11" (with a warning if none is available) when nothing matches.
 */
std::string GnuplotInteractiveTerminal();

#endif
