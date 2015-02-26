/*
 * $Id: product_view_node.hh 2806 2013-02-06 15:04:12Z hchen3 $
 */

#ifndef PRODUCT_VIEW_NODE_HH
#define PRODUCT_VIEW_NODE_HH

#include <vector>
#include "DPC/DPC_product.hh"
#include "page_view_node.hh"
#include "table_view_node.hh"

/**
 * This class represents a Trick Data Product.
 * @author John M. Penn
 */
class ProductViewNode {
public:

    DPC_product *product;
    Widget       toplevel;
    std::vector <PageViewNode *> page_node_list;
    std::vector <TableViewNode*> table_node_list;

    static int dpIndex;

    ProductViewNode(Widget Toplevel, DPC_product* Product);

    ~ProductViewNode();

    void generate_PS(const char* base_name);

    void print();

};

#endif
