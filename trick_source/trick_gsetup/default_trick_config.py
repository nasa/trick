default_trick_config = {
    "name": "Trick's Graphical Configuration",
    "landing": {
        "version": "1.0",
        "desc": "This setup guide will allow you to easily see all the options that are available to configure Trick with."
    },
    "sections": {
        "Configuration:": {
            "options": {
                "help" : {
                    "type" : "radio",
                    "options" : {"recursive" : {"desc" : "Display the short help of all the included packages and exit."}, "short" : {"desc" : "Display the short help and exit."}, "empty" : {"desc" : "Display the help and exit.", "label" :"regular"}}
                },
                "version": {
                    "desc": "display version information and exit",
                    "type": "flag",
                    "value": "no"
                },
                "quiet": {
                    "desc": "do not print `checking ...' messages",
                    "label" : "quite mode",
                    "type": "flag",
                    "value": "no"
                },
                "config-file" : {
                    "label" : "configuration cache file",
                    "desc" : "file to cache test results in",
                    "type" : "string"
                },
                "config-cache": {
                    "desc": "set the configuration cache file to 'config.cache'",
                    "type": "flag",
                    "value": "no"
                },
                "no-create": {
                    "desc": "do not create output files",
                    "type": "flag",
                    "value": "no"
                },
                "srcdir": {
                    "desc": "directory to find the sources in [configure dir or '..']",
                    "type": "dir",
                    "value": "",
                    "label": "source directory"
                }
            }
        },
        "Installation directories" : {
            "options" : {
                "prefix" : {
                    "type" : "string",
                    "desc" : "This is where we will install architecture-independent files.  Default value is '/usr/local'."
                },
                "exec-prefix" : {
                    "type" : "string",
                    "desc" : "This is where we will install architecture-dependent files.  Default value is set to [PREFIX].",
                    "label" : "executive prefix"
                }
            }
        },
        "Fine tuning of the installation directories:": {
            "options": {
                "bindir": {
                    "desc": "default is [executive prefix]/bin",
                    "type": "dir",
                    "value": "",
                    "label" : "user executables"
                },
                "sbindir": {
                    "desc": "default value is [executive prefix]/sbin",
                    "type": "dir",
                    "value": "",
                    "label": "system admin executables"
                },
                "libexecdir": {
                    "desc": "default value is [EPREFIX]/libexec",
                    "type": "dir",
                    "value": "",
                    "label":"program executables"
                },
                "sysconfdir": {
                    "desc": "default value is [PREFIX]/etc",
                    "type": "dir",
                    "value": "",
                    "label" : "read-only single-machine data"
                },
                "sharedstatedir": {
                    "desc": "default value is [PREFIX]/com",
                    "type": "dir",
                    "value": "",
                    "label" : "modifiable architecture-independent data"
                },
                "localstatedir": {
                    "desc": "default value is [PREFIX]/var",
                    "type": "dir",
                    "value": "",
                    "label": "modifiable single-machine data"
                },
                "libdir": {
                    "desc": "default value is [EPREFIX]/lib",
                    "type": "dir",
                    "value": "",
                    "label": "object code libraries"
                },
                "includedir": {
                    "desc": "default value is [PREFIX]/include",
                    "type": "dir",
                    "value": "",
                    "label" : "C header files"
                },
                "oldincludedir": {
                    "desc": "default value is [/usr/include]",
                    "type": "dir",
                    "value": "",
                    "label": "C header files for non-gcc"
                },
                "datarootdir": {
                    "desc": "default value is [PREFIX]/share",
                    "type": "dir",
                    "value": "",
                    "label":"read-only arch.-independent data root",
                    "hidden": True
                },
                "datadir": {
                    "desc": "default value is [PREFIX]/share",
                    "type": "dir",
                    "value": "",
                    "label" : "read-only architecture-independent data"
                },
                "infodir": {
                    "desc": "default value is [DATAROOTDIR]/info",
                    "type": "dir",
                    "value": "",
                    "label" : "info documentation"
                },
                "localedir": {
                    "desc": "default value is [DATAROOTDIR]/locale",
                    "type": "dir",
                    "value": "",
                    "label": "locale-dependent data"
                },
                "mandir": {
                    "desc": "default value is [DATAROOTDIR]/man",
                    "type": "dir",
                    "value": "",
                    "label": "man documentation"
                },
                "docdir": {
                    "desc": "default value is [DATAROOTDIR]/doc/trick",
                    "type": "dir",
                    "value": "",
                    "label": "documentation root"
                },
                "htmldir": {
                    "desc": "default value is [DOCDIR]",
                    "type": "dir",
                    "value": "",
                    "label": "html documentation"
                },
                "dvidir": {
                    "desc": "default value is [DOCDIR]",
                    "type": "dir",
                    "value": "",
                    "label": "dvi documentation"
                },
                "pdfdir": {
                    "desc": "default value is [DOCDIR]",
                    "type": "dir",
                    "value": "",
                    "label": "pdf documentation"
                },
                "psdir": {
                    "desc": "default value is [DOCDIR]",
                    "type": "dir",
                    "value": "",
                    "label": "ps documentation"
                }
            }
        },
        "X features:": {
            "options": {
                "x-includes": {
                    "desc": "where to put X include files",
                    "type": "dir",
                    "value": ""
                },
                "x-libraries": {
                    "desc": "where to put X library files",
                    "type": "dir",
                    "value": ""
                }
            }
        },
        "System types:": {
            "options": {
                "build" : {
                    "desc": "configure for building on ",
                    "hidden" : True,
                    "type" : "string"
                },
                "host": {
                    "desc" : "host to corss-compile for",
                    "label" : "corss compile host",
                    "type" : "string"
                }
            }
        },
        "Optional Features:": {
            "options": {
                "disable-option-checking":    {
                    "desc": "--enable and --with options will be ignored if they are not recognized",
                    "type": "flag",
                    "value": "no",
                    "label" : "ignore unrecognized options. "
                },
                "32bit mode": {
                    "desc": "Force Trick and sims to compile 32bit",
                    "type": "flag",
                },
                "enable-offline": {
                    "desc": "Compile Trick in offline mode. Un-tar the trick's-offline directory in TRICK_HOME to use this feature",
                    "type": "flag",
                    "value": "no",
                    "label": "offline mode"
                },
                "disable-civet": {
                    "desc": "Compile Trick without webserver capabilities.",
                    "type": "flag",
                    "label" : "disable civet"
                },
                "disable-java": {
                    "desc": "disable the use java",
                    "type": "flag",
                    "label": "disable java"
                },
                "disable-er7utils": {
                    "desc": "disable the use of er7 utils",
                    "type": "flag",
                    "label": "disable er7utils"
                },
            }
        },
        "Optional Packages:": {
            "options": {
                "with-x": {
                    "desc": "",
                    "type": "flag",
                    "value": "no",
                    "label": "Use the X Window System"
                },
                "with-dmtcp": {
                    "desc": "",
                    "type": "dir",
                    "label": "DMTCP root directory"
                },
                "with-gsl": {
                    "desc": "",
                    "type": "dir",
                    "label": "GSL root directory"
                },
                "with-gtest": {
                    "desc": "",
                    "type": "dir",
                    "label": "GTEST root directory"
                },
                "with-hdf5": {
                    "type": "dir",
                    "label": "HDF5 root directory"
                },
                "with-llvm": {
                    "type": "dir",
                    "label": "LLVM root directory"
                },
                "with-python": {
                    "type": "dir",
                    "label": "python root directory"
                },
                "with-swig": {
                    "desc": "",
                    "type": "dir",
                    "label": "path of directory containing the SWIG executable."
                },
                "with-tpro": {
                    "desc": "",
                    "type": "dir",
                    "label": "TPRO root directory"
                },
                "with-udunits": {
                    "desc": "",
                    "type": "dir",
                    "label": "enable udunits"
                },
                "without-zlib": {
                    "desc": "check to disable zlib usage completely",
                    "type": "flag",
                    "value": "no",
                    "label": "disable zlib"
                },
                "with-zlib": {
                    "desc": "root directory path of zlib installation [defaults to /usr/local or /usr if not found in /usr/local]",
                    "type": "dir",
                    "label": "zlib directory"
                },
                "with-bc635": {
                    "desc": "",
                    "type": "dir",
                    "label": "BC635 root directory"
                },
                "with-prepend-path": {
                    "desc": "prepend this directory to path",
                    "type": "dir",
                    "label": "dir to prepend"
                }
            }
        },
        "Some influential environment variables:": {
            "options": {
                "CC": {
                    "desc": "C compiler command",
                    "type": "envvar",
                    "value": ""
                },
                "CFLAGS": {
                    "desc": "C compiler flags",
                    "type": "envvar",
                    "value": ""
                },
                "CPP": {
                    "desc": "C preprocessor",
                    "type": "envvar",
                    "value": ""
                },
                "CPPFLAGS": {
                    "desc": "(Objective) C/C++ preprocessor flags, e.g. -I<include dir> if you have headers in a nonstandard directory <include dir>",
                    "type": "envvar",
                    "value": ""
                },
                "CXX": {
                    "desc": "C++ compiler command",
                    "type": "envvar",
                    "value": ""
                },
                "CXXCPP": {
                    "desc": "C++ preprocessor",
                    "type": "envvar",
                    "value": ""
                },
                "CXXFLAGS": {
                    "desc": "C++ compiler flags",
                    "type": "envvar",
                    "value": ""
                },
                "JAVA_CC": {
                    "desc": "java compiler command",
                    "type": "envvar",
                    "value": ""
                },
                "JAVA_CC_FLAGS": {
                    "desc": "java compiler flags",
                    "type": "envvar",
                    "value": ""
                },
                "LDFLAGS": {
                    "desc": "linker flags, e.g. -L<lib dir> if you have libraries in a nonstandard directory <lib dir>",
                    "type": "envvar",
                    "value": ""
                },
                "LIBS": {
                    "desc": "libraries to pass to the linker, e.g. -l<library>",
                    "type": "envvar",
                    "value": ""
                }
            }
        }
    }
}