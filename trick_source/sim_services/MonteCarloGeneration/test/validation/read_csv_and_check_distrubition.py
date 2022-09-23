#!/usr/bin/python
# Purpose: To read a CSV file and extract lowest / highest values from a particular column.
#          They are checked against the lowest / highest values from user input.
#          Error(s) is(are) emitted when there are unexpected values.
#
# Return:  0 is returned indicating CSV file was read, column boudaries were verified as correct.
#          1 is returned if CSV file was not found, or if CSV file was read but one or both
#            boundaries were violated. An error message is printed for each boundary violation.

import sys, os, csv, operator


exec(file("validation/readCSV.py").read())
exec(file("validation/checkRange.py").read())
exec(file("validation/verifyUniformDistribution.py").read())


if __name__ == "__main__":
  numArgs = int(len(sys.argv))
  if (numArgs < 5):
    sys.exit("ERROR: missing required parameters: RUN directory, CSV column #, lower and upper range bounaries.\n"
             "       ex: RUN_nominal 1 -1.0 1.0\n       terminating!")

  fileName = "MONTE_"+sys.argv[1]+"/monte_values_all_runs"

  data,rc = readCSV(fileName, int(sys.argv[2]))
  if(rc):
    if(checkRange(data, float(sys.argv[3]), float(sys.argv[4])) == 0): # 0 = success, 1 = one or both boundaries were violated
      sys.exit(verifyUniformDistribution(data, float(sys.argv[3]), float(sys.argv[4]))) # 0 = success, 1 = one or both boundaries were violated
    else:
      sys.exit("ERROR: file 'MONTE_%s/monte_values_all_runs' data range check failed!"%sys.argv[1]) # failure
  else:
    sys.exit("ERROR: file 'MONTE_%s/monte_values_all_runs' was not found! nothing to do!"%sys.argv[1]) # failure
