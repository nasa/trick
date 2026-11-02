
#include <stdio.h>
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>

#include "gp_utilities.hh"
#include "DPV/UTILS/DPV_textbuffer.hh"
#include "gp_colors.hh"
#include "gp_view_page_node.hh"

using namespace std;


/*!
 * Filter post script: make lines thinner etc...
 */
int filterPostScriptBW( const char *psName )
{
    DPV_TextBuffer buf;

    //! Populate buf with file content.
    buf.readFile( psName );

    //! If this is not a ps file, don't filter
    if ( strncmp(buf.getText(), "%!PS-Adobe", 10) != 0 ) {
        return (-1);
    }

    //! Do the filtering
    buf.subst_g("gnulinewidth 5.000 def", "gnulinewidth 2.000 def");
    buf.subst_g("/LTa { AL [1 dl 2 dl]",
                "/LTa { AL [1 dl 2 div 2 dl 2 div]");
    buf.subst_g("/LTa { AL [1 udl mul 2 udl mul]",
                "/LTa { AL [1 dl 2 div 2 dl 2 div]");

    //! Write buffer back out to file
    return ( buf.writeFile(psName) );
}

/*!
 * Filter color post script:
 */
int filterPostScriptColor( const char *psName, GPViewPageNode* gp_view_page_node )
{
    DPV_TextBuffer buf;
    DPV_TextBuffer newBuf;
    string tmp_string;
    stringstream tmp_stream;
    string directive;
    string page_fg_color = "";
    string page_bg_color = "";
    int i;
    unsigned int red, blue, green;

    //! Populate buf with file content.
    buf.readFile( psName );

    //! If this is not a ps file, don't filter
    if ( strncmp(buf.getText(), "%!PS-Adobe", 10) != 0 ) {
        return (-1);
    }

    //! Change the linewidth
    buf.subst_g("gnulinewidth 5.000 def", "gnulinewidth 2.000 def");

    //! Change the background color
    if ( gp_view_page_node->page->getAttribute("background_color") ) {
        page_bg_color = gp_view_page_node->page->getAttribute("background_color");
    }
    if ( ! page_bg_color.empty() ) {
        if ( page_bg_color[0] == '#' ) {

            sscanf( page_bg_color.c_str(), "#%02x%02x%02x", &red , &green , &blue );

            if ( gp_view_page_node->terminalType == GPViewPageNode::PS_COLOR ) {
                directive = "Page: 1 1" ;
            } else {
                directive = "EndProlog" ;
            }
            tmp_stream.str("");
            tmp_stream << directive.c_str() << endl;
            tmp_stream << " " << (float)red / 255 ;
            tmp_stream << " " << (float)green / 255 ;
            tmp_stream << " " << (float)blue / 255 ;
            tmp_stream << " setrgbcolor" << endl;
            tmp_stream << "clippath fill" ;

            buf.subst_g( directive.c_str(), tmp_stream.str().c_str() );

        } else {

            for ( i = 0 ; i < num_colors ; i++ ) {

                if ( page_bg_color.compare(color_trans[i].color) == 0 ) {

                    if ( gp_view_page_node->terminalType == GPViewPageNode::PS_COLOR ) {
                        directive = "Page: 1 1" ;
                    } else {
                        directive = "EndProlog" ;
                    }
                    tmp_stream.str("");
                    tmp_stream << directive.c_str() << endl;
                    tmp_stream << " " << color_trans[i].red ;
                    tmp_stream << " " << color_trans[i].green ;
                    tmp_stream << " " << color_trans[i].blue ;
                    tmp_stream << " setrgbcolor" << endl;
                    tmp_stream << "clippath fill" ;

                    buf.subst_g( directive.c_str(), tmp_stream.str().c_str() );

                    break ;
                }
            }
        }
    }

    //! Change the foreground color
    if ( gp_view_page_node->page->getAttribute("foreground_color") ) {
        page_fg_color = gp_view_page_node->page->getAttribute("foreground_color");
    }
    if ( ! page_fg_color.empty() ) {
        if ( page_fg_color[0] == '#' ) {

            sscanf( page_fg_color.c_str(), "#%02x%02x%02x", &red , &green , &blue );

            tmp_stream.str("");
            tmp_stream << "/LTa { AL [1 dl 2 div 2 dl 2 div] 0 setdash" ;
            tmp_stream << " " << (float)red / 255 ;
            tmp_stream << " " << (float)green / 255 ;
            tmp_stream << " " << (float)blue / 255 ;

            buf.subst_g( "/LTa { AL [1 dl 2 dl] 0 setdash 0 0 0",
                         tmp_stream.str().c_str() );
            buf.subst_g( "/LTa { AL [1 udl mul 2 udl mul] 0 setdash 0 0 0",
                         tmp_stream.str().c_str() );

            tmp_stream.str("");
            tmp_stream << "/LTb { BL []" ;
            tmp_stream << " " << (float)red / 255 ;
            tmp_stream << " " << (float)green / 255 ;
            tmp_stream << " " << (float)blue / 255 ;

            buf.subst_g( "/LTb { BL [] 0 0 0", tmp_stream.str().c_str() );

        } else {

            for ( i = 0 ; i < num_colors ; i++ ) {

                if ( page_fg_color.compare(color_trans[i].color) == 0 ) {

                    tmp_stream.str("");
                    tmp_stream << "/LTa { AL [1 dl 2 div 2 dl 2 div] 0 setdash";
                    tmp_stream << " " << color_trans[i].red ;
                    tmp_stream << " " << color_trans[i].green ;
                    tmp_stream << " " << color_trans[i].blue ;

                    buf.subst_g( "/LTa { AL [1 dl 2 dl] 0 setdash 0 0 0",
                                tmp_stream.str().c_str() );
                    buf.subst_g( "/LTa { AL [1 udl mul 2 udl mul] 0 setdash 0 0 0",
                                tmp_stream.str().c_str() );

                    tmp_stream.str("");
                    tmp_stream << "/LTb { BL []";
                    tmp_stream << " " << color_trans[i].red ;
                    tmp_stream << " " << color_trans[i].green ;
                    tmp_stream << " " << color_trans[i].blue ;

                    buf.subst_g("/LTb { BL [] 0 0 0", tmp_stream.str().c_str());

                    break ;
                }
            }
        }
    }

    //! Write buffer back out to file
    return ( buf.writeFile(psName) );
}

/*!
 * Generate and print postscript file.
 */
int printPS( string psName )
{

    string printerName;
    string printCmd;
    stringstream cmd;

    //! Get print command
    printCmd = getenv("TRICK_PRINT_CMD");
    if ( printCmd.empty() ) {
        cerr << "ERROR: Sorry env var TRICK_PRINT_CMD not set." << endl;
        cerr << "       Cannot print until set." << endl;
        return (-1);
    }
    //! Get printer name
    printerName = getenv("TRICK_PRINTER_NAME");
    if ( printerName.empty() ) {
        cerr << "ERROR: Sorry env var TRICK_PRINTER_NAME not set." << endl;
        cerr << "       Cannot print until set." << endl;
        return (-1);
    }
    //! Build print command
    if ( printCmd.compare("lpr") == 0 ) {
        cmd << printCmd << " -P" << printerName << " " << psName;
    } else if ( printCmd.compare("lp") == 0 ) {
        cmd << printCmd << " -d" << printerName << " " << psName;
    } else {
        //! Assume print command is ready for printer name appendage.
        cmd << printCmd << " " << printerName << " " << psName;
    }

    //! Actually print
    cout << "Printing : " << psName << "..." << endl;
    cout.flush();
    return ( system(cmd.str().c_str()) );

}

int printPS( const char *psName ) {
    string str_ = psName;
    return( printPS(str_) );
}
