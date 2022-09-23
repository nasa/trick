
#ifndef DPC_TABLE_HH
#define DPC_TABLE_HH

#include <stdexcept>
#include "DPM/DPM_time_constraints.hh"
#include "DPC/DPC_datastream_supplier.hh"
#include "DPM/DPM_run.hh"
#include "DPM/DPM_table.hh"
#include "DPC/DPC_column_info.hh"
#include "DPV/DPV_view.hh"

typedef struct {
   double time;
   double value;
} DPC_datum;

/**
 * This class represents a data table.
 * @author John M. Penn
 */
class DPC_table {

public:

  /**
   * Constructor
   */
     DPC_table( DPC_datastream_supplier *DS_Supplier,
		DPM_run   * Run,
		DPM_table *Table,
		DPM_time_constraints *parent_time_constraints )
              ;

  /**
   * Destructor.
   */
  ~DPC_table();

  /**
   * Return the Run directory for the table.
   */
  const char *getRunDir();

  /**
   * Return the title of the table, which may be NULL.
   */
  const char *getTitle();

  /**
   * Return the number of columns that have been generated for this table.
   */
  int getNumColumns();

  /**
   * Return the Label (which may be NULL) of the of the indicated column.
   */
  const char *getColumnLabel(unsigned int index);

  /**
   * Return the name of the variable for the indicated column.
   */
  const char *getColumnVarName(unsigned int index);

  /**
   * Return the name of the units for the indicated column.
   */
  const char *getColumnUnits(unsigned int index);

  /**
   * Return the value of the attribute associated with the
   * given key for the Column variable indicated by index.
   */
  const char *getColumnAttribute(unsigned int index, const char *key);

  /**
   * Return the value of the attribute associated with the
   * given key for the var variable.
   */
  const char *getVarAttribute(unsigned int index, const char *key);

  /**
   * Get the next row of values for the table.
   * @return 1 if data was returned in values, 0 if there is no more data.
   */
   int getRow(double* time, double *values);

  /**
   * Reset the source DataStreams for this table to their beginnings, so
   * getRow() will get points from the beginning.
   */
   void begin();

  /**
   * Return the value of the attribute (if any) associated with the key.
   * If the attribute wasn't specified or is invalid, return NULL.
   */
  const char *getAttribute(const char *key);

  /**
   * Tell the view to create an external representation of this DPC_table object.
   * NOTE: This member function is ONLY meant to be called by the DPC_page
   * class, or subclasses thereof.
   */
  DPV_pointer render( DPV_view *view,
                      DPV_pointer parent_view_data );

  /**
   *
   */
  void notify( DPV_view *view, DPV_message msg );


private:

  DPM_table            *table_spec;
  DPV_pointer           view_data;
  DPM_run              *run;
  std::vector <DataStream *> datastream_list;
  std::vector <DPC_column_info *> column_info_list;
  DPC_datum            *dsData;
  double               *synchronized_values;
  bool                 *end_of_stream;
};

#endif
