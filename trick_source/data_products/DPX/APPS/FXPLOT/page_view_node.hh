
#ifndef PAGE_VIEW_NODE_HH
#define PAGE_VIEW_NODE_HH

#include <vector>
#include "plot_view_node.hh"

#include "DPC/DPC_page.hh"

extern "C"{
#include <X11/Intrinsic.h>
}

/**
 * This class represents an X-Windows Page of plots for Trick Data Products.
 * @author John M. Penn
 */
class PageViewNode {
public:
    DPC_page *page;                    /**< The source of this page's data. */
    Widget toplevel;                   /**< This is the toplevel X-application
                                            Widget. */
    Widget dialog_shell_widget;        /**< This is the DialogShell (the window) for
                                            the Page. */
    Widget main_form_widget;           /**< This is the main Form for the page. */
    Widget ctrls_form_widget;          /**< This is the form on which controls
                                            (the print_button_widget,
                                            expand_plot_button_widget) and the
                                            Trick logo are placed. */
    Widget print_button_widget;        /**< This is the print-button Widget. */
    Widget expand_plot_button_widget;  /**< This is the expand-button Widget. */
    Widget pdf_button_widget;		   /**< This is the pdf-button Widget. */
    Widget trick_label;                /**< This is the label Widget that displays the
                                            Trick logo. */
    Widget plots_form_widget;          /**< This is the form Widget on which
                                            sub-ordinate plots are placed. */
    int n_hcells;
    int n_vcells;
    int layout;
    char *title;
    std::vector <PlotViewNode *> plot_node_list;

    /**
     * PageViewNode constructor.
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
    PageViewNode( Widget Toplevel,  DPC_page* Page, int Xpos, int Ypos, int Width, int Height);

    /**
     * PageViewNode destructor.
     */
    ~PageViewNode();

    void print();
    void generate_PS(const char* ps_file_name);
    void get_cell_geometry( int  number_of_display_plots, int& n_horizontal_cells, int& n_vertical_cells );
    void finalize();
    int exists_program(const char *programName) ;

    static void pdf_cb(Widget w, XtPointer client_data, XtPointer call_data) ;
    static void pdf_cancel_cb(Widget w, XtPointer client_data, XtPointer call_data);
    static void pdf_file_cb(Widget w, XtPointer client_data, XtPointer call_data);

private:

    static int instance_count;
};

#endif
