# Parms:  list of values, low / high boundaries
# Return: always 0 since no boundary checking is performed.
#         TODO: check the size of the buckets for uniform distribution.
import numpy as np
from bisect import bisect_left


def verifyUniformDistribution(data, lowerBound, upperBound):
  rc = 0
  sz = len(data)
  cnt = 1
  numBuckets = 20
  data0 = []
  data1 = []
  data2 = []
  data3 = []
  data4 = []
  data5 = []
  data6 = []
  data7 = []
  data8 = []
  data9 = []
  data10 = []
  data11 = []
  data12 = []
  data13 = []
  data14 = []
  data15 = []
  data16 = []
  data17 = []
  data18 = []
  data19 = []
  print("")
  print("lowerBound:   "+str(lowerBound))
  print("upperBound:   "+str(upperBound))
  print("numBuckets:   "+str(numBuckets))
  print("data size:    "+str(sz))
  # this call is for python3.
  #arr,rng = np.linspace(lowerBound, upperBound, num=int(numBuckets), endpoint=False, retstep=True, dtype=float)
  # this call is for python2. dtype parameter is unknown to this version of numpy!
  arr,rng = np.linspace(lowerBound, upperBound, num=int(numBuckets), endpoint=False, retstep=True)
  print("")
  for i in data:
    # find where current value falls within numpy range array built above
    ndx = bisect_left(arr, i)
    case = lambda x: ndx == x
    if   case(1):  data0.append(i)
    elif case(2):  data1.append(i)
    elif case(3):  data2.append(i)
    elif case(4):  data3.append(i)
    elif case(5):  data4.append(i)
    elif case(6):  data5.append(i)
    elif case(7):  data6.append(i)
    elif case(8):  data7.append(i)
    elif case(9):  data8.append(i)
    elif case(10): data9.append(i)
    elif case(11): data10.append(i)
    elif case(12): data11.append(i)
    elif case(13): data12.append(i)
    elif case(14): data13.append(i)
    elif case(15): data14.append(i)
    elif case(16): data15.append(i)
    elif case(17): data16.append(i)
    elif case(18): data17.append(i)
    elif case(19): data18.append(i)
    elif case(20): data19.append(i)

  data0size = len(data0)
  data1size = len(data1)
  data2size = len(data2)
  data3size = len(data3)
  data4size = len(data4)
  data5size = len(data5)
  data6size = len(data6)
  data7size = len(data7)
  data8size = len(data8)
  data9size = len(data9)
  data10size = len(data10)
  data11size = len(data11)
  data12size = len(data12)
  data13size = len(data13)
  data14size = len(data14)
  data15size = len(data15)
  data16size = len(data16)
  data17size = len(data17)
  data18size = len(data18)
  data19size = len(data19)
  print("1st  bucket, contaning numbers between %lf   and %lf,   has %d elements"%(arr[0],arr[1],data0size))
  print("2nd  bucket, contaning numbers between %lf  and %lf,  has %d elements"%(arr[1],arr[2],data1size))
  print("3rd  bucket, contaning numbers between %lf and %lf,  has %d elements"%(arr[2],arr[3],data2size))
  print("4th  bucket, contaning numbers between %lf and %lf,  has %d elements"%(arr[3],arr[4],data3size))
  print("5th  bucket, contaning numbers between %lf and %lf,  has %d elements"%(arr[4],arr[5],data4size))
  print("6th  bucket, contaning numbers between %lf and %lf,  has %d elements"%(arr[5],arr[6],data5size))
  print("7th  bucket, contaning numbers between %lf and %lf,  has %d elements"%(arr[6],arr[7],data6size))
  print("8th  bucket, contaning numbers between %lf and %lf,  has %d elements"%(arr[7],arr[8],data7size))
  print("9th  bucket, contaning numbers between %lf and %lf,  has %d elements"%(arr[8],arr[9],data8size))
  print("10th bucket, contaning numbers between %lf and %lf,  has %d elements"%(arr[9],arr[10],data9size))
  print("11th bucket, contaning numbers between %lf and %lf,  has %d elements"%(arr[10],arr[11],data10size))
  print("12th bucket, contaning numbers between %lf and %lf,  has %d elements"%(arr[11],arr[12],data11size))
  print("13th bucket, contaning numbers between %lf and %lf,  has %d elements"%(arr[12],arr[13],data12size))
  print("14th bucket, contaning numbers between %lf and %lf,  has %d elements"%(arr[13],arr[14],data13size))
  print("15th bucket, contaning numbers between %lf and %lf,  has %d elements"%(arr[14],arr[15],data14size))
  print("16th bucket, contaning numbers between %lf and %lf,  has %d elements"%(arr[15],arr[16],data15size))
  print("17th bucket, contaning numbers between %lf and %lf,  has %d elements"%(arr[16],arr[17],data16size))
  print("18th bucket, contaning numbers between %lf and %lf,  has %d elements"%(arr[17],arr[18],data17size))
  print("19th bucket, contaning numbers between %lf and %lf,  has %d elements"%(arr[18],arr[19],data18size))
  print("20th bucket, contaning numbers between %lf and %lf, has %d elements"%(arr[19],upperBound,data19size))

  return(rc)
