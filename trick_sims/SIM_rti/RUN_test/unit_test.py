
import math
from trick.unit_test import *

def main():

    my_integ_loop.getIntegrator( trick.Runge_Kutta_2, 4 );

    # An example of removing collect in the input file (original collect added in S_define file)
    #ball.obj.state.work.external_force = trick.delete_collect(ball.obj.state.work.external_force, ball.obj.force.output.force)
    # An example of adding a collect in the input file
    #ball.obj.state.work.external_force = trick.add_collect(ball.obj.state.work.external_force, ball.obj.force.output.force)

    # An example of turning off a sim_object
    trick.exec_set_sim_object_onoff("disabled_obj" , False)

    trick.exec_set_terminate_time(1.0)

    trick_utest.unit_tests.enable()
    trick_utest.unit_tests.set_file_name( os.getenv("TRICK_HOME") + "/trick_test/SIM_test_ip_rti.xml" )
    trick_utest.unit_tests.set_test_name( "RTI" )

######################################################################################################################

    test_suite = "rti_add_int"

    ball.obj.d = 0.0
    trick.rti_add("ball.obj.d", 50)
    ball.obj.da[1] = 0.0
    trick.rti_add("ball.obj.da[1]", 51)
    ball.obj.dp = trick.TMM_declare_var_s("double[6]")
    trick.rti_add("ball.obj.dp[1]", 52)
    ball.obj.daa[1][1] = 0.0
    trick.rti_add("ball.obj.daa[1][1]", 53)
    ball.obj.dap[1] = trick.TMM_declare_var_1d( "double", 4)
    trick.rti_add("ball.obj.dap[1][1]", 54)
    ball.obj.dpp = trick.TMM_declare_var_s("double *[4]")
    ball.obj.dpp[1] = trick.TMM_declare_var_1d( "double", 5)
    trick.rti_add("ball.obj.dpp[1][1]", 55)

    #trick.rti_list()
    trick.rti_fire()
    trick_inject.rtis.GetRtiExecutor(0).Exec()

    TRICK_EXPECT_NEAR( ball.obj.d , 50.0 , 0.000001 , test_suite , "double" )
    TRICK_EXPECT_NEAR( ball.obj.da[1] , 51.0 , 0.000001 , test_suite , "double array" )
    TRICK_EXPECT_NEAR( ball.obj.dp[1] , 52.0 , 0.000001 , test_suite , "double pointer" )
    TRICK_EXPECT_NEAR( ball.obj.daa[1][1] , 53.0 , 0.000001 , test_suite , "2D double array" )
    TRICK_EXPECT_NEAR( ball.obj.dap[1][1] , 54.0 , 0.000001 , test_suite , "2D double array of ptr" )
    TRICK_EXPECT_NEAR( ball.obj.dpp[1][1] , 55.0 , 0.000001 , test_suite , "2D double ptr of ptr" )

######################################################################################################################

    ball.obj.f = 0.0
    trick.rti_add("ball.obj.f", 50)
    ball.obj.fa[1] = 0.0
    trick.rti_add("ball.obj.fa[1]", 51)
    ball.obj.fp = trick.TMM_declare_var_s("float[6]")
    trick.rti_add("ball.obj.fp[1]", 52)
    ball.obj.faa[1][1] = 0.0
    trick.rti_add("ball.obj.faa[1][1]", 53)
    ball.obj.fap[1] = trick.TMM_declare_var_1d( "float", 4)
    trick.rti_add("ball.obj.fap[1][1]", 54)
    ball.obj.fpp = trick.TMM_declare_var_s("float *[4]")
    ball.obj.fpp[1] = trick.TMM_declare_var_1d( "float", 5)
    trick.rti_add("ball.obj.fpp[1][1]", 55)

    #trick.rti_list()
    trick.rti_fire()
    trick_inject.rtis.GetRtiExecutor(0).Exec()

    TRICK_EXPECT_NEAR( ball.obj.f , 50.0 , 0.000001 , test_suite , "float" )
    TRICK_EXPECT_NEAR( ball.obj.fa[1] , 51.0 , 0.000001 , test_suite , "float array" )
    TRICK_EXPECT_NEAR( ball.obj.fp[1] , 52.0 , 0.000001 , test_suite , "float pointer" )
    TRICK_EXPECT_NEAR( ball.obj.faa[1][1] , 53.0 , 0.000001 , test_suite , "2D float array" )
    TRICK_EXPECT_NEAR( ball.obj.fap[1][1] , 54.0 , 0.000001 , test_suite , "2D float array of ptr" )
    TRICK_EXPECT_NEAR( ball.obj.fpp[1][1] , 55.0 , 0.000001 , test_suite , "2D float ptr of ptr" )

######################################################################################################################

    ball.obj.c = 0
    trick.rti_add("ball.obj.c", 50)
    ball.obj.ca[1] = 0
    trick.rti_add("ball.obj.ca[1]", 51)
    ball.obj.cp = trick.TMM_declare_var_s("char[6]")
    trick.rti_add("ball.obj.cp[1]", 52)
    ball.obj.caa[1][1] = 0
    trick.rti_add("ball.obj.caa[1][1]", 53)
    ball.obj.cap[1] = trick.TMM_declare_var_1d( "char", 4)
    trick.rti_add("ball.obj.cap[1][1]", 54)
    ball.obj.cpp = trick.TMM_declare_var_s("char *[4]")
    ball.obj.cpp[1] = trick.TMM_declare_var_1d( "char", 5)
    trick.rti_add("ball.obj.cpp[1][1]", 55)

    #trick.rti_list()
    trick.rti_fire()
    trick_inject.rtis.GetRtiExecutor(0).Exec()

    TRICK_EXPECT_EQ( ball.obj.c , 50 , test_suite , "char" )
    TRICK_EXPECT_EQ( ball.obj.ca[1] , 51 , test_suite , "char array" )
    TRICK_EXPECT_EQ( ball.obj.cp[1] , 52 , test_suite , "char pointer" )
    TRICK_EXPECT_EQ( ball.obj.caa[1][1] , 53 , test_suite , "2D char array" )
    TRICK_EXPECT_EQ( ball.obj.cap[1][1] , 54 , test_suite , "2D char array of ptr" )
    TRICK_EXPECT_EQ( ball.obj.cpp[1][1] , 55 , test_suite , "2D char ptr of ptr" )

######################################################################################################################

    ball.obj.uc = 0
    trick.rti_add("ball.obj.uc", 50)
    ball.obj.uca[1] = 0
    trick.rti_add("ball.obj.uca[1]", 51)
    ball.obj.ucp = trick.TMM_declare_var_s("unsigned char[6]")
    trick.rti_add("ball.obj.ucp[1]", 52)
    ball.obj.ucaa[1][1] = 0
    trick.rti_add("ball.obj.ucaa[1][1]", 53)
    ball.obj.ucap[1] = trick.TMM_declare_var_1d( "unsigned char", 4)
    trick.rti_add("ball.obj.ucap[1][1]", 54)
    ball.obj.ucpp = trick.TMM_declare_var_s("unsigned char *[4]")
    ball.obj.ucpp[1] = trick.TMM_declare_var_1d( "unsigned char", 5)
    trick.rti_add("ball.obj.ucpp[1][1]", 55)

    #trick.rti_list()
    trick.rti_fire()
    trick_inject.rtis.GetRtiExecutor(0).Exec()

    TRICK_EXPECT_EQ( ball.obj.uc , 50 , test_suite , "unsigned char" )
    TRICK_EXPECT_EQ( ball.obj.uca[1] , 51 , test_suite , "unsigned char array" )
    TRICK_EXPECT_EQ( ball.obj.ucp[1] , 52 , test_suite , "unsigned char pointer" )
    TRICK_EXPECT_EQ( ball.obj.ucaa[1][1] , 53 , test_suite , "2D unsigned char array" )
    TRICK_EXPECT_EQ( ball.obj.ucap[1][1] , 54 , test_suite , "2D unsigned char array of ptr" )
    TRICK_EXPECT_EQ( ball.obj.ucpp[1][1] , 55 , test_suite , "2D unsigned char ptr of ptr" )

######################################################################################################################

    ball.obj.s = 0
    trick.rti_add("ball.obj.s", 50)
    ball.obj.sa[1] = 0
    trick.rti_add("ball.obj.sa[1]", 51)
    ball.obj.sp = trick.TMM_declare_var_s("short[6]")
    trick.rti_add("ball.obj.sp[1]", 52)
    ball.obj.saa[1][1] = 0
    trick.rti_add("ball.obj.saa[1][1]", 53)
    ball.obj.sap[1] = trick.TMM_declare_var_1d( "short", 4)
    trick.rti_add("ball.obj.sap[1][1]", 54)
    ball.obj.spp = trick.TMM_declare_var_s("short *[4]")
    ball.obj.spp[1] = trick.TMM_declare_var_1d( "short", 5)
    trick.rti_add("ball.obj.spp[1][1]", 55)

    #trick.rti_list()
    trick.rti_fire()
    trick_inject.rtis.GetRtiExecutor(0).Exec()

    TRICK_EXPECT_EQ( ball.obj.s , 50 , test_suite , "short" )
    TRICK_EXPECT_EQ( ball.obj.sa[1] , 51 , test_suite , "short array" )
    TRICK_EXPECT_EQ( ball.obj.sp[1] , 52 , test_suite , "short pointer" )
    TRICK_EXPECT_EQ( ball.obj.saa[1][1] , 53 , test_suite , "2D short array" )
    TRICK_EXPECT_EQ( ball.obj.sap[1][1] , 54 , test_suite , "2D short array of ptr" )
    TRICK_EXPECT_EQ( ball.obj.spp[1][1] , 55 , test_suite , "2D short ptr of ptr" )

######################################################################################################################

    ball.obj.us = 0
    trick.rti_add("ball.obj.us", 50)
    ball.obj.usa[1] = 0
    trick.rti_add("ball.obj.usa[1]", 51)
    ball.obj.usp = trick.TMM_declare_var_s("unsigned short[6]")
    trick.rti_add("ball.obj.usp[1]", 52)
    ball.obj.usaa[1][1] = 0
    trick.rti_add("ball.obj.usaa[1][1]", 53)
    ball.obj.usap[1] = trick.TMM_declare_var_1d( "unsigned short", 4)
    trick.rti_add("ball.obj.usap[1][1]", 54)
    ball.obj.uspp = trick.TMM_declare_var_s("unsigned short *[4]")
    ball.obj.uspp[1] = trick.TMM_declare_var_1d( "unsigned short", 5)
    trick.rti_add("ball.obj.uspp[1][1]", 55)

    #trick.rti_list()
    trick.rti_fire()
    trick_inject.rtis.GetRtiExecutor(0).Exec()

    TRICK_EXPECT_EQ( ball.obj.us , 50 , test_suite , "unsigned short" )
    TRICK_EXPECT_EQ( ball.obj.usa[1] , 51 , test_suite , "unsigned short array" )
    TRICK_EXPECT_EQ( ball.obj.usp[1] , 52 , test_suite , "unsigned short pointer" )
    TRICK_EXPECT_EQ( ball.obj.usaa[1][1] , 53 , test_suite , "2D unsigned short array" )
    TRICK_EXPECT_EQ( ball.obj.usap[1][1] , 54 , test_suite , "2D unsigned short array of ptr" )
    TRICK_EXPECT_EQ( ball.obj.uspp[1][1] , 55 , test_suite , "2D unsigned short ptr of ptr" )

######################################################################################################################

    ball.obj.i = 0
    trick.rti_add("ball.obj.i", 50)
    ball.obj.ia[1] = 0
    trick.rti_add("ball.obj.ia[1]", 51)
    ball.obj.ip = trick.TMM_declare_var_s("int[6]")
    trick.rti_add("ball.obj.ip[1]", 52)
    ball.obj.iaa[1][1] = 0
    trick.rti_add("ball.obj.iaa[1][1]", 53)
    ball.obj.iap[1] = trick.TMM_declare_var_1d( "int", 4)
    trick.rti_add("ball.obj.iap[1][1]", 54)
    ball.obj.ipp = trick.TMM_declare_var_s("int *[4]")
    ball.obj.ipp[1] = trick.TMM_declare_var_1d( "int", 5)
    trick.rti_add("ball.obj.ipp[1][1]", 55)

    #trick.rti_list()
    trick.rti_fire()
    trick_inject.rtis.GetRtiExecutor(0).Exec()

    TRICK_EXPECT_EQ( ball.obj.i , 50 , test_suite , "int" )
    TRICK_EXPECT_EQ( ball.obj.ia[1] , 51 , test_suite , "int array" )
    TRICK_EXPECT_EQ( ball.obj.ip[1] , 52 , test_suite , "int pointer" )
    TRICK_EXPECT_EQ( ball.obj.iaa[1][1] , 53 , test_suite , "2D int array" )
    TRICK_EXPECT_EQ( ball.obj.iap[1][1] , 54 , test_suite , "2D int array of ptr" )
    TRICK_EXPECT_EQ( ball.obj.ipp[1][1] , 55 , test_suite , "2D int ptr of ptr" )

######################################################################################################################

    ball.obj.ui = 0
    trick.rti_add("ball.obj.ui", 50)
    ball.obj.uia[1] = 0
    trick.rti_add("ball.obj.uia[1]", 51)
    ball.obj.uip = trick.TMM_declare_var_s("unsigned int[6]")
    trick.rti_add("ball.obj.uip[1]", 52)
    ball.obj.uiaa[1][1] = 0
    trick.rti_add("ball.obj.uiaa[1][1]", 53)
    ball.obj.uiap[1] = trick.TMM_declare_var_1d( "unsigned int", 4)
    trick.rti_add("ball.obj.uiap[1][1]", 54)
    ball.obj.uipp = trick.TMM_declare_var_s("unsigned int *[4]")
    ball.obj.uipp[1] = trick.TMM_declare_var_1d( "unsigned int", 5)
    trick.rti_add("ball.obj.uipp[1][1]", 55)

    #trick.rti_list()
    trick.rti_fire()
    trick_inject.rtis.GetRtiExecutor(0).Exec()

    TRICK_EXPECT_EQ( ball.obj.ui , 50 , test_suite , "unsigned int" )
    TRICK_EXPECT_EQ( ball.obj.uia[1] , 51 , test_suite , "unsigned int array" )
    TRICK_EXPECT_EQ( ball.obj.uip[1] , 52 , test_suite , "unsigned int pointer" )
    TRICK_EXPECT_EQ( ball.obj.uiaa[1][1] , 53 , test_suite , "2D unsigned int array" )
    TRICK_EXPECT_EQ( ball.obj.uiap[1][1] , 54 , test_suite , "2D unsigned int array of ptr" )
    TRICK_EXPECT_EQ( ball.obj.uipp[1][1] , 55 , test_suite , "2D unsigned int ptr of ptr" )

######################################################################################################################

    ball.obj.l = 0
    trick.rti_add("ball.obj.l", 50)
    ball.obj.la[1] = 0
    trick.rti_add("ball.obj.la[1]", 51)
    ball.obj.lp = trick.TMM_declare_var_s("long[6]")
    trick.rti_add("ball.obj.lp[1]", 52)
    ball.obj.laa[1][1] = 0
    trick.rti_add("ball.obj.laa[1][1]", 53)
    ball.obj.lap[1] = trick.TMM_declare_var_1d( "long", 4)
    trick.rti_add("ball.obj.lap[1][1]", 54)
    ball.obj.lpp = trick.TMM_declare_var_s("long *[4]")
    ball.obj.lpp[1] = trick.TMM_declare_var_1d( "long", 5)
    trick.rti_add("ball.obj.lpp[1][1]", 55)

    #trick.rti_list()
    trick.rti_fire()
    trick_inject.rtis.GetRtiExecutor(0).Exec()

    TRICK_EXPECT_EQ( ball.obj.l , 50 , test_suite , "long" )
    TRICK_EXPECT_EQ( ball.obj.la[1] , 51 , test_suite , "long array" )
    TRICK_EXPECT_EQ( ball.obj.lp[1] , 52 , test_suite , "long polonger" )
    TRICK_EXPECT_EQ( ball.obj.laa[1][1] , 53 , test_suite , "2D long array" )
    TRICK_EXPECT_EQ( ball.obj.lap[1][1] , 54 , test_suite , "2D long array of ptr" )
    TRICK_EXPECT_EQ( ball.obj.lpp[1][1] , 55 , test_suite , "2D long ptr of ptr" )

######################################################################################################################

    ball.obj.ul = 0
    trick.rti_add("ball.obj.ul", 50)
    ball.obj.ula[1] = 0
    trick.rti_add("ball.obj.ula[1]", 51)
    ball.obj.ulp = trick.TMM_declare_var_s("unsigned long[6]")
    trick.rti_add("ball.obj.ulp[1]", 52)
    ball.obj.ulaa[1][1] = 0
    trick.rti_add("ball.obj.ulaa[1][1]", 53)
    ball.obj.ulap[1] = trick.TMM_declare_var_1d( "unsigned long", 4)
    trick.rti_add("ball.obj.ulap[1][1]", 54)
    ball.obj.ulpp = trick.TMM_declare_var_s("unsigned long *[4]")
    ball.obj.ulpp[1] = trick.TMM_declare_var_1d( "unsigned long", 5)
    trick.rti_add("ball.obj.ulpp[1][1]", 55)

    #trick.rti_list()
    trick.rti_fire()
    trick_inject.rtis.GetRtiExecutor(0).Exec()

    TRICK_EXPECT_EQ( ball.obj.ul , 50 , test_suite , "unsigned long" )
    TRICK_EXPECT_EQ( ball.obj.ula[1] , 51 , test_suite , "unsigned long array" )
    TRICK_EXPECT_EQ( ball.obj.ulp[1] , 52 , test_suite , "unsigned long pointer" )
    TRICK_EXPECT_EQ( ball.obj.ulaa[1][1] , 53 , test_suite , "2D unsigned long array" )
    TRICK_EXPECT_EQ( ball.obj.ulap[1][1] , 54 , test_suite , "2D unsigned long array of ptr" )
    TRICK_EXPECT_EQ( ball.obj.ulpp[1][1] , 55 , test_suite , "2D unsigned long ptr of ptr" )

######################################################################################################################

    ball.obj.ll = 0
    trick.rti_add("ball.obj.ll", 50)
    ball.obj.lla[1] = 0
    trick.rti_add("ball.obj.lla[1]", 51)
    ball.obj.llp = trick.TMM_declare_var_s("long long[6]")
    trick.rti_add("ball.obj.llp[1]", 52)
    ball.obj.llaa[1][1] = 0
    trick.rti_add("ball.obj.llaa[1][1]", 53)
    ball.obj.llap[1] = trick.TMM_declare_var_1d( "long long", 4)
    trick.rti_add("ball.obj.llap[1][1]", 54)
    ball.obj.llpp = trick.TMM_declare_var_s("long long *[4]")
    ball.obj.llpp[1] = trick.TMM_declare_var_1d( "long long", 5)
    trick.rti_add("ball.obj.llpp[1][1]", 55)

    #trick.rti_list()
    trick.rti_fire()
    trick_inject.rtis.GetRtiExecutor(0).Exec()

    TRICK_EXPECT_EQ( ball.obj.ll , 50 , test_suite , "long long" )
    TRICK_EXPECT_EQ( ball.obj.lla[1] , 51 , test_suite , "long long array" )
    TRICK_EXPECT_EQ( ball.obj.llp[1] , 52 , test_suite , "long long pointer" )
    TRICK_EXPECT_EQ( ball.obj.llaa[1][1] , 53 , test_suite , "2D long long array" )
    TRICK_EXPECT_EQ( ball.obj.llap[1][1] , 54 , test_suite , "2D long long array of ptr" )
    TRICK_EXPECT_EQ( ball.obj.llpp[1][1] , 55 , test_suite , "2D long long ptr of ptr" )

######################################################################################################################

    ball.obj.ull = 0
    trick.rti_add("ball.obj.ull", 50)
    ball.obj.ulla[1] = 0
    trick.rti_add("ball.obj.ulla[1]", 51)
    ball.obj.ullp = trick.TMM_declare_var_s("unsigned long long[6]")
    trick.rti_add("ball.obj.ullp[1]", 52)
    ball.obj.ullaa[1][1] = 0
    trick.rti_add("ball.obj.ullaa[1][1]", 53)
    ball.obj.ullap[1] = trick.TMM_declare_var_1d( "unsigned long long", 4)
    trick.rti_add("ball.obj.ullap[1][1]", 54)
    ball.obj.ullpp = trick.TMM_declare_var_s("unsigned long long *[4]")
    ball.obj.ullpp[1] = trick.TMM_declare_var_1d( "unsigned long long", 5)
    trick.rti_add("ball.obj.ullpp[1][1]", 55)

    #trick.rti_list()
    trick.rti_fire()
    trick_inject.rtis.GetRtiExecutor(0).Exec()

    TRICK_EXPECT_EQ( ball.obj.ull , 50 , test_suite , "unsigned long long" )
    TRICK_EXPECT_EQ( ball.obj.ulla[1] , 51 , test_suite , "unsigned long long array" )
    TRICK_EXPECT_EQ( ball.obj.ullp[1] , 52 , test_suite , "unsigned long long pointer" )
    TRICK_EXPECT_EQ( ball.obj.ullaa[1][1] , 53 , test_suite , "2D unsigned long long array" )
    TRICK_EXPECT_EQ( ball.obj.ullap[1][1] , 54 , test_suite , "2D unsigned long long array of ptr" )
    TRICK_EXPECT_EQ( ball.obj.ullpp[1][1] , 55 , test_suite , "2D unsigned long long ptr of ptr" )

######################################################################################################################

    ball.obj.e = 0
    trick.rti_add("ball.obj.e", 50)
    ball.obj.ea[1] = 0
    trick.rti_add("ball.obj.ea[1]", 51)
    ball.obj.ep = trick.TMM_declare_var_s("MY_ENUM[6]")
    trick.rti_add("ball.obj.ep[1]", 52)
    ball.obj.eaa[1][1] = 0
    trick.rti_add("ball.obj.eaa[1][1]", 53)
    ball.obj.eap[1] = trick.TMM_declare_var_1d( "MY_ENUM", 4)
    trick.rti_add("ball.obj.eap[1][1]", 54)
    ball.obj.epp = trick.TMM_declare_var_s("MY_ENUM *[4]")
    ball.obj.epp[1] = trick.TMM_declare_var_1d( "MY_ENUM", 5)
    trick.rti_add("ball.obj.epp[1][1]", 55)

    #trick.rti_list()
    trick.rti_fire()
    trick_inject.rtis.GetRtiExecutor(0).Exec()

    TRICK_EXPECT_EQ( ball.obj.e , 50 , test_suite , "enum" )
    TRICK_EXPECT_EQ( ball.obj.ea[1] , 51 , test_suite , "enum array" )
    TRICK_EXPECT_EQ( ball.obj.ep[1] , 52 , test_suite , "enum pointer" )
    TRICK_EXPECT_EQ( ball.obj.eaa[1][1] , 53 , test_suite , "2D enum array" )
    TRICK_EXPECT_EQ( ball.obj.eap[1][1] , 54 , test_suite , "2D enum array of ptr" )
    TRICK_EXPECT_EQ( ball.obj.epp[1][1] , 55 , test_suite , "2D enum ptr of ptr" )

######################################################################################################################

    ball.obj.b = 0
    trick.rti_add("ball.obj.b", 50)
    ball.obj.ba[1] = 0
    trick.rti_add("ball.obj.ba[1]", 51)
    ball.obj.bp = trick.TMM_declare_var_s("bool[6]")
    trick.rti_add("ball.obj.bp[1]", 52)
    ball.obj.baa[1][1] = 0
    trick.rti_add("ball.obj.baa[1][1]", 53)
    ball.obj.bap[1] = trick.TMM_declare_var_1d( "bool", 4)
    trick.rti_add("ball.obj.bap[1][1]", 54)
    ball.obj.bpp = trick.TMM_declare_var_s("bool *[4]")
    ball.obj.bpp[1] = trick.TMM_declare_var_1d( "bool", 5)
    trick.rti_add("ball.obj.bpp[1][1]", 55)

    #trick.rti_list()
    trick.rti_fire()
    trick_inject.rtis.GetRtiExecutor(0).Exec()

    TRICK_EXPECT_EQ( ball.obj.b , 1 , test_suite , "bool" )
    TRICK_EXPECT_EQ( ball.obj.ba[1] , 1 , test_suite , "bool array" )
    TRICK_EXPECT_EQ( ball.obj.bp[1] , 1 , test_suite , "bool pointer" )
    TRICK_EXPECT_EQ( ball.obj.baa[1][1] , 1 , test_suite , "2D bool array" )
    TRICK_EXPECT_EQ( ball.obj.bap[1][1] , 1 , test_suite , "2D bool array of ptr" )
    TRICK_EXPECT_EQ( ball.obj.bpp[1][1] , 1 , test_suite , "2D bool ptr of ptr" )

######################################################################################################################

    ball.obj.cbit_0 = 0
    trick.rti_add("ball.obj.cbit_0", -1)
    ball.obj.cbit_1 = 0
    trick.rti_add("ball.obj.cbit_1", 31)
    ball.obj.cbit_2 = 0
    trick.rti_add("ball.obj.cbit_2", 2)

    ball.obj.ucbit_0 = 0
    trick.rti_add("ball.obj.ucbit_0", 2)
    ball.obj.ucbit_1 = 0
    trick.rti_add("ball.obj.ucbit_1", 31)
    ball.obj.ucbit_2 = 0
    trick.rti_add("ball.obj.ucbit_2", -1)

    trick.rti_fire()
    trick_inject.rtis.GetRtiExecutor(0).Exec()

    print os.uname()[4]
    # The raspberry pi has a bug and returns bitfields in unsigned format.
    if os.uname()[4] == 'armv6l':
        TRICK_EXPECT_EQ( ball.obj.cbit_0 , 3 , test_suite , "char bitfield" )
        TRICK_EXPECT_EQ( ball.obj.cbit_1 , 7 , test_suite , "char bitfield" )
    else :
        TRICK_EXPECT_EQ( ball.obj.cbit_0 , -1 , test_suite , "char bitfield" )
        TRICK_EXPECT_EQ( ball.obj.cbit_1 , -1 , test_suite , "char bitfield" )
    TRICK_EXPECT_EQ( ball.obj.cbit_2 , 2 , test_suite , "char bitfield" )

    TRICK_EXPECT_EQ( ball.obj.ucbit_0 , 2 , test_suite , "unsigned char bitfield" )
    TRICK_EXPECT_EQ( ball.obj.ucbit_1 , 7 , test_suite , "unsigned char bitfield" )
    TRICK_EXPECT_EQ( ball.obj.ucbit_2 , 7 , test_suite , "unsigned char bitfield" )

######################################################################################################################

    ball.obj.sbit_0 = 0
    trick.rti_add("ball.obj.sbit_0", -1)
    ball.obj.sbit_1 = 0
    trick.rti_add("ball.obj.sbit_1", 255)
    ball.obj.sbit_2 = 0
    trick.rti_add("ball.obj.sbit_2", 2)

    ball.obj.usbit_0 = 0
    trick.rti_add("ball.obj.usbit_0", 2)
    ball.obj.usbit_1 = 0
    trick.rti_add("ball.obj.usbit_1", 255)
    ball.obj.usbit_2 = 0
    trick.rti_add("ball.obj.usbit_2", -1)

    trick.rti_fire()
    trick_inject.rtis.GetRtiExecutor(0).Exec()

    TRICK_EXPECT_EQ( ball.obj.sbit_0 , -1 , test_suite , "short bitfield" )
    TRICK_EXPECT_EQ( ball.obj.sbit_1 , -1 , test_suite , "short bitfield" )
    TRICK_EXPECT_EQ( ball.obj.sbit_2 , 2 , test_suite , "short bitfield" )

    TRICK_EXPECT_EQ( ball.obj.usbit_0 , 2 , test_suite , "unsigned short bitfield" )
    TRICK_EXPECT_EQ( ball.obj.usbit_1 , 31 , test_suite , "unsigned short bitfield" )
    TRICK_EXPECT_EQ( ball.obj.usbit_2 , 127 , test_suite , "unsigned short bitfield" )

######################################################################################################################

    ball.obj.bit_0 = 0
    trick.rti_add("ball.obj.bit_0", -1)
    ball.obj.bit_1 = 0
    trick.rti_add("ball.obj.bit_1", 255)
    ball.obj.bit_2 = 0
    trick.rti_add("ball.obj.bit_2", 2)

    ball.obj.ubit_0 = 0
    trick.rti_add("ball.obj.ubit_0", 2)
    ball.obj.ubit_1 = 0
    trick.rti_add("ball.obj.ubit_1", 255)
    ball.obj.ubit_2 = 0
    trick.rti_add("ball.obj.ubit_2", -1)

    trick.rti_fire()
    trick_inject.rtis.GetRtiExecutor(0).Exec()

    TRICK_EXPECT_EQ( ball.obj.bit_0 , -1 , test_suite , "int bitfield" )
    TRICK_EXPECT_EQ( ball.obj.bit_1 , -1 , test_suite , "int bitfield" )
    TRICK_EXPECT_EQ( ball.obj.bit_2 , 2 , test_suite , "int bitfield" )

    TRICK_EXPECT_EQ( ball.obj.ubit_0 , 2 , test_suite , "unsigned int bitfield" )
    TRICK_EXPECT_EQ( ball.obj.ubit_1 , 31 , test_suite , "unsigned int bitfield" )
    TRICK_EXPECT_EQ( ball.obj.ubit_2 , 63 , test_suite , "unsigned int bitfield" )

######################################################################################################################

    # long and long long bitfields are not currently supported by
    # sim_services/MemoryManager/src/insert_bitfield_any.c

    #ball.obj.lbit_0 = 0
    #trick.rti_add("ball.obj.lbit_0", -1)
    #ball.obj.lbit_1 = 0
    #trick.rti_add("ball.obj.lbit_1", 255)
    #ball.obj.lbit_2 = 0
    #trick.rti_add("ball.obj.lbit_2", 2)

    #ball.obj.ulbit_0 = 0
    #trick.rti_add("ball.obj.ulbit_0", 2)
    #ball.obj.ulbit_1 = 0
    #trick.rti_add("ball.obj.ulbit_1", 255)
    #ball.obj.ulbit_2 = 0
    #trick.rti_add("ball.obj.ulbit_2", -1)

    #trick.rti_fire()
    #trick_inject.rtis.GetRtiExecutor(0).Exec()

    #TRICK_EXPECT_EQ( ball.obj.lbit_0 , -1 , test_suite , "long bitfield" )
    #TRICK_EXPECT_EQ( ball.obj.lbit_1 , -1 , test_suite , "long bitfield" )
    #TRICK_EXPECT_EQ( ball.obj.lbit_2 , 2 , test_suite , "long bitfield" )

    #TRICK_EXPECT_EQ( ball.obj.ulbit_0 , 2 , test_suite , "unsigned long bitfield" )
    #TRICK_EXPECT_EQ( ball.obj.ulbit_1 , 31 , test_suite , "unsigned long bitfield" )
    #TRICK_EXPECT_EQ( ball.obj.ulbit_2 , 63 , test_suite , "unsigned long bitfield" )

######################################################################################################################

    ball.obj.boolbit_0 = 0
    trick.rti_add("ball.obj.boolbit_0", 2)
    ball.obj.boolbit_1 = 0
    trick.rti_add("ball.obj.boolbit_1", 1)
    ball.obj.boolbit_2 = 0
    trick.rti_add("ball.obj.boolbit_2", 255)

    trick.rti_fire()
    trick_inject.rtis.GetRtiExecutor(0).Exec()

    TRICK_EXPECT_EQ( ball.obj.boolbit_0 , 0 , test_suite , "bool bitfield" )
    TRICK_EXPECT_EQ( ball.obj.boolbit_1 , 1 , test_suite , "bool bitfield" )
    TRICK_EXPECT_EQ( ball.obj.boolbit_2 , 1 , test_suite , "bool bitfield" )

######################################################################################################################
######################################################################################################################

    test_suite = "rti_add_float"

    ball.obj.d = 0.0
    trick.rti_add("ball.obj.d", 50.1)
    ball.obj.da[1] = 0.0
    trick.rti_add("ball.obj.da[1]", 51.1)
    ball.obj.dp = trick.TMM_declare_var_s("double[6]")
    trick.rti_add("ball.obj.dp[1]", 52.1)
    ball.obj.daa[1][1] = 0.0
    trick.rti_add("ball.obj.daa[1][1]", 53.1)
    ball.obj.dap[1] = trick.TMM_declare_var_1d( "double", 4)
    trick.rti_add("ball.obj.dap[1][1]", 54.1)
    ball.obj.dpp = trick.TMM_declare_var_s("double *[4]")
    ball.obj.dpp[1] = trick.TMM_declare_var_1d( "double", 5)
    trick.rti_add("ball.obj.dpp[1][1]", 55.1)

    #trick.rti_list()
    trick.rti_fire()
    trick_inject.rtis.GetRtiExecutor(0).Exec()

    TRICK_EXPECT_NEAR( ball.obj.d , 50.1 , 0.000001 , test_suite , "double" )
    TRICK_EXPECT_NEAR( ball.obj.da[1] , 51.1 , 0.000001 , test_suite , "double array" )
    TRICK_EXPECT_NEAR( ball.obj.dp[1] , 52.1 , 0.000001 , test_suite , "double pointer" )
    TRICK_EXPECT_NEAR( ball.obj.daa[1][1] , 53.1 , 0.000001 , test_suite , "2D double array" )
    TRICK_EXPECT_NEAR( ball.obj.dap[1][1] , 54.1 , 0.000001 , test_suite , "2D double array of ptr" )
    TRICK_EXPECT_NEAR( ball.obj.dpp[1][1] , 55.1 , 0.000001 , test_suite , "2D double ptr of ptr" )

######################################################################################################################

    ball.obj.f = 0.0
    trick.rti_add("ball.obj.f", 50.1)
    ball.obj.fa[1] = 0.0
    trick.rti_add("ball.obj.fa[1]", 51.1)
    ball.obj.fp = trick.TMM_declare_var_s("float[6]")
    trick.rti_add("ball.obj.fp[1]", 52.1)
    ball.obj.faa[1][1] = 0.0
    trick.rti_add("ball.obj.faa[1][1]", 53.1)
    ball.obj.fap[1] = trick.TMM_declare_var_1d( "float", 4)
    trick.rti_add("ball.obj.fap[1][1]", 54.1)
    ball.obj.fpp = trick.TMM_declare_var_s("float *[4]")
    ball.obj.fpp[1] = trick.TMM_declare_var_1d( "float", 5)
    trick.rti_add("ball.obj.fpp[1][1]", 55.1)

    #trick.rti_list()
    trick.rti_fire()
    trick_inject.rtis.GetRtiExecutor(0).Exec()

    TRICK_EXPECT_NEAR( ball.obj.f , 50.1 , 0.00001 , test_suite , "float" )
    TRICK_EXPECT_NEAR( ball.obj.fa[1] , 51.1 , 0.00001 , test_suite , "float array" )
    TRICK_EXPECT_NEAR( ball.obj.fp[1] , 52.1 , 0.00001 , test_suite , "float pointer" )
    TRICK_EXPECT_NEAR( ball.obj.faa[1][1] , 53.1 , 0.00001 , test_suite , "2D float array" )
    TRICK_EXPECT_NEAR( ball.obj.fap[1][1] , 54.1 , 0.00001 , test_suite , "2D float array of ptr" )
    TRICK_EXPECT_NEAR( ball.obj.fpp[1][1] , 55.1 , 0.00001 , test_suite , "2D float ptr of ptr" )

######################################################################################################################

    ball.obj.c = 0
    trick.rti_add("ball.obj.c", 40.1)
    ball.obj.ca[1] = 0
    trick.rti_add("ball.obj.ca[1]", 41.1)
    ball.obj.cp = trick.TMM_declare_var_s("char[6]")
    trick.rti_add("ball.obj.cp[1]", 42.1)
    ball.obj.caa[1][1] = 0
    trick.rti_add("ball.obj.caa[1][1]", 43.1)
    ball.obj.cap[1] = trick.TMM_declare_var_1d( "char", 4)
    trick.rti_add("ball.obj.cap[1][1]", 44.1)
    ball.obj.cpp = trick.TMM_declare_var_s("char *[4]")
    ball.obj.cpp[1] = trick.TMM_declare_var_1d( "char", 5)
    trick.rti_add("ball.obj.cpp[1][1]", 45.1)

    #trick.rti_list()
    trick.rti_fire()
    trick_inject.rtis.GetRtiExecutor(0).Exec()

    TRICK_EXPECT_EQ( ball.obj.c , 40 , test_suite , "char" )
    TRICK_EXPECT_EQ( ball.obj.ca[1] , 41 , test_suite , "char array" )
    TRICK_EXPECT_EQ( ball.obj.cp[1] , 42 , test_suite , "char pointer" )
    TRICK_EXPECT_EQ( ball.obj.caa[1][1] , 43 , test_suite , "2D char array" )
    TRICK_EXPECT_EQ( ball.obj.cap[1][1] , 44 , test_suite , "2D char array of ptr" )
    TRICK_EXPECT_EQ( ball.obj.cpp[1][1] , 45 , test_suite , "2D char ptr of ptr" )

######################################################################################################################

    ball.obj.uc = 0
    trick.rti_add("ball.obj.uc", 40.1)
    ball.obj.uca[1] = 0
    trick.rti_add("ball.obj.uca[1]", 41.1)
    ball.obj.ucp = trick.TMM_declare_var_s("unsigned char[6]")
    trick.rti_add("ball.obj.ucp[1]", 42.1)
    ball.obj.ucaa[1][1] = 0
    trick.rti_add("ball.obj.ucaa[1][1]", 43.1)
    ball.obj.ucap[1] = trick.TMM_declare_var_1d( "unsigned char", 4)
    trick.rti_add("ball.obj.ucap[1][1]", 44.1)
    ball.obj.ucpp = trick.TMM_declare_var_s("unsigned char *[4]")
    ball.obj.ucpp[1] = trick.TMM_declare_var_1d( "unsigned char", 5)
    trick.rti_add("ball.obj.ucpp[1][1]", 45.1)

    #trick.rti_list()
    trick.rti_fire()
    trick_inject.rtis.GetRtiExecutor(0).Exec()

    TRICK_EXPECT_EQ( ball.obj.uc , 40 , test_suite , "unsigned char" )
    TRICK_EXPECT_EQ( ball.obj.uca[1] , 41 , test_suite , "unsigned char array" )
    TRICK_EXPECT_EQ( ball.obj.ucp[1] , 42 , test_suite , "unsigned char pointer" )
    TRICK_EXPECT_EQ( ball.obj.ucaa[1][1] , 43 , test_suite , "2D unsigned char array" )
    TRICK_EXPECT_EQ( ball.obj.ucap[1][1] , 44 , test_suite , "2D unsigned char array of ptr" )
    TRICK_EXPECT_EQ( ball.obj.ucpp[1][1] , 45 , test_suite , "2D unsigned char ptr of ptr" )

######################################################################################################################

    ball.obj.s = 0
    trick.rti_add("ball.obj.s", 40.1)
    ball.obj.sa[1] = 0
    trick.rti_add("ball.obj.sa[1]", 41.1)
    ball.obj.sp = trick.TMM_declare_var_s("short[6]")
    trick.rti_add("ball.obj.sp[1]", 42.1)
    ball.obj.saa[1][1] = 0
    trick.rti_add("ball.obj.saa[1][1]", 43.1)
    ball.obj.sap[1] = trick.TMM_declare_var_1d( "short", 4)
    trick.rti_add("ball.obj.sap[1][1]", 44.1)
    ball.obj.spp = trick.TMM_declare_var_s("short *[4]")
    ball.obj.spp[1] = trick.TMM_declare_var_1d( "short", 5)
    trick.rti_add("ball.obj.spp[1][1]", 45.1)

    #trick.rti_list()
    trick.rti_fire()
    trick_inject.rtis.GetRtiExecutor(0).Exec()

    TRICK_EXPECT_EQ( ball.obj.s , 40 , test_suite , "short" )
    TRICK_EXPECT_EQ( ball.obj.sa[1] , 41 , test_suite , "short array" )
    TRICK_EXPECT_EQ( ball.obj.sp[1] , 42 , test_suite , "short pointer" )
    TRICK_EXPECT_EQ( ball.obj.saa[1][1] , 43 , test_suite , "2D short array" )
    TRICK_EXPECT_EQ( ball.obj.sap[1][1] , 44 , test_suite , "2D short array of ptr" )
    TRICK_EXPECT_EQ( ball.obj.spp[1][1] , 45 , test_suite , "2D short ptr of ptr" )

######################################################################################################################

    ball.obj.us = 0
    trick.rti_add("ball.obj.us", 40.1)
    ball.obj.usa[1] = 0
    trick.rti_add("ball.obj.usa[1]", 41.1)
    ball.obj.usp = trick.TMM_declare_var_s("unsigned short[6]")
    trick.rti_add("ball.obj.usp[1]", 42.1)
    ball.obj.usaa[1][1] = 0
    trick.rti_add("ball.obj.usaa[1][1]", 43.1)
    ball.obj.usap[1] = trick.TMM_declare_var_1d( "unsigned short", 4)
    trick.rti_add("ball.obj.usap[1][1]", 44.1)
    ball.obj.uspp = trick.TMM_declare_var_s("unsigned short *[4]")
    ball.obj.uspp[1] = trick.TMM_declare_var_1d( "unsigned short", 5)
    trick.rti_add("ball.obj.uspp[1][1]", 45.1)

    #trick.rti_list()
    trick.rti_fire()
    trick_inject.rtis.GetRtiExecutor(0).Exec()

    TRICK_EXPECT_EQ( ball.obj.us , 40 , test_suite , "unsigned short" )
    TRICK_EXPECT_EQ( ball.obj.usa[1] , 41 , test_suite , "unsigned short array" )
    TRICK_EXPECT_EQ( ball.obj.usp[1] , 42 , test_suite , "unsigned short pointer" )
    TRICK_EXPECT_EQ( ball.obj.usaa[1][1] , 43 , test_suite , "2D unsigned short array" )
    TRICK_EXPECT_EQ( ball.obj.usap[1][1] , 44 , test_suite , "2D unsigned short array of ptr" )
    TRICK_EXPECT_EQ( ball.obj.uspp[1][1] , 45 , test_suite , "2D unsigned short ptr of ptr" )

######################################################################################################################

    ball.obj.i = 0
    trick.rti_add("ball.obj.i", 40.1)
    ball.obj.ia[1] = 0
    trick.rti_add("ball.obj.ia[1]", 41.1)
    ball.obj.ip = trick.TMM_declare_var_s("int[6]")
    trick.rti_add("ball.obj.ip[1]", 42.1)
    ball.obj.iaa[1][1] = 0
    trick.rti_add("ball.obj.iaa[1][1]", 43.1)
    ball.obj.iap[1] = trick.TMM_declare_var_1d( "int", 4)
    trick.rti_add("ball.obj.iap[1][1]", 44.1)
    ball.obj.ipp = trick.TMM_declare_var_s("int *[4]")
    ball.obj.ipp[1] = trick.TMM_declare_var_1d( "int", 5)
    trick.rti_add("ball.obj.ipp[1][1]", 45.1)

    #trick.rti_list()
    trick.rti_fire()
    trick_inject.rtis.GetRtiExecutor(0).Exec()

    TRICK_EXPECT_EQ( ball.obj.i , 40 , test_suite , "int" )
    TRICK_EXPECT_EQ( ball.obj.ia[1] , 41 , test_suite , "int array" )
    TRICK_EXPECT_EQ( ball.obj.ip[1] , 42 , test_suite , "int pointer" )
    TRICK_EXPECT_EQ( ball.obj.iaa[1][1] , 43 , test_suite , "2D int array" )
    TRICK_EXPECT_EQ( ball.obj.iap[1][1] , 44 , test_suite , "2D int array of ptr" )
    TRICK_EXPECT_EQ( ball.obj.ipp[1][1] , 45 , test_suite , "2D int ptr of ptr" )

######################################################################################################################

    ball.obj.ui = 0
    trick.rti_add("ball.obj.ui", 40.1)
    ball.obj.uia[1] = 0
    trick.rti_add("ball.obj.uia[1]", 41.1)
    ball.obj.uip = trick.TMM_declare_var_s("unsigned int[6]")
    trick.rti_add("ball.obj.uip[1]", 42.1)
    ball.obj.uiaa[1][1] = 0
    trick.rti_add("ball.obj.uiaa[1][1]", 43.1)
    ball.obj.uiap[1] = trick.TMM_declare_var_1d( "unsigned int", 4)
    trick.rti_add("ball.obj.uiap[1][1]", 44.1)
    ball.obj.uipp = trick.TMM_declare_var_s("unsigned int *[4]")
    ball.obj.uipp[1] = trick.TMM_declare_var_1d( "unsigned int", 5)
    trick.rti_add("ball.obj.uipp[1][1]", 45.1)

    #trick.rti_list()
    trick.rti_fire()
    trick_inject.rtis.GetRtiExecutor(0).Exec()

    TRICK_EXPECT_EQ( ball.obj.ui , 40 , test_suite , "unsigned int" )
    TRICK_EXPECT_EQ( ball.obj.uia[1] , 41 , test_suite , "unsigned int array" )
    TRICK_EXPECT_EQ( ball.obj.uip[1] , 42 , test_suite , "unsigned int pointer" )
    TRICK_EXPECT_EQ( ball.obj.uiaa[1][1] , 43 , test_suite , "2D unsigned int array" )
    TRICK_EXPECT_EQ( ball.obj.uiap[1][1] , 44 , test_suite , "2D unsigned int array of ptr" )
    TRICK_EXPECT_EQ( ball.obj.uipp[1][1] , 45 , test_suite , "2D unsigned int ptr of ptr" )

######################################################################################################################

    ball.obj.l = 0
    trick.rti_add("ball.obj.l", 40.1)
    ball.obj.la[1] = 0
    trick.rti_add("ball.obj.la[1]", 41.1)
    ball.obj.lp = trick.TMM_declare_var_s("long[6]")
    trick.rti_add("ball.obj.lp[1]", 42.1)
    ball.obj.laa[1][1] = 0
    trick.rti_add("ball.obj.laa[1][1]", 43.1)
    ball.obj.lap[1] = trick.TMM_declare_var_1d( "long", 4)
    trick.rti_add("ball.obj.lap[1][1]", 44.1)
    ball.obj.lpp = trick.TMM_declare_var_s("long *[4]")
    ball.obj.lpp[1] = trick.TMM_declare_var_1d( "long", 5)
    trick.rti_add("ball.obj.lpp[1][1]", 45.1)

    #trick.rti_list()
    trick.rti_fire()
    trick_inject.rtis.GetRtiExecutor(0).Exec()

    TRICK_EXPECT_EQ( ball.obj.l , 40 , test_suite , "long" )
    TRICK_EXPECT_EQ( ball.obj.la[1] , 41 , test_suite , "long array" )
    TRICK_EXPECT_EQ( ball.obj.lp[1] , 42 , test_suite , "long polonger" )
    TRICK_EXPECT_EQ( ball.obj.laa[1][1] , 43 , test_suite , "2D long array" )
    TRICK_EXPECT_EQ( ball.obj.lap[1][1] , 44 , test_suite , "2D long array of ptr" )
    TRICK_EXPECT_EQ( ball.obj.lpp[1][1] , 45 , test_suite , "2D long ptr of ptr" )

######################################################################################################################

    ball.obj.ul = 0
    trick.rti_add("ball.obj.ul", 40.1)
    ball.obj.ula[1] = 0
    trick.rti_add("ball.obj.ula[1]", 41.1)
    ball.obj.ulp = trick.TMM_declare_var_s("unsigned long[6]")
    trick.rti_add("ball.obj.ulp[1]", 42.1)
    ball.obj.ulaa[1][1] = 0
    trick.rti_add("ball.obj.ulaa[1][1]", 43.1)
    ball.obj.ulap[1] = trick.TMM_declare_var_1d( "unsigned long", 4)
    trick.rti_add("ball.obj.ulap[1][1]", 44.1)
    ball.obj.ulpp = trick.TMM_declare_var_s("unsigned long *[4]")
    ball.obj.ulpp[1] = trick.TMM_declare_var_1d( "unsigned long", 5)
    trick.rti_add("ball.obj.ulpp[1][1]", 45.1)

    #trick.rti_list()
    trick.rti_fire()
    trick_inject.rtis.GetRtiExecutor(0).Exec()

    TRICK_EXPECT_EQ( ball.obj.ul , 40 , test_suite , "unsigned long" )
    TRICK_EXPECT_EQ( ball.obj.ula[1] , 41 , test_suite , "unsigned long array" )
    TRICK_EXPECT_EQ( ball.obj.ulp[1] , 42 , test_suite , "unsigned long pointer" )
    TRICK_EXPECT_EQ( ball.obj.ulaa[1][1] , 43 , test_suite , "2D unsigned long array" )
    TRICK_EXPECT_EQ( ball.obj.ulap[1][1] , 44 , test_suite , "2D unsigned long array of ptr" )
    TRICK_EXPECT_EQ( ball.obj.ulpp[1][1] , 45 , test_suite , "2D unsigned long ptr of ptr" )

######################################################################################################################

    ball.obj.ll = 0
    trick.rti_add("ball.obj.ll", 40.1)
    ball.obj.lla[1] = 0
    trick.rti_add("ball.obj.lla[1]", 41.1)
    ball.obj.llp = trick.TMM_declare_var_s("long long[6]")
    trick.rti_add("ball.obj.llp[1]", 42.1)
    ball.obj.llaa[1][1] = 0
    trick.rti_add("ball.obj.llaa[1][1]", 43.1)
    ball.obj.llap[1] = trick.TMM_declare_var_1d( "long long", 4)
    trick.rti_add("ball.obj.llap[1][1]", 44.1)
    ball.obj.llpp = trick.TMM_declare_var_s("long long *[4]")
    ball.obj.llpp[1] = trick.TMM_declare_var_1d( "long long", 5)
    trick.rti_add("ball.obj.llpp[1][1]", 45.1)

    #trick.rti_list()
    trick.rti_fire()
    trick_inject.rtis.GetRtiExecutor(0).Exec()

    TRICK_EXPECT_EQ( ball.obj.ll , 40 , test_suite , "long long" )
    TRICK_EXPECT_EQ( ball.obj.lla[1] , 41 , test_suite , "long long array" )
    TRICK_EXPECT_EQ( ball.obj.llp[1] , 42 , test_suite , "long long pointer" )
    TRICK_EXPECT_EQ( ball.obj.llaa[1][1] , 43 , test_suite , "2D long long array" )
    TRICK_EXPECT_EQ( ball.obj.llap[1][1] , 44 , test_suite , "2D long long array of ptr" )
    TRICK_EXPECT_EQ( ball.obj.llpp[1][1] , 45 , test_suite , "2D long long ptr of ptr" )

######################################################################################################################

    ball.obj.ull = 0
    trick.rti_add("ball.obj.ull", 40.1)
    ball.obj.ulla[1] = 0
    trick.rti_add("ball.obj.ulla[1]", 41.1)
    ball.obj.ullp = trick.TMM_declare_var_s("unsigned long long[6]")
    trick.rti_add("ball.obj.ullp[1]", 42.1)
    ball.obj.ullaa[1][1] = 0
    trick.rti_add("ball.obj.ullaa[1][1]", 43.1)
    ball.obj.ullap[1] = trick.TMM_declare_var_1d( "unsigned long long", 4)
    trick.rti_add("ball.obj.ullap[1][1]", 44.1)
    ball.obj.ullpp = trick.TMM_declare_var_s("unsigned long long *[4]")
    ball.obj.ullpp[1] = trick.TMM_declare_var_1d( "unsigned long long", 5)
    trick.rti_add("ball.obj.ullpp[1][1]", 45.1)

    #trick.rti_list()
    trick.rti_fire()
    trick_inject.rtis.GetRtiExecutor(0).Exec()

    TRICK_EXPECT_EQ( ball.obj.ull , 40 , test_suite , "unsigned long long" )
    TRICK_EXPECT_EQ( ball.obj.ulla[1] , 41 , test_suite , "unsigned long long array" )
    TRICK_EXPECT_EQ( ball.obj.ullp[1] , 42 , test_suite , "unsigned long long pointer" )
    TRICK_EXPECT_EQ( ball.obj.ullaa[1][1] , 43 , test_suite , "2D unsigned long long array" )
    TRICK_EXPECT_EQ( ball.obj.ullap[1][1] , 44 , test_suite , "2D unsigned long long array of ptr" )
    TRICK_EXPECT_EQ( ball.obj.ullpp[1][1] , 45 , test_suite , "2D unsigned long long ptr of ptr" )

######################################################################################################################

    ball.obj.e = 0
    trick.rti_add("ball.obj.e", 40.1)
    ball.obj.ea[1] = 0
    trick.rti_add("ball.obj.ea[1]", 41.1)
    ball.obj.ep = trick.TMM_declare_var_s("MY_ENUM[6]")
    trick.rti_add("ball.obj.ep[1]", 42.1)
    ball.obj.eaa[1][1] = 0
    trick.rti_add("ball.obj.eaa[1][1]", 43.1)
    ball.obj.eap[1] = trick.TMM_declare_var_1d( "MY_ENUM", 4)
    trick.rti_add("ball.obj.eap[1][1]", 44.1)
    ball.obj.epp = trick.TMM_declare_var_s("MY_ENUM *[4]")
    ball.obj.epp[1] = trick.TMM_declare_var_1d( "MY_ENUM", 5)
    trick.rti_add("ball.obj.epp[1][1]", 45.1)

    #trick.rti_list()
    trick.rti_fire()
    trick_inject.rtis.GetRtiExecutor(0).Exec()

    TRICK_EXPECT_EQ( ball.obj.e , 40 , test_suite , "enum" )
    TRICK_EXPECT_EQ( ball.obj.ea[1] , 41 , test_suite , "enum array" )
    TRICK_EXPECT_EQ( ball.obj.ep[1] , 42 , test_suite , "enum pointer" )
    TRICK_EXPECT_EQ( ball.obj.eaa[1][1] , 43 , test_suite , "2D enum array" )
    TRICK_EXPECT_EQ( ball.obj.eap[1][1] , 44 , test_suite , "2D enum array of ptr" )
    TRICK_EXPECT_EQ( ball.obj.epp[1][1] , 45 , test_suite , "2D enum ptr of ptr" )

######################################################################################################################

    ball.obj.b = 0
    trick.rti_add("ball.obj.b", 40)
    ball.obj.ba[1] = 0
    trick.rti_add("ball.obj.ba[1]", 41)
    ball.obj.bp = trick.TMM_declare_var_s("bool[6]")
    trick.rti_add("ball.obj.bp[1]", 42)
    ball.obj.baa[1][1] = 0
    trick.rti_add("ball.obj.baa[1][1]", 43)
    ball.obj.bap[1] = trick.TMM_declare_var_1d( "bool", 4)
    trick.rti_add("ball.obj.bap[1][1]", 44)
    ball.obj.bpp = trick.TMM_declare_var_s("bool *[4]")
    ball.obj.bpp[1] = trick.TMM_declare_var_1d( "bool", 5)
    trick.rti_add("ball.obj.bpp[1][1]", 45)

    #trick.rti_list()
    trick.rti_fire()
    trick_inject.rtis.GetRtiExecutor(0).Exec()

    TRICK_EXPECT_EQ( ball.obj.b , 1 , test_suite , "bool" )
    TRICK_EXPECT_EQ( ball.obj.ba[1] , 1 , test_suite , "bool array" )
    TRICK_EXPECT_EQ( ball.obj.bp[1] , 1 , test_suite , "bool pointer" )
    TRICK_EXPECT_EQ( ball.obj.baa[1][1] , 1 , test_suite , "2D bool array" )
    TRICK_EXPECT_EQ( ball.obj.bap[1][1] , 1 , test_suite , "2D bool array of ptr" )
    TRICK_EXPECT_EQ( ball.obj.bpp[1][1] , 1 , test_suite , "2D bool ptr of ptr" )

######################################################################################################################

    ball.obj.cbit_0 = 0
    trick.rti_add("ball.obj.cbit_0", -1)
    ball.obj.cbit_1 = 0
    trick.rti_add("ball.obj.cbit_1", 31)
    ball.obj.cbit_2 = 0
    trick.rti_add("ball.obj.cbit_2", 2)

    ball.obj.ucbit_0 = 0
    trick.rti_add("ball.obj.ucbit_0", 2)
    ball.obj.ucbit_1 = 0
    trick.rti_add("ball.obj.ucbit_1", 31)
    ball.obj.ucbit_2 = 0
    trick.rti_add("ball.obj.ucbit_2", -1)

    trick.rti_fire()
    trick_inject.rtis.GetRtiExecutor(0).Exec()

    # The raspberry pi has a bug and returns bitfields in unsigned format.
    if os.uname()[4] == 'armv61':
        TRICK_EXPECT_EQ( ball.obj.cbit_0 , 3 , test_suite , "char bitfield" )
        TRICK_EXPECT_EQ( ball.obj.cbit_1 , 7 , test_suite , "char bitfield" )
    else :
        TRICK_EXPECT_EQ( ball.obj.cbit_0 , -1 , test_suite , "char bitfield" )
        TRICK_EXPECT_EQ( ball.obj.cbit_1 , -1 , test_suite , "char bitfield" )
    TRICK_EXPECT_EQ( ball.obj.cbit_2 , 2 , test_suite , "char bitfield" )

    TRICK_EXPECT_EQ( ball.obj.ucbit_0 , 2 , test_suite , "unsigned char bitfield" )
    TRICK_EXPECT_EQ( ball.obj.ucbit_1 , 7 , test_suite , "unsigned char bitfield" )
    TRICK_EXPECT_EQ( ball.obj.ucbit_2 , 7 , test_suite , "unsigned char bitfield" )

######################################################################################################################

    ball.obj.sbit_0 = 0
    trick.rti_add("ball.obj.sbit_0", -1)
    ball.obj.sbit_1 = 0
    trick.rti_add("ball.obj.sbit_1", 255)
    ball.obj.sbit_2 = 0
    trick.rti_add("ball.obj.sbit_2", 2)

    ball.obj.usbit_0 = 0
    trick.rti_add("ball.obj.usbit_0", 2)
    ball.obj.usbit_1 = 0
    trick.rti_add("ball.obj.usbit_1", 255)
    ball.obj.usbit_2 = 0
    trick.rti_add("ball.obj.usbit_2", -1)

    trick.rti_fire()
    trick_inject.rtis.GetRtiExecutor(0).Exec()

    TRICK_EXPECT_EQ( ball.obj.sbit_0 , -1 , test_suite , "short bitfield" )
    TRICK_EXPECT_EQ( ball.obj.sbit_1 , -1 , test_suite , "short bitfield" )
    TRICK_EXPECT_EQ( ball.obj.sbit_2 , 2 , test_suite , "short bitfield" )

    TRICK_EXPECT_EQ( ball.obj.usbit_0 , 2 , test_suite , "unsigned short bitfield" )
    TRICK_EXPECT_EQ( ball.obj.usbit_1 , 31 , test_suite , "unsigned short bitfield" )
    TRICK_EXPECT_EQ( ball.obj.usbit_2 , 127 , test_suite , "unsigned short bitfield" )

######################################################################################################################

    ball.obj.bit_0 = 0
    trick.rti_add("ball.obj.bit_0", -1)
    ball.obj.bit_1 = 0
    trick.rti_add("ball.obj.bit_1", 255)
    ball.obj.bit_2 = 0
    trick.rti_add("ball.obj.bit_2", 2)

    ball.obj.ubit_0 = 0
    trick.rti_add("ball.obj.ubit_0", 2)
    ball.obj.ubit_1 = 0
    trick.rti_add("ball.obj.ubit_1", 255)
    ball.obj.ubit_2 = 0
    trick.rti_add("ball.obj.ubit_2", -1)

    trick.rti_fire()
    trick_inject.rtis.GetRtiExecutor(0).Exec()

    TRICK_EXPECT_EQ( ball.obj.bit_0 , -1 , test_suite , "int bitfield" )
    TRICK_EXPECT_EQ( ball.obj.bit_1 , -1 , test_suite , "int bitfield" )
    TRICK_EXPECT_EQ( ball.obj.bit_2 , 2 , test_suite , "int bitfield" )

    TRICK_EXPECT_EQ( ball.obj.ubit_0 , 2 , test_suite , "unsigned int bitfield" )
    TRICK_EXPECT_EQ( ball.obj.ubit_1 , 31 , test_suite , "unsigned int bitfield" )
    TRICK_EXPECT_EQ( ball.obj.ubit_2 , 63 , test_suite , "unsigned int bitfield" )

######################################################################################################################

    # long and long long bitfields are not currently supported by
    # sim_services/MemoryManager/src/insert_bitfield_any.c

    #ball.obj.lbit_0 = 0
    #trick.rti_add("ball.obj.lbit_0", -1)
    #ball.obj.lbit_1 = 0
    #trick.rti_add("ball.obj.lbit_1", 255)
    #ball.obj.lbit_2 = 0
    #trick.rti_add("ball.obj.lbit_2", 2)

    #ball.obj.ulbit_0 = 0
    #trick.rti_add("ball.obj.ulbit_0", 2)
    #ball.obj.ulbit_1 = 0
    #trick.rti_add("ball.obj.ulbit_1", 255)
    #ball.obj.ulbit_2 = 0
    #trick.rti_add("ball.obj.ulbit_2", -1)

    #trick.rti_fire()
    #trick_inject.rtis.GetRtiExecutor(0).Exec()

    #TRICK_EXPECT_EQ( ball.obj.lbit_0 , -1 , test_suite , "long bitfield" )
    #TRICK_EXPECT_EQ( ball.obj.lbit_1 , -1 , test_suite , "long bitfield" )
    #TRICK_EXPECT_EQ( ball.obj.lbit_2 , 2 , test_suite , "long bitfield" )

    #TRICK_EXPECT_EQ( ball.obj.ulbit_0 , 2 , test_suite , "unsigned long bitfield" )
    #TRICK_EXPECT_EQ( ball.obj.ulbit_1 , 31 , test_suite , "unsigned long bitfield" )
    #TRICK_EXPECT_EQ( ball.obj.ulbit_2 , 63 , test_suite , "unsigned long bitfield" )

######################################################################################################################

    ball.obj.boolbit_0 = 0
    trick.rti_add("ball.obj.boolbit_0", 2)
    ball.obj.boolbit_1 = 0
    trick.rti_add("ball.obj.boolbit_1", 1)
    ball.obj.boolbit_2 = 0
    trick.rti_add("ball.obj.boolbit_2", 255)

    trick.rti_fire()
    trick_inject.rtis.GetRtiExecutor(0).Exec()

    TRICK_EXPECT_EQ( ball.obj.boolbit_0 , 0 , test_suite , "bool bitfield" )
    TRICK_EXPECT_EQ( ball.obj.boolbit_1 , 1 , test_suite , "bool bitfield" )
    TRICK_EXPECT_EQ( ball.obj.boolbit_2 , 1 , test_suite , "bool bitfield" )

######################################################################################################################

if __name__ == "__main__":

    main()

