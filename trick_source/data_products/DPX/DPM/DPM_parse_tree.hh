/*
 * $Id: DPM_parse_tree.hh 1842 2011-08-01 00:02:00Z penn $
 */

#ifndef DPM_PARSE_TREE_HH
#define DPM_PARSE_TREE_HH

#include <iostream>
#include <stdexcept>
#include <libxml/parser.h>
#include <libxml/tree.h>

/**
 * This class represents an XML parse tree.
 * @author John M. Penn
 */
class DPM_parse_tree {

public:
  /**
   * Constructor.
   */
  DPM_parse_tree(const char *XMLFileName) throw (std::invalid_argument);

  /**
   * Destructor
   */
  ~DPM_parse_tree();

  /**
   *
   */
  xmlNode *getRootNode();

  /**
   *
   */
  int is_valid();

private:
    xmlDocPtr  doc;
    int        valid;
    int Initialize(const char *XMLFileName);
};
#endif
