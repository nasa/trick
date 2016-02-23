
import math
from trick.unit_test import *

def main():

    trick.exec_set_terminate_time(1.0)

    trick_utest.unit_tests.enable()
    trick_utest.unit_tests.set_file_name( os.getenv("TRICK_HOME") + "/trick_test/SIM_test_ip_rti.xml" )
    trick_utest.unit_tests.set_test_name( "RTI" )

######################################################################################################################

    test_suite = "rti_add_int"

    test_so.obj.d = 0.0
    trick.rti_add("test_so.obj.d", 50)
    test_so.obj.da[1] = 0.0
    trick.rti_add("test_so.obj.da[1]", 51)
    test_so.obj.dp = trick.TMM_declare_var_s("double[6]")
    trick.rti_add("test_so.obj.dp[1]", 52)
    test_so.obj.daa[1][1] = 0.0
    trick.rti_add("test_so.obj.daa[1][1]", 53)
    test_so.obj.dap[1] = trick.TMM_declare_var_1d( "double", 4)
    trick.rti_add("test_so.obj.dap[1][1]", 54)
    test_so.obj.dpp = trick.TMM_declare_var_s("double *[4]")
    test_so.obj.dpp[1] = trick.TMM_declare_var_1d( "double", 5)
    trick.rti_add("test_so.obj.dpp[1][1]", 55)

    #trick.rti_list()
    trick.rti_fire()
    trick_inject.rtis.GetRtiExecutor(0).Exec()

    TRICK_EXPECT_NEAR( test_so.obj.d , 50.0 , 0.000001 , test_suite , "double" )
    TRICK_EXPECT_NEAR( test_so.obj.da[1] , 51.0 , 0.000001 , test_suite , "double array" )
    TRICK_EXPECT_NEAR( test_so.obj.dp[1] , 52.0 , 0.000001 , test_suite , "double pointer" )
    TRICK_EXPECT_NEAR( test_so.obj.daa[1][1] , 53.0 , 0.000001 , test_suite , "2D double array" )
    TRICK_EXPECT_NEAR( test_so.obj.dap[1][1] , 54.0 , 0.000001 , test_suite , "2D double array of ptr" )
    TRICK_EXPECT_NEAR( test_so.obj.dpp[1][1] , 55.0 , 0.000001 , test_suite , "2D double ptr of ptr" )

######################################################################################################################

    test_so.obj.f = 0.0
    trick.rti_add("test_so.obj.f", 50)
    test_so.obj.fa[1] = 0.0
    trick.rti_add("test_so.obj.fa[1]", 51)
    test_so.obj.fp = trick.TMM_declare_var_s("float[6]")
    trick.rti_add("test_so.obj.fp[1]", 52)
    test_so.obj.faa[1][1] = 0.0
    trick.rti_add("test_so.obj.faa[1][1]", 53)
    test_so.obj.fap[1] = trick.TMM_declare_var_1d( "float", 4)
    trick.rti_add("test_so.obj.fap[1][1]", 54)
    test_so.obj.fpp = trick.TMM_declare_var_s("float *[4]")
    test_so.obj.fpp[1] = trick.TMM_declare_var_1d( "float", 5)
    trick.rti_add("test_so.obj.fpp[1][1]", 55)

    #trick.rti_list()
    trick.rti_fire()
    trick_inject.rtis.GetRtiExecutor(0).Exec()

    TRICK_EXPECT_NEAR( test_so.obj.f , 50.0 , 0.000001 , test_suite , "float" )
    TRICK_EXPECT_NEAR( test_so.obj.fa[1] , 51.0 , 0.000001 , test_suite , "float array" )
    TRICK_EXPECT_NEAR( test_so.obj.fp[1] , 52.0 , 0.000001 , test_suite , "float pointer" )
    TRICK_EXPECT_NEAR( test_so.obj.faa[1][1] , 53.0 , 0.000001 , test_suite , "2D float array" )
    TRICK_EXPECT_NEAR( test_so.obj.fap[1][1] , 54.0 , 0.000001 , test_suite , "2D float array of ptr" )
    TRICK_EXPECT_NEAR( test_so.obj.fpp[1][1] , 55.0 , 0.000001 , test_suite , "2D float ptr of ptr" )

######################################################################################################################

    test_so.obj.c = 0
    trick.rti_add("test_so.obj.c", 50)
    test_so.obj.ca[1] = 0
    trick.rti_add("test_so.obj.ca[1]", 51)
    test_so.obj.cp = trick.TMM_declare_var_s("char[6]")
    trick.rti_add("test_so.obj.cp[1]", 52)
    test_so.obj.caa[1][1] = 0
    trick.rti_add("test_so.obj.caa[1][1]", 53)
    test_so.obj.cap[1] = trick.TMM_declare_var_1d( "char", 4)
    trick.rti_add("test_so.obj.cap[1][1]", 54)
    test_so.obj.cpp = trick.TMM_declare_var_s("char *[4]")
    test_so.obj.cpp[1] = trick.TMM_declare_var_1d( "char", 5)
    trick.rti_add("test_so.obj.cpp[1][1]", 55)

    #trick.rti_list()
    trick.rti_fire()
    trick_inject.rtis.GetRtiExecutor(0).Exec()

    TRICK_EXPECT_EQ( test_so.obj.c , 50 , test_suite , "char" )
    TRICK_EXPECT_EQ( test_so.obj.ca[1] , 51 , test_suite , "char array" )
    TRICK_EXPECT_EQ( test_so.obj.cp[1] , 52 , test_suite , "char pointer" )
    TRICK_EXPECT_EQ( test_so.obj.caa[1][1] , 53 , test_suite , "2D char array" )
    TRICK_EXPECT_EQ( test_so.obj.cap[1][1] , 54 , test_suite , "2D char array of ptr" )
    TRICK_EXPECT_EQ( test_so.obj.cpp[1][1] , 55 , test_suite , "2D char ptr of ptr" )

######################################################################################################################

    test_so.obj.uc = 0
    trick.rti_add("test_so.obj.uc", 50)
    test_so.obj.uca[1] = 0
    trick.rti_add("test_so.obj.uca[1]", 51)
    test_so.obj.ucp = trick.TMM_declare_var_s("unsigned char[6]")
    trick.rti_add("test_so.obj.ucp[1]", 52)
    test_so.obj.ucaa[1][1] = 0
    trick.rti_add("test_so.obj.ucaa[1][1]", 53)
    test_so.obj.ucap[1] = trick.TMM_declare_var_1d( "unsigned char", 4)
    trick.rti_add("test_so.obj.ucap[1][1]", 54)
    test_so.obj.ucpp = trick.TMM_declare_var_s("unsigned char *[4]")
    test_so.obj.ucpp[1] = trick.TMM_declare_var_1d( "unsigned char", 5)
    trick.rti_add("test_so.obj.ucpp[1][1]", 55)

    #trick.rti_list()
    trick.rti_fire()
    trick_inject.rtis.GetRtiExecutor(0).Exec()

    TRICK_EXPECT_EQ( test_so.obj.uc , 50 , test_suite , "unsigned char" )
    TRICK_EXPECT_EQ( test_so.obj.uca[1] , 51 , test_suite , "unsigned char array" )
    TRICK_EXPECT_EQ( test_so.obj.ucp[1] , 52 , test_suite , "unsigned char pointer" )
    TRICK_EXPECT_EQ( test_so.obj.ucaa[1][1] , 53 , test_suite , "2D unsigned char array" )
    TRICK_EXPECT_EQ( test_so.obj.ucap[1][1] , 54 , test_suite , "2D unsigned char array of ptr" )
    TRICK_EXPECT_EQ( test_so.obj.ucpp[1][1] , 55 , test_suite , "2D unsigned char ptr of ptr" )

######################################################################################################################

    test_so.obj.s = 0
    trick.rti_add("test_so.obj.s", 50)
    test_so.obj.sa[1] = 0
    trick.rti_add("test_so.obj.sa[1]", 51)
    test_so.obj.sp = trick.TMM_declare_var_s("short[6]")
    trick.rti_add("test_so.obj.sp[1]", 52)
    test_so.obj.saa[1][1] = 0
    trick.rti_add("test_so.obj.saa[1][1]", 53)
    test_so.obj.sap[1] = trick.TMM_declare_var_1d( "short", 4)
    trick.rti_add("test_so.obj.sap[1][1]", 54)
    test_so.obj.spp = trick.TMM_declare_var_s("short *[4]")
    test_so.obj.spp[1] = trick.TMM_declare_var_1d( "short", 5)
    trick.rti_add("test_so.obj.spp[1][1]", 55)

    #trick.rti_list()
    trick.rti_fire()
    trick_inject.rtis.GetRtiExecutor(0).Exec()

    TRICK_EXPECT_EQ( test_so.obj.s , 50 , test_suite , "short" )
    TRICK_EXPECT_EQ( test_so.obj.sa[1] , 51 , test_suite , "short array" )
    TRICK_EXPECT_EQ( test_so.obj.sp[1] , 52 , test_suite , "short pointer" )
    TRICK_EXPECT_EQ( test_so.obj.saa[1][1] , 53 , test_suite , "2D short array" )
    TRICK_EXPECT_EQ( test_so.obj.sap[1][1] , 54 , test_suite , "2D short array of ptr" )
    TRICK_EXPECT_EQ( test_so.obj.spp[1][1] , 55 , test_suite , "2D short ptr of ptr" )

######################################################################################################################

    test_so.obj.us = 0
    trick.rti_add("test_so.obj.us", 50)
    test_so.obj.usa[1] = 0
    trick.rti_add("test_so.obj.usa[1]", 51)
    test_so.obj.usp = trick.TMM_declare_var_s("unsigned short[6]")
    trick.rti_add("test_so.obj.usp[1]", 52)
    test_so.obj.usaa[1][1] = 0
    trick.rti_add("test_so.obj.usaa[1][1]", 53)
    test_so.obj.usap[1] = trick.TMM_declare_var_1d( "unsigned short", 4)
    trick.rti_add("test_so.obj.usap[1][1]", 54)
    test_so.obj.uspp = trick.TMM_declare_var_s("unsigned short *[4]")
    test_so.obj.uspp[1] = trick.TMM_declare_var_1d( "unsigned short", 5)
    trick.rti_add("test_so.obj.uspp[1][1]", 55)

    #trick.rti_list()
    trick.rti_fire()
    trick_inject.rtis.GetRtiExecutor(0).Exec()

    TRICK_EXPECT_EQ( test_so.obj.us , 50 , test_suite , "unsigned short" )
    TRICK_EXPECT_EQ( test_so.obj.usa[1] , 51 , test_suite , "unsigned short array" )
    TRICK_EXPECT_EQ( test_so.obj.usp[1] , 52 , test_suite , "unsigned short pointer" )
    TRICK_EXPECT_EQ( test_so.obj.usaa[1][1] , 53 , test_suite , "2D unsigned short array" )
    TRICK_EXPECT_EQ( test_so.obj.usap[1][1] , 54 , test_suite , "2D unsigned short array of ptr" )
    TRICK_EXPECT_EQ( test_so.obj.uspp[1][1] , 55 , test_suite , "2D unsigned short ptr of ptr" )

######################################################################################################################

    test_so.obj.i = 0
    trick.rti_add("test_so.obj.i", 50)
    test_so.obj.ia[1] = 0
    trick.rti_add("test_so.obj.ia[1]", 51)
    test_so.obj.ip = trick.TMM_declare_var_s("int[6]")
    trick.rti_add("test_so.obj.ip[1]", 52)
    test_so.obj.iaa[1][1] = 0
    trick.rti_add("test_so.obj.iaa[1][1]", 53)
    test_so.obj.iap[1] = trick.TMM_declare_var_1d( "int", 4)
    trick.rti_add("test_so.obj.iap[1][1]", 54)
    test_so.obj.ipp = trick.TMM_declare_var_s("int *[4]")
    test_so.obj.ipp[1] = trick.TMM_declare_var_1d( "int", 5)
    trick.rti_add("test_so.obj.ipp[1][1]", 55)

    #trick.rti_list()
    trick.rti_fire()
    trick_inject.rtis.GetRtiExecutor(0).Exec()

    TRICK_EXPECT_EQ( test_so.obj.i , 50 , test_suite , "int" )
    TRICK_EXPECT_EQ( test_so.obj.ia[1] , 51 , test_suite , "int array" )
    TRICK_EXPECT_EQ( test_so.obj.ip[1] , 52 , test_suite , "int pointer" )
    TRICK_EXPECT_EQ( test_so.obj.iaa[1][1] , 53 , test_suite , "2D int array" )
    TRICK_EXPECT_EQ( test_so.obj.iap[1][1] , 54 , test_suite , "2D int array of ptr" )
    TRICK_EXPECT_EQ( test_so.obj.ipp[1][1] , 55 , test_suite , "2D int ptr of ptr" )

######################################################################################################################

    test_so.obj.ui = 0
    trick.rti_add("test_so.obj.ui", 50)
    test_so.obj.uia[1] = 0
    trick.rti_add("test_so.obj.uia[1]", 51)
    test_so.obj.uip = trick.TMM_declare_var_s("unsigned int[6]")
    trick.rti_add("test_so.obj.uip[1]", 52)
    test_so.obj.uiaa[1][1] = 0
    trick.rti_add("test_so.obj.uiaa[1][1]", 53)
    test_so.obj.uiap[1] = trick.TMM_declare_var_1d( "unsigned int", 4)
    trick.rti_add("test_so.obj.uiap[1][1]", 54)
    test_so.obj.uipp = trick.TMM_declare_var_s("unsigned int *[4]")
    test_so.obj.uipp[1] = trick.TMM_declare_var_1d( "unsigned int", 5)
    trick.rti_add("test_so.obj.uipp[1][1]", 55)

    #trick.rti_list()
    trick.rti_fire()
    trick_inject.rtis.GetRtiExecutor(0).Exec()

    TRICK_EXPECT_EQ( test_so.obj.ui , 50 , test_suite , "unsigned int" )
    TRICK_EXPECT_EQ( test_so.obj.uia[1] , 51 , test_suite , "unsigned int array" )
    TRICK_EXPECT_EQ( test_so.obj.uip[1] , 52 , test_suite , "unsigned int pointer" )
    TRICK_EXPECT_EQ( test_so.obj.uiaa[1][1] , 53 , test_suite , "2D unsigned int array" )
    TRICK_EXPECT_EQ( test_so.obj.uiap[1][1] , 54 , test_suite , "2D unsigned int array of ptr" )
    TRICK_EXPECT_EQ( test_so.obj.uipp[1][1] , 55 , test_suite , "2D unsigned int ptr of ptr" )

######################################################################################################################

    test_so.obj.l = 0
    trick.rti_add("test_so.obj.l", 50)
    test_so.obj.la[1] = 0
    trick.rti_add("test_so.obj.la[1]", 51)
    test_so.obj.lp = trick.TMM_declare_var_s("long[6]")
    trick.rti_add("test_so.obj.lp[1]", 52)
    test_so.obj.laa[1][1] = 0
    trick.rti_add("test_so.obj.laa[1][1]", 53)
    test_so.obj.lap[1] = trick.TMM_declare_var_1d( "long", 4)
    trick.rti_add("test_so.obj.lap[1][1]", 54)
    test_so.obj.lpp = trick.TMM_declare_var_s("long *[4]")
    test_so.obj.lpp[1] = trick.TMM_declare_var_1d( "long", 5)
    trick.rti_add("test_so.obj.lpp[1][1]", 55)

    #trick.rti_list()
    trick.rti_fire()
    trick_inject.rtis.GetRtiExecutor(0).Exec()

    TRICK_EXPECT_EQ( test_so.obj.l , 50 , test_suite , "long" )
    TRICK_EXPECT_EQ( test_so.obj.la[1] , 51 , test_suite , "long array" )
    TRICK_EXPECT_EQ( test_so.obj.lp[1] , 52 , test_suite , "long polonger" )
    TRICK_EXPECT_EQ( test_so.obj.laa[1][1] , 53 , test_suite , "2D long array" )
    TRICK_EXPECT_EQ( test_so.obj.lap[1][1] , 54 , test_suite , "2D long array of ptr" )
    TRICK_EXPECT_EQ( test_so.obj.lpp[1][1] , 55 , test_suite , "2D long ptr of ptr" )

######################################################################################################################

    test_so.obj.ul = 0
    trick.rti_add("test_so.obj.ul", 50)
    test_so.obj.ula[1] = 0
    trick.rti_add("test_so.obj.ula[1]", 51)
    test_so.obj.ulp = trick.TMM_declare_var_s("unsigned long[6]")
    trick.rti_add("test_so.obj.ulp[1]", 52)
    test_so.obj.ulaa[1][1] = 0
    trick.rti_add("test_so.obj.ulaa[1][1]", 53)
    test_so.obj.ulap[1] = trick.TMM_declare_var_1d( "unsigned long", 4)
    trick.rti_add("test_so.obj.ulap[1][1]", 54)
    test_so.obj.ulpp = trick.TMM_declare_var_s("unsigned long *[4]")
    test_so.obj.ulpp[1] = trick.TMM_declare_var_1d( "unsigned long", 5)
    trick.rti_add("test_so.obj.ulpp[1][1]", 55)

    #trick.rti_list()
    trick.rti_fire()
    trick_inject.rtis.GetRtiExecutor(0).Exec()

    TRICK_EXPECT_EQ( test_so.obj.ul , 50 , test_suite , "unsigned long" )
    TRICK_EXPECT_EQ( test_so.obj.ula[1] , 51 , test_suite , "unsigned long array" )
    TRICK_EXPECT_EQ( test_so.obj.ulp[1] , 52 , test_suite , "unsigned long pointer" )
    TRICK_EXPECT_EQ( test_so.obj.ulaa[1][1] , 53 , test_suite , "2D unsigned long array" )
    TRICK_EXPECT_EQ( test_so.obj.ulap[1][1] , 54 , test_suite , "2D unsigned long array of ptr" )
    TRICK_EXPECT_EQ( test_so.obj.ulpp[1][1] , 55 , test_suite , "2D unsigned long ptr of ptr" )

######################################################################################################################

    test_so.obj.ll = 0
    trick.rti_add("test_so.obj.ll", 50)
    test_so.obj.lla[1] = 0
    trick.rti_add("test_so.obj.lla[1]", 51)
    test_so.obj.llp = trick.TMM_declare_var_s("long long[6]")
    trick.rti_add("test_so.obj.llp[1]", 52)
    test_so.obj.llaa[1][1] = 0
    trick.rti_add("test_so.obj.llaa[1][1]", 53)
    test_so.obj.llap[1] = trick.TMM_declare_var_1d( "long long", 4)
    trick.rti_add("test_so.obj.llap[1][1]", 54)
    test_so.obj.llpp = trick.TMM_declare_var_s("long long *[4]")
    test_so.obj.llpp[1] = trick.TMM_declare_var_1d( "long long", 5)
    trick.rti_add("test_so.obj.llpp[1][1]", 55)

    #trick.rti_list()
    trick.rti_fire()
    trick_inject.rtis.GetRtiExecutor(0).Exec()

    TRICK_EXPECT_EQ( test_so.obj.ll , 50 , test_suite , "long long" )
    TRICK_EXPECT_EQ( test_so.obj.lla[1] , 51 , test_suite , "long long array" )
    TRICK_EXPECT_EQ( test_so.obj.llp[1] , 52 , test_suite , "long long pointer" )
    TRICK_EXPECT_EQ( test_so.obj.llaa[1][1] , 53 , test_suite , "2D long long array" )
    TRICK_EXPECT_EQ( test_so.obj.llap[1][1] , 54 , test_suite , "2D long long array of ptr" )
    TRICK_EXPECT_EQ( test_so.obj.llpp[1][1] , 55 , test_suite , "2D long long ptr of ptr" )

######################################################################################################################

    test_so.obj.ull = 0
    trick.rti_add("test_so.obj.ull", 50)
    test_so.obj.ulla[1] = 0
    trick.rti_add("test_so.obj.ulla[1]", 51)
    test_so.obj.ullp = trick.TMM_declare_var_s("unsigned long long[6]")
    trick.rti_add("test_so.obj.ullp[1]", 52)
    test_so.obj.ullaa[1][1] = 0
    trick.rti_add("test_so.obj.ullaa[1][1]", 53)
    test_so.obj.ullap[1] = trick.TMM_declare_var_1d( "unsigned long long", 4)
    trick.rti_add("test_so.obj.ullap[1][1]", 54)
    test_so.obj.ullpp = trick.TMM_declare_var_s("unsigned long long *[4]")
    test_so.obj.ullpp[1] = trick.TMM_declare_var_1d( "unsigned long long", 5)
    trick.rti_add("test_so.obj.ullpp[1][1]", 55)

    #trick.rti_list()
    trick.rti_fire()
    trick_inject.rtis.GetRtiExecutor(0).Exec()

    TRICK_EXPECT_EQ( test_so.obj.ull , 50 , test_suite , "unsigned long long" )
    TRICK_EXPECT_EQ( test_so.obj.ulla[1] , 51 , test_suite , "unsigned long long array" )
    TRICK_EXPECT_EQ( test_so.obj.ullp[1] , 52 , test_suite , "unsigned long long pointer" )
    TRICK_EXPECT_EQ( test_so.obj.ullaa[1][1] , 53 , test_suite , "2D unsigned long long array" )
    TRICK_EXPECT_EQ( test_so.obj.ullap[1][1] , 54 , test_suite , "2D unsigned long long array of ptr" )
    TRICK_EXPECT_EQ( test_so.obj.ullpp[1][1] , 55 , test_suite , "2D unsigned long long ptr of ptr" )

######################################################################################################################

    test_so.obj.e = 0
    trick.rti_add("test_so.obj.e", 50)
    test_so.obj.ea[1] = 0
    trick.rti_add("test_so.obj.ea[1]", 51)
    test_so.obj.ep = trick.TMM_declare_var_s("MY_ENUM[6]")
    trick.rti_add("test_so.obj.ep[1]", 52)
    test_so.obj.eaa[1][1] = 0
    trick.rti_add("test_so.obj.eaa[1][1]", 53)
    test_so.obj.eap[1] = trick.TMM_declare_var_1d( "MY_ENUM", 4)
    trick.rti_add("test_so.obj.eap[1][1]", 54)
    test_so.obj.epp = trick.TMM_declare_var_s("MY_ENUM *[4]")
    test_so.obj.epp[1] = trick.TMM_declare_var_1d( "MY_ENUM", 5)
    trick.rti_add("test_so.obj.epp[1][1]", 55)

    #trick.rti_list()
    trick.rti_fire()
    trick_inject.rtis.GetRtiExecutor(0).Exec()

    TRICK_EXPECT_EQ( test_so.obj.e , 50 , test_suite , "enum" )
    TRICK_EXPECT_EQ( test_so.obj.ea[1] , 51 , test_suite , "enum array" )
    TRICK_EXPECT_EQ( test_so.obj.ep[1] , 52 , test_suite , "enum pointer" )
    TRICK_EXPECT_EQ( test_so.obj.eaa[1][1] , 53 , test_suite , "2D enum array" )
    TRICK_EXPECT_EQ( test_so.obj.eap[1][1] , 54 , test_suite , "2D enum array of ptr" )
    TRICK_EXPECT_EQ( test_so.obj.epp[1][1] , 55 , test_suite , "2D enum ptr of ptr" )

######################################################################################################################

    test_so.obj.b = 0
    trick.rti_add("test_so.obj.b", 50)
    test_so.obj.ba[1] = 0
    trick.rti_add("test_so.obj.ba[1]", 51)
    test_so.obj.bp = trick.TMM_declare_var_s("bool[6]")
    trick.rti_add("test_so.obj.bp[1]", 52)
    test_so.obj.baa[1][1] = 0
    trick.rti_add("test_so.obj.baa[1][1]", 53)
    test_so.obj.bap[1] = trick.TMM_declare_var_1d( "bool", 4)
    trick.rti_add("test_so.obj.bap[1][1]", 54)
    test_so.obj.bpp = trick.TMM_declare_var_s("bool *[4]")
    test_so.obj.bpp[1] = trick.TMM_declare_var_1d( "bool", 5)
    trick.rti_add("test_so.obj.bpp[1][1]", 55)

    #trick.rti_list()
    trick.rti_fire()
    trick_inject.rtis.GetRtiExecutor(0).Exec()

    TRICK_EXPECT_EQ( test_so.obj.b , 1 , test_suite , "bool" )
    TRICK_EXPECT_EQ( test_so.obj.ba[1] , 1 , test_suite , "bool array" )
    TRICK_EXPECT_EQ( test_so.obj.bp[1] , 1 , test_suite , "bool pointer" )
    TRICK_EXPECT_EQ( test_so.obj.baa[1][1] , 1 , test_suite , "2D bool array" )
    TRICK_EXPECT_EQ( test_so.obj.bap[1][1] , 1 , test_suite , "2D bool array of ptr" )
    TRICK_EXPECT_EQ( test_so.obj.bpp[1][1] , 1 , test_suite , "2D bool ptr of ptr" )

######################################################################################################################

    test_so.obj.cbit_0 = 0
    trick.rti_add("test_so.obj.cbit_0", -1)
    test_so.obj.cbit_1 = 0
    trick.rti_add("test_so.obj.cbit_1", 31)
    test_so.obj.cbit_2 = 0
    trick.rti_add("test_so.obj.cbit_2", 2)

    test_so.obj.ucbit_0 = 0
    trick.rti_add("test_so.obj.ucbit_0", 2)
    test_so.obj.ucbit_1 = 0
    trick.rti_add("test_so.obj.ucbit_1", 31)
    test_so.obj.ucbit_2 = 0
    trick.rti_add("test_so.obj.ucbit_2", -1)

    trick.rti_fire()
    trick_inject.rtis.GetRtiExecutor(0).Exec()

    print os.uname()[4]
    # The raspberry pi has a bug and returns bitfields in unsigned format.
    if os.uname()[4] == 'armv6l':
        TRICK_EXPECT_EQ( test_so.obj.cbit_0 , 3 , test_suite , "char bitfield" )
        TRICK_EXPECT_EQ( test_so.obj.cbit_1 , 7 , test_suite , "char bitfield" )
    else :
        TRICK_EXPECT_EQ( test_so.obj.cbit_0 , -1 , test_suite , "char bitfield" )
        TRICK_EXPECT_EQ( test_so.obj.cbit_1 , -1 , test_suite , "char bitfield" )
    TRICK_EXPECT_EQ( test_so.obj.cbit_2 , 2 , test_suite , "char bitfield" )

    TRICK_EXPECT_EQ( test_so.obj.ucbit_0 , 2 , test_suite , "unsigned char bitfield" )
    TRICK_EXPECT_EQ( test_so.obj.ucbit_1 , 7 , test_suite , "unsigned char bitfield" )
    TRICK_EXPECT_EQ( test_so.obj.ucbit_2 , 7 , test_suite , "unsigned char bitfield" )

######################################################################################################################

    test_so.obj.sbit_0 = 0
    trick.rti_add("test_so.obj.sbit_0", -1)
    test_so.obj.sbit_1 = 0
    trick.rti_add("test_so.obj.sbit_1", 255)
    test_so.obj.sbit_2 = 0
    trick.rti_add("test_so.obj.sbit_2", 2)

    test_so.obj.usbit_0 = 0
    trick.rti_add("test_so.obj.usbit_0", 2)
    test_so.obj.usbit_1 = 0
    trick.rti_add("test_so.obj.usbit_1", 255)
    test_so.obj.usbit_2 = 0
    trick.rti_add("test_so.obj.usbit_2", -1)

    trick.rti_fire()
    trick_inject.rtis.GetRtiExecutor(0).Exec()

    TRICK_EXPECT_EQ( test_so.obj.sbit_0 , -1 , test_suite , "short bitfield" )
    TRICK_EXPECT_EQ( test_so.obj.sbit_1 , -1 , test_suite , "short bitfield" )
    TRICK_EXPECT_EQ( test_so.obj.sbit_2 , 2 , test_suite , "short bitfield" )

    TRICK_EXPECT_EQ( test_so.obj.usbit_0 , 2 , test_suite , "unsigned short bitfield" )
    TRICK_EXPECT_EQ( test_so.obj.usbit_1 , 31 , test_suite , "unsigned short bitfield" )
    TRICK_EXPECT_EQ( test_so.obj.usbit_2 , 127 , test_suite , "unsigned short bitfield" )

######################################################################################################################

    test_so.obj.bit_0 = 0
    trick.rti_add("test_so.obj.bit_0", -1)
    test_so.obj.bit_1 = 0
    trick.rti_add("test_so.obj.bit_1", 255)
    test_so.obj.bit_2 = 0
    trick.rti_add("test_so.obj.bit_2", 2)

    test_so.obj.ubit_0 = 0
    trick.rti_add("test_so.obj.ubit_0", 2)
    test_so.obj.ubit_1 = 0
    trick.rti_add("test_so.obj.ubit_1", 255)
    test_so.obj.ubit_2 = 0
    trick.rti_add("test_so.obj.ubit_2", -1)

    trick.rti_fire()
    trick_inject.rtis.GetRtiExecutor(0).Exec()

    TRICK_EXPECT_EQ( test_so.obj.bit_0 , -1 , test_suite , "int bitfield" )
    TRICK_EXPECT_EQ( test_so.obj.bit_1 , -1 , test_suite , "int bitfield" )
    TRICK_EXPECT_EQ( test_so.obj.bit_2 , 2 , test_suite , "int bitfield" )

    TRICK_EXPECT_EQ( test_so.obj.ubit_0 , 2 , test_suite , "unsigned int bitfield" )
    TRICK_EXPECT_EQ( test_so.obj.ubit_1 , 31 , test_suite , "unsigned int bitfield" )
    TRICK_EXPECT_EQ( test_so.obj.ubit_2 , 63 , test_suite , "unsigned int bitfield" )

######################################################################################################################

    # long and long long bitfields are not currently supported by
    # sim_services/MemoryManager/src/insert_bitfield_any.c

    #test_so.obj.lbit_0 = 0
    #trick.rti_add("test_so.obj.lbit_0", -1)
    #test_so.obj.lbit_1 = 0
    #trick.rti_add("test_so.obj.lbit_1", 255)
    #test_so.obj.lbit_2 = 0
    #trick.rti_add("test_so.obj.lbit_2", 2)

    #test_so.obj.ulbit_0 = 0
    #trick.rti_add("test_so.obj.ulbit_0", 2)
    #test_so.obj.ulbit_1 = 0
    #trick.rti_add("test_so.obj.ulbit_1", 255)
    #test_so.obj.ulbit_2 = 0
    #trick.rti_add("test_so.obj.ulbit_2", -1)

    #trick.rti_fire()
    #trick_inject.rtis.GetRtiExecutor(0).Exec()

    #TRICK_EXPECT_EQ( test_so.obj.lbit_0 , -1 , test_suite , "long bitfield" )
    #TRICK_EXPECT_EQ( test_so.obj.lbit_1 , -1 , test_suite , "long bitfield" )
    #TRICK_EXPECT_EQ( test_so.obj.lbit_2 , 2 , test_suite , "long bitfield" )

    #TRICK_EXPECT_EQ( test_so.obj.ulbit_0 , 2 , test_suite , "unsigned long bitfield" )
    #TRICK_EXPECT_EQ( test_so.obj.ulbit_1 , 31 , test_suite , "unsigned long bitfield" )
    #TRICK_EXPECT_EQ( test_so.obj.ulbit_2 , 63 , test_suite , "unsigned long bitfield" )

######################################################################################################################

    test_so.obj.boolbit_0 = 0
    trick.rti_add("test_so.obj.boolbit_0", 2)
    test_so.obj.boolbit_1 = 0
    trick.rti_add("test_so.obj.boolbit_1", 1)
    test_so.obj.boolbit_2 = 0
    trick.rti_add("test_so.obj.boolbit_2", 255)

    trick.rti_fire()
    trick_inject.rtis.GetRtiExecutor(0).Exec()

    TRICK_EXPECT_EQ( test_so.obj.boolbit_0 , 0 , test_suite , "bool bitfield" )
    TRICK_EXPECT_EQ( test_so.obj.boolbit_1 , 1 , test_suite , "bool bitfield" )
    TRICK_EXPECT_EQ( test_so.obj.boolbit_2 , 1 , test_suite , "bool bitfield" )

######################################################################################################################
######################################################################################################################

    test_suite = "rti_add_float"

    test_so.obj.d = 0.0
    trick.rti_add("test_so.obj.d", 50.1)
    test_so.obj.da[1] = 0.0
    trick.rti_add("test_so.obj.da[1]", 51.1)
    test_so.obj.dp = trick.TMM_declare_var_s("double[6]")
    trick.rti_add("test_so.obj.dp[1]", 52.1)
    test_so.obj.daa[1][1] = 0.0
    trick.rti_add("test_so.obj.daa[1][1]", 53.1)
    test_so.obj.dap[1] = trick.TMM_declare_var_1d( "double", 4)
    trick.rti_add("test_so.obj.dap[1][1]", 54.1)
    test_so.obj.dpp = trick.TMM_declare_var_s("double *[4]")
    test_so.obj.dpp[1] = trick.TMM_declare_var_1d( "double", 5)
    trick.rti_add("test_so.obj.dpp[1][1]", 55.1)

    #trick.rti_list()
    trick.rti_fire()
    trick_inject.rtis.GetRtiExecutor(0).Exec()

    TRICK_EXPECT_NEAR( test_so.obj.d , 50.1 , 0.000001 , test_suite , "double" )
    TRICK_EXPECT_NEAR( test_so.obj.da[1] , 51.1 , 0.000001 , test_suite , "double array" )
    TRICK_EXPECT_NEAR( test_so.obj.dp[1] , 52.1 , 0.000001 , test_suite , "double pointer" )
    TRICK_EXPECT_NEAR( test_so.obj.daa[1][1] , 53.1 , 0.000001 , test_suite , "2D double array" )
    TRICK_EXPECT_NEAR( test_so.obj.dap[1][1] , 54.1 , 0.000001 , test_suite , "2D double array of ptr" )
    TRICK_EXPECT_NEAR( test_so.obj.dpp[1][1] , 55.1 , 0.000001 , test_suite , "2D double ptr of ptr" )

######################################################################################################################

    test_so.obj.f = 0.0
    trick.rti_add("test_so.obj.f", 50.1)
    test_so.obj.fa[1] = 0.0
    trick.rti_add("test_so.obj.fa[1]", 51.1)
    test_so.obj.fp = trick.TMM_declare_var_s("float[6]")
    trick.rti_add("test_so.obj.fp[1]", 52.1)
    test_so.obj.faa[1][1] = 0.0
    trick.rti_add("test_so.obj.faa[1][1]", 53.1)
    test_so.obj.fap[1] = trick.TMM_declare_var_1d( "float", 4)
    trick.rti_add("test_so.obj.fap[1][1]", 54.1)
    test_so.obj.fpp = trick.TMM_declare_var_s("float *[4]")
    test_so.obj.fpp[1] = trick.TMM_declare_var_1d( "float", 5)
    trick.rti_add("test_so.obj.fpp[1][1]", 55.1)

    #trick.rti_list()
    trick.rti_fire()
    trick_inject.rtis.GetRtiExecutor(0).Exec()

    TRICK_EXPECT_NEAR( test_so.obj.f , 50.1 , 0.00001 , test_suite , "float" )
    TRICK_EXPECT_NEAR( test_so.obj.fa[1] , 51.1 , 0.00001 , test_suite , "float array" )
    TRICK_EXPECT_NEAR( test_so.obj.fp[1] , 52.1 , 0.00001 , test_suite , "float pointer" )
    TRICK_EXPECT_NEAR( test_so.obj.faa[1][1] , 53.1 , 0.00001 , test_suite , "2D float array" )
    TRICK_EXPECT_NEAR( test_so.obj.fap[1][1] , 54.1 , 0.00001 , test_suite , "2D float array of ptr" )
    TRICK_EXPECT_NEAR( test_so.obj.fpp[1][1] , 55.1 , 0.00001 , test_suite , "2D float ptr of ptr" )

######################################################################################################################

    test_so.obj.c = 0
    trick.rti_add("test_so.obj.c", 40.1)
    test_so.obj.ca[1] = 0
    trick.rti_add("test_so.obj.ca[1]", 41.1)
    test_so.obj.cp = trick.TMM_declare_var_s("char[6]")
    trick.rti_add("test_so.obj.cp[1]", 42.1)
    test_so.obj.caa[1][1] = 0
    trick.rti_add("test_so.obj.caa[1][1]", 43.1)
    test_so.obj.cap[1] = trick.TMM_declare_var_1d( "char", 4)
    trick.rti_add("test_so.obj.cap[1][1]", 44.1)
    test_so.obj.cpp = trick.TMM_declare_var_s("char *[4]")
    test_so.obj.cpp[1] = trick.TMM_declare_var_1d( "char", 5)
    trick.rti_add("test_so.obj.cpp[1][1]", 45.1)

    #trick.rti_list()
    trick.rti_fire()
    trick_inject.rtis.GetRtiExecutor(0).Exec()

    TRICK_EXPECT_EQ( test_so.obj.c , 40 , test_suite , "char" )
    TRICK_EXPECT_EQ( test_so.obj.ca[1] , 41 , test_suite , "char array" )
    TRICK_EXPECT_EQ( test_so.obj.cp[1] , 42 , test_suite , "char pointer" )
    TRICK_EXPECT_EQ( test_so.obj.caa[1][1] , 43 , test_suite , "2D char array" )
    TRICK_EXPECT_EQ( test_so.obj.cap[1][1] , 44 , test_suite , "2D char array of ptr" )
    TRICK_EXPECT_EQ( test_so.obj.cpp[1][1] , 45 , test_suite , "2D char ptr of ptr" )

######################################################################################################################

    test_so.obj.uc = 0
    trick.rti_add("test_so.obj.uc", 40.1)
    test_so.obj.uca[1] = 0
    trick.rti_add("test_so.obj.uca[1]", 41.1)
    test_so.obj.ucp = trick.TMM_declare_var_s("unsigned char[6]")
    trick.rti_add("test_so.obj.ucp[1]", 42.1)
    test_so.obj.ucaa[1][1] = 0
    trick.rti_add("test_so.obj.ucaa[1][1]", 43.1)
    test_so.obj.ucap[1] = trick.TMM_declare_var_1d( "unsigned char", 4)
    trick.rti_add("test_so.obj.ucap[1][1]", 44.1)
    test_so.obj.ucpp = trick.TMM_declare_var_s("unsigned char *[4]")
    test_so.obj.ucpp[1] = trick.TMM_declare_var_1d( "unsigned char", 5)
    trick.rti_add("test_so.obj.ucpp[1][1]", 45.1)

    #trick.rti_list()
    trick.rti_fire()
    trick_inject.rtis.GetRtiExecutor(0).Exec()

    TRICK_EXPECT_EQ( test_so.obj.uc , 40 , test_suite , "unsigned char" )
    TRICK_EXPECT_EQ( test_so.obj.uca[1] , 41 , test_suite , "unsigned char array" )
    TRICK_EXPECT_EQ( test_so.obj.ucp[1] , 42 , test_suite , "unsigned char pointer" )
    TRICK_EXPECT_EQ( test_so.obj.ucaa[1][1] , 43 , test_suite , "2D unsigned char array" )
    TRICK_EXPECT_EQ( test_so.obj.ucap[1][1] , 44 , test_suite , "2D unsigned char array of ptr" )
    TRICK_EXPECT_EQ( test_so.obj.ucpp[1][1] , 45 , test_suite , "2D unsigned char ptr of ptr" )

######################################################################################################################

    test_so.obj.s = 0
    trick.rti_add("test_so.obj.s", 40.1)
    test_so.obj.sa[1] = 0
    trick.rti_add("test_so.obj.sa[1]", 41.1)
    test_so.obj.sp = trick.TMM_declare_var_s("short[6]")
    trick.rti_add("test_so.obj.sp[1]", 42.1)
    test_so.obj.saa[1][1] = 0
    trick.rti_add("test_so.obj.saa[1][1]", 43.1)
    test_so.obj.sap[1] = trick.TMM_declare_var_1d( "short", 4)
    trick.rti_add("test_so.obj.sap[1][1]", 44.1)
    test_so.obj.spp = trick.TMM_declare_var_s("short *[4]")
    test_so.obj.spp[1] = trick.TMM_declare_var_1d( "short", 5)
    trick.rti_add("test_so.obj.spp[1][1]", 45.1)

    #trick.rti_list()
    trick.rti_fire()
    trick_inject.rtis.GetRtiExecutor(0).Exec()

    TRICK_EXPECT_EQ( test_so.obj.s , 40 , test_suite , "short" )
    TRICK_EXPECT_EQ( test_so.obj.sa[1] , 41 , test_suite , "short array" )
    TRICK_EXPECT_EQ( test_so.obj.sp[1] , 42 , test_suite , "short pointer" )
    TRICK_EXPECT_EQ( test_so.obj.saa[1][1] , 43 , test_suite , "2D short array" )
    TRICK_EXPECT_EQ( test_so.obj.sap[1][1] , 44 , test_suite , "2D short array of ptr" )
    TRICK_EXPECT_EQ( test_so.obj.spp[1][1] , 45 , test_suite , "2D short ptr of ptr" )

######################################################################################################################

    test_so.obj.us = 0
    trick.rti_add("test_so.obj.us", 40.1)
    test_so.obj.usa[1] = 0
    trick.rti_add("test_so.obj.usa[1]", 41.1)
    test_so.obj.usp = trick.TMM_declare_var_s("unsigned short[6]")
    trick.rti_add("test_so.obj.usp[1]", 42.1)
    test_so.obj.usaa[1][1] = 0
    trick.rti_add("test_so.obj.usaa[1][1]", 43.1)
    test_so.obj.usap[1] = trick.TMM_declare_var_1d( "unsigned short", 4)
    trick.rti_add("test_so.obj.usap[1][1]", 44.1)
    test_so.obj.uspp = trick.TMM_declare_var_s("unsigned short *[4]")
    test_so.obj.uspp[1] = trick.TMM_declare_var_1d( "unsigned short", 5)
    trick.rti_add("test_so.obj.uspp[1][1]", 45.1)

    #trick.rti_list()
    trick.rti_fire()
    trick_inject.rtis.GetRtiExecutor(0).Exec()

    TRICK_EXPECT_EQ( test_so.obj.us , 40 , test_suite , "unsigned short" )
    TRICK_EXPECT_EQ( test_so.obj.usa[1] , 41 , test_suite , "unsigned short array" )
    TRICK_EXPECT_EQ( test_so.obj.usp[1] , 42 , test_suite , "unsigned short pointer" )
    TRICK_EXPECT_EQ( test_so.obj.usaa[1][1] , 43 , test_suite , "2D unsigned short array" )
    TRICK_EXPECT_EQ( test_so.obj.usap[1][1] , 44 , test_suite , "2D unsigned short array of ptr" )
    TRICK_EXPECT_EQ( test_so.obj.uspp[1][1] , 45 , test_suite , "2D unsigned short ptr of ptr" )

######################################################################################################################

    test_so.obj.i = 0
    trick.rti_add("test_so.obj.i", 40.1)
    test_so.obj.ia[1] = 0
    trick.rti_add("test_so.obj.ia[1]", 41.1)
    test_so.obj.ip = trick.TMM_declare_var_s("int[6]")
    trick.rti_add("test_so.obj.ip[1]", 42.1)
    test_so.obj.iaa[1][1] = 0
    trick.rti_add("test_so.obj.iaa[1][1]", 43.1)
    test_so.obj.iap[1] = trick.TMM_declare_var_1d( "int", 4)
    trick.rti_add("test_so.obj.iap[1][1]", 44.1)
    test_so.obj.ipp = trick.TMM_declare_var_s("int *[4]")
    test_so.obj.ipp[1] = trick.TMM_declare_var_1d( "int", 5)
    trick.rti_add("test_so.obj.ipp[1][1]", 45.1)

    #trick.rti_list()
    trick.rti_fire()
    trick_inject.rtis.GetRtiExecutor(0).Exec()

    TRICK_EXPECT_EQ( test_so.obj.i , 40 , test_suite , "int" )
    TRICK_EXPECT_EQ( test_so.obj.ia[1] , 41 , test_suite , "int array" )
    TRICK_EXPECT_EQ( test_so.obj.ip[1] , 42 , test_suite , "int pointer" )
    TRICK_EXPECT_EQ( test_so.obj.iaa[1][1] , 43 , test_suite , "2D int array" )
    TRICK_EXPECT_EQ( test_so.obj.iap[1][1] , 44 , test_suite , "2D int array of ptr" )
    TRICK_EXPECT_EQ( test_so.obj.ipp[1][1] , 45 , test_suite , "2D int ptr of ptr" )

######################################################################################################################

    test_so.obj.ui = 0
    trick.rti_add("test_so.obj.ui", 40.1)
    test_so.obj.uia[1] = 0
    trick.rti_add("test_so.obj.uia[1]", 41.1)
    test_so.obj.uip = trick.TMM_declare_var_s("unsigned int[6]")
    trick.rti_add("test_so.obj.uip[1]", 42.1)
    test_so.obj.uiaa[1][1] = 0
    trick.rti_add("test_so.obj.uiaa[1][1]", 43.1)
    test_so.obj.uiap[1] = trick.TMM_declare_var_1d( "unsigned int", 4)
    trick.rti_add("test_so.obj.uiap[1][1]", 44.1)
    test_so.obj.uipp = trick.TMM_declare_var_s("unsigned int *[4]")
    test_so.obj.uipp[1] = trick.TMM_declare_var_1d( "unsigned int", 5)
    trick.rti_add("test_so.obj.uipp[1][1]", 45.1)

    #trick.rti_list()
    trick.rti_fire()
    trick_inject.rtis.GetRtiExecutor(0).Exec()

    TRICK_EXPECT_EQ( test_so.obj.ui , 40 , test_suite , "unsigned int" )
    TRICK_EXPECT_EQ( test_so.obj.uia[1] , 41 , test_suite , "unsigned int array" )
    TRICK_EXPECT_EQ( test_so.obj.uip[1] , 42 , test_suite , "unsigned int pointer" )
    TRICK_EXPECT_EQ( test_so.obj.uiaa[1][1] , 43 , test_suite , "2D unsigned int array" )
    TRICK_EXPECT_EQ( test_so.obj.uiap[1][1] , 44 , test_suite , "2D unsigned int array of ptr" )
    TRICK_EXPECT_EQ( test_so.obj.uipp[1][1] , 45 , test_suite , "2D unsigned int ptr of ptr" )

######################################################################################################################

    test_so.obj.l = 0
    trick.rti_add("test_so.obj.l", 40.1)
    test_so.obj.la[1] = 0
    trick.rti_add("test_so.obj.la[1]", 41.1)
    test_so.obj.lp = trick.TMM_declare_var_s("long[6]")
    trick.rti_add("test_so.obj.lp[1]", 42.1)
    test_so.obj.laa[1][1] = 0
    trick.rti_add("test_so.obj.laa[1][1]", 43.1)
    test_so.obj.lap[1] = trick.TMM_declare_var_1d( "long", 4)
    trick.rti_add("test_so.obj.lap[1][1]", 44.1)
    test_so.obj.lpp = trick.TMM_declare_var_s("long *[4]")
    test_so.obj.lpp[1] = trick.TMM_declare_var_1d( "long", 5)
    trick.rti_add("test_so.obj.lpp[1][1]", 45.1)

    #trick.rti_list()
    trick.rti_fire()
    trick_inject.rtis.GetRtiExecutor(0).Exec()

    TRICK_EXPECT_EQ( test_so.obj.l , 40 , test_suite , "long" )
    TRICK_EXPECT_EQ( test_so.obj.la[1] , 41 , test_suite , "long array" )
    TRICK_EXPECT_EQ( test_so.obj.lp[1] , 42 , test_suite , "long polonger" )
    TRICK_EXPECT_EQ( test_so.obj.laa[1][1] , 43 , test_suite , "2D long array" )
    TRICK_EXPECT_EQ( test_so.obj.lap[1][1] , 44 , test_suite , "2D long array of ptr" )
    TRICK_EXPECT_EQ( test_so.obj.lpp[1][1] , 45 , test_suite , "2D long ptr of ptr" )

######################################################################################################################

    test_so.obj.ul = 0
    trick.rti_add("test_so.obj.ul", 40.1)
    test_so.obj.ula[1] = 0
    trick.rti_add("test_so.obj.ula[1]", 41.1)
    test_so.obj.ulp = trick.TMM_declare_var_s("unsigned long[6]")
    trick.rti_add("test_so.obj.ulp[1]", 42.1)
    test_so.obj.ulaa[1][1] = 0
    trick.rti_add("test_so.obj.ulaa[1][1]", 43.1)
    test_so.obj.ulap[1] = trick.TMM_declare_var_1d( "unsigned long", 4)
    trick.rti_add("test_so.obj.ulap[1][1]", 44.1)
    test_so.obj.ulpp = trick.TMM_declare_var_s("unsigned long *[4]")
    test_so.obj.ulpp[1] = trick.TMM_declare_var_1d( "unsigned long", 5)
    trick.rti_add("test_so.obj.ulpp[1][1]", 45.1)

    #trick.rti_list()
    trick.rti_fire()
    trick_inject.rtis.GetRtiExecutor(0).Exec()

    TRICK_EXPECT_EQ( test_so.obj.ul , 40 , test_suite , "unsigned long" )
    TRICK_EXPECT_EQ( test_so.obj.ula[1] , 41 , test_suite , "unsigned long array" )
    TRICK_EXPECT_EQ( test_so.obj.ulp[1] , 42 , test_suite , "unsigned long pointer" )
    TRICK_EXPECT_EQ( test_so.obj.ulaa[1][1] , 43 , test_suite , "2D unsigned long array" )
    TRICK_EXPECT_EQ( test_so.obj.ulap[1][1] , 44 , test_suite , "2D unsigned long array of ptr" )
    TRICK_EXPECT_EQ( test_so.obj.ulpp[1][1] , 45 , test_suite , "2D unsigned long ptr of ptr" )

######################################################################################################################

    test_so.obj.ll = 0
    trick.rti_add("test_so.obj.ll", 40.1)
    test_so.obj.lla[1] = 0
    trick.rti_add("test_so.obj.lla[1]", 41.1)
    test_so.obj.llp = trick.TMM_declare_var_s("long long[6]")
    trick.rti_add("test_so.obj.llp[1]", 42.1)
    test_so.obj.llaa[1][1] = 0
    trick.rti_add("test_so.obj.llaa[1][1]", 43.1)
    test_so.obj.llap[1] = trick.TMM_declare_var_1d( "long long", 4)
    trick.rti_add("test_so.obj.llap[1][1]", 44.1)
    test_so.obj.llpp = trick.TMM_declare_var_s("long long *[4]")
    test_so.obj.llpp[1] = trick.TMM_declare_var_1d( "long long", 5)
    trick.rti_add("test_so.obj.llpp[1][1]", 45.1)

    #trick.rti_list()
    trick.rti_fire()
    trick_inject.rtis.GetRtiExecutor(0).Exec()

    TRICK_EXPECT_EQ( test_so.obj.ll , 40 , test_suite , "long long" )
    TRICK_EXPECT_EQ( test_so.obj.lla[1] , 41 , test_suite , "long long array" )
    TRICK_EXPECT_EQ( test_so.obj.llp[1] , 42 , test_suite , "long long pointer" )
    TRICK_EXPECT_EQ( test_so.obj.llaa[1][1] , 43 , test_suite , "2D long long array" )
    TRICK_EXPECT_EQ( test_so.obj.llap[1][1] , 44 , test_suite , "2D long long array of ptr" )
    TRICK_EXPECT_EQ( test_so.obj.llpp[1][1] , 45 , test_suite , "2D long long ptr of ptr" )

######################################################################################################################

    test_so.obj.ull = 0
    trick.rti_add("test_so.obj.ull", 40.1)
    test_so.obj.ulla[1] = 0
    trick.rti_add("test_so.obj.ulla[1]", 41.1)
    test_so.obj.ullp = trick.TMM_declare_var_s("unsigned long long[6]")
    trick.rti_add("test_so.obj.ullp[1]", 42.1)
    test_so.obj.ullaa[1][1] = 0
    trick.rti_add("test_so.obj.ullaa[1][1]", 43.1)
    test_so.obj.ullap[1] = trick.TMM_declare_var_1d( "unsigned long long", 4)
    trick.rti_add("test_so.obj.ullap[1][1]", 44.1)
    test_so.obj.ullpp = trick.TMM_declare_var_s("unsigned long long *[4]")
    test_so.obj.ullpp[1] = trick.TMM_declare_var_1d( "unsigned long long", 5)
    trick.rti_add("test_so.obj.ullpp[1][1]", 45.1)

    #trick.rti_list()
    trick.rti_fire()
    trick_inject.rtis.GetRtiExecutor(0).Exec()

    TRICK_EXPECT_EQ( test_so.obj.ull , 40 , test_suite , "unsigned long long" )
    TRICK_EXPECT_EQ( test_so.obj.ulla[1] , 41 , test_suite , "unsigned long long array" )
    TRICK_EXPECT_EQ( test_so.obj.ullp[1] , 42 , test_suite , "unsigned long long pointer" )
    TRICK_EXPECT_EQ( test_so.obj.ullaa[1][1] , 43 , test_suite , "2D unsigned long long array" )
    TRICK_EXPECT_EQ( test_so.obj.ullap[1][1] , 44 , test_suite , "2D unsigned long long array of ptr" )
    TRICK_EXPECT_EQ( test_so.obj.ullpp[1][1] , 45 , test_suite , "2D unsigned long long ptr of ptr" )

######################################################################################################################

    test_so.obj.e = 0
    trick.rti_add("test_so.obj.e", 40.1)
    test_so.obj.ea[1] = 0
    trick.rti_add("test_so.obj.ea[1]", 41.1)
    test_so.obj.ep = trick.TMM_declare_var_s("MY_ENUM[6]")
    trick.rti_add("test_so.obj.ep[1]", 42.1)
    test_so.obj.eaa[1][1] = 0
    trick.rti_add("test_so.obj.eaa[1][1]", 43.1)
    test_so.obj.eap[1] = trick.TMM_declare_var_1d( "MY_ENUM", 4)
    trick.rti_add("test_so.obj.eap[1][1]", 44.1)
    test_so.obj.epp = trick.TMM_declare_var_s("MY_ENUM *[4]")
    test_so.obj.epp[1] = trick.TMM_declare_var_1d( "MY_ENUM", 5)
    trick.rti_add("test_so.obj.epp[1][1]", 45.1)

    #trick.rti_list()
    trick.rti_fire()
    trick_inject.rtis.GetRtiExecutor(0).Exec()

    TRICK_EXPECT_EQ( test_so.obj.e , 40 , test_suite , "enum" )
    TRICK_EXPECT_EQ( test_so.obj.ea[1] , 41 , test_suite , "enum array" )
    TRICK_EXPECT_EQ( test_so.obj.ep[1] , 42 , test_suite , "enum pointer" )
    TRICK_EXPECT_EQ( test_so.obj.eaa[1][1] , 43 , test_suite , "2D enum array" )
    TRICK_EXPECT_EQ( test_so.obj.eap[1][1] , 44 , test_suite , "2D enum array of ptr" )
    TRICK_EXPECT_EQ( test_so.obj.epp[1][1] , 45 , test_suite , "2D enum ptr of ptr" )

######################################################################################################################

    test_so.obj.b = 0
    trick.rti_add("test_so.obj.b", 40)
    test_so.obj.ba[1] = 0
    trick.rti_add("test_so.obj.ba[1]", 41)
    test_so.obj.bp = trick.TMM_declare_var_s("bool[6]")
    trick.rti_add("test_so.obj.bp[1]", 42)
    test_so.obj.baa[1][1] = 0
    trick.rti_add("test_so.obj.baa[1][1]", 43)
    test_so.obj.bap[1] = trick.TMM_declare_var_1d( "bool", 4)
    trick.rti_add("test_so.obj.bap[1][1]", 44)
    test_so.obj.bpp = trick.TMM_declare_var_s("bool *[4]")
    test_so.obj.bpp[1] = trick.TMM_declare_var_1d( "bool", 5)
    trick.rti_add("test_so.obj.bpp[1][1]", 45)

    #trick.rti_list()
    trick.rti_fire()
    trick_inject.rtis.GetRtiExecutor(0).Exec()

    TRICK_EXPECT_EQ( test_so.obj.b , 1 , test_suite , "bool" )
    TRICK_EXPECT_EQ( test_so.obj.ba[1] , 1 , test_suite , "bool array" )
    TRICK_EXPECT_EQ( test_so.obj.bp[1] , 1 , test_suite , "bool pointer" )
    TRICK_EXPECT_EQ( test_so.obj.baa[1][1] , 1 , test_suite , "2D bool array" )
    TRICK_EXPECT_EQ( test_so.obj.bap[1][1] , 1 , test_suite , "2D bool array of ptr" )
    TRICK_EXPECT_EQ( test_so.obj.bpp[1][1] , 1 , test_suite , "2D bool ptr of ptr" )

######################################################################################################################

    test_so.obj.cbit_0 = 0
    trick.rti_add("test_so.obj.cbit_0", -1)
    test_so.obj.cbit_1 = 0
    trick.rti_add("test_so.obj.cbit_1", 31)
    test_so.obj.cbit_2 = 0
    trick.rti_add("test_so.obj.cbit_2", 2)

    test_so.obj.ucbit_0 = 0
    trick.rti_add("test_so.obj.ucbit_0", 2)
    test_so.obj.ucbit_1 = 0
    trick.rti_add("test_so.obj.ucbit_1", 31)
    test_so.obj.ucbit_2 = 0
    trick.rti_add("test_so.obj.ucbit_2", -1)

    trick.rti_fire()
    trick_inject.rtis.GetRtiExecutor(0).Exec()

    # The raspberry pi has a bug and returns bitfields in unsigned format.
    if os.uname()[4] == 'armv61':
        TRICK_EXPECT_EQ( test_so.obj.cbit_0 , 3 , test_suite , "char bitfield" )
        TRICK_EXPECT_EQ( test_so.obj.cbit_1 , 7 , test_suite , "char bitfield" )
    else :
        TRICK_EXPECT_EQ( test_so.obj.cbit_0 , -1 , test_suite , "char bitfield" )
        TRICK_EXPECT_EQ( test_so.obj.cbit_1 , -1 , test_suite , "char bitfield" )
    TRICK_EXPECT_EQ( test_so.obj.cbit_2 , 2 , test_suite , "char bitfield" )

    TRICK_EXPECT_EQ( test_so.obj.ucbit_0 , 2 , test_suite , "unsigned char bitfield" )
    TRICK_EXPECT_EQ( test_so.obj.ucbit_1 , 7 , test_suite , "unsigned char bitfield" )
    TRICK_EXPECT_EQ( test_so.obj.ucbit_2 , 7 , test_suite , "unsigned char bitfield" )

######################################################################################################################

    test_so.obj.sbit_0 = 0
    trick.rti_add("test_so.obj.sbit_0", -1)
    test_so.obj.sbit_1 = 0
    trick.rti_add("test_so.obj.sbit_1", 255)
    test_so.obj.sbit_2 = 0
    trick.rti_add("test_so.obj.sbit_2", 2)

    test_so.obj.usbit_0 = 0
    trick.rti_add("test_so.obj.usbit_0", 2)
    test_so.obj.usbit_1 = 0
    trick.rti_add("test_so.obj.usbit_1", 255)
    test_so.obj.usbit_2 = 0
    trick.rti_add("test_so.obj.usbit_2", -1)

    trick.rti_fire()
    trick_inject.rtis.GetRtiExecutor(0).Exec()

    TRICK_EXPECT_EQ( test_so.obj.sbit_0 , -1 , test_suite , "short bitfield" )
    TRICK_EXPECT_EQ( test_so.obj.sbit_1 , -1 , test_suite , "short bitfield" )
    TRICK_EXPECT_EQ( test_so.obj.sbit_2 , 2 , test_suite , "short bitfield" )

    TRICK_EXPECT_EQ( test_so.obj.usbit_0 , 2 , test_suite , "unsigned short bitfield" )
    TRICK_EXPECT_EQ( test_so.obj.usbit_1 , 31 , test_suite , "unsigned short bitfield" )
    TRICK_EXPECT_EQ( test_so.obj.usbit_2 , 127 , test_suite , "unsigned short bitfield" )

######################################################################################################################

    test_so.obj.bit_0 = 0
    trick.rti_add("test_so.obj.bit_0", -1)
    test_so.obj.bit_1 = 0
    trick.rti_add("test_so.obj.bit_1", 255)
    test_so.obj.bit_2 = 0
    trick.rti_add("test_so.obj.bit_2", 2)

    test_so.obj.ubit_0 = 0
    trick.rti_add("test_so.obj.ubit_0", 2)
    test_so.obj.ubit_1 = 0
    trick.rti_add("test_so.obj.ubit_1", 255)
    test_so.obj.ubit_2 = 0
    trick.rti_add("test_so.obj.ubit_2", -1)

    trick.rti_fire()
    trick_inject.rtis.GetRtiExecutor(0).Exec()

    TRICK_EXPECT_EQ( test_so.obj.bit_0 , -1 , test_suite , "int bitfield" )
    TRICK_EXPECT_EQ( test_so.obj.bit_1 , -1 , test_suite , "int bitfield" )
    TRICK_EXPECT_EQ( test_so.obj.bit_2 , 2 , test_suite , "int bitfield" )

    TRICK_EXPECT_EQ( test_so.obj.ubit_0 , 2 , test_suite , "unsigned int bitfield" )
    TRICK_EXPECT_EQ( test_so.obj.ubit_1 , 31 , test_suite , "unsigned int bitfield" )
    TRICK_EXPECT_EQ( test_so.obj.ubit_2 , 63 , test_suite , "unsigned int bitfield" )

######################################################################################################################

    # long and long long bitfields are not currently supported by
    # sim_services/MemoryManager/src/insert_bitfield_any.c

    #test_so.obj.lbit_0 = 0
    #trick.rti_add("test_so.obj.lbit_0", -1)
    #test_so.obj.lbit_1 = 0
    #trick.rti_add("test_so.obj.lbit_1", 255)
    #test_so.obj.lbit_2 = 0
    #trick.rti_add("test_so.obj.lbit_2", 2)

    #test_so.obj.ulbit_0 = 0
    #trick.rti_add("test_so.obj.ulbit_0", 2)
    #test_so.obj.ulbit_1 = 0
    #trick.rti_add("test_so.obj.ulbit_1", 255)
    #test_so.obj.ulbit_2 = 0
    #trick.rti_add("test_so.obj.ulbit_2", -1)

    #trick.rti_fire()
    #trick_inject.rtis.GetRtiExecutor(0).Exec()

    #TRICK_EXPECT_EQ( test_so.obj.lbit_0 , -1 , test_suite , "long bitfield" )
    #TRICK_EXPECT_EQ( test_so.obj.lbit_1 , -1 , test_suite , "long bitfield" )
    #TRICK_EXPECT_EQ( test_so.obj.lbit_2 , 2 , test_suite , "long bitfield" )

    #TRICK_EXPECT_EQ( test_so.obj.ulbit_0 , 2 , test_suite , "unsigned long bitfield" )
    #TRICK_EXPECT_EQ( test_so.obj.ulbit_1 , 31 , test_suite , "unsigned long bitfield" )
    #TRICK_EXPECT_EQ( test_so.obj.ulbit_2 , 63 , test_suite , "unsigned long bitfield" )

######################################################################################################################

    test_so.obj.boolbit_0 = 0
    trick.rti_add("test_so.obj.boolbit_0", 2)
    test_so.obj.boolbit_1 = 0
    trick.rti_add("test_so.obj.boolbit_1", 1)
    test_so.obj.boolbit_2 = 0
    trick.rti_add("test_so.obj.boolbit_2", 255)

    trick.rti_fire()
    trick_inject.rtis.GetRtiExecutor(0).Exec()

    TRICK_EXPECT_EQ( test_so.obj.boolbit_0 , 0 , test_suite , "bool bitfield" )
    TRICK_EXPECT_EQ( test_so.obj.boolbit_1 , 1 , test_suite , "bool bitfield" )
    TRICK_EXPECT_EQ( test_so.obj.boolbit_2 , 1 , test_suite , "bool bitfield" )

######################################################################################################################

if __name__ == "__main__":

    main()

