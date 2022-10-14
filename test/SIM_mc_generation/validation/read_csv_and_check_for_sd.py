#!/usr/bin/python
# Purpose: To read a CSV file and extract lowest / highest values from a particular column.
#          Compare standard deviations of the distrubution. By laws of statistics, the read
#          values should be less than 5 standard deviations away from the mean.
#          Error(s) is(are) emitted when there are unexpected values.
#
# Return:  0 is returned indicating CSV file was read, column boudaries were verified as correct.
#          1 is returned if CSV file was not found, or if CSV file was read but one or both
#            boundaries were violated. An error message is printed for each boundary violation.

import sys, os, csv, operator


exec(file("validation/readCSV.py").read())
exec(file("validation/checkRange.py").read())


if __name__ == "__main__":
  numArgs = int(len(sys.argv))
  if (numArgs < 4):
    sys.exit("ERROR: missing required parameters: RUN directory, CSV column # to read, mean, sd.\n"
             "       ex: RUN_nominal 1 0.0 5.0\n       terminating!")

  fileName1 = "MONTE_"+sys.argv[1]+"/monte_values_all_runs"

  data,rc = readCSV(fileName1, int(sys.argv[2])) # file name, column #
  print "processing column",str(sys.argv[2])
  if(rc):
    arg3 = float(sys.argv[3])
    arg4 = float(sys.argv[4])

    print("mean:        %f"%arg3)
    print("data s_d:    %f"%arg4)
    print("data lwr:    %f"%data[0])
    print("data upr:    %f"%data[-1])
    # compute sd mult for bounds
    compSdLwr = ((data[0] - arg3) / arg4)
    compSdUpr = ((data[-1] - arg3) / arg4)
    print("sd mult lwr: %f"%compSdLwr)
    print("sd mult upr: %f"%compSdUpr)
    # it is statistically impossible to exceed 5 standard deviations
    # assume that these are the absolute limits!
    lwrDataLmt = arg3 - (5.0 * arg4)
    uprDataLmt = arg3 + (5.0 * arg4)
    if (compSdLwr > -5.0 and compSdUpr < 5.0):
      rc2 = checkRange(data, lwrDataLmt, uprDataLmt)
      if (rc2):
        sys.exit("ERROR: file 'MONTE_%s/monte_values_all_runs' has invalid range of data!\n"
               "       data was not found within expected range: %f to %f"%(sys.argv[1],lwrDataLmt,uprDataLmt)) # failure
    else:
      sys.exit("ERROR: file 'MONTE_%s/monte_values_all_runs' has invalid range of data!\n"
               "       data was not found within expected range of 5 standard deviations (%f) from mean (%f)"%(sys.argv[1],arg4,arg3)) # failure
  else:
    sys.exit("ERROR: file 'MONTE_%s/monte_values_all_runs' was not found! nothing to do!"%sys.argv[1]) # failure
