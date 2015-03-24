
#ifndef GP_VIEW_PAGE_NODE_HH
#define GP_VIEW_PAGE_NODE_HH

#include <vector>
#include "gp_view_plot_node.hh"
#include "gp_version.hh"

#include "DPC/DPC_page.hh"
#include "DPV/UTILS/DPV_textbuffer.hh"

extern "C"{
#include <X11/Intrinsic.h>
}

/**
 * This class represents an Gnuplot Page of plots for Trick Data Products.
 * @author John M. Penn
 */
class GPViewPageNode {

public:
    DPC_page *page;                 /**< The source of this page's data. */
    DPV_TextBuffer textbuf;
    Widget toplevel;                /**< This is the toplevel X-application
                                         Widget. */
    Widget dialog_shell_widget;     /**< This is the DialogShell (the window)
                                         for the Page. */
    int n_hcells;
    int n_vcells;
    int layout;
    char *title;
    bool save_tmp_files;
    vector <GPViewPlotNode *> plot_node_list;

    enum Presentation { SIMPLE, COMPARISON, DELTA, STREAM };
    Presentation presentation;
    std::string presentation_txt;

    enum Device { TERMINAL, DISK, PRINTER, NOT_SET };
    Device device;
    std::string device_txt;
    std::string diskFileName;

    enum TerminalType { X11, PS, PS_COLOR, PNG, EPS, AQUA };
    TerminalType terminalType;
    std::string terminalType_txt;

    /**
     * GPViewPageNode constructor.
     * @param Toplevel - the toplevel X-application Widget.
     * @param Table - the DPC_page object from which we get all of the data to
     * create the page.
     * @param Xpos - The X coordinate of the upper left hand corner of the page
     * window.
     * @param Ypos - The Y coordinate of the upper left hand corner of the page
     * window.
     * @param Width - The width of the of the page window.
     * @param Height - The height of the of the page window.
     */
    GPViewPageNode( Widget Toplevel,  DPC_page* Page, int Xpos, int Ypos, int Width, int Height);

    /**
     * GPViewPageNode destructor.
     */
    ~GPViewPageNode();

    void finalize();

private:
    static int instance_count;
};

#endif
