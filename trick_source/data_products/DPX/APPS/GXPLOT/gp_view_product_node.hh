
#ifndef GP_VIEW_PRODUCT_NODE_HH
#define GP_VIEW_PRODUCT_NODE_HH

#include <vector>
#include "DPC/DPC_product.hh"
#include "gp_view_page_node.hh"

/**
 * This class represents a Trick Data Product.
 * @author John M. Penn
 */
class GPViewProductNode {
public:

    DPC_product *product;
    Widget       toplevel;
    std::vector <GPViewPageNode *> page_node_list;

    GPViewProductNode(Widget Toplevel, DPC_product* Product);

    ~GPViewProductNode();
};

#endif
