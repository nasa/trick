/*
 * $Id: gp_view_product_node.hh 3274 2013-10-29 23:06:24Z jpenn1 $
 */

#ifndef GP_VIEW_PRODUCT_NODE_HH
#define GP_VIEW_PRODUCT_NODE_HH

#include <vector>
#include "DPC/DPC_product.hh"
#include "gp_view_page_node.hh"
#include "table_view_node.hh"

/**
 * This class represents a Trick Data Product.
 * @author John M. Penn
 */
class GPViewProductNode {
public:

    DPC_product *product;
    Widget       toplevel;
    std::vector <GPViewPageNode *> page_node_list;
    std::vector <TableViewNode*> table_node_list;

    GPViewProductNode(Widget Toplevel, DPC_product* Product);

    ~GPViewProductNode();
};

#endif
