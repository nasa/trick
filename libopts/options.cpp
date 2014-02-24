#include "options.h"

// For sscanf,sprintf's value conversion
#include <stdio.h>
#include <math.h>

// Trying c++ stream classes for formatting std::strings
#include <iostream>
#include <sstream>
using namespace std;

// For exit()
#include <stdlib.h>

Options::Options()
{
}

Options::~Options()
{
}

string Options::usage()
{
    ostringstream os;
    os << endl;
    os << "usage: " <<  _plugin_name << endl;
    std::set<string>::iterator pos;
    for ( pos = _opts.begin(); pos != _opts.end(); ++pos ) {
        string opt = *pos;
        os << right << setw(15) << opt
           << "      "
           << left << setw(60) << _info[opt] << endl;
    }
    os << endl;
    return os.str();
}

#ifdef USE_DOUG_H
// Handle tcl commands:
//    get <option>
//    set <option> <value>
//    options               ; # returns list of options with types
//
// dsp_drawstar get -min-stars
// dsp_drawstar set -max-stars 1000 -min-stars 500
// dsp_video options ; # returns -width <int> -height <int> etc.
int Options::tcl_cmd(void *data, Tcl_Interp *interp,
                       int objc, Tcl_Obj *objv[] )
{
    int ret = TCL_OK;

    Tcl_ResetResult( interp );

    bool is_first = true;
    int ii = 1 ;
    char* cmd = Tcl_GetString( objv[ii] ); ii++;

    if ( !strcmp(cmd,"options") ) {
        Tcl_AppendResult( interp, _options().c_str(), NULL );
        return ret;
    }

    while( ii < objc ) {

        bool ok;
        char* opt =  Tcl_GetString( objv[ii] ); ii++;
        string qopt(opt);

        if ( _opts.find(qopt) == _opts.end() ) {

            fprintf(stderr, "options [error]: tcl command "
                    "\"%s %s\" has no option \"%s\"\n"
                    "    Valid options are: %s\n",
                    Tcl_GetString(objv[0]),
                    Tcl_GetString(objv[1]),
                    opt,
                    _options().c_str());
            ret = TCL_ERROR;
            break;
        }

        if ( !strcmp(cmd,"get") ) {

            string valstr = _get(qopt,&ok);
            if ( !ok ) {
                ret = TCL_ERROR;
                break;
            }
            if ( is_first ) {
                is_first = false ;
            } else {
                // get with multiple options - separate list with spaces
                valstr.insert(0," ");
            }

            Tcl_AppendResult( interp, valstr.c_str(),NULL );

        } else if ( !strcmp(cmd,"set") ) {

            string val(Tcl_GetString(objv[ii])); ii++;
            _set(qopt, val, &ok);
            if ( !ok ) {
                stringstream stream;
                stream << "could not set option=\""
                       << qopt << "\""
                       << " to value=\""
                       << val << "\"";
                string msg(stream.str());
                Tcl_AppendResult( interp, msg.c_str(),NULL );
                ret = TCL_ERROR;
                break;
            }

        } else {
            ret = TCL_ERROR;
            break;
        }
    }

    return ret;
}
#endif // use doug

// Parser assumes -key val pairs (except if type needs more args like rgb)
void Options::parse(int argc, char **argv, const char* plugin_name,
                    bool* ok)
{
    *ok = true;

    _plugin_name = plugin_name;

    const char hyphen('-');

    // Make cmdline into one string
    string cmdline;
    for ( int ii = 1 ; ii < argc; ii++ ) {
        cmdline += argv[ii];
        cmdline += ' '; // single space delimited
    }
    cmdline = cmdline.substr(0,cmdline.length()-1); // trim last space

    //
    // Take cmdline string and extract option/value from it
    // The cmdline string will shrink
    // Once all is extracted, there may be one and only one
    // argument which is not a hyphenated opt/val pair
    // e.g. snap <RUN_dir>
    string unhyphenated_opt;
    set<string>::iterator pos;
    map<string,string>::iterator tpos;
    for (pos = _opts.begin(); pos != _opts.end(); ++pos) {

        string opt = *pos;
        string trimmed_opt(opt);

        // Is opt mandatory and hyphenated
        // (surrounded in <> e.g. <RUN_directory> <-width>)
        // Also trim off <> if present
        bool is_mandatory = false;
        if ( opt.at(0) == '<' && opt.at(opt.length()-1) == '>' ) {
            is_mandatory = true;
            trimmed_opt = opt.substr(1,opt.length()-2);
        }

        // Does opt have a hyphen e.g. -length or [-length] or <-length>
        // NOT something like <RUN_dir>
        bool is_hyphened = false;
        if ( opt.at(0) == hyphen  ||
             (opt.at(0) == '<' && opt.at(1) == hyphen) ||
             (opt.at(0) == '[' && opt.at(1) == hyphen) ) {
            is_hyphened = true;
        } else {
            if ( unhyphenated_opt.length() > 0 ) {
                fprintf(stderr,"%s [error]: while parsing commandline. "
                        "Had problem with option=\"%s\". "
                        "Only one argument is allowed without "
                        "hyphenation.  The first was \"%s\".\n",
                        plugin_name, opt.c_str(),
                        unhyphenated_opt.c_str());
                *ok = false;
                break;
            }
            unhyphenated_opt = opt;
        }

        // Trim off [] chars if present
        if ( opt.at(0) == '[' && opt.at(opt.length()-1) == ']' ) {
            trimmed_opt = opt.substr(1,opt.length()-2);
        }

        // Search for option in commandline
        string::size_type idx = cmdline.find(trimmed_opt);
        if ( idx != string::npos ) {

            //
            // cmdline has option
            //

            // get argidx from argv list
            int argidx = -1;
            for ( argidx = 0; argidx < argc; ++argidx) {
                if ( trimmed_opt == argv[argidx] ) {
                    break;
                }
            }
            if ( argidx == argc ) {
                // This should not happen
                fprintf(stderr, "%s [bad scoobies]: while searching for "
                        "option=%s\n",
                        plugin_name,
                        trimmed_opt.c_str());
                exit(-1);
            }

            // Get string value from args
            string strval;
            if ( _types[opt] != string("QColor") ) {
                // Normal -opt val pair
                if ( argidx+1 < argc ) {
                    strval = argv[argidx+1];
                } else {
                    fprintf(stderr,"%s [error]: while parsing commandline. "
                            "No associated value for option=\"%s\". \n",
                            plugin_name,
                            trimmed_opt.c_str());
                    *ok = false;
                    break;
                }

            } else {
                // -opt r g b
                if ( argidx+3 < argc ) {
                    strval = argv[argidx+1];
                    strval += " ";
                    strval += argv[argidx+2];
                    strval += " ";
                    strval += argv[argidx+3];
                } else {
                    fprintf(stderr,"%s [error]:"
                            " while parsing commandline. "
                            "Color option value spec is wrong "
                            "for option=\"%s\". "
                            "Should be three space delimited values "
                            "for r g b.\n",
                            plugin_name,
                            trimmed_opt.c_str());
                    *ok = false;
                    break;
                }
            }

            // Set option value
            _set(opt,strval,ok);
            if ( *ok == false ) {
                fprintf(stderr,"%s [error]:"
                        " while parsing commandline"
                        " had problem with value=\"%s\""
                        " for option=\"%s\" with type=\"%s\"\n",
                        plugin_name,
                        strval.c_str(), trimmed_opt.c_str(),
                        _types[opt].c_str());
                break;
            }

            // Extract parsed bit from cmdline string
            string::size_type idx2 = cmdline.find(strval,
                                                  idx+trimmed_opt.length());
            if ( idx2 != string::npos ) {
                string s1;
                if ( idx > 0 ) {
                    s1 = cmdline.substr(0,idx-1);
                }
                string::size_type idx3 = idx2+strval.length();
                string s2 = cmdline.substr(idx3,cmdline.length()-idx3);
                cmdline = s1 + s2;
            } else {
                // This should not happen
                fprintf(stderr, "%s [bad scoobies]: while searching for "
                                 "\"%s\" in cmdline=\"%s\"\n",
                        plugin_name,
                        strval.c_str(),
                        cmdline.c_str());
                exit(-1);
            }

        } else {

            //
            // cmdline does not have option
            //

            if ( is_mandatory && is_hyphened ) {
                fprintf(stderr,"%s [error]: while parsing commandline. "
                        "Mandatory option=\"%s\" "
                        "was not specified.\n",
                        plugin_name, trimmed_opt.c_str());
                *ok = false;
                break;
            }
        }
    }

    if ( *ok ) {

        // cmdline string has all hyphened opt/val strings extracted
        // cmdline is either empty or may have
        // standalone argument e.g. <RUN_dir>
        // First, trimoff spaces
        string delims(" ");
        string::size_type idx1 = cmdline.find_first_not_of(delims);
        string::size_type idx2 = cmdline.find_last_not_of(delims);
        if ( idx1 <= idx2 && cmdline.length() > 0 ) {
            cmdline = cmdline.substr(idx1,idx2-idx1+1);
        }

        idx1 = cmdline.find(delims);
        if ( idx1 == string::npos ) {
            // There should be no spaces... cmdline is either empty
            // or has one item

            // If non-hyphenated option is mandatory but not present
            // it is an error
            if ( cmdline.length() == 0 &&
                 unhyphenated_opt.length() > 0 &&
                 unhyphenated_opt.at(0) == '<' &&
                 unhyphenated_opt.at(unhyphenated_opt.length()-1) == '>' ) {
                 fprintf(stderr,"%s [error]: while parsing commandline. "
                        "Mandatory argument for %s not specified. ",
                        plugin_name, unhyphenated_opt.c_str());
                 *ok = false;
            } else {

                // handle non-hyphenated arg  e.g. <RUN_dir>
                // Set option value
                _set(unhyphenated_opt,cmdline,ok);
                if ( *ok == false ) {
                    fprintf(stderr,"%s [error]:"
                            " while parsing commandline"
                            " had problem with value=\"%s\""
                            " for option=\"%s\" with type=\"%s\"\n",
                            plugin_name,
                            cmdline.c_str(),
                            unhyphenated_opt.c_str(),
                            _types[unhyphenated_opt].c_str());
                }
            }

        } else {
            // There should only be one option left at this point
            // It should be the type of option that is not hyphenated
            // like <RUN_dir>
            idx2 = cmdline.find(hyphen);
            if ( idx2 != string::npos ) {
                idx1 = cmdline.find(delims,idx2);
                if ( idx1 == string::npos ) {
                    idx1 = cmdline.length();
                }
                string badopt = cmdline.substr(idx2,idx1-idx2);
                fprintf(stderr,"%s [error]: while parsing commandline. "
                        "Bad option \"%s\" was found. "
                        "Maybe the option was misspelled.\n",
                        plugin_name, badopt.c_str());
            } else {
                fprintf(stderr,"%s [error]: while parsing commandline. "
                        "Could not resolve \"%s\". for option %s. "
                        "Is there some extraneous argument? "
                        "Are there extra spaces for value?\n",
                        plugin_name, cmdline.c_str(),
                        unhyphenated_opt.c_str());
            }
            *ok = false;
        }
    }
}


string Options::_options()
{
    string lopts;

    set<string>::iterator pos;
    map<string,string>::iterator tpos;
    for (pos = _opts.begin(); pos != _opts.end(); ++pos) {
        string opt = *pos;
        bool ok;
        lopts.append(opt);
        lopts.append("<");
        tpos = _types.find(opt);
        string strval = tpos->second;
        lopts.append(strval);
        lopts.append(">=");
        lopts.append(_get(opt,&ok));
        if ( pos != _opts.end() ) {
            lopts.append(" ");
        }
    }

    return lopts;
}


string Options::bool_to_str(const bool* const valptr, bool* ok)
{
    string strval("false");
    *ok = true;

    if ( *valptr == true ) {
        strval = string("true");
    }

    return strval;
}

void Options::str_to_bool(const string& sval, bool* valptr, bool* ok)
{
    *ok = true;
    if ( sval == string("0") || sval == string("false") ) {
        *valptr = false;
    } else if ( sval == string("1") || sval == string("true") ) {
        *valptr = true;
    } else {
        *ok = false;
    }
}

string Options::DougBool_to_str(const DougBool* const valptr, bool* ok)
{
    bool b =  valptr->get();
    return bool_to_str(&b,ok);
}

void Options::str_to_DougBool(const string& sval, DougBool* valptr,
                                 bool* ok)
{
    valptr->set(sval,ok);
}

string Options::CDougBool_to_str(const CDougBool* const valptr, bool* ok)
{
    bool b =  valptr->get();
    return bool_to_str(&b,ok);
}

void Options::str_to_CDougBool(const string& sval, CDougBool* valptr,
                                 bool* ok)
{
    valptr->set(sval,ok);
}

string Options::DougString_to_str(const DougString* const valptr, bool* ok)
{
    *ok = true;
    return valptr->get();
}

void Options::str_to_DougString(const string& sval, DougString* valptr,
                                 bool* ok)
{
    *ok = true;
    valptr->set(sval);
}

string Options::CDougString_to_str(const CDougString* const valptr, bool* ok)
{
    string s =  valptr->get();
    *ok = true;
    return s;
}

void Options::str_to_CDougString(const string& sval, CDougString* valptr,
                                 bool* ok)
{
    *ok = true;
    valptr->set(sval);
}

#ifdef USE_DOUG_H
string Options::DougNode_to_str(const DougNode* const valptr, bool* ok)
{
    *ok = true;
    return valptr->name();
}

void Options::str_to_DougNode(const string& val, DougNode* valptr, bool* ok)
{
    *ok = true;
    DougNode node(val) ;
    if ( node.isNull()) {
        *ok = false;
    } else {
        *valptr = node;
    }
}

string Options::CDougNode_to_str(const CDougNode* const valptr, bool* ok)
{
    return DougNode_to_str(valptr,ok);
}

void Options::str_to_CDougNode(const string& val,CDougNode* valptr, bool* ok)
{
    str_to_DougNode(val,valptr,ok);
}
#endif

string Options::DougHost_to_str(const DougHost* const valptr, bool* ok)
{
    *ok = true;
    return valptr->toString();
}

void Options::str_to_DougHost(const string& val, DougHost* valptr, bool* ok)
{
    *ok = true;
    DougHost host(val);
    if ( host.isNull() ) {
        *ok = false;
    } else {
        *valptr = host;
    }
}

string Options::CDougHost_to_str(const CDougHost* const valptr, bool* ok)
{
    return DougHost_to_str(valptr,ok);
}

void Options::str_to_CDougHost(const string& val,CDougHost* valptr, bool* ok)
{
    str_to_DougHost(val,valptr,ok);
}

string Options::DougColor_to_str(const DougColor* const valptr, bool* ok)
{
    *ok = true;
    stringstream stream;
    stream << valptr->red()   << " "
           << valptr->green() << " "
           << valptr->blue()  << " "
           << valptr->alpha();
    string valstr(stream.str());
    return valstr;
}

void Options::str_to_DougColor(const string& sv, DougColor* valptr, bool* ok)
{
    *ok = true;
    istringstream is(sv);
    vector<string> vcolor;
    string color;
    while ( is >> color ) {
        // Split color delimited by spaces
        // e.g. "255 0 0"
        vcolor.push_back(color);
    }
    if ( vcolor.size() == 3 || vcolor.size() == 4 ) {

        // rgb
        int rr ; int gg; int bb;
        str_to_int(vcolor[0],&rr,ok);
        if ( *ok ) str_to_int(vcolor[1],&gg,ok);
        if ( *ok ) str_to_int(vcolor[2],&bb,ok);
        if ( *ok ) {
            if ( rr < 0 || rr > 255 ||
                 gg < 0 || gg > 255 ||
                 bb < 0 || bb > 255 ) {
                *ok = false;
            } else {
                valptr->set_rgb(rr,gg,bb);
            }
        }

        // rgba
        if ( *ok && vcolor.size() == 4 ) {
            int aa ;
            str_to_int(vcolor[3],&aa,ok);
            if ( *ok ) {
                if ( aa < 0 || aa > 255 ) {
                    *ok = false;
                } else {
                    valptr->set_rgba(rr,gg,bb,aa);
                }
            }
        }

    } else {
        *ok = false;
    }
}

string Options::CDougColor_to_str(const CDougColor* const valptr, bool* ok)
{
    return DougColor_to_str(valptr,ok);
}

void Options::str_to_CDougColor(const string& val,CDougColor* valptr,bool* ok)
{
    str_to_DougColor(val,valptr,ok);
}

bool Options::is_hexadecimal(const string& sval)
{
    string::const_iterator pos;

    if ( sval.length() < 3 ) {
        // can't have 0x
        return false;
    }

    // If not all digits, maybe it's hex e.g. 0xEF
    bool is_hex = false;
    if (sval.at(0) == '0' && ( sval.at(1) == 'X' || sval.at(1) == 'x' )) {
        int ii = 0;
        is_hex = true;
        for (pos = sval.begin(); pos != sval.end(); ++pos,++ii) {
            if ( ii < 2 ) continue; // skip 0x
            if ( ! isxdigit(*pos) ) {
                is_hex = false;
                break;
            }
        }
    }

    return is_hex;
}

// TODO:  check for overflow/underflow (see uint)
void Options::str_to_int(const string& sval, int* valptr, bool* ok)
{
    *ok = true;

    string::const_iterator pos;

    //
    // See if each character is a digit (or sign)
    //
    bool is_digits = true;
    int ii = 0;
    for (pos = sval.begin(); pos != sval.end(); ++pos,++ii) {
        if ( ii == 0 ) {
            if ( *pos == '+' || *pos == '-' ) {
                continue;
            }
        }
        if ( ! isdigit(*pos) ) {
            is_digits = false;
            break;
        }
    }

    // If not all digits, maybe it's hex e.g. 0xEF
    bool is_hex = false;
    if ( !is_digits ) {
        is_hex = Options::is_hexadecimal(sval);
    }

    if ( is_digits || is_hex ) {
        // If sval is all digits, use sscanf()
        int ii;
        int ret = sscanf(const_cast<char *>(sval.c_str()),"%i",&ii);
        if ( !ret || ret == EOF ) {
            *ok = false;
        } else {
            *valptr = ii;
        }
    } else {
        *ok = false;
    }


}

string Options::int_to_str(const int* const valptr, bool* ok)
{
    *ok = true;
    string strval("");
    char buf[256];
    int ret = sprintf(buf, "%d", *valptr);
    if ( ret < 0 ) {
        *ok = false ;
    } else {
        strval = buf;
    }
    return strval;
}

void Options::str_to_uint(const string& sval, uint* valptr, bool* ok)
{
    *ok = true;

    if ( sval.length() > 16 ) {
        // Quick sanity check (also buf will be okay below)
        *ok = false ;
        return ;
    }

    string::const_iterator pos;

    // See if each character is a digit (or signed positive)
    bool is_digits = true;
    int ii = 0;
    for (pos = sval.begin(); pos != sval.end(); ++pos,++ii) {
        if ( ii == 0 &&  *pos == '+') continue;
        if ( ! isdigit(*pos) ) {
            is_digits = false;
            break;
        }
    }

    // If not all digits, maybe it's hex e.g. 0xEF
    bool is_hex = false;
    if ( !is_digits ) {
        is_hex = Options::is_hexadecimal(sval);
    }

    if ( is_digits || is_hex ) {
        // If sval is all digits, use sscanf()
        int ret;
        uint ui;
        if ( is_digits ) {
            char buf[32];
            ret  = sscanf(const_cast<char *>(sval.c_str()),"%u",&ui);
            ret = sprintf(buf, "%u", ui);
            if ( sval != buf ) {
                // Check for overflow
                *ok = false;
            }
        } else {
            if ( sval.length() > 10 ) {
                // Overflow e.g. 0xabcdabcdf
                *ok = false ;
            } else {
                ret  = sscanf(const_cast<char *>(sval.c_str()),"%x",&ui);
            }
        }

        if ( *ok ) {
            if ( !ret || ret == EOF ) {
                *ok = false;
            } else {
                *valptr = ui;
            }
        }

    } else {
        *ok = false;
    }

}

string Options::uint_to_str(const uint* const valptr, bool* ok)
{
    *ok = true;
    string strval("");
    char buf[256];
    int ret = sprintf(buf, "%u", *valptr);
    if ( ret < 0 ) {
        *ok = false ;
    } else {
        strval = buf;
    }
    return strval;
}

void Options::str_to_double(const string& sval, double* valptr, bool* ok)
{
    *ok = true;
    double val;

    string::const_iterator pos;

    // See if each character is a digit
    bool is_chars_valid = true;
    int ii = 0 ;
    bool is_exponent = false;
    bool is_decimal = false;
    for (pos = sval.begin(); pos != sval.end(); ++pos,++ii) {
        if ( ii == 0 ) {
            // sign
            if ( *pos == '+' || *pos == '-' ) {
                continue;
            }
        }
        if ( *pos == '.' ) {
            if ( is_decimal ) {
                is_chars_valid = false; // at least two decimals  e.g. 1.2.6
                break;
            }
            is_decimal = true;
            continue;
        }
        if ( *pos == 'e' || *pos == 'E' ) {
            // exp e.g. 1e14
            if ( ii == 0 ) {
                is_chars_valid = false ; // starts with 'e' e.g "e123"
                break;
            }
            if ( is_exponent ) {
                is_chars_valid = false ; // two 'e's e.g "3e12e4"
                break;
            }

            // get next character should be +,- or digit
            ++pos ; ++ii;

            if ( pos == sval.end() ) {
                is_chars_valid = false; // nothing after 'e' e.g. 1.26e
                break;
            }
            if ( *pos == '+' || *pos == '-' ) {
                ++pos ; ++ii;
                if ( pos == sval.end() ) {
                    is_chars_valid = false; // nothing after 'e+-' e.g. 1.26e+
                    break;
                }

            }
            is_exponent = true;
        }
        if ( ! isdigit(*pos) ) {
            is_chars_valid = false ; // exp, signs and decimal handled above
            break;
        }
    }

    if ( is_chars_valid ) {
        int ret = sscanf(const_cast<char *>(sval.c_str()),"%lf",&val);
        if ( !ret || ret == EOF ) {
            *ok = false;
        } else {
            *valptr = val;
        }
    } else {
        *ok = false;
    }
}

string Options::double_to_str(const double* const valptr, bool* ok)
{
    *ok = true;
    string strval("");
    char buf[256];
    int ret = sprintf(buf, "%g", *valptr);
    if ( ret < 0 ) {
        *ok = false ;
    } else {
        strval = buf;
    }
    return strval;
}

string Options::_get(const string &opt, bool* ok)
{
    string strval("");

    *ok = true;
    map<string,string>::iterator tpos;
    tpos = _types.find(opt);
    if ( tpos == _types.end() ) {
        *ok = false ;
        return strval;
    }

    #undef FOREACH_TYPE
    #define FOREACH_TYPE(T) \
        } else if  ( valtype == string(#T) ) {\
            OPTIONS_GET(T);

    string valtype = _types[opt];
    if ( valtype.length() == 0 ) {
        #include "option_types.h"   // yikes! yes, you saw this correctly
    }

    return strval;
}

void Options::_set(const string &opt, const string &val, bool* ok)
{
    *ok = true;

    map<string,string>::iterator tpos;
    tpos =  _types.find(opt);
    if ( tpos == _types.end() ) {
        *ok = false ;
        return;
    }

    string valtype = _types[opt];

    // Hack, hopefully nobody sees this :O
    #undef FOREACH_TYPE
    #define FOREACH_TYPE(T) \
        } else if  ( valtype == string(#T) ) {\
            OPTIONS_SET(T);

    if ( valtype.length() == 0 ) {
        #include "option_types.h"   // the hack
    }
}

// Function defs for Options.add(type)
#define FOREACH_TYPE(T) OPTIONS_ADD(T)
#include "option_types.h"
