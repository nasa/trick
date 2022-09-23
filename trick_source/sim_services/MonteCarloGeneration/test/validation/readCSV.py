# Parms:  file name, column #
# Return: list of loaded values,
#         True if read was successful, otherwise False.
#         If an error occurred reading CSV, it is more than likely
#         due to an incorrect column number specified. The CSV file reader
#         kicks out those errors.
def readCSV(fName, cNum):
  if os.path.exists(fName):
    with open(fName) as csvFile:
      data = csv.reader(csvFile,delimiter=',')
      tempList = sorted(data, key=operator.itemgetter(cNum)) 
      dataList = []
      for row in tempList:
        if (row[cNum] == ' True'): # boolean True
          dataList.append(True)
        elif (row[cNum] == ' False'): # boolean False
          dataList.append(False)
	else:
          dataList.append(float(row[cNum]))
      sortedList = sorted(dataList) 
      print("read %s"%fName)
      return(sortedList,True) # success
  return(0,False)
