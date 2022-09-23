
#include "post_dialog.h"

static void DestroyWidget_cb( Widget w, XtPointer client_data, XtPointer call_data) {
    XtDestroyWidget(w);
}

Widget post_dialog( Widget parent, int dialog_type, const char * msg) {

    Widget dialog;
    XmString text;

    dialog = XmCreateMessageDialog( parent, "dialog", NULL, 0);
    text = XmStringCreateLocalized( (char *)msg);
    XtVaSetValues( dialog,
                   XmNdialogType, dialog_type,
                   XmNmessageString, text,
                   NULL);
            XmStringFree( text);

            // Remove the Cancel Button from the dialog.
            XtUnmanageChild( XmMessageBoxGetChild( dialog, XmDIALOG_CANCEL_BUTTON));

            // Grey out the Help Button
            XtSetSensitive( XmMessageBoxGetChild( dialog, XmDIALOG_HELP_BUTTON), False);

            // Destroy the widget when the user clicks OK.
            XtAddCallback( dialog, XmNokCallback, DestroyWidget_cb, (XtPointer)NULL);

            XtManageChild( dialog);

            return dialog;
}
