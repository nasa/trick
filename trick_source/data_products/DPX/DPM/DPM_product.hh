
#ifndef DPM_PRODUCT_HH
#define DPM_PRODUCT_HH

#include <stdexcept>
#include "DPM/DPM_component.hh"
#include "DPM/DPM_extfn.hh"
#include "DPM/DPM_page.hh"
#include "DPM/DPM_table.hh"

/**
 * This class represents a description of a Trick Data Product. A Trick Data
 * Product is a collection of data plots and data tables, displayed on one
 * or more pages (or windows). This class does not represent the data product
 * itself. It represents a recipe for creating a data product.
 * @author John M. Penn
 */
class DPM_product : public DPM_component {

public:
  /**
   * XML Constructor.
   */
  DPM_product(DPM_component *parent, xmlNode *product_node) ;

  /**
   * Destructor
   */
  ~DPM_product();

  /**
   * Return the title of the product, if it has one, otherwise return NULL.
   */
  const char *getTitle();

  /**
   * Return a pointer to the DPM_time_constraints object.
   */
  DPM_time_constraints *getTimeConstraints();

  /**
   * Returns the number of DPM_page objects contained by this DPM_product.
   */
  int NumberOfPages();

  /**
   * Return a pointer to the i'th DPM_page object contained in the DPM_product.
   * @param index Ranging from 0 to the number of pages minus one. If the index
   * is invalid, this function returns NULL;
   */
  DPM_page *getPage( unsigned int index);

  /**
   * Returns the number of DPM_table objects contained by this DPM_product.
   */
  int NumberOfTables();

  /**
   * Return a pointer to the i'th DPM_table object contained in the DPM_product.
   * @param index Ranging from 0 to the number of tables minus one. If the index
   * is invalid, this function returns NULL;
   */
  DPM_table *getTable( unsigned int index);

  /**
   * Returns the number of DPM_extfn objects contained by this DPM_product.
   */
  int NumberOfExtFns();

  /**
   * Return a pointer to the i'th DPM_extfn object contained in the DPM_product.
   * @param index Ranging from 0 to the number of pages minus one. If the index
   * is invalid, this function returns NULL;
   */
  DPM_extfn *getExtFn( unsigned int index);

  /**
   * Output an XML representation of an DPM_product object.
   * @param s is the ostream to which this function writes.
   * @param product is a pointer to the DPM_product object whose representation
   * is to be written.
   */
  friend std::ostream& operator<< (std::ostream& s, const DPM_product *product);

private:

  char *title;
  DPM_time_constraints *time_constraints;
  std::vector <DPM_extfn *> extfn_list;
  std::vector <DPM_page *> page_list;
  std::vector <DPM_table *> table_list;

  int Initialize(xmlNode *product_elem_node);
};
#endif
