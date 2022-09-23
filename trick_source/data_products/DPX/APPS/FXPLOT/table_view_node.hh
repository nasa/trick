
#ifndef TABLE_VIEW_NODE_HH
#define TABLE_VIEW_NODE_HH

#include "DPC/DPC_table.hh"

extern "C"{
#include <X11/Intrinsic.h>
}

/**
 * This class represents an X-Windows based Table for Trick Data Products.
 * @author John M. Penn
 */
class TableViewNode {
public:
    DPC_table* table;            /**< The source of this table's data. */
    Widget toplevel;             /**< This is the toplevel X-application
                                      Widget. */
    Widget dialog_shell_widget;  /**< This is the DialogShell (the window) for
                                       the Table. */
    Widget main_form_widget;     /**< This is the main Form for the table. */
    Widget ctrls_form_widget;    /**< This is the form on which the SAVE
                                       button and the Trick logo is placed. */
    Widget save_button_widget;   /**< This is the save-button Widget. */
    Widget trick_label;          /**< This is the label Widget that displays the
                                       Trick logo. */
    Widget text_widget;          /**< This is the ScrolledText Widget that
                                       contains the printed table text. */
    char * table_text_buf;       /**< This is a pointer to the text buffer
                                      for the table. It is allocated and
                                      realloced as needed by twprint(). When all
                                      of the printing is done, it is used to
                                      set the text of the text_widget. */
    size_t table_insertion_pos;  /**< This is an index into table_text_buf where
                                      more text should be added by twprint(). */
    size_t table_buf_size;       /**< The size (in bytes) of the buffer
                                       referenced by table_text_buf. */

    /**
     * TableViewNode constructor.
     * @param Toplevel - the toplevel X-application Widget.
     * @param Table - the DPC_table object from which we get all of the data to
     * create the X-window based table.
     * @param Xpos - The X coordinate of the upper left hand corner of the table's
     * X-window.
     * @param Ypos - The Y coordinate of the upper left hand corner of the table's
     * X-window.
     * @param Width - The width of the of the table's X-window.
     * @param Height - The height of the of the table's X-window.
     */
    TableViewNode( Widget Toplevel,
                   DPC_table* Table,
                   int Xpos, int Ypos, int Width, int Height);

    /**
     * TableViewNode destructor.
     */
    ~TableViewNode();

    /**
     * XXX
     */
    void finalize();

private:
    static int instance_count;
};

#endif
