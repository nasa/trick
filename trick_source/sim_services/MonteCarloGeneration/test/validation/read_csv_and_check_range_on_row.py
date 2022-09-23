#!/usr/bin/python
# Purpose: To read a CSV file and extract lowest / highest values from a cell at specific row/column.
#          They are checked against the lowest / highest values from user input.
#          Error(s) is(are) emitted when there are unexpected values.
#
# Return:  0 is returned indicating CSV file was read, column boudaries were verified as correct.
#          1 is returned if CSV file was not found, or if CSV file was read but one or both
#            boundaries were violated. An error message is printed for each boundary violation.

import sys, os, csv, operator


exec(file("validation/readCsvRow.py").read())
exec(file("validation/checkRange.py").read())


if __name__ == "__main__":
  numArgs = int(len(sys.argv))
  if (numArgs < 6):
    sys.exit("ERROR: missing required parameters: RUN directory, CSV row#, total # of columns, lower / upper value range bounaries.\n"
             "       ex: RUN_nominal 1 3 -10.0 10.0\n       terminating!")

  fileName = "MONTE_"+sys.argv[1]+"/monte_values_all_runs"

  numCols = int(sys.argv[3])
  row = int(sys.argv[2])
  data,rc = readCsvRow(fileName, row, numCols)
  print "processing row",str(row),",",str(numCols),"columns"
  if(rc):
    col = 0
    while col < numCols:
      val = float(data[col])
      low = float(sys.argv[4])
      high = float(sys.argv[5])
      if (val < low or val > high):
        sys.exit("ERROR: row %d in %d columns: value %lf is out of bounds of %lf to %lf"%(row,col,val,low,high))
      col += 1

    print "valid range found"
  else:
    sys.exit("ERROR: file 'MONTE_%s/monte_values_all_runs' was not found! nothing to do!"%sys.argv[1]) # failure

