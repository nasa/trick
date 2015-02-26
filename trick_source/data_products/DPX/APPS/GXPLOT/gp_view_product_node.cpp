/*
 * $Id: gp_view_product_node.cpp 1582 2011-05-05 20:30:21Z lin $
 */

#include "gp_view_product_node.hh"


GPViewProductNode::GPViewProductNode(Widget Toplevel, DPC_product* Product) {

    product = Product;
    toplevel = Toplevel;
}


//DESTRUCTOR
GPViewProductNode::~GPViewProductNode() {

    #ifdef DEBUG
    cout << "GPViewProductNode DESTRUCTOR." << endl;
    #endif

    GPViewPageNode  *page_view_node;
    TableViewNode   *table_view_node;
    int n,i;

    n = (int)page_node_list.size();
    for (i=0 ; i<n; i++ ) {
        page_view_node = page_node_list[i];
        if (page_view_node) { delete page_view_node; }
    }


    n = (int)table_node_list.size();
    for (i=0 ; i<n ; i++ ) {
        table_view_node = table_node_list[i];
        if (table_view_node) { delete table_view_node; }
    }
}
