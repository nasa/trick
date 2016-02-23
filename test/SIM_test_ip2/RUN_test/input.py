
from array import *

"""
ip2.c_test.c = 123
ip2.c_test.ca[0] = [ 0 , 5 , 6 , 7 ]
ip2.c_test.caa[0][0] = [ 0 , 50 , 60 , 70 ]
ip2.c_test.caa[1][0] = [ 0 , 20 , 30 , 40 ]

ip2.c_test.cp = id(ip2.c_test.c)
ip2.c_test.cp[0] = 65

ip2.c_test.cp = id(ip2.c_test.ca[2]
ip2.c_test.cp[0] = 234

ip2.c_test.cpp2 = trick.TMM_declare_var_s( "char* [4]" )
ip2.c_test.cpp2[1] = id(ip2.c_test.ca[5]
ip2.c_test.cpp2[1][0] = 111

ip2.c_test.cpp = trick.TMM_declare_var_s( "char* [6]" )
ip2.c_test.cpp[0] = trick.TMM_declare_var_s( "char [2]" )
ip2.c_test.cpp[1] = trick.TMM_declare_var_s( "char [4]" )
ip2.c_test.cpp[2] = trick.TMM_declare_var_s( "char [4]" )
ip2.c_test.cpp[3] = trick.TMM_declare_var_s( "char [8]" )
ip2.c_test.cpp[4] = trick.TMM_declare_var_s( "char [10]" )
ip2.c_test.cpp[5] = trick.TMM_declare_var_s( "char [12]" )

ip2.c_test.cppp = trick.TMM_declare_var_s( "char** [5]" )
ip2.c_test.cppp[0] = trick.TMM_declare_var_s( "char* [6]" )
ip2.c_test.cppp[1] = trick.TMM_declare_var_s( "char* [7]" )
ip2.c_test.cppp[2] = trick.TMM_declare_var_s( "char* [8]" )
ip2.c_test.cppp[3] = trick.TMM_declare_var_s( "char* [9]" )
ip2.c_test.cppp[4] = trick.TMM_declare_var_s( "char* [10]" )


ip2.c_test.cppp[0][0] = trick.TMM_declare_var_s( "char [14]" )
ip2.c_test.cppp[0][1] = trick.TMM_declare_var_s( "char [15]" )
ip2.c_test.cppp[0][2] = trick.TMM_declare_var_s( "char [16]" )
ip2.c_test.cppp[0][3] = trick.TMM_declare_var_s( "char [17]" )
ip2.c_test.cppp[0][4] = trick.TMM_declare_var_s( "char [18]" )

ip2.c_test.cppp[1][0] = trick.TMM_declare_var_s( "char [21]" )
ip2.c_test.cppp[1][1] = trick.TMM_declare_var_s( "char [22]" )
ip2.c_test.cppp[1][2] = trick.TMM_declare_var_s( "char [23]" )
ip2.c_test.cppp[1][3] = trick.TMM_declare_var_s( "char [24]" )
ip2.c_test.cppp[1][4] = trick.TMM_declare_var_s( "char [25]" )

ip2.c_test.cppp[1][4][10] = [ 56 , 67 , 78 , 89 , 90 ]

ip2.c_test.cp2 = id(ip2.c_test.cppp[1][4][12]
ip2.c_test.cp2[0] = 200 , 201
ip2.c_test.cp2[12] = 300
'''should cause an error'''
#ip2.c_test.cp2[13] = 301

ip2.c_test.cp3 = id(ip2.c_test.cppp[0][3][6]

ip2.c_test.cpp3 = id(ip2.c_test.cppp[1][2]
ip2.c_test.cpp3[2][3] = 40
"""

'''-----------------------------------------------'''

ip2.uc_test.uc = 123
ip2.uc_test.uca = [ 0 , 5 , 6 , 7 ]
ip2.uc_test.ucaa[0] = [ 0 , 50 , 60 , 70 ]
ip2.uc_test.ucaa[1] = [ 0 , 20 , 30 , 40 ]

ip2.uc_test.ucp = id(ip2.uc_test.uc)
ip2.uc_test.ucp[0] = 65

ip2.uc_test.ucp = id(ip2.uc_test.uca[2])
ip2.uc_test.ucp[0] = 234

ip2.uc_test.ucpp2 = trick.TMM_declare_var_s( "unsigned char [4]" )
ip2.uc_test.ucpp2[1] = id(ip2.uc_test.uca[5])
ip2.uc_test.ucpp2[1][0] = 111

ip2.uc_test.ucpp = trick.TMM_declare_var_s( "unsigned char* [6]" )
ip2.uc_test.ucpp[0] = trick.TMM_declare_var_s( "unsigned char [2]" )
ip2.uc_test.ucpp[1] = trick.TMM_declare_var_s( "unsigned char [4]" )
ip2.uc_test.ucpp[2] = trick.TMM_declare_var_s( "unsigned char [6]" )
ip2.uc_test.ucpp[3] = trick.TMM_declare_var_s( "unsigned char [8]" )
ip2.uc_test.ucpp[4] = trick.TMM_declare_var_s( "unsigned char [10]" )
ip2.uc_test.ucpp[5] = trick.TMM_declare_var_s( "unsigned char [12]" )

ip2.uc_test.ucppp = trick.TMM_declare_var_s( "unsigned char** [5]" )
ip2.uc_test.ucppp[0] = trick.TMM_declare_var_s( "unsigned char* [6]" )
ip2.uc_test.ucppp[1] = trick.TMM_declare_var_s( "unsigned char* [7]" )
ip2.uc_test.ucppp[2] = trick.TMM_declare_var_s( "unsigned char* [8]" )
ip2.uc_test.ucppp[3] = trick.TMM_declare_var_s( "unsigned char* [9]" )
ip2.uc_test.ucppp[4] = trick.TMM_declare_var_s( "unsigned char* [10]" )

ip2.uc_test.ucppp[0][0] = trick.TMM_declare_var_s( "unsigned char [14]" )
ip2.uc_test.ucppp[0][1] = trick.TMM_declare_var_s( "unsigned char [15]" )
ip2.uc_test.ucppp[0][2] = trick.TMM_declare_var_s( "unsigned char [16]" )
ip2.uc_test.ucppp[0][3] = trick.TMM_declare_var_s( "unsigned char [17]" )
ip2.uc_test.ucppp[0][4] = trick.TMM_declare_var_s( "unsigned char [18]" )

ip2.uc_test.ucppp[1][0] = trick.TMM_declare_var_s( "unsigned char [21]" )
ip2.uc_test.ucppp[1][1] = trick.TMM_declare_var_s( "unsigned char [22]" )
ip2.uc_test.ucppp[1][2] = trick.TMM_declare_var_s( "unsigned char [23]" )
ip2.uc_test.ucppp[1][3] = trick.TMM_declare_var_s( "unsigned char [24]" )
ip2.uc_test.ucppp[1][4] = trick.TMM_declare_var_s( "unsigned char [25]" )

ip2.uc_test.ucppp[1][4][10] = [ 56 , 67 , 78 , 89 , 90 ]

ip2.uc_test.ucp2 = id(ip2.uc_test.ucppp[1][4][12])
ip2.uc_test.ucp2[0] = [ 200 , 201 ]
ip2.uc_test.ucp2[12] = 300
'''should cause an error'''
#ip2.uc_test.ucp2[13] = 301

ip2.uc_test.ucp3 = id(ip2.uc_test.ucppp[0][3][6])

ip2.uc_test.ucpp3 = id(ip2.uc_test.ucppp[1][2])
ip2.uc_test.ucpp3[2][3] = 40

'''-----------------------------------------------'''

ip2.s_test.s = 123
ip2.s_test.sa = [ 4 , 5 , 6 , 7 ]
ip2.s_test.saa[0] = [ 4000 , 5000 , 6000 , 7000 ]
ip2.s_test.saa[1] = [ 1000 , 2000 , 3000 , 4000 ]

ip2.s_test.sp = id(ip2.s_test.s)
ip2.s_test.sp[0] = 345

ip2.s_test.sp = id(ip2.s_test.sa[2])
ip2.s_test.sp[0] = 234

ip2.s_test.spp2 = trick.TMM_declare_var_s( "short* [4]" )
ip2.s_test.spp2[1] = id(ip2.s_test.sa[5])
ip2.s_test.spp2[1][0] = 111

ip2.s_test.spp = trick.TMM_declare_var_s( "short* [6]" )
ip2.s_test.spp[0] = trick.TMM_declare_var_s( "short [2]" )
ip2.s_test.spp[1] = trick.TMM_declare_var_s( "short [4]" )
ip2.s_test.spp[2] = trick.TMM_declare_var_s( "short [6]" )
ip2.s_test.spp[3] = trick.TMM_declare_var_s( "short [8]" )
ip2.s_test.spp[4] = trick.TMM_declare_var_s( "short [10]" )
ip2.s_test.spp[5] = trick.TMM_declare_var_s( "short [12]" )

ip2.s_test.sppp = trick.TMM_declare_var_s( "short** [5]" )
ip2.s_test.sppp[0] = trick.TMM_declare_var_s( "short* [6]" )
ip2.s_test.sppp[1] = trick.TMM_declare_var_s( "short* [7]" )
ip2.s_test.sppp[2] = trick.TMM_declare_var_s( "short* [8]" )
ip2.s_test.sppp[3] = trick.TMM_declare_var_s( "short* [9]" )
ip2.s_test.sppp[4] = trick.TMM_declare_var_s( "short* [10]" )


ip2.s_test.sppp[0][0] = trick.TMM_declare_var_s( "short [14]" )
ip2.s_test.sppp[0][1] = trick.TMM_declare_var_s( "short [15]" )
ip2.s_test.sppp[0][2] = trick.TMM_declare_var_s( "short [16]" )
ip2.s_test.sppp[0][3] = trick.TMM_declare_var_s( "short [17]" )
ip2.s_test.sppp[0][4] = trick.TMM_declare_var_s( "short [18]" )

ip2.s_test.sppp[1][0] = trick.TMM_declare_var_s( "short [21]" )
ip2.s_test.sppp[1][1] = trick.TMM_declare_var_s( "short [22]" )
ip2.s_test.sppp[1][2] = trick.TMM_declare_var_s( "short [23]" )
ip2.s_test.sppp[1][3] = trick.TMM_declare_var_s( "short [24]" )
ip2.s_test.sppp[1][4] = trick.TMM_declare_var_s( "short [25]" )

ip2.s_test.sppp[1][4][10] = [ 56 , 67 , 78 , 89 , 90 ]

ip2.s_test.sp2 = id(ip2.s_test.sppp[1][4][12])
ip2.s_test.sp2[0] = [ 200 , 201 ]
ip2.s_test.sp2[12] = 300
'''should cause an error'''
#ip2.s_test.sp2[13] = 301

ip2.s_test.sp3 = id(ip2.s_test.sppp[0][3][6])

ip2.s_test.spp3 = id(ip2.s_test.sppp[1][2])
ip2.s_test.spp3[2][3] = 400

'''-----------------------------------------------'''

ip2.us_test.us = 123
ip2.us_test.usa = [ 4 , 5 , 6 , 7 ]
ip2.us_test.usaa[0] = [ 4000 , 5000 , 6000 , 7000 ]
ip2.us_test.usaa[1] = [ 1000 , 2000 , 3000 , 4000 ]

ip2.d_test.d = 123
ip2.d_test.da = [ 4 , 5 , 6 , 7 ]
ip2.d_test.daa[0] = [ 4000 , 5000 , 6000 , 7000 ]
ip2.d_test.daa[1] = [ 1000 , 2000 , 3000 , 4000 ]

ip2.us_test.usp = id(ip2.us_test.us)
ip2.us_test.usp[0] = 345

ip2.us_test.usp = id(ip2.us_test.usa[2])
ip2.us_test.usp[0] = 234

ip2.us_test.uspp2 = trick.TMM_declare_var_s( "unsigned short* [4]" )
ip2.us_test.uspp2[1] = id(ip2.us_test.usa[5])
ip2.us_test.uspp2[1][0] = 111

ip2.us_test.uspp = trick.TMM_declare_var_s( "unsigned short* [6]" )
ip2.us_test.uspp[0] = trick.TMM_declare_var_s( "unsigned short [2]" )
ip2.us_test.uspp[1] = trick.TMM_declare_var_s( "unsigned short [4]" )
ip2.us_test.uspp[2] = trick.TMM_declare_var_s( "unsigned short [6]" )
ip2.us_test.uspp[3] = trick.TMM_declare_var_s( "unsigned short [8]" )
ip2.us_test.uspp[4] = trick.TMM_declare_var_s( "unsigned short [10]" )
ip2.us_test.uspp[5] = trick.TMM_declare_var_s( "unsigned short [12]" )

ip2.us_test.usppp = trick.TMM_declare_var_s( "unsigned short** [5]" )
ip2.us_test.usppp[0] = trick.TMM_declare_var_s( "unsigned short* [6]" )
ip2.us_test.usppp[1] = trick.TMM_declare_var_s( "unsigned short* [7]" )
ip2.us_test.usppp[2] = trick.TMM_declare_var_s( "unsigned short* [8]" )
ip2.us_test.usppp[3] = trick.TMM_declare_var_s( "unsigned short* [9]" )
ip2.us_test.usppp[4] = trick.TMM_declare_var_s( "unsigned short* [10]" )


ip2.us_test.usppp[0][0] = trick.TMM_declare_var_s( "unsigned short [14]" )
ip2.us_test.usppp[0][1] = trick.TMM_declare_var_s( "unsigned short [15]" )
ip2.us_test.usppp[0][2] = trick.TMM_declare_var_s( "unsigned short [16]" )
ip2.us_test.usppp[0][3] = trick.TMM_declare_var_s( "unsigned short [17]" )
ip2.us_test.usppp[0][4] = trick.TMM_declare_var_s( "unsigned short [18]" )

ip2.us_test.usppp[1][0] = trick.TMM_declare_var_s( "unsigned short [21]" )
ip2.us_test.usppp[1][1] = trick.TMM_declare_var_s( "unsigned short [22]" )
ip2.us_test.usppp[1][2] = trick.TMM_declare_var_s( "unsigned short [23]" )
ip2.us_test.usppp[1][3] = trick.TMM_declare_var_s( "unsigned short [24]" )
ip2.us_test.usppp[1][4] = trick.TMM_declare_var_s( "unsigned short [25]" )

ip2.us_test.usppp[1][4][10] = [ 56 , 67 , 78 , 89 , 90 ]

ip2.us_test.usp2 = id(ip2.us_test.usppp[1][4][12])
ip2.us_test.usp2[0] = [ 200 , 201 ]
ip2.us_test.usp2[12] = 300
'''should cause an error'''
#ip2.us_test.usp2[13] = 301

ip2.us_test.usp3 = id(ip2.us_test.usppp[0][3][6])

ip2.us_test.uspp3 = id(ip2.us_test.usppp[1][2])
ip2.us_test.uspp3[2][3] = 400

'''-----------------------------------------------'''

ip2.i_test.i = 123
ip2.i_test.ia = [ 4 , 5 , 6 , 7 ]
ip2.i_test.iaa[0][0] = [ 4000 , 5000 , 6000 , 7000 ]
ip2.i_test.iaa[1][0] = [ 1000 , 2000 , 3000 , 4000 ]

ip2.i_test.ip = id(ip2.i_test.i)
ip2.i_test.ip[0] = 345

ip2.i_test.ip = id(ip2.i_test.ia[2])
ip2.i_test.ip[0] = 234

ip2.i_test.ipp2 = trick.TMM_declare_var_s( "int* [4]" )
ip2.i_test.ipp2[1] = id(ip2.i_test.ia[5])
ip2.i_test.ipp2[1][0] = 111

ip2.i_test.ipp = trick.TMM_declare_var_s( "int* [6]" )
ip2.i_test.ipp[0] = trick.TMM_declare_var_s( "int [2]" )
ip2.i_test.ipp[1] = trick.TMM_declare_var_s( "int [4]" )
ip2.i_test.ipp[2] = trick.TMM_declare_var_s( "int [6]" )
ip2.i_test.ipp[3] = trick.TMM_declare_var_s( "int [8]" )
ip2.i_test.ipp[4] = trick.TMM_declare_var_s( "int [10]" )
ip2.i_test.ipp[5] = trick.TMM_declare_var_s( "int [12]" )

ip2.i_test.ippp = trick.TMM_declare_var_s( "int** [5]" )
ip2.i_test.ippp[0] = trick.TMM_declare_var_s( "int* [6]" )
ip2.i_test.ippp[1] = trick.TMM_declare_var_s( "int* [7]" )
ip2.i_test.ippp[2] = trick.TMM_declare_var_s( "int* [8]" )
ip2.i_test.ippp[3] = trick.TMM_declare_var_s( "int* [9]" )
ip2.i_test.ippp[4] = trick.TMM_declare_var_s( "int* [10]" )


ip2.i_test.ippp[0][0] = trick.TMM_declare_var_s( "int [14]" )
ip2.i_test.ippp[0][1] = trick.TMM_declare_var_s( "int [15]" )
ip2.i_test.ippp[0][2] = trick.TMM_declare_var_s( "int [16]" )
ip2.i_test.ippp[0][3] = trick.TMM_declare_var_s( "int [17]" )
ip2.i_test.ippp[0][4] = trick.TMM_declare_var_s( "int [18]" )

ip2.i_test.ippp[1][0] = trick.TMM_declare_var_s( "int [21]" )
ip2.i_test.ippp[1][1] = trick.TMM_declare_var_s( "int [22]" )
ip2.i_test.ippp[1][2] = trick.TMM_declare_var_s( "int [23]" )
ip2.i_test.ippp[1][3] = trick.TMM_declare_var_s( "int [24]" )
ip2.i_test.ippp[1][4] = trick.TMM_declare_var_s( "int [25]" )

ip2.i_test.ippp[1][4][10] = [ 56 , 67 , 78 , 89 , 90 ]

ip2.i_test.ip2 = id(ip2.i_test.ippp[1][4][12])
ip2.i_test.ip2[0] = [ 200 , 201 ]
ip2.i_test.ip2[12] = 300
'''should cause an error'''
#ip2.i_test.ip2[13] = 301

ip2.i_test.ip3 = id(ip2.i_test.ippp[0][3][6])

ip2.i_test.ipp3 = id(ip2.i_test.ippp[1][2])
ip2.i_test.ipp3[2][3] = 400


'''-----------------------------------------------'''

ip2.ui_test.ui = 123
ip2.ui_test.uia[0] = [ 4 , 5 , 6 , 7 ]
ip2.ui_test.uiaa[0][0] = [ 4000 , 5000 , 6000 , 7000 ]
ip2.ui_test.uiaa[1][0] = [ 1000 , 2000 , 3000 , 4000 ]

ip2.ui_test.uip = id(ip2.ui_test.ui)
ip2.ui_test.uip[0] = 345

ip2.ui_test.uip = id(ip2.ui_test.uia[2])
ip2.ui_test.uip[0] = 234

ip2.ui_test.uipp2 = trick.TMM_declare_var_s( "unsigned int*  [4]" )
ip2.ui_test.uipp2[1] = id(ip2.ui_test.uia[5])
ip2.ui_test.uipp2[1][0] = 111

ip2.ui_test.uipp = trick.TMM_declare_var_s( "unsigned int* [6]" )
ip2.ui_test.uipp[0] = trick.TMM_declare_var_s( "unsigned int [2]" )
ip2.ui_test.uipp[1] = trick.TMM_declare_var_s( "unsigned int [4]" )
ip2.ui_test.uipp[2] = trick.TMM_declare_var_s( "unsigned int [6]" )
ip2.ui_test.uipp[3] = trick.TMM_declare_var_s( "unsigned int [8]" )
ip2.ui_test.uipp[4] = trick.TMM_declare_var_s( "unsigned int [10]" )
ip2.ui_test.uipp[5] = trick.TMM_declare_var_s( "unsigned int [12]" )

ip2.ui_test.uippp = trick.TMM_declare_var_s( "unsigned int** [5]" )
ip2.ui_test.uippp[0] = trick.TMM_declare_var_s( "unsigned int* [6]" )
ip2.ui_test.uippp[1] = trick.TMM_declare_var_s( "unsigned int* [7]" )
ip2.ui_test.uippp[2] = trick.TMM_declare_var_s( "unsigned int* [8]" )
ip2.ui_test.uippp[3] = trick.TMM_declare_var_s( "unsigned int* [9]" )
ip2.ui_test.uippp[4] = trick.TMM_declare_var_s( "unsigned int* [10]" )


ip2.ui_test.uippp[0][0] = trick.TMM_declare_var_s( "unsigned int [14]" )
ip2.ui_test.uippp[0][1] = trick.TMM_declare_var_s( "unsigned int [15]" )
ip2.ui_test.uippp[0][2] = trick.TMM_declare_var_s( "unsigned int [16]" )
ip2.ui_test.uippp[0][3] = trick.TMM_declare_var_s( "unsigned int [17]" )
ip2.ui_test.uippp[0][4] = trick.TMM_declare_var_s( "unsigned int [18]" )

ip2.ui_test.uippp[1][0] = trick.TMM_declare_var_s( "unsigned int [21]" )
ip2.ui_test.uippp[1][1] = trick.TMM_declare_var_s( "unsigned int [22]" )
ip2.ui_test.uippp[1][2] = trick.TMM_declare_var_s( "unsigned int [23]" )
ip2.ui_test.uippp[1][3] = trick.TMM_declare_var_s( "unsigned int [24]" )
ip2.ui_test.uippp[1][4] = trick.TMM_declare_var_s( "unsigned int [25]" )

ip2.ui_test.uippp[1][4][10] = [ 56 , 67 , 78 , 89 , 90 ]

ip2.ui_test.uip2 = id(ip2.ui_test.uippp[1][4][12])
ip2.ui_test.uip2[0] = [ 200 , 201 ]
ip2.ui_test.uip2[12] = 300
'''should cause an error'''
#ip2.ui_test.uip2[13] = 301

ip2.ui_test.uip3 = id(ip2.ui_test.uippp[0][3][6])

ip2.ui_test.uipp3 = id(ip2.ui_test.uippp[1][2])
ip2.ui_test.uipp3[2][3] = 400

'''-----------------------------------------------'''

ip2.l_test.l = 123
ip2.l_test.la[0] = [ 4 , 5 , 6 , 7 ]
ip2.l_test.laa[0][0] = [ 4000 , 5000 , 6000 , 7000 ]
ip2.l_test.laa[1][0] = [ 1000 , 2000 , 3000 , 4000 ]

ip2.l_test.lp = id(ip2.l_test.l)
ip2.l_test.lp[0] = 345

ip2.l_test.lp = id(ip2.l_test.la[2])
ip2.l_test.lp[0] = 234

ip2.l_test.lpp2 = trick.TMM_declare_var_s( "long* [4]" )
ip2.l_test.lpp2[1] = id(ip2.l_test.la[5])
ip2.l_test.lpp2[1][0] = 111

ip2.l_test.lpp = trick.TMM_declare_var_s( "long* [6]" )
ip2.l_test.lpp[0] = trick.TMM_declare_var_s( "long [2]" )
ip2.l_test.lpp[1] = trick.TMM_declare_var_s( "long [4]" )
ip2.l_test.lpp[2] = trick.TMM_declare_var_s( "long [6]" )
ip2.l_test.lpp[3] = trick.TMM_declare_var_s( "long [8]" )
ip2.l_test.lpp[4] = trick.TMM_declare_var_s( "long [10]" )
ip2.l_test.lpp[5] = trick.TMM_declare_var_s( "long [12]" )

ip2.l_test.lppp = trick.TMM_declare_var_s( "long** [5]" )
ip2.l_test.lppp[0] = trick.TMM_declare_var_s( "long* [6]" )
ip2.l_test.lppp[1] = trick.TMM_declare_var_s( "long* [7]" )
ip2.l_test.lppp[2] = trick.TMM_declare_var_s( "long* [8]" )
ip2.l_test.lppp[3] = trick.TMM_declare_var_s( "long* [9]" )
ip2.l_test.lppp[4] = trick.TMM_declare_var_s( "long* [10]" )


ip2.l_test.lppp[0][0] = trick.TMM_declare_var_s( "long [14]" )
ip2.l_test.lppp[0][1] = trick.TMM_declare_var_s( "long [15]" )
ip2.l_test.lppp[0][2] = trick.TMM_declare_var_s( "long [16]" )
ip2.l_test.lppp[0][3] = trick.TMM_declare_var_s( "long [17]" )
ip2.l_test.lppp[0][4] = trick.TMM_declare_var_s( "long [18]" )

ip2.l_test.lppp[1][0] = trick.TMM_declare_var_s( "long [21]" )
ip2.l_test.lppp[1][1] = trick.TMM_declare_var_s( "long [22]" )
ip2.l_test.lppp[1][2] = trick.TMM_declare_var_s( "long [23]" )
ip2.l_test.lppp[1][3] = trick.TMM_declare_var_s( "long [24]" )
ip2.l_test.lppp[1][4] = trick.TMM_declare_var_s( "long [25]" )

ip2.l_test.lppp[1][4][10] = [ 56 , 67 , 78 , 89 , 90 ]

ip2.l_test.lp2 = id(ip2.l_test.lppp[1][4][12])
ip2.l_test.lp2[0] = [ 200 , 201 ]
ip2.l_test.lp2[12] = 300
'''should cause an error'''
#ip2.l_test.lp2[13] = 301

ip2.l_test.lp3 = id(ip2.l_test.lppp[0][3][6])

ip2.l_test.lpp3 = id(ip2.l_test.lppp[1][2])
ip2.l_test.lpp3[2][3] = 400

'''-----------------------------------------------'''

ip2.ul_test.ul = 123
ip2.ul_test.ula[0] = [ 4 , 5 , 6 , 7 ]
ip2.ul_test.ulaa[0][0] = [ 4000 , 5000 , 6000 , 7000 ]
ip2.ul_test.ulaa[1][0] = [ 1000 , 2000 , 3000 , 4000 ]

ip2.ul_test.ulp = id(ip2.ul_test.ul)
ip2.ul_test.ulp[0] = 345

ip2.ul_test.ulp = id(ip2.ul_test.ula[2])
ip2.ul_test.ulp[0] = 234

ip2.ul_test.ulpp2 = trick.TMM_declare_var_s( "unsigned long* [4]" )
ip2.ul_test.ulpp2[1] = id(ip2.ul_test.ula[5])
ip2.ul_test.ulpp2[1][0] = 111

ip2.ul_test.ulpp = trick.TMM_declare_var_s( "unsigned long* [6]" )
ip2.ul_test.ulpp[0] = trick.TMM_declare_var_s( "unsigned long [2]" )
ip2.ul_test.ulpp[1] = trick.TMM_declare_var_s( "unsigned long [4]" )
ip2.ul_test.ulpp[2] = trick.TMM_declare_var_s( "unsigned long [6]" )
ip2.ul_test.ulpp[3] = trick.TMM_declare_var_s( "unsigned long [8]" )
ip2.ul_test.ulpp[4] = trick.TMM_declare_var_s( "unsigned long [10]" )
ip2.ul_test.ulpp[5] = trick.TMM_declare_var_s( "unsigned long [12]" )

ip2.ul_test.ulppp = trick.TMM_declare_var_s( "unsigned long** [5]" )
ip2.ul_test.ulppp[0] = trick.TMM_declare_var_s( "unsigned long* [6]" )
ip2.ul_test.ulppp[1] = trick.TMM_declare_var_s( "unsigned long* [7]" )
ip2.ul_test.ulppp[2] = trick.TMM_declare_var_s( "unsigned long* [8]" )
ip2.ul_test.ulppp[3] = trick.TMM_declare_var_s( "unsigned long* [9]" )
ip2.ul_test.ulppp[4] = trick.TMM_declare_var_s( "unsigned long* [10]" )


ip2.ul_test.ulppp[0][0] = trick.TMM_declare_var_s( "unsigned long [14]" )
ip2.ul_test.ulppp[0][1] = trick.TMM_declare_var_s( "unsigned long [15]" )
ip2.ul_test.ulppp[0][2] = trick.TMM_declare_var_s( "unsigned long [16]" )
ip2.ul_test.ulppp[0][3] = trick.TMM_declare_var_s( "unsigned long [17]" )
ip2.ul_test.ulppp[0][4] = trick.TMM_declare_var_s( "unsigned long [18]" )

ip2.ul_test.ulppp[1][0] = trick.TMM_declare_var_s( "unsigned long [21]" )
ip2.ul_test.ulppp[1][1] = trick.TMM_declare_var_s( "unsigned long [22]" )
ip2.ul_test.ulppp[1][2] = trick.TMM_declare_var_s( "unsigned long [23]" )
ip2.ul_test.ulppp[1][3] = trick.TMM_declare_var_s( "unsigned long [24]" )
ip2.ul_test.ulppp[1][4] = trick.TMM_declare_var_s( "unsigned long [25]" )

ip2.ul_test.ulppp[1][4][10] = [ 56 , 67 , 78 , 89 , 90 ]

ip2.ul_test.ulp2 = id(ip2.ul_test.ulppp[1][4][12])
ip2.ul_test.ulp2[0] = [ 200 , 201 ]
ip2.ul_test.ulp2[12] = 300
'''should cause an error'''
#ip2.ul_test.ulp2[13] = 301

ip2.ul_test.ulp3 = id(ip2.ul_test.ulppp[0][3][6])

ip2.ul_test.ulpp3 = id(ip2.ul_test.ulppp[1][2])
ip2.ul_test.ulpp3[2][3] = 400

'''-----------------------------------------------'''

ip2.ll_test.ll = 123
ip2.ll_test.lla[0] = [ 4 , 5 , 6 , 7 ]
ip2.ll_test.llaa[0][0] = [ 4000 , 5000 , 6000 , 7000 ]
ip2.ll_test.llaa[1][0] = [ 1000 , 2000 , 3000 , 4000 ]

ip2.ll_test.llp = id(ip2.ll_test.ll)
ip2.ll_test.llp[0] = 345

ip2.ll_test.llp = id(ip2.ll_test.lla[2])
ip2.ll_test.llp[0] = 234

ip2.ll_test.llpp2 = trick.TMM_declare_var_s( "long long* [4]" )
ip2.ll_test.llpp2[1] = id(ip2.ll_test.lla[5])
ip2.ll_test.llpp2[1][0] = 111

ip2.ll_test.llpp = trick.TMM_declare_var_s( "long long* [6]" )
ip2.ll_test.llpp[0] = trick.TMM_declare_var_s( "long long [2]" )
ip2.ll_test.llpp[1] = trick.TMM_declare_var_s( "long long [4]" )
ip2.ll_test.llpp[2] = trick.TMM_declare_var_s( "long long [6]" )
ip2.ll_test.llpp[3] = trick.TMM_declare_var_s( "long long [8]" )
ip2.ll_test.llpp[4] = trick.TMM_declare_var_s( "long long [10]" )
ip2.ll_test.llpp[5] = trick.TMM_declare_var_s( "long long [12]" )

ip2.ll_test.llppp = trick.TMM_declare_var_s( "long long** [5]" )
ip2.ll_test.llppp[0] = trick.TMM_declare_var_s( "long long* [6]" )
ip2.ll_test.llppp[1] = trick.TMM_declare_var_s( "long long* [7]" )
ip2.ll_test.llppp[2] = trick.TMM_declare_var_s( "long long* [8]" )
ip2.ll_test.llppp[3] = trick.TMM_declare_var_s( "long long* [9]" )
ip2.ll_test.llppp[4] = trick.TMM_declare_var_s( "long long* [10]" )


ip2.ll_test.llppp[0][0] = trick.TMM_declare_var_s( "long long [14]" )
ip2.ll_test.llppp[0][1] = trick.TMM_declare_var_s( "long long [15]" )
ip2.ll_test.llppp[0][2] = trick.TMM_declare_var_s( "long long [16]" )
ip2.ll_test.llppp[0][3] = trick.TMM_declare_var_s( "long long [17]" )
ip2.ll_test.llppp[0][4] = trick.TMM_declare_var_s( "long long [18]" )

ip2.ll_test.llppp[1][0] = trick.TMM_declare_var_s( "long long [21]" )
ip2.ll_test.llppp[1][1] = trick.TMM_declare_var_s( "long long [22]" )
ip2.ll_test.llppp[1][2] = trick.TMM_declare_var_s( "long long [23]" )
ip2.ll_test.llppp[1][3] = trick.TMM_declare_var_s( "long long [24]" )
ip2.ll_test.llppp[1][4] = trick.TMM_declare_var_s( "long long [25]" )

ip2.ll_test.llppp[1][4][10] = [ 56 , 67 , 78 , 89 , 90 ]

ip2.ll_test.llp2 = id(ip2.ll_test.llppp[1][4][12])
ip2.ll_test.llp2[0] = [ 200 , 201 ]
ip2.ll_test.llp2[12] = 300
'''should cause an error'''
#ip2.ll_test.llp2[13] = 301

ip2.ll_test.llp3 = id(ip2.ll_test.llppp[0][3][6])

ip2.ll_test.llpp3 = id(ip2.ll_test.llppp[1][2])
ip2.ll_test.llpp3[2][3] = 400

'''-----------------------------------------------'''

ip2.ull_test.ull = 123
ip2.ull_test.ulla[0] = [ 4 , 5 , 6 , 7 ]
ip2.ull_test.ullaa[0][0] = [ 4000 , 5000 , 6000 , 7000 ]
ip2.ull_test.ullaa[1][0] = [ 1000 , 2000 , 3000 , 4000 ]

ip2.ull_test.ullp = id(ip2.ull_test.ull)
ip2.ull_test.ullp[0] = 345

ip2.ull_test.ullp = id(ip2.ull_test.ulla[2])
ip2.ull_test.ullp[0] = 234

ip2.ull_test.ullpp2 = trick.TMM_declare_var_s( "unsigned long long* [4]" )
ip2.ull_test.ullpp2[1] = id(ip2.ull_test.ulla[5])
ip2.ull_test.ullpp2[1][0] = 111

ip2.ull_test.ullpp = trick.TMM_declare_var_s( "unsigned long long* [6]" )
ip2.ull_test.ullpp[0] = trick.TMM_declare_var_s( "unsigned long long [2]" )
ip2.ull_test.ullpp[1] = trick.TMM_declare_var_s( "unsigned long long [4]" )
ip2.ull_test.ullpp[2] = trick.TMM_declare_var_s( "unsigned long long [6]" )
ip2.ull_test.ullpp[3] = trick.TMM_declare_var_s( "unsigned long long [8]" )
ip2.ull_test.ullpp[4] = trick.TMM_declare_var_s( "unsigned long long [10]" )
ip2.ull_test.ullpp[5] = trick.TMM_declare_var_s( "unsigned long long [12]" )

ip2.ull_test.ullppp = trick.TMM_declare_var_s( "unsigned long long** [5]" )
ip2.ull_test.ullppp[0] = trick.TMM_declare_var_s( "unsigned long long* [6]" )
ip2.ull_test.ullppp[1] = trick.TMM_declare_var_s( "unsigned long long* [7]" )
ip2.ull_test.ullppp[2] = trick.TMM_declare_var_s( "unsigned long long* [8]" )
ip2.ull_test.ullppp[3] = trick.TMM_declare_var_s( "unsigned long long* [9]" )
ip2.ull_test.ullppp[4] = trick.TMM_declare_var_s( "unsigned long long* [10]" )


ip2.ull_test.ullppp[0][0] = trick.TMM_declare_var_s( "unsigned long long [14]" )
ip2.ull_test.ullppp[0][1] = trick.TMM_declare_var_s( "unsigned long long [15]" )
ip2.ull_test.ullppp[0][2] = trick.TMM_declare_var_s( "unsigned long long [16]" )
ip2.ull_test.ullppp[0][3] = trick.TMM_declare_var_s( "unsigned long long [17]" )
ip2.ull_test.ullppp[0][4] = trick.TMM_declare_var_s( "unsigned long long [18]" )

ip2.ull_test.ullppp[1][0] = trick.TMM_declare_var_s( "unsigned long long [21]" )
ip2.ull_test.ullppp[1][1] = trick.TMM_declare_var_s( "unsigned long long [22]" )
ip2.ull_test.ullppp[1][2] = trick.TMM_declare_var_s( "unsigned long long [23]" )
ip2.ull_test.ullppp[1][3] = trick.TMM_declare_var_s( "unsigned long long [24]" )
ip2.ull_test.ullppp[1][4] = trick.TMM_declare_var_s( "unsigned long long [25]" )

ip2.ull_test.ullppp[1][4][10] = [ 56 , 67 , 78 , 89 , 90 ]

ip2.ull_test.ullp2 = id(ip2.ull_test.ullppp[1][4][12])
ip2.ull_test.ullp2[0] = [ 200 , 201 ]
ip2.ull_test.ullp2[12] = 300
'''should cause an error'''
#ip2.ull_test.ullp2[13] = 301

ip2.ull_test.ullp3 = id(ip2.ull_test.ullppp[0][3][6])

ip2.ull_test.ullpp3 = id(ip2.ull_test.ullppp[1][2])
ip2.ull_test.ullpp3[2][3] = 400

'''-----------------------------------------------'''

ip2.f_test.f = 123
ip2.f_test.fa[0] = [ 4 , 5 , 6 , 7 ]
ip2.f_test.faa[0][0] = [ 4000 , 5000 , 6000 , 7000 ]
ip2.f_test.faa[1][0] = [ 1000 , 2000 , 3000 , 4000 ]

ip2.f_test.fp = id(ip2.f_test.f)
ip2.f_test.fp[0] = 345

ip2.f_test.fp = id(ip2.f_test.fa[2])
ip2.f_test.fp[0] = 234

ip2.f_test.fpp2 = trick.TMM_declare_var_s( "float* [4]" )
ip2.f_test.fpp2[1] = id(ip2.f_test.fa[5])
ip2.f_test.fpp2[1][0] = 111

ip2.f_test.fpp = trick.TMM_declare_var_s( "float* [6]" )
ip2.f_test.fpp[0] = trick.TMM_declare_var_s( "float [2]" )
ip2.f_test.fpp[1] = trick.TMM_declare_var_s( "float [4]" )
ip2.f_test.fpp[2] = trick.TMM_declare_var_s( "float [6]" )
ip2.f_test.fpp[3] = trick.TMM_declare_var_s( "float [8]" )
ip2.f_test.fpp[4] = trick.TMM_declare_var_s( "float [10]" )
ip2.f_test.fpp[5] = trick.TMM_declare_var_s( "float [12]" )

ip2.f_test.fppp = trick.TMM_declare_var_s( "float** [5]" )
ip2.f_test.fppp[0] = trick.TMM_declare_var_s( "float* [6]" )
ip2.f_test.fppp[1] = trick.TMM_declare_var_s( "float* [7]" )
ip2.f_test.fppp[2] = trick.TMM_declare_var_s( "float* [8]" )
ip2.f_test.fppp[3] = trick.TMM_declare_var_s( "float* [9]" )
ip2.f_test.fppp[4] = trick.TMM_declare_var_s( "float* [10]" )


ip2.f_test.fppp[0][0] = trick.TMM_declare_var_s( "float [14]" )
ip2.f_test.fppp[0][1] = trick.TMM_declare_var_s( "float [15]" )
ip2.f_test.fppp[0][2] = trick.TMM_declare_var_s( "float [16]" )
ip2.f_test.fppp[0][3] = trick.TMM_declare_var_s( "float [17]" )
ip2.f_test.fppp[0][4] = trick.TMM_declare_var_s( "float [18]" )

ip2.f_test.fppp[1][0] = trick.TMM_declare_var_s( "float [21]" )
ip2.f_test.fppp[1][1] = trick.TMM_declare_var_s( "float [22]" )
ip2.f_test.fppp[1][2] = trick.TMM_declare_var_s( "float [23]" )
ip2.f_test.fppp[1][3] = trick.TMM_declare_var_s( "float [24]" )
ip2.f_test.fppp[1][4] = trick.TMM_declare_var_s( "float [25]" )

ip2.f_test.fppp[1][4][10] = [ 56 , 67 , 78 , 89 , 90 ]

ip2.f_test.fp2 = id(ip2.f_test.fppp[1][4][12])
ip2.f_test.fp2[0] = [ 200 , 201 ]
ip2.f_test.fp2[12] = 300

'''should cause an error'''
#ip2.f_test.fp2[13] = 301

ip2.f_test.fp3 = id(ip2.f_test.fppp[0][3][6])

ip2.f_test.fpp3 = id(ip2.f_test.fppp[1][2])

ip2.f_test.fpp3[2][3] = 400

'''-----------------------------------------------'''

ip2.d_test.d = 123
ip2.d_test.da[0] = [ 4 , 5 , 6 , 7 ]
ip2.d_test.daa[0][0] = [ 4000 , 5000 , 6000 , 7000 ]
ip2.d_test.daa[1][0] = [ 1000 , 2000 , 3000 , 4000 ]

ip2.d_test.dp = id(ip2.d_test.d)
ip2.d_test.dp[0] = 345

ip2.d_test.dp = id(ip2.d_test.da[2])
ip2.d_test.dp[0] = 234

ip2.d_test.dpp2 = trick.TMM_declare_var_s( "double* [4]" )
ip2.d_test.dpp2[1] = id(ip2.d_test.da[5])
ip2.d_test.dpp2[1][0] = 111

ip2.d_test.dpp = trick.TMM_declare_var_s( "double* [6]" )
ip2.d_test.dpp[0] = trick.TMM_declare_var_s( "double [2]" )
ip2.d_test.dpp[1] = trick.TMM_declare_var_s( "double [4]" )
ip2.d_test.dpp[2] = trick.TMM_declare_var_s( "double [6]" )
ip2.d_test.dpp[3] = trick.TMM_declare_var_s( "double [8]" )
ip2.d_test.dpp[4] = trick.TMM_declare_var_s( "double [10]" )
ip2.d_test.dpp[5] = trick.TMM_declare_var_s( "double [12]" )

ip2.d_test.dppp = trick.TMM_declare_var_s( "double** [5]" )
ip2.d_test.dppp[0] = trick.TMM_declare_var_s( "double* [6]" )
ip2.d_test.dppp[1] = trick.TMM_declare_var_s( "double* [7]" )
ip2.d_test.dppp[2] = trick.TMM_declare_var_s( "double* [8]" )
ip2.d_test.dppp[3] = trick.TMM_declare_var_s( "double* [9]" )
ip2.d_test.dppp[4] = trick.TMM_declare_var_s( "double* [10]" )


ip2.d_test.dppp[0][0] = trick.TMM_declare_var_s( "double [14]" )
ip2.d_test.dppp[0][1] = trick.TMM_declare_var_s( "double [15]" )
ip2.d_test.dppp[0][2] = trick.TMM_declare_var_s( "double [16]" )
ip2.d_test.dppp[0][3] = trick.TMM_declare_var_s( "double [17]" )
ip2.d_test.dppp[0][4] = trick.TMM_declare_var_s( "double [18]" )

ip2.d_test.dppp[1][0] = trick.TMM_declare_var_s( "double [21]" )
ip2.d_test.dppp[1][1] = trick.TMM_declare_var_s( "double [22]" )
ip2.d_test.dppp[1][2] = trick.TMM_declare_var_s( "double [23]" )
ip2.d_test.dppp[1][3] = trick.TMM_declare_var_s( "double [24]" )
ip2.d_test.dppp[1][4] = trick.TMM_declare_var_s( "double [25]" )

ip2.d_test.dppp[1][4][10] = [ 56 , 67 , 78 , 89 , 90 ]

ip2.d_test.dp2 = id(ip2.d_test.dppp[1][4][12])
ip2.d_test.dp2[0] = [ 200 , 201 ]
ip2.d_test.dp2[12] = 300

'''should cause an error'''
#ip2.d_test.dp2[13] = 301

ip2.d_test.dp3 = id(ip2.d_test.dppp[0][3][6])

ip2.d_test.dpp3 = id(ip2.d_test.dppp[1][2])

ip2.d_test.dpp3[2][3] = 400

'''-----------------------------------------------'''

"""
ip2.everything.cs.c = 66
ip2.everything.csa[1].c = 67
ip2.everything.csaa[2][2].c = 68

ip2.everything.csp = id(ip2.everything.cs)
ip2.everything.csp[0].ca[3] = 69
ip2.everything.csp[0].caa[2][2] = 73

ip2.everything.csp1 = id(ip2.everything.csaa[2][2])
ip2.everything.csp1[0].caa[1][1] = 70
ip2.everything.csp1[1].caa[1][0] = 71

ip2.everything.csp2 = trick.TMM_declare_var_s( "CHAR_STR [8]" )
ip2.everything.csp2[0].ca[1] = 74
ip2.everything.csp2[1].ca[2] = 75
ip2.everything.csp2[5].caa[1][1] = 76
ip2.everything.csp2[5].cp = id(ip2.everything.csp2[5].caa[1][1])
ip2.everything.csp2[5].cp2 = id(ip2.everything.csp2[2].caa[2][2])
"""

'''-----------------------------------------------'''

ip2.everything.ucs.uc = 66
ip2.everything.ucsa[1].uc = 67
ip2.everything.ucsaa[2][2].uc = 68

ip2.everything.ucsp = id(ip2.everything.ucs)
ip2.everything.ucsp[0].uca[3] = 69
ip2.everything.ucsp[0].ucaa[2][2] = 73

ip2.everything.ucsp1 = id(ip2.everything.ucsaa[2][2])
ip2.everything.ucsp1[0].ucaa[1][1] = 70
ip2.everything.ucsp1[1].ucaa[1][0] = 71

ip2.everything.ucsp2 = trick.TMM_declare_var_s( "UCHAR_STR [8]" )
ip2.everything.ucsp2[0].uca[1] = 74
ip2.everything.ucsp2[1].uca[2] = 75
ip2.everything.ucsp2[5].ucaa[1][1] = 76
ip2.everything.ucsp2[5].ucp = id(ip2.everything.ucsp2[5].ucaa[1][1])
ip2.everything.ucsp2[5].ucp2 = id(ip2.everything.ucsp2[2].ucaa[2][2])

'''-----------------------------------------------'''

ip2.everything.ss.s = 66
ip2.everything.ssa[1].s = 67
ip2.everything.ssaa[2][2].s = 68

ip2.everything.ssp = id(ip2.everything.ss)
ip2.everything.ssp[0].sa[3] = 69
ip2.everything.ssp[0].saa[2][2] = 73

ip2.everything.ssp1 = id(ip2.everything.ssaa[2][2])
ip2.everything.ssp1[0].saa[1][1] = 70
ip2.everything.ssp1[1].saa[1][0] = 71

ip2.everything.ssp2 = trick.TMM_declare_var_s( "SHORT_STR [8]" )
ip2.everything.ssp2[0].sa[1] = 74
ip2.everything.ssp2[1].sa[2] = 75
ip2.everything.ssp2[5].saa[1][1] = 76
ip2.everything.ssp2[5].sp = id(ip2.everything.ssp2[5].saa[1][1])
ip2.everything.ssp2[5].sp2 = id(ip2.everything.ssp2[2].saa[2][2])

'''-----------------------------------------------'''

ip2.everything.uss.us = 66
ip2.everything.ussa[1].us = 67
ip2.everything.ussaa[2][2].us = 68

ip2.everything.ussp = id(ip2.everything.uss)
ip2.everything.ussp[0].usa[3] = 69
ip2.everything.ussp[0].usaa[2][2] = 73

ip2.everything.ussp1 = id(ip2.everything.ussaa[2][2])
ip2.everything.ussp1[0].usaa[1][1] = 70
ip2.everything.ussp1[1].usaa[1][0] = 71

ip2.everything.ussp2 = trick.TMM_declare_var_s( "SHORT_STR [8]" )
ip2.everything.ussp2[0].usa[1] = 74
ip2.everything.ussp2[1].usa[2] = 75
ip2.everything.ussp2[5].usaa[1][1] = 76
ip2.everything.ussp2[5].usp = id(ip2.everything.ussp2[5].usaa[1][1])
ip2.everything.ussp2[5].usp2 = id(ip2.everything.ussp2[2].usaa[2][2])

'''-----------------------------------------------'''

ip2.everything._is.i = 66
ip2.everything.isa[1].i = 67
ip2.everything.isaa[2][2].i = 68

ip2.everything.isp = id(ip2.everything._is)
ip2.everything.isp[0].ia[3] = 69
ip2.everything.isp[0].iaa[2][2] = 73

ip2.everything.isp1 = id(ip2.everything.isaa[2][2])
ip2.everything.isp1[0].iaa[1][1] = 70
ip2.everything.isp1[1].iaa[1][0] = 71

ip2.everything.isp2 = trick.TMM_declare_var_s( "INT_STR [8]" )
ip2.everything.isp2[0].ia[1] = 74
ip2.everything.isp2[1].ia[2] = 75
ip2.everything.isp2[5].iaa[1][1] = 76
ip2.everything.isp2[5].ip = id(ip2.everything.isp2[5].iaa[1][1])
ip2.everything.isp2[5].ip2 = id(ip2.everything.isp2[2].iaa[2][2])

'''-----------------------------------------------'''

ip2.everything.uis.ui = 66
ip2.everything.uisa[1].ui = 67
ip2.everything.uisaa[2][2].ui = 68

ip2.everything.uisp = id(ip2.everything.uis)
ip2.everything.uisp[0].uia[3] = 69
ip2.everything.uisp[0].uiaa[2][2] = 73

ip2.everything.uisp1 = id(ip2.everything.uisaa[2][2])
ip2.everything.uisp1[0].uiaa[1][1] = 70
ip2.everything.uisp1[1].uiaa[1][0] = 71

ip2.everything.uisp2 = trick.TMM_declare_var_s( "UINT_STR [8]" )
ip2.everything.uisp2[0].uia[1] = 74
ip2.everything.uisp2[1].uia[2] = 75
ip2.everything.uisp2[5].uiaa[1][1] = 76
ip2.everything.uisp2[5].uip = id(ip2.everything.uisp2[5].uiaa[1][1])
ip2.everything.uisp2[5].uip2 = id(ip2.everything.uisp2[2].uiaa[2][2])

'''-----------------------------------------------'''

ip2.everything.ls.l = 66
ip2.everything.lsa[1].l = 67
ip2.everything.lsaa[2][2].l = 68

ip2.everything.lsp = id(ip2.everything.ls)
ip2.everything.lsp[0].la[3] = 69
ip2.everything.lsp[0].laa[2][2] = 73

ip2.everything.lsp1 = id(ip2.everything.lsaa[2][2])
ip2.everything.lsp1[0].laa[1][1] = 70
ip2.everything.lsp1[1].laa[1][0] = 71

ip2.everything.lsp2 = trick.TMM_declare_var_s( "LONG_STR [8]" )
ip2.everything.lsp2[0].la[1] = 74
ip2.everything.lsp2[1].la[2] = 75
ip2.everything.lsp2[5].laa[1][1] = 76
ip2.everything.lsp2[5].lp = id(ip2.everything.lsp2[5].laa[1][1])
ip2.everything.lsp2[5].lp2 = id(ip2.everything.lsp2[2].laa[2][2])

'''-----------------------------------------------'''

ip2.everything.uls.ul = 66
ip2.everything.ulsa[1].ul = 67
ip2.everything.ulsaa[2][2].ul = 68

ip2.everything.ulsp = id(ip2.everything.uls)
ip2.everything.ulsp[0].ula[3] = 69
ip2.everything.ulsp[0].ulaa[2][2] = 73

ip2.everything.ulsp1 = id(ip2.everything.ulsaa[2][2])
ip2.everything.ulsp1[0].ulaa[1][1] = 70
ip2.everything.ulsp1[1].ulaa[1][0] = 71

ip2.everything.ulsp2 = trick.TMM_declare_var_s( "ULONG_STR [8]" )
ip2.everything.ulsp2[0].ula[1] = 74
ip2.everything.ulsp2[1].ula[2] = 75
ip2.everything.ulsp2[5].ulaa[1][1] = 76
ip2.everything.ulsp2[5].ulp = id(ip2.everything.ulsp2[5].ulaa[1][1])
ip2.everything.ulsp2[5].ulp2 = id(ip2.everything.ulsp2[2].ulaa[2][2])

'''-----------------------------------------------'''

ip2.everything.lls.ll = 66
ip2.everything.llsa[1].ll = 67
ip2.everything.llsaa[2][2].ll = 68

ip2.everything.llsp = id(ip2.everything.lls)
ip2.everything.llsp[0].lla[3] = 69
ip2.everything.llsp[0].llaa[2][2] = 73

ip2.everything.llsp1 = id(ip2.everything.llsaa[2][2])
ip2.everything.llsp1[0].llaa[1][1] = 70
ip2.everything.llsp1[1].llaa[1][0] = 71

ip2.everything.llsp2 = trick.TMM_declare_var_s( "LONG_LONG_STR [8]" )
ip2.everything.llsp2[0].lla[1] = 74
ip2.everything.llsp2[1].lla[2] = 75
ip2.everything.llsp2[5].llaa[1][1] = 76
ip2.everything.llsp2[5].llp = id(ip2.everything.llsp2[5].llaa[1][1])
ip2.everything.llsp2[5].llp2 = id(ip2.everything.llsp2[2].llaa[2][2])

'''-----------------------------------------------'''

ip2.everything.ulls.ull = 66
ip2.everything.ullsa[1].ull = 67
ip2.everything.ullsaa[2][2].ull = 68

ip2.everything.ullsp = id(ip2.everything.ulls)
ip2.everything.ullsp[0].ulla[3] = 69
ip2.everything.ullsp[0].ullaa[2][2] = 73

ip2.everything.ullsp1 = id(ip2.everything.ullsaa[2][2])
ip2.everything.ullsp1[0].ullaa[1][1] = 70
ip2.everything.ullsp1[1].ullaa[1][0] = 71

ip2.everything.ullsp2 = trick.TMM_declare_var_s( "ULONG_LONG_STR [8]" )
ip2.everything.ullsp2[0].ulla[1] = 74
ip2.everything.ullsp2[1].ulla[2] = 75
ip2.everything.ullsp2[5].ullaa[1][1] = 76
ip2.everything.ullsp2[5].ullp = id(ip2.everything.ullsp2[5].ullaa[1][1])
ip2.everything.ullsp2[5].ullp2 = id(ip2.everything.ullsp2[2].ullaa[2][2])

'''-----------------------------------------------'''

ip2.everything.fs.f = 66
ip2.everything.fsa[1].f = 67
ip2.everything.fsaa[2][2].f = 68

ip2.everything.fsp = id(ip2.everything.fs)
ip2.everything.fsp[0].fa[3] = 69
ip2.everything.fsp[0].faa[2][2] = 73

ip2.everything.fsp1 = id(ip2.everything.fsaa[2][2])
ip2.everything.fsp1[0].faa[1][1] = 70
ip2.everything.fsp1[1].faa[1][0] = 71

ip2.everything.fsp2 = trick.TMM_declare_var_s( "FLOAT_STR [8]" )
ip2.everything.fsp2[0].fa[1] = 74
ip2.everything.fsp2[1].fa[2] = 75
ip2.everything.fsp2[5].faa[1][1] = 76
ip2.everything.fsp2[5].fp = id(ip2.everything.fsp2[5].faa[1][1])
ip2.everything.fsp2[5].fp2 = id(ip2.everything.fsp2[2].faa[2][2])

'''-----------------------------------------------'''

ip2.everything.ds.d = 66
ip2.everything.dsa[1].d = 67
ip2.everything.dsaa[2][2].d = 68

ip2.everything.dsp = id(ip2.everything.ds)
ip2.everything.dsp[0].da[3] = 69
ip2.everything.dsp[0].daa[2][2] = 73

ip2.everything.dsp1 = id(ip2.everything.dsaa[2][2])
ip2.everything.dsp1[0].daa[1][1] = 70
ip2.everything.dsp1[1].daa[1][0] = 71

ip2.everything.dsp2 = trick.TMM_declare_var_s( "DOUBLE_STR [8]" )
ip2.everything.dsp2[0].da[1] = 74
ip2.everything.dsp2[1].da[2] = 75
ip2.everything.dsp2[5].daa[1][1] = 76
ip2.everything.dsp2[5].dp = id(ip2.everything.dsp2[5].daa[1][1])
ip2.everything.dsp2[5].dp2 = id(ip2.everything.dsp2[2].daa[2][2])

'''-----------------------------------------------'''

ip2.linked_list.i = 500
ip2.linked_list.next = trick.TMM_declare_var_s( "LinkedList [1]" )

ip2.linked_list.next[0].i = 501
ip2.linked_list.next[0].next = trick.TMM_declare_var_s( "LinkedList [1]" )

"""
ip2.linked_list.next[0].next[0].i = 502
ip2.linked_list.next[0].next[0].next = trick.TMM_declare_var_s( "LinkedList [1]" )

ip2.linked_list.next[0].next[0].next[0].i = 503
ip2.linked_list.next[0].next[0].next[0].next = trick.TMM_declare_var_s( "LinkedList [1]" )

ip2.linked_list.next[0].next[0].next[0].next[0].i = 504
ip2.linked_list.next[0].next[0].next[0].next[0].next = trick.TMM_declare_var_s( "LinkedList [1]" )
"""

ip2.llp = id(ip2.linked_list.next[0])
ip2.llp[0].i = 602

ip2.llp[0].next = trick.TMM_declare_var_s( "LINKED_LIST [1]" )
ip2.llp[0].next[0].i = 603

"""
ip2.linked_list.next[0].next = trick.TMM_declare_var_s( "LINKED_LIST [1]" )
ip2.linked_list.next[0].next[0].i = 502
ip2.llp = id(ip2.linked_list.next[0])
ip2.llp.next = trick.TMM_declare_var_s( "LINKED_LIST [1]" )
ip2.llp[0].next[0].i = 502
"""

'''-----------------------------------------------'''

ip2.everything.flp = id(ip2.everything.fla[0])
ip2.everything.fl = On

trick_ip.ip.add_read(1 , """trick.sim_services.checkpoint("end_checkpoint")""")
trick_ip.ip.add_read(1 , """trick.sim_services.checkpoint("chkpnt_pre_init")""")

trick.sim_services.exec_set_terminate_time(2)
