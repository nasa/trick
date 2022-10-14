#!/usr/bin/python
# Purpose: To read a CSV file and extract lowest / highest values from a particular column.
#          They are verified to be either 'True' or 'False'.
#          Error(s) is(are) emitted when there are unexpected values.
#
# Return:  0 is returned indicating CSV file was read, column boudaries were verified as correct.
#          1 is returned if CSV file was not found, or if CSV file was read but one or both
#            boundaries were violated. An error message is printed for each boundary violation.

import sys, os, csv, operator


exec(file("validation/readCSV.py").read())


if __name__ == "__main__":
  numArgs = int(len(sys.argv))
  if (numArgs < 3):
    sys.exit("ERROR: missing required parameters: RUN directory, CSV column #.\n"
             "       ex: RUN_nominal 1\n       terminating!")

  fileName = "MONTE_"+sys.argv[1]+"/monte_values_all_runs"

  data,rc = readCSV(fileName, int(sys.argv[2]))
  print "processing column",str(sys.argv[2])
  if(rc):
    for val in data:
      if (val != True and val != False):
        rc = 0

  if (rc):
    print("contains only expected boolean values")
    sys.exit(0)
  else:
    sys.exit("ERROR: does not contain only expected boolean values!") # failure
