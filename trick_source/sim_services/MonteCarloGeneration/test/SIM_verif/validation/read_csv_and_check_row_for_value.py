#!/usr/bin/python
# Purpose: To read a CSV file and extract lowest / highest values from a particular column.
#          They are checked against the exact value from user input.
#          Error(s) is(are) emitted when there are unexpected values.
#
# Return:  0 is returned indicating CSV file was read, column boudaries were verified as correct.
#          1 is returned if CSV file was not found, or if CSV file was read but one or both
#            boundaries were violated. An error message is printed for each boundary violation.

import sys, os, csv, operator


exec(file("validation/readCSV.py").read())


if __name__ == "__main__":
  numArgs = int(len(sys.argv))
  if (numArgs < 5):
    sys.exit("ERROR: missing required parameters: RUN directory, CSV column #, CSV row # and value.\n"
             "       ex: RUN_nominal 1 2 10.0\n       terminating!")

  fileName = "MONTE_"+sys.argv[1]+"/monte_values_all_runs"

  data,rc = readCSV(fileName, int(sys.argv[2]))
  print "processing column",str(sys.argv[2]),"row",str(sys.argv[3])
  if(rc):
    if (data[int(sys.argv[3])] == float(sys.argv[4])):
      print("contains the expected value")
      sys.exit(0)
  print int(sys.argv[3]),float(sys.argv[4])
  sys.exit("ERROR: does not contain the expected value %lf; it contains %lf!!"%(float(sys.argv[4]),data[int(sys.argv[3])])) # failure
