# Parms:  list of values, low and high boundaries.
# Return: 1 if one or both boundaries were violated, otherwise 0.
#         Rationalle is that this is the return value to the OS.
#         0 means success, 1 means failure.
def checkRange(dataList, lowerBound, upperBound):
  rc = 0
  # if we specify negative infinity, do not compare...
  if (lowerBound > float("-inf")):
    print("checking lower bound %f from CSV file vs %f user-supplied lower bound"%(dataList[0],lowerBound))
    # check lower bounds against first entry. note: boundaries are not inclusive!
    if (dataList[0] < lowerBound):
      print("ERROR: first entry is lower than expected! expected %f but got %f"%(lowerBound,dataList[0]))
      rc = 1
    else:
      print("success")
  # if we specify infinity, do not compare...
  if (upperBound < float("inf")):
    print("checking upper bound %f from CSV file vs %f user-supplied upper bound"%(dataList[-1],upperBound))
    # check upper bounds against last entry. note: boundaries are not inclusive!
    if (dataList[-1] > upperBound):
      print("ERROR: last entry is higher than expected! expected %f but got %f"%(upperBound,dataList[-1]))
      rc = 1
    else:
      print("success")
  return(rc)
