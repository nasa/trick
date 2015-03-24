
#ifndef POST_DIALOG_H
#define POST_DIALOG_H

#include <Xm/MessageB.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Post an X-Windows Dialog box with the given message.
 * @param parent - parent Widget for the dialog.
 * @param dialog_type - of type XmNdialogType
 * Possible values are:
 * - XmDIALOG_ERROR
 * - XmDIALOG_INFORMATION
 * - XmDIALOG_MESSAGE
 * - XmDIALOG_QUESTION
 * - XmDIALOG_TEMPLATE
 * - XmDIALOG_WARNING
 * - XmDIALOG_WORKING
 * @param msg - message string to be displayed on the dialog.
 */
Widget post_dialog( Widget parent, int dialog_type, const char * msg);

#ifdef __cplusplus
}
#endif
#endif
