# Parms:  file name, row#, total # of columns
# Return: list of loaded values,
#         True if read was successful, otherwise False.
#         If an error occurred reading CSV, it is more than likely
#         due to an incorrect column number specified. The CSV file reader
#         kicks out those errors.
def readCsvRow(fName, rNum, numCol):
  if os.path.exists(fName):
    with open(fName) as csvFile:
      data = csv.reader(csvFile,delimiter=',')
      print("read %s"%fName)
      dataList = []
      for row in data:
        if (int(row[0]) == rNum):
          col = 1
          while col <= numCol:
            if (row[col] == ' True'): # boolean True
              dataList.append(True)
            elif (row[col] == ' False'): # boolean False
              dataList.append(False)
            else:
              dataList.append(float(row[col]))
	    col += 1
          sortedList = sorted(dataList) 
          return(sortedList,True) # success
  return(0,False)
