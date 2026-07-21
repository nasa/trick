import os

from trick.unit_test import *

# Enable unit tests
trick_utest.unit_tests.enable()

# Set the XML file name to SIM_test_ip_vs.xml
xml_file_path = os.getenv("TRICK_HOME") + "/trick_test/SIM_test_ip_vs.xml"
trick_utest.unit_tests.set_file_name(xml_file_path)

VS_PORT = 7000


def run_vs_ip_tests():
    """Connect to the variable server as a client and verify variable values set by addData."""
    from trick.variable_server import Variable, VariableServer

    with VariableServer("localhost", VS_PORT) as vs:
        test_suite = "Dynamic array boundary tests"

        ### Test dynamic arrays enum
        # --- Color* color_p: addData sets color_p (5) to i % 3 ---
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.color_p[0]", type_=int),
            trick.Color_RED,
            test_suite,
            "color_p[0]",
        )
        print("Expected: the_object.vsip_test.color_p[5] could not be found.")
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.color_p[5]", type_=str),
            "BAD_REF",
            test_suite,
            "color_p[5]:BAD_REF",
        )
        # --- Color* color_ap[4]: addData sets color_ap (0x2, 1x3, 2x4, 3x5) to i % 3 ---
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.color_ap[1][1]", type_=int),
            trick.Color_GREEN,
            test_suite,
            "color_ap[1][1]",
        )
        print("Expected: the_object.vsip_test.color_ap[5][2] could not be found.")
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.color_ap[5][2]", type_=str),
            "BAD_REF",
            test_suite,
            "color_ap[5][2]:BAD_REF",
        )
        print("Expected: the_object.vsip_test.color_ap[3][5] could not be found.")
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.color_ap[3][5]", type_=str),
            "BAD_REF",
            test_suite,
            "color_ap[3][5]:BAD_REF",
        )
        # --- Color** color_pp: addData sets color_pp (3x3) to i % 3 ---
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.color_pp[1][2]", type_=int),
            trick.Color_BLUE,
            test_suite,
            "color_pp[1][2]",
        )
        print("Expected: the_object.vsip_test.color_pp[1][4] could not be found.")
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.color_pp[1][4]", type_=str),
            "BAD_REF",
            test_suite,
            "color_pp[1][4]:BAD_REF",
        )
        print("Expected: the_object.vsip_test.color_pp[4][2] could not be found.")
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.color_pp[4][2]", type_=str),
            "BAD_REF",
            test_suite,
            "color_pp[4][2]:BAD_REF",
        )
        # --- Color*** color_ppp: addData sets color_ppp (3x3x3) to i % 3 ---
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.color_ppp[0][0][0]", type_=int),
            trick.Color_RED,
            test_suite,
            "color_ppp[0][0][0]",
        )
        print("Expected: the_object.vsip_test.color_ppp[3][3][3] could not be found.")
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.color_ppp[3][3][3]", type_=str),
            "BAD_REF",
            test_suite,
            "color_ppp[3][3][3]:BAD_REF",
        )
        print("Expected: the_object.vsip_test.color_ppp[0][3][3] could not be found.")
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.color_ppp[0][3][3]", type_=str),
            "BAD_REF",
            test_suite,
            "color_ppp[0][3][3]:BAD_REF",
        )
        print("Expected: the_object.vsip_test.color_ppp[3][0][3] could not be found.")
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.color_ppp[3][0][3]", type_=str),
            "BAD_REF",
            test_suite,
            "color_ppp[3][0][3]:BAD_REF",
        )
        print("Expected: the_object.vsip_test.color_ppp[0][1][4] could not be found.")
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.color_ppp[0][1][4]", type_=str),
            "BAD_REF",
            test_suite,
            "color_ppp[0][1][4]:BAD_REF",
        )

        ### Test dynamic arrays short
        # --- short* sp: addData sets sp to [0, -1, -2, -3, -4] ---
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.sp[2]", type_=int),
            -2,
            test_suite,
            "sp[2]",
        )
        print("Expected: the_object.vsip_test.sp[5] could not be found.")
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.sp[5]", type_=str),
            "BAD_REF",
            test_suite,
            "sp[5]:BAD_REF",
        )
        # --- short* sap[4]: addData sets sap to [[0, -1], [0, 1, 2], [0, 1, 2, 3], [0, 1, 2, 3, 4]] ---
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.sap[0][1]", type_=int),
            -1,
            test_suite,
            "sap[0][1]",
        )
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.sap[1][2]", type_=int),
            2,
            test_suite,
            "sap[1][2]",
        )
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.sap[2][3]", type_=int),
            3,
            test_suite,
            "sap[2][3]",
        )
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.sap[3][4]", type_=int),
            4,
            test_suite,
            "sap[3][4]",
        )
        print("Expected: the_object.vsip_test.sap[4][0] could not be found.")
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.sap[4][0]", type_=str),
            "BAD_REF",
            test_suite,
            "sap[4][0]:BAD_REF",
        )
        print("Expected: the_object.vsip_test.sap[3][5] could not be found.")
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.sap[3][5]", type_=str),
            "BAD_REF",
            test_suite,
            "sap[3][5]:BAD_REF",
        )
        # --- short** spp: addData sets spp to [[0, 1, 2], [0, 1, 2], [0, 1, 2]] ---
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.spp[0][0]", type_=int),
            0,
            test_suite,
            "spp[0][0]",
        )
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.spp[1][1]", type_=int),
            1,
            test_suite,
            "spp[1][1]",
        )
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.spp[2][2]", type_=int),
            2,
            test_suite,
            "spp[2][2]",
        )
        print("Expected: the_object.vsip_test.spp[3][3] could not be found.")
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.spp[3][3]", type_=str),
            "BAD_REF",
            test_suite,
            "spp[3][3]:BAD_REF",
        )
        print("Expected: the_object.vsip_test.spp[0][3] could not be found.")
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.spp[0][3]", type_=str),
            "BAD_REF",
            test_suite,
            "spp[0][3]:BAD_REF",
        )
        print("Expected: the_object.vsip_test.spp[3][0] could not be found.")
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.spp[3][0]", type_=str),
            "BAD_REF",
            test_suite,
            "spp[3][0]:BAD_REF",
        )
        # --- short*** sppp: addData sets sppp to [[[0, 1], [0, 1]], [[0, 1], [0, 1]]] ---
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.sppp[0][0][0]", type_=int),
            0,
            test_suite,
            "sppp[0][0][0]",
        )
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.sppp[0][1][1]", type_=int),
            1,
            test_suite,
            "sppp[0][1][1]",
        )
        print("Expected: the_object.vsip_test.sppp[0][2][2] could not be found.")
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.sppp[0][2][2]", type_=str),
            "BAD_REF",
            test_suite,
            "sppp[0][2][2]:BAD_REF",
        )
        print("Expected: the_object.vsip_test.sppp[3][0][0] could not be found.")
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.sppp[3][0][0]", type_=str),
            "BAD_REF",
            test_suite,
            "sppp[3][0][0]:BAD_REF",
        )
        print("Expected: the_object.vsip_test.sppp[3][3][3] could not be found.")
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.sppp[3][3][3]", type_=str),
            "BAD_REF",
            test_suite,
            "sppp[3][3][3]:BAD_REF",
        )
        print("Expected: the_object.vsip_test.sapp[0][3][0] could not be found.")
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.sapp[0][3][0]", type_=str),
            "BAD_REF",
            test_suite,
            "sapp[0][3][0]:BAD_REF",
        )
        print("Expected: the_object.vsip_test.sapp[3][0][3] could not be found.")
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.sapp[3][0][3]", type_=str),
            "BAD_REF",
            test_suite,
            "sapp[3][0][3]:BAD_REF",
        )
        print("Expected: the_object.vsip_test.sapp[0][0][3] could not be found.")
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.sapp[0][0][3]", type_=str),
            "BAD_REF",
            test_suite,
            "sapp[0][0][3]:BAD_REF",
        )

        ### Test dynamic arrays unsigned short
        # --- unsigned short* usp: addData sets usp to [0, 1, 2, 3, 4]
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.usp[3]", type_=int),
            3,
            test_suite,
            "usp[3]",
        )
        print("Expected: the_object.vsip_test.usp[5] could not be found.")
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.usp[5]", type_=str),
            "BAD_REF",
            test_suite,
            "usp[5]:BAD_REF",
        )
        # --- unsigned short* usap[4]: addData sets usap [[0, 1], [0, 1, 2], [0, 1, 2, 3], [0, 1, 2, 3, 4]]
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.usap[3][2]", type_=int),
            2,
            test_suite,
            "usap[3][2]",
        )
        print("Expected: the_object.vsip_test.usap[0][2] could not be found.")
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.usap[0][2]", type_=str),
            "BAD_REF",
            test_suite,
            "usap[0][2]:BAD_REF",
        )
        print("Expected: the_object.vsip_test.usap[1][3] could not be found.")
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.usap[1][3]", type_=str),
            "BAD_REF",
            test_suite,
            "usap[1][3]:BAD_REF",
        )
        print("Expected: the_object.vsip_test.usap[2][4] could not be found.")
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.usap[2][4]", type_=str),
            "BAD_REF",
            test_suite,
            "usap[2][4]:BAD_REF",
        )
        print("Expected: the_object.vsip_test.usap[3][5] could not be found.")
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.usap[3][5]", type_=str),
            "BAD_REF",
            test_suite,
            "usap[3][5]:BAD_REF",
        )
        print("Expected: the_object.vsip_test.usap[4][1] could not be found.")
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.usap[4][1]", type_=str),
            "BAD_REF",
            test_suite,
            "usap[4][1]:BAD_REF",
        )
        print("Expected: the_object.vsip_test.usap[5][1] could not be found.")
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.usap[5][1]", type_=str),
            "BAD_REF",
            test_suite,
            "usap[5][1]:BAD_REF",
        )
        # --- unsigned short** uspp: addData sets uspp to [[0, 1, 2], [0, 1, 2], [0, 1, 2]]
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.uspp[2][1]", type_=int),
            1,
            test_suite,
            "uspp[2][1]",
        )
        print("Expected: the_object.vsip_test.uspp[3][0] could not be found.")
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.uspp[3][0]", type_=str),
            "BAD_REF",
            test_suite,
            "uspp[3][0]:BAD_REF",
        )
        print("Expected: the_object.vsip_test.uspp[1][3] could not be found.")
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.uspp[1][3]", type_=str),
            "BAD_REF",
            test_suite,
            "uspp[1][3]:BAD_REF",
        )
        print("Expected: the_object.vsip_test.uspp[3][3] could not be found.")
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.uspp[3][3]", type_=str),
            "BAD_REF",
            test_suite,
            "uspp[3][3]:BAD_REF",
        )
        # --- unsigned short*** usppp: addData sets usppp to [[[0, 1], [0, 1]], [[0, 2], [0, 2]]] ---
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.usppp[0][1][0]", type_=int),
            0,
            test_suite,
            "usppp[0][1][0]",
        )
        print("Expected: the_object.vsip_test.usppp[0][0][2] could not be found.")
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.usppp[0][0][2]", type_=str),
            "BAD_REF",
            test_suite,
            "usppp[0][0][2]:BAD_REF",
        )
        print("Expected: the_object.vsip_test.usppp[2][0][1] could not be found.")
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.usppp[2][0][1]", type_=str),
            "BAD_REF",
            test_suite,
            "usppp[2][0][1]:BAD_REF",
        )
        print("Expected: the_object.vsip_test.usppp[1][2][0] could not be found.")
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.usppp[1][2][0]", type_=str),
            "BAD_REF",
            test_suite,
            "usppp[1][2][0]:BAD_REF",
        )
        print("Expected: the_object.vsip_test.usppp[3][2][2] could not be found.")
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.usppp[3][2][2]", type_=str),
            "BAD_REF",
            test_suite,
            "usppp[3][2][2]:BAD_REF",
        )

        ### Test dynamic arrays int
        # --- int* ip: addData sets ip to [0, -1, -2, -3, -4] ---
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.ip[2]", type_=int),
            -2,
            test_suite,
            "ip[2]",
        )
        print("Expected: the_object.vsip_test.ip[5] could not be found.")
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.ip[5]", type_=str),
            "BAD_REF",
            test_suite,
            "ip[5]:BAD_REF",
        )
        # --- int* iap[4]: addData sets iap to [[0, -1], [0, 1, 2], [0, 1, 2, 3], [0, 1, 2, 3, 4]] ---
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.iap[0][1]", type_=int),
            -1,
            test_suite,
            "iap[0][1]",
        )
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.iap[1][2]", type_=int),
            2,
            test_suite,
            "iap[1][2]",
        )
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.iap[2][3]", type_=int),
            3,
            test_suite,
            "iap[2][3]",
        )
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.iap[3][4]", type_=int),
            4,
            test_suite,
            "iap[3][4]",
        )
        print("Expected: the_object.vsip_test.iap[4][0] could not be found.")
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.iap[4][0]", type_=str),
            "BAD_REF",
            test_suite,
            "iap[4][0]:BAD_REF",
        )
        print("Expected: the_object.vsip_test.iap[3][5] could not be found.")
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.iap[3][5]", type_=str),
            "BAD_REF",
            test_suite,
            "iap[3][5]:BAD_REF",
        )
        # --- int** ipp: addData sets ipp to [[0, 1, 2], [0, 1, 2], [0, 1, 2]] ---
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.ipp[0][0]", type_=int),
            0,
            test_suite,
            "ipp[0][0]",
        )
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.ipp[1][1]", type_=int),
            1,
            test_suite,
            "ipp[1][1]",
        )
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.ipp[2][2]", type_=int),
            2,
            test_suite,
            "ipp[2][2]",
        )
        print("Expected: the_object.vsip_test.ipp[3][3] could not be found.")
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.ipp[3][3]", type_=str),
            "BAD_REF",
            test_suite,
            "ipp[3][3]:BAD_REF",
        )
        print("Expected: the_object.vsip_test.ipp[0][3] could not be found.")
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.ipp[0][3]", type_=str),
            "BAD_REF",
            test_suite,
            "ipp[0][3]:BAD_REF",
        )
        print("Expected: the_object.vsip_test.ipp[3][0] could not be found.")
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.ipp[3][0]", type_=str),
            "BAD_REF",
            test_suite,
            "ipp[3][0]:BAD_REF",
        )
        # --- int*** ippp: addData sets ippp to [[[0, 1], [0, 1]], [[0, 1], [0, 1]]] ---
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.ippp[0][0][0]", type_=int),
            0,
            test_suite,
            "ippp[0][0][0]",
        )
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.ippp[0][1][1]", type_=int),
            1,
            test_suite,
            "ippp[0][1][1]",
        )
        print("Expected: the_object.vsip_test.ippp[0][2][2] could not be found.")
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.ippp[0][2][2]", type_=str),
            "BAD_REF",
            test_suite,
            "ippp[0][2][2]:BAD_REF",
        )
        print("Expected: the_object.vsip_test.ippp[3][0][0] could not be found.")
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.ippp[3][0][0]", type_=str),
            "BAD_REF",
            test_suite,
            "ippp[3][0][0]:BAD_REF",
        )
        print("Expected: the_object.vsip_test.ippp[3][3][3] could not be found.")
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.ippp[3][3][3]", type_=str),
            "BAD_REF",
            test_suite,
            "ippp[3][3][3]:BAD_REF",
        )
        print("Expected: the_object.vsip_test.iapp[0][3][0] could not be found.")
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.iapp[0][3][0]", type_=str),
            "BAD_REF",
            test_suite,
            "iapp[0][3][0]:BAD_REF",
        )
        print("Expected: the_object.vsip_test.iapp[3][0][3] could not be found.")
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.iapp[3][0][3]", type_=str),
            "BAD_REF",
            test_suite,
            "iapp[3][0][3]:BAD_REF",
        )
        print("Expected: the_object.vsip_test.iapp[0][0][3] could not be found.")
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.iapp[0][0][3]", type_=str),
            "BAD_REF",
            test_suite,
            "iapp[0][0][3]:BAD_REF",
        )

        ### Test dynamic arrays unsigned int
        # --- unsigned int* uip: addData sets uip to [0, 1, 2, 3, 4]
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.uip[3]", type_=int),
            3,
            test_suite,
            "uip[3]",
        )
        print("Expected: the_object.vsip_test.uip[5] could not be found.")
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.uip[5]", type_=str),
            "BAD_REF",
            test_suite,
            "uip[5]:BAD_REF",
        )
        # --- unsigned int* uiap[4]: addData sets uiap [[0, 1], [0, 1, 2], [0, 1, 2, 3], [0, 1, 2, 3, 4]]
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.uiap[3][2]", type_=int),
            2,
            test_suite,
            "uiap[3][2]",
        )
        print("Expected: the_object.vsip_test.uiap[0][2] could not be found.")
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.uiap[0][2]", type_=str),
            "BAD_REF",
            test_suite,
            "uiap[0][2]:BAD_REF",
        )
        print("Expected: the_object.vsip_test.uiap[1][3] could not be found.")
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.uiap[1][3]", type_=str),
            "BAD_REF",
            test_suite,
            "uiap[1][3]:BAD_REF",
        )
        print("Expected: the_object.vsip_test.uiap[2][4] could not be found.")
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.uiap[2][4]", type_=str),
            "BAD_REF",
            test_suite,
            "uiap[2][4]:BAD_REF",
        )
        print("Expected: the_object.vsip_test.uiap[3][5] could not be found.")
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.uiap[3][5]", type_=str),
            "BAD_REF",
            test_suite,
            "uiap[3][5]:BAD_REF",
        )
        print("Expected: the_object.vsip_test.uiap[4][1] could not be found.")
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.uiap[4][1]", type_=str),
            "BAD_REF",
            test_suite,
            "uiap[4][1]:BAD_REF",
        )
        print("Expected: the_object.vsip_test.uiap[5][1] could not be found.")
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.uiap[5][1]", type_=str),
            "BAD_REF",
            test_suite,
            "uiap[5][1]:BAD_REF",
        )
        # --- unsigned int** uipp: addData sets uipp to [[0, 1, 2], [0, 1, 2], [0, 1, 2]]
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.uipp[2][1]", type_=int),
            1,
            test_suite,
            "uipp[2][1]",
        )
        print("Expected: the_object.vsip_test.uipp[3][0] could not be found.")
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.uipp[3][0]", type_=str),
            "BAD_REF",
            test_suite,
            "uipp[3][0]:BAD_REF",
        )
        print("Expected: the_object.vsip_test.uipp[1][3] could not be found.")
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.uipp[1][3]", type_=str),
            "BAD_REF",
            test_suite,
            "uipp[1][3]:BAD_REF",
        )
        print("Expected: the_object.vsip_test.uipp[3][3] could not be found.")
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.uipp[3][3]", type_=str),
            "BAD_REF",
            test_suite,
            "uipp[3][3]:BAD_REF",
        )
        # --- unsigned int*** uippp: addData sets uippp to [[[0, 1], [0, 1]], [[0, 2], [0, 2]]] ---
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.uippp[0][1][0]", type_=int),
            0,
            test_suite,
            "uippp[0][1][0]",
        )
        print("Expected: the_object.vsip_test.uippp[0][0][2] could not be found.")
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.uippp[0][0][2]", type_=str),
            "BAD_REF",
            test_suite,
            "uippp[0][0][2]:BAD_REF",
        )
        print("Expected: the_object.vsip_test.uippp[2][0][1] could not be found.")
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.uippp[2][0][1]", type_=str),
            "BAD_REF",
            test_suite,
            "uippp[2][0][1]:BAD_REF",
        )
        print("Expected: the_object.vsip_test.uippp[1][2][0] could not be found.")
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.uippp[1][2][0]", type_=str),
            "BAD_REF",
            test_suite,
            "uippp[1][2][0]:BAD_REF",
        )
        print("Expected: the_object.vsip_test.uippp[3][2][2] could not be found.")
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.uippp[3][2][2]", type_=str),
            "BAD_REF",
            test_suite,
            "uippp[3][2][2]:BAD_REF",
        )

        ### Test dynamic arrays long
        # --- long* lp: addData sets lp to [0, -1, -2, -3, -4] ---
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.lp[2]", type_=int),
            -2,
            test_suite,
            "lp[2]",
        )
        print("Expected: the_object.vsip_test.lp[5] could not be found.")
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.lp[5]", type_=str),
            "BAD_REF",
            test_suite,
            "lp[5]:BAD_REF",
        )
        # --- long* lap[4]: addData sets lap to [[0, -1], [0, 1, 2], [0, 1, 2, 3], [0, 1, 2, 3, 4]] ---
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.lap[0][1]", type_=int),
            -1,
            test_suite,
            "lap[0][1]",
        )
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.lap[1][2]", type_=int),
            2,
            test_suite,
            "lap[1][2]",
        )
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.lap[2][3]", type_=int),
            3,
            test_suite,
            "lap[2][3]",
        )
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.lap[3][4]", type_=int),
            4,
            test_suite,
            "lap[3][4]",
        )
        print("Expected: the_object.vsip_test.lap[4][0] could not be found.")
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.lap[4][0]", type_=str),
            "BAD_REF",
            test_suite,
            "lap[4][0]:BAD_REF",
        )
        print("Expected: the_object.vsip_test.lap[3][5] could not be found.")
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.lap[3][5]", type_=str),
            "BAD_REF",
            test_suite,
            "lap[3][5]:BAD_REF",
        )
        # --- long** lpp: addData sets lpp to [[0, 1, 2], [0, 1, 2], [0, 1, 2]] ---
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.lpp[0][0]", type_=int),
            0,
            test_suite,
            "lpp[0][0]",
        )
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.lpp[1][1]", type_=int),
            1,
            test_suite,
            "lpp[1][1]",
        )
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.lpp[2][2]", type_=int),
            2,
            test_suite,
            "lpp[2][2]",
        )
        print("Expected: the_object.vsip_test.lpp[3][3] could not be found.")
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.lpp[3][3]", type_=str),
            "BAD_REF",
            test_suite,
            "lpp[3][3]:BAD_REF",
        )
        print("Expected: the_object.vsip_test.lpp[0][3] could not be found.")
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.lpp[0][3]", type_=str),
            "BAD_REF",
            test_suite,
            "lpp[0][3]:BAD_REF",
        )
        print("Expected: the_object.vsip_test.lpp[3][0] could not be found.")
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.lpp[3][0]", type_=str),
            "BAD_REF",
            test_suite,
            "lpp[3][0]:BAD_REF",
        )
        # --- long*** lppp: addData sets lppp to [[[0, 1], [0, 1]], [[0, 1], [0, 1]]] ---
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.lppp[0][0][0]", type_=int),
            0,
            test_suite,
            "lppp[0][0][0]",
        )
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.lppp[0][1][1]", type_=int),
            1,
            test_suite,
            "lppp[0][1][1]",
        )
        print("Expected: the_object.vsip_test.lppp[0][2][2] could not be found.")
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.lppp[0][2][2]", type_=str),
            "BAD_REF",
            test_suite,
            "lppp[0][2][2]:BAD_REF",
        )
        print("Expected: the_object.vsip_test.lppp[3][0][0] could not be found.")
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.lppp[3][0][0]", type_=str),
            "BAD_REF",
            test_suite,
            "lppp[3][0][0]:BAD_REF",
        )
        print("Expected: the_object.vsip_test.lppp[3][3][3] could not be found.")
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.lppp[3][3][3]", type_=str),
            "BAD_REF",
            test_suite,
            "lppp[3][3][3]:BAD_REF",
        )

        ### Test dynamic arrays unsigned long
        # --- unsigned long* ulp: addData sets ulp to [0, 1, 2, 3, 4]
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.ulp[3]", type_=int),
            3,
            test_suite,
            "ulp[3]",
        )
        print("Expected: the_object.vsip_test.ulp[5] could not be found.")
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.ulp[5]", type_=str),
            "BAD_REF",
            test_suite,
            "ulp[5]:BAD_REF",
        )
        # --- unsigned long* ulap[4]: addData sets ulap [[0, 1], [0, 1, 2], [0, 1, 2, 3], [0, 1, 2, 3, 4]]
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.ulap[3][2]", type_=int),
            2,
            test_suite,
            "ulap[3][2]",
        )
        print("Expected: the_object.vsip_test.ulap[0][2] could not be found.")
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.ulap[0][2]", type_=str),
            "BAD_REF",
            test_suite,
            "ulap[0][2]:BAD_REF",
        )
        print("Expected: the_object.vsip_test.ulap[1][3] could not be found.")
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.ulap[1][3]", type_=str),
            "BAD_REF",
            test_suite,
            "ulap[1][3]:BAD_REF",
        )
        print("Expected: the_object.vsip_test.ulap[2][4] could not be found.")
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.ulap[2][4]", type_=str),
            "BAD_REF",
            test_suite,
            "ulap[2][4]:BAD_REF",
        )
        print("Expected: the_object.vsip_test.ulap[3][5] could not be found.")
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.ulap[3][5]", type_=str),
            "BAD_REF",
            test_suite,
            "ulap[3][5]:BAD_REF",
        )
        print("Expected: the_object.vsip_test.ulap[4][1] could not be found.")
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.ulap[4][1]", type_=str),
            "BAD_REF",
            test_suite,
            "ulap[4][1]:BAD_REF",
        )
        print("Expected: the_object.vsip_test.ulap[5][1] could not be found.")
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.ulap[5][1]", type_=str),
            "BAD_REF",
            test_suite,
            "ulap[5][1]:BAD_REF",
        )
        # --- unsigned long** ulpp: addData sets ulpp to [[0, 1, 2], [0, 1, 2], [0, 1, 2]]
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.ulpp[2][1]", type_=int),
            1,
            test_suite,
            "ulpp[2][1]",
        )
        print("Expected: the_object.vsip_test.ulpp[3][0] could not be found.")
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.ulpp[3][0]", type_=str),
            "BAD_REF",
            test_suite,
            "ulpp[3][0]:BAD_REF",
        )
        print("Expected: the_object.vsip_test.ulpp[1][3] could not be found.")
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.ulpp[1][3]", type_=str),
            "BAD_REF",
            test_suite,
            "ulpp[1][3]:BAD_REF",
        )
        print("Expected: the_object.vsip_test.ulpp[3][3] could not be found.")
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.ulpp[3][3]", type_=str),
            "BAD_REF",
            test_suite,
            "ulpp[3][3]:BAD_REF",
        )
        # --- unsigned long*** ulppp: addData sets ulppp to [[[0, 1], [0, 1]], [[0, 2], [0, 2]]] ---
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.ulppp[0][1][0]", type_=int),
            0,
            test_suite,
            "ulppp[0][1][0]",
        )
        print("Expected: the_object.vsip_test.ulppp[0][0][2] could not be found.")
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.ulppp[0][0][2]", type_=str),
            "BAD_REF",
            test_suite,
            "ulppp[0][0][2]:BAD_REF",
        )
        print("Expected: the_object.vsip_test.ulppp[2][0][1] could not be found.")
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.ulppp[2][0][1]", type_=str),
            "BAD_REF",
            test_suite,
            "ulppp[2][0][1]:BAD_REF",
        )
        print("Expected: the_object.vsip_test.ulppp[1][2][0] could not be found.")
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.ulppp[1][2][0]", type_=str),
            "BAD_REF",
            test_suite,
            "ulppp[1][2][0]:BAD_REF",
        )
        print("Expected: the_object.vsip_test.ulppp[3][2][2] could not be found.")
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.ulppp[3][2][2]", type_=str),
            "BAD_REF",
            test_suite,
            "ulppp[3][2][2]:BAD_REF",
        )

        ### Test long long
        # --- long long* llp: addData sets llp to [0, -1, -2, -3, -4] ---
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.llp[2]", type_=int),
            -2,
            test_suite,
            "llp[2]",
        )
        print("Expected: the_object.vsip_test.llp[5] could not be found.")
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.llp[5]", type_=str),
            "BAD_REF",
            test_suite,
            "llp[5]:BAD_REF",
        )
        # --- long long* llap[4]: addData sets llap to [[0, -1], [0, 1, 2], [0, 1, 2, 3], [0, 1, 2, 3, 4]] ---
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.llap[0][1]", type_=int),
            -1,
            test_suite,
            "llap[0][1]",
        )
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.llap[1][2]", type_=int),
            2,
            test_suite,
            "llap[1][2]",
        )
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.llap[2][3]", type_=int),
            3,
            test_suite,
            "llap[2][3]",
        )
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.llap[3][4]", type_=int),
            4,
            test_suite,
            "llap[3][4]",
        )
        print("Expected: the_object.vsip_test.llap[4][0] could not be found.")
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.llap[4][0]", type_=str),
            "BAD_REF",
            test_suite,
            "llap[4][0]:BAD_REF",
        )
        print("Expected: the_object.vsip_test.llap[3][5] could not be found.")
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.llap[3][5]", type_=str),
            "BAD_REF",
            test_suite,
            "llap[3][5]:BAD_REF",
        )
        # --- long long** llpp: addData sets llpp to [[0, 1, 2], [0, 1, 2], [0, 1, 2]] ---
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.llpp[0][0]", type_=int),
            0,
            test_suite,
            "llpp[0][0]",
        )
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.llpp[1][1]", type_=int),
            1,
            test_suite,
            "llpp[1][1]",
        )
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.llpp[2][2]", type_=int),
            2,
            test_suite,
            "llpp[2][2]",
        )
        print("Expected: the_object.vsip_test.llpp[3][3] could not be found.")
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.llpp[3][3]", type_=str),
            "BAD_REF",
            test_suite,
            "llpp[3][3]:BAD_REF",
        )
        print("Expected: the_object.vsip_test.llpp[0][3] could not be found.")
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.llpp[0][3]", type_=str),
            "BAD_REF",
            test_suite,
            "llpp[0][3]:BAD_REF",
        )
        print("Expected: the_object.vsip_test.llpp[3][0] could not be found.")
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.llpp[3][0]", type_=str),
            "BAD_REF",
            test_suite,
            "llpp[3][0]:BAD_REF",
        )
        # --- long long*** llppp: addData sets llppp to [[[0, 1], [0, 1]], [[0, 1], [0, 1]]] ---
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.llppp[0][0][0]", type_=int),
            0,
            test_suite,
            "llppp[0][0][0]",
        )
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.llppp[0][1][1]", type_=int),
            1,
            test_suite,
            "llppp[0][1][1]",
        )
        print("Expected: the_object.vsip_test.llppp[0][2][2] could not be found.")
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.llppp[0][2][2]", type_=str),
            "BAD_REF",
            test_suite,
            "llppp[0][2][2]:BAD_REF",
        )
        print("Expected: the_object.vsip_test.llppp[3][0][0] could not be found.")
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.llppp[3][0][0]", type_=str),
            "BAD_REF",
            test_suite,
            "llppp[3][0][0]:BAD_REF",
        )
        print("Expected: the_object.vsip_test.llppp[3][3][3] could not be found.")
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.llppp[3][3][3]", type_=str),
            "BAD_REF",
            test_suite,
            "llppp[3][3][3]:BAD_REF",
        )
        print("Expected: the_object.vsip_test.llppp[0][3][0] could not be found.")
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.llppp[0][3][0]", type_=str),
            "BAD_REF",
            test_suite,
            "llppp[0][3][0]:BAD_REF",
        )
        print("Expected: the_object.vsip_test.llppp[3][0][3] could not be found.")
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.llppp[3][0][3]", type_=str),
            "BAD_REF",
            test_suite,
            "llppp[3][0][3]:BAD_REF",
        )
        print("Expected: the_object.vsip_test.llppp[0][0][3] could not be found.")
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.llppp[0][0][3]", type_=str),
            "BAD_REF",
            test_suite,
            "llppp[0][0][3]:BAD_REF",
        )

        ### Test dynamic arrays unsigned long long
        # --- unsigned long* ullp: addData sets ullp to [0, 1, 2, 3, 4]
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.ullp[3]", type_=int),
            3,
            test_suite,
            "ullp[3]",
        )
        print("Expected: the_object.vsip_test.ullp[5] could not be found.")
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.ullp[5]", type_=str),
            "BAD_REF",
            test_suite,
            "ullp[5]:BAD_REF",
        )
        # --- unsigned long* ullap[4]: addData sets ullap [[0, 1], [0, 1, 2], [0, 1, 2, 3], [0, 1, 2, 3, 4]]
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.ullap[3][2]", type_=int),
            2,
            test_suite,
            "ullap[3][2]",
        )
        print("Expected: the_object.vsip_test.ullap[0][2] could not be found.")
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.ullap[0][2]", type_=str),
            "BAD_REF",
            test_suite,
            "ullap[0][2]:BAD_REF",
        )
        print("Expected: the_object.vsip_test.ullap[1][3] could not be found.")
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.ullap[1][3]", type_=str),
            "BAD_REF",
            test_suite,
            "ullap[1][3]:BAD_REF",
        )
        print("Expected: the_object.vsip_test.ullap[2][4] could not be found.")
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.ullap[2][4]", type_=str),
            "BAD_REF",
            test_suite,
            "ullap[2][4]:BAD_REF",
        )
        print("Expected: the_object.vsip_test.ullap[3][5] could not be found.")
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.ullap[3][5]", type_=str),
            "BAD_REF",
            test_suite,
            "ullap[3][5]:BAD_REF",
        )
        print("Expected: the_object.vsip_test.ullap[4][1] could not be found.")
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.ullap[4][1]", type_=str),
            "BAD_REF",
            test_suite,
            "ullap[4][1]:BAD_REF",
        )
        print("Expected: the_object.vsip_test.ullap[5][1] could not be found.")
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.ullap[5][1]", type_=str),
            "BAD_REF",
            test_suite,
            "ullap[5][1]:BAD_REF",
        )
        # --- unsigned long** ullpp: addData sets ullpp to [[0, 1, 2], [0, 1, 2], [0, 1, 2]]
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.ullap[2][1]", type_=int),
            1,
            test_suite,
            "ullpp[2][1]",
        )
        print("Expected: the_object.vsip_test.ullpp[3][0] could not be found.")
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.ullpp[3][0]", type_=str),
            "BAD_REF",
            test_suite,
            "ullpp[3][0]:BAD_REF",
        )
        print("Expected: the_object.vsip_test.ullpp[1][3] could not be found.")
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.ullpp[1][3]", type_=str),
            "BAD_REF",
            test_suite,
            "ullpp[1][3]:BAD_REF",
        )
        print("Expected: the_object.vsip_test.ullpp[3][3] could not be found.")
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.ullpp[3][3]", type_=str),
            "BAD_REF",
            test_suite,
            "ullpp[3][3]:BAD_REF",
        )
        # --- unsigned long*** ullppp: addData sets ullppp to [[[0, 1], [0, 1]], [[0, 2], [0, 2]]] ---
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.ullppp[0][1][0]", type_=int),
            0,
            test_suite,
            "ullppp[0][1][0]",
        )
        print("Expected: the_object.vsip_test.ullppp[0][0][2] could not be found.")
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.ullppp[0][0][2]", type_=str),
            "BAD_REF",
            test_suite,
            "ullppp[0][0][2]:BAD_REF",
        )
        print("Expected: the_object.vsip_test.ullppp[2][0][1] could not be found.")
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.ullppp[2][0][1]", type_=str),
            "BAD_REF",
            test_suite,
            "ullppp[2][0][1]:BAD_REF",
        )
        print("Expected: the_object.vsip_test.ullppp[1][2][0] could not be found.")
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.ullppp[1][2][0]", type_=str),
            "BAD_REF",
            test_suite,
            "ullppp[1][2][0]:BAD_REF",
        )
        print("Expected: the_object.vsip_test.ullppp[3][2][2] could not be found.")
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.ullppp[3][2][2]", type_=str),
            "BAD_REF",
            test_suite,
            "ullppp[3][2][2]:BAD_REF",
        )

        ### Test dynamic arrays float
        # --- float* fp: addData sets fp to [0.0, 1.0, 2.0, 3.0, 4.0] ---
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.fp[0]", type_=float),
            0.0,
            test_suite,
            "fp[0]",
        )
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.fp[1]", type_=float),
            1.0,
            test_suite,
            "fp[1]",
        )
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.fp[2]", type_=float),
            2.0,
            test_suite,
            "fp[2]",
        )
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.fp[3]", type_=float),
            3.0,
            test_suite,
            "fp[3]",
        )
        print("Expected: the_object.vsip_test.fp[5] could not be found.")
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.fp[5]", type_=str),
            "BAD_REF",
            test_suite,
            "fp[5]:BAD_REF",
        )
        # --- float* fap[4]: addData sets fap to [[0.0, 2.0], [0.0, 3.0, 6.0], [0.0, 4.0, 8.0, 12.0], [0.0, 5.0, 10.0, 15.0, 20.0]] ---
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.fap[0][0]", type_=float),
            0.0,
            test_suite,
            "fap[0][0]",
        )
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.fap[1][2]", type_=float),
            (2.0 * 3.0),
            test_suite,
            "fap[1][2]",
        )
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.fap[2][3]", type_=float),
            (3.0 * 4.0),
            test_suite,
            "fap[2][3]",
        )
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.fap[3][4]", type_=float),
            (4.0 * 5.0),
            test_suite,
            "fap[3][4]",
        )
        print("Expected: the_object.vsip_test.fap[4][0] could not be found.")
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.fap[4][0]", type_=str),
            "BAD_REF",
            test_suite,
            "fap[4][0]:BAD_REF",
        )
        print("Expected: the_object.vsip_test.fap[0][5] could not be found.")
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.fap[0][5]", type_=str),
            "BAD_REF",
            test_suite,
            "fap[0][5]:BAD_REF",
        )
        print("Expected: the_object.vsip_test.fap[3][5] could not be found.")
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.fap[3][5]", type_=str),
            "BAD_REF",
            test_suite,
            "fap[3][5]:BAD_REF",
        )
        # --- float** fpp: addData sets fpp to a 2x2 array with values i*1.0 + j ---
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.fpp[0][0]", type_=float),
            (0 * 10.0 + 0),
            test_suite,
            "fpp[0][0]",
        )
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.fpp[1][1]", type_=float),
            (1 * 1.0 + 1),
            test_suite,
            "fpp[1][1]",
        )
        print("Expected: the_object.vsip_test.fpp[2][0] could not be found.")
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.fpp[2][0]", type_=str),
            "BAD_REF",
            test_suite,
            "fpp[2][0]:BAD_REF",
        )
        print("Expected: the_object.vsip_test.fpp[0][2] could not be found.")
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.fpp[0][2]", type_=str),
            "BAD_REF",
            test_suite,
            "fpp[0][2]:BAD_REF",
        )
        # --- float*** fppp: addData sets fppp to a 2x2x2 array with values i*1.0 + j + k ---
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.fppp[0][0][0]", type_=float),
            0.0,
            test_suite,
            "fppp[0][0][0]",
        )
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.fppp[0][0][1]", type_=float),
            (0 * 1.0 + 0 * 1.0 + 1),
            test_suite,
            "fppp[0][0][1]",
        )
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.fppp[0][1][0]", type_=float),
            (0 * 1.0 + 1 * 1.0 + 0),
            test_suite,
            "fppp[0][1][0]",
        )
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.fppp[1][0][0]", type_=float),
            (1 * 1.0 + 0 * 1.0 + 0),
            test_suite,
            "fppp[1][0][0]",
        )
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.fppp[1][0][1]", type_=float),
            (1 * 1.0 + 0 * 1.0 + 1),
            test_suite,
            "fppp[1][0][1]",
        )
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.fppp[1][1][0]", type_=float),
            (1 * 1.0 + 1 * 1.0 + 0),
            test_suite,
            "fppp[1][1][0]",
        )
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.fppp[1][1][1]", type_=float),
            (1 * 1.0 + 1 * 1.0 + 1),
            test_suite,
            "fppp[1][1][1]",
        )
        print("Expected: the_object.vsip_test.fppp[2][0][0] could not be found.")
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.fppp[2][0][0]", type_=str),
            "BAD_REF",
            test_suite,
            "fppp[2][0][0]:BAD_REF",
        )
        print("Expected: the_object.vsip_test.fppp[0][2][0] could not be found.")
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.fppp[0][2][0]", type_=str),
            "BAD_REF",
            test_suite,
            "fppp[0][2][0]:BAD_REF",
        )
        print("Expected: the_object.vsip_test.fppp[0][0][2] could not be found.")
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.fppp[0][0][2]", type_=str),
            "BAD_REF",
            test_suite,
            "fppp[0][0][2]:BAD_REF",
        )

        ### Test dynamic arrays double
        # --- double* dp: addData sets dp to [0.0, 1.0, 2.0, 3.0, 4.0] ---
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.dp[0]", type_=float),
            0.0,
            test_suite,
            "dp[0]",
        )
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.dp[1]", type_=float),
            1.0,
            test_suite,
            "dp[1]",
        )
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.dp[2]", type_=float),
            2.0,
            test_suite,
            "dp[2]",
        )
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.dp[3]", type_=float),
            3.0,
            test_suite,
            "dp[3]",
        )
        print("Expected: the_object.vsip_test.dp[5] could not be found.")
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.dp[5]", type_=str),
            "BAD_REF",
            test_suite,
            "dp[5]:BAD_REF",
        )
        # --- double* dap[4]: addData sets dap to [[0.0, 2.0], [0.0, 3.0, 6.0], [0.0, 4.0, 8.0, 12.0], [0.0, 5.0, 10.0, 15.0, 20.0]] ---
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.dap[0][0]", type_=float),
            0.0,
            test_suite,
            "dap[0][0]",
        )
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.dap[1][2]", type_=float),
            (2.0 * 3.0),
            test_suite,
            "dap[1][2]",
        )
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.dap[2][3]", type_=float),
            (3.0 * 4.0),
            test_suite,
            "dap[2][3]",
        )
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.dap[3][4]", type_=float),
            (4.0 * 5.0),
            test_suite,
            "dap[3][4]",
        )
        print("Expected: the_object.vsip_test.dap[4][0] could not be found.")
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.dap[4][0]", type_=str),
            "BAD_REF",
            test_suite,
            "dap[4][0]:BAD_REF",
        )
        print("Expected: the_object.vsip_test.dap[0][5] could not be found.")
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.dap[0][5]", type_=str),
            "BAD_REF",
            test_suite,
            "dap[0][5]:BAD_REF",
        )
        print("Expected: the_object.vsip_test.dap[3][5] could not be found.")
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.dap[3][5]", type_=str),
            "BAD_REF",
            test_suite,
            "dap[3][5]:BAD_REF",
        )
        # --- double** dpp: addData sets dpp to a 5x5 array with values i*10 + j ---
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.dpp[0][0]", type_=float),
            (0 * 10.0 + 0),
            test_suite,
            "dpp[0][0]",
        )
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.dpp[4][4]", type_=float),
            (4 * 10.0 + 4),
            test_suite,
            "dpp[4][4]",
        )
        print("Expected: the_object.vsip_test.dpp[5][0] could not be found.")
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.dpp[5][0]", type_=str),
            "BAD_REF",
            test_suite,
            "dpp[5][0]:BAD_REF",
        )
        print("Expected: the_object.vsip_test.dpp[0][5] could not be found.")
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.dpp[0][5]", type_=str),
            "BAD_REF",
            test_suite,
            "dpp[0][5]:BAD_REF",
        )
        # --- double*** dppp: addData sets dppp to a 5x5x5 array with values i*100 + j*10 + k ---
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.dppp[0][0][0]", type_=float),
            0.0,
            test_suite,
            "dppp[0][0][0]",
        )
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.dppp[0][0][1]", type_=float),
            (0 * 100.0 + 0 * 10.0 + 1),
            test_suite,
            "dppp[0][0][1]",
        )
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.dppp[0][1][0]", type_=float),
            (0 * 100.0 + 1 * 10.0 + 0),
            test_suite,
            "dppp[0][1][0]",
        )
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.dppp[1][0][0]", type_=float),
            (1 * 100.0 + 0 * 10.0 + 0),
            test_suite,
            "dppp[1][0][0]",
        )
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.dppp[1][0][1]", type_=float),
            (1 * 100.0 + 0 * 10.0 + 1),
            test_suite,
            "dppp[1][0][1]",
        )
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.dppp[1][1][0]", type_=float),
            (1 * 100.0 + 1 * 10.0 + 0),
            test_suite,
            "dppp[1][1][0]",
        )
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.dppp[1][1][1]", type_=float),
            (1 * 100.0 + 1 * 10.0 + 1),
            test_suite,
            "dppp[1][1][1]",
        )
        print("Expected: the_object.vsip_test.dppp[5][0][0] could not be found.")
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.dppp[5][0][0]", type_=str),
            "BAD_REF",
            test_suite,
            "dppp[5][0][0]:BAD_REF",
        )
        print("Expected: the_object.vsip_test.dppp[0][5][0] could not be found.")
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.dppp[0][5][0]", type_=str),
            "BAD_REF",
            test_suite,
            "dppp[0][5][0]:BAD_REF",
        )
        print("Expected: the_object.vsip_test.dppp[0][0][5] could not be found.")
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.dppp[0][0][5]", type_=str),
            "BAD_REF",
            test_suite,
            "dppp[0][0][5]:BAD_REF",
        )

        ### Test dynamic arrays bool
        # --- bool* bp: addData sets bp to an array of 5 bool values for true (even) and false (odd) ---
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.bp[0]", type_=int),
            True,
            test_suite,
            "bp[0]",
        )
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.bp[1]", type_=int),
            False,
            test_suite,
            "bp[1]",
        )
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.bp[2]", type_=int),
            True,
            test_suite,
            "bp[2]",
        )
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.bp[3]", type_=int),
            False,
            test_suite,
            "bp[3]",
        )
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.bp[4]", type_=int),
            True,
            test_suite,
            "bp[4]",
        )
        # --- bool* bap[4]: addData sets bap to an array of 4 bool* arrays ---
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.bap[0][0]", type_=int),
            True,
            test_suite,
            "bap[0][0]",
        )
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.bap[2][1]", type_=int),
            False,
            test_suite,
            "bap[2][1]",
        )
        print("Expected: the_object.vsip_test.bap[4][1] could not be found.")
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.bap[4][1]", type_=str),
            "BAD_REF",
            test_suite,
            "bap[4][1]:BAD_REF",
        )
        print("Expected: the_object.vsip_test.bap[2][4] could not be found.")
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.bap[2][4]", type_=str),
            "BAD_REF",
            test_suite,
            "bap[2][4]:BAD_REF",
        )
        # --- bool** bpp: addData sets bpp to a 5x5 array of bool values for true (even) and false (odd) ---
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.bpp[0][0]", type_=int),
            True,
            test_suite,
            "bpp[0][0]",
        )
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.bpp[1][1]", type_=int),
            False,
            test_suite,
            "bpp[1][1]",
        )
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.bpp[2][2]", type_=int),
            True,
            test_suite,
            "bpp[2][2]",
        )
        print("Expected: the_object.vsip_test.bpp[5][0] could not be found.")
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.bpp[5][0]", type_=str),
            "BAD_REF",
            test_suite,
            "bpp[5][0]:BAD_REF",
        )
        print("Expected: the_object.vsip_test.bpp[0][5] could not be found.")
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.bpp[0][5]", type_=str),
            "BAD_REF",
            test_suite,
            "bpp[0][5]:BAD_REF",
        )
        print("Expected: the_object.vsip_test.bpp[5][5] could not be found.")
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.bpp[5][5]", type_=str),
            "BAD_REF",
            test_suite,
            "bpp[5][5]:BAD_REF",
        )
        # --- bool *** bppp: addData sets bppp to a 5x5x5 array of bool values for true (even) and false (odd) ---
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.bppp[0][0][0]", type_=int),
            True,
            test_suite,
            "bppp[0][0][0]",
        )
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.bppp[1][1][1]", type_=int),
            False,
            test_suite,
            "bppp[1][1][1]",
        )
        print("Expected: the_object.vsip_test.bppp[5][0][0] could not be found.")
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.bppp[5][0][0]", type_=str),
            "BAD_REF",
            test_suite,
            "bppp[5][0][0]:BAD_REF",
        )
        print("Expected: the_object.vsip_test.bppp[0][5][0] could not be found.")
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.bppp[0][5][0]", type_=str),
            "BAD_REF",
            test_suite,
            "bppp[0][5][0]:BAD_REF",
        )
        print("Expected: the_object.vsip_test.bppp[0][0][5] could not be found.")
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.bppp[0][0][5]", type_=str),
            "BAD_REF",
            test_suite,
            "bppp[0][0][5]:BAD_REF",
        )
        print("Expected: the_object.vsip_test.bppp[5][5][5] could not be found.")
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.bppp[5][5][5]", type_=str),
            "BAD_REF",
            test_suite,
            "bppp[5][5][5]:BAD_REF",
        )

        ### Test dynamic arrays SimpleWrapper*
        # --- SimpleWrapper* swp: addData sets swp to an array of 5 SimpleWrapper objects with a=555 and vec containing 0-9 ---
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.swp[0][0].a", type_=int),
            555,
            test_suite,
            "swp[0][0].a",
        )
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.swp[0][0].vec[0]", type_=int),
            0,
            test_suite,
            "swp[0][0].vec[0]",
        )
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.swp[0][0].vec[9]", type_=int),
            9,
            test_suite,
            "swp[0][0].vec[9]",
        )
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.swp[4][0].vec[3]", type_=int),
            3,
            test_suite,
            "swp[4][0].vec[3]",
        )
        print("Expected: the_object.vsip_test.swp[0][0].vec[10] could not be found.")
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.swp[0][0].vec[10]", type_=str),
            "BAD_REF",
            test_suite,
            "swp[0][0].vec[10]:BAD_REF",
        )
        print("Expected: the_object.vsip_test.swp[5][0].a could not be found.")
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.swp[5][0].a", type_=str),
            "BAD_REF",
            test_suite,
            "swp[5][0].a:BAD_REF",
        )

        ### Test dynamic arrays SimpleWrapper**
        # --- SimpleWrapper** swpp: addData sets swpp to a 6x7 array of SimpleWrapper objects with a=666 and vec containing 0-9 ---
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.swpp[0][0][0].a", type_=int),
            666,
            test_suite,
            "swpp[0][0][0].a",
        )
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.swpp[0][0][0].vec[0]", type_=int),
            0,
            test_suite,
            "swpp[0][0][0].vec[0]",
        )
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.swpp[0][0][0].vec[9]", type_=int),
            9,
            test_suite,
            "swpp[0][0].vec[9]",
        )
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.swpp[5][6][0].a", type_=int),
            666,
            test_suite,
            "swpp[5][6][0].a",
        )
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.swpp[5][6][0].vec[9]", type_=int),
            9,
            test_suite,
            "swpp[5][6][0].vec[9]",
        )
        print("Expected: the_object.vsip_test.swpp[0][0].vec[10] could not be found.")
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.swpp[0][0][0].vec[10]", type_=str),
            "BAD_REF",
            test_suite,
            "swpp[0][0][0].vec[10]:BAD_REF",
        )
        print("Expected: the_object.vsip_test.swpp[5][7][0].a could not be found.")
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.swpp[5][7][0].a", type_=str),
            "BAD_REF",
            test_suite,
            "swpp[5][7][0].a:BAD_REF",
        )
        print("Expected: the_object.vsip_test.swpp[6][0][0].a could not be found.")
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.swpp[6][0][0].a", type_=str),
            "BAD_REF",
            test_suite,
            "swpp[6][0][0].a:BAD_REF",
        )



        # ------------------------------------------------------- #

        test_suite = "STL boundary tests"

        ### Test STL container: std::vector<enum>
        # --- std::vector<enum>: addData pushes Color::RED, Color::GREEN, Color::BLUE ---
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.color_vector[0]", type_=int),
            trick.Color_RED,
            test_suite,
            "color_vector[0]",
        )
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.color_vector[1]", type_=int),
            trick.Color_GREEN,
            test_suite,
            "color_vector[1]",
        )
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.color_vector[2]", type_=int),
            trick.Color_BLUE,
            test_suite,
            "color_vector[2]",
        )
        print("Expected: the_object.vsip_test.color_vector[3] could not be found.")
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.color_vector[3]", type_=str),
            "BAD_REF",
            test_suite,
            "color_vector[3]:BAD_REF",
        )

        ### Test STL container: std::vector<short>
        # --- std::vector<short>: addData pushes -1, 2, 3 ---
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.short_vector[0]", type_=int),
            -1,
            test_suite,
            "short_vector[0]",
        )
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.short_vector[1]", type_=int),
            2,
            test_suite,
            "short_vector[1]",
        )
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.short_vector[2]", type_=int),
            3,
            test_suite,
            "short_vector[2]",
        )
        print("Expected: the_object.vsip_test.short_vector[3] could not be found.")
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.short_vector[3]", type_=str),
            "BAD_REF",
            test_suite,
            "short_vector[3]:BAD_REF",
        )

        ### Test STL container: std::vector<unsigned short>
        # --- std::vector<unsigned short>: addData pushes 4, 5, 6 ---
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.unsigned_short_vector[0]", type_=int),
            4,
            test_suite,
            "unsigned_short_vector[0]",
        )
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.unsigned_short_vector[1]", type_=int),
            5,
            test_suite,
            "unsigned_short_vector[1]",
        )
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.unsigned_short_vector[2]", type_=int),
            6,
            test_suite,
            "unsigned_short_vector[2]",
        )
        print("Expected: the_object.vsip_test.unsigned_short_vector[3] could not be found.")
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.unsigned_short_vector[3]", type_=str),
            "BAD_REF",
            test_suite,
            "unsigned_short_vector[3]:BAD_REF",
        )

        ### Test STL container: std::vector<int>
        # --- std::vector<int>: addData pushes -7, 8, 9 ---
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.int_vector[0]", type_=int),
            -7,
            test_suite,
            "int_vector[0]",
        )
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.int_vector[1]", type_=int),
            8,
            test_suite,
            "int_vector[1]",
        )
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.int_vector[2]", type_=int),
            9,
            test_suite,
            "int_vector[2]",
        )
        print("Expected: the_object.vsip_test.int_vector[3] could not be found.")
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.int_vector[3]", type_=str),
            "BAD_REF",
            test_suite,
            "int_vector[3]:BAD_REF",
        )

        ### Test STL container: std::vector<unsigned int>
        # --- std::vector<unsigned int>: addData pushes 10, 11, 12 ---
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.unsigned_int_vector[0]", type_=int),
            10,
            test_suite,
            "unsigned_int_vector[0]",
        )
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.unsigned_int_vector[1]", type_=int),
            11,
            test_suite,
            "unsigned_int_vector[1]",
        )
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.unsigned_int_vector[2]", type_=int),
            12,
            test_suite,
            "unsigned_int_vector[2]",
        )
        print("Expected: the_object.vsip_test.unsigned_int_vector[3] could not be found.")
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.unsigned_int_vector[3]", type_=str),
            "BAD_REF",
            test_suite,
            "unsigned_int_vector[3]:BAD_REF",
        )

        ### Test STL container: std::vector<long>
        # --- std::vector<long>: addData pushes -13, 14, 15 ---
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.long_vector[0]", type_=int),
            -13,
            test_suite,
            "long_vector[0]",
        )
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.long_vector[1]", type_=int),
            14,
            test_suite,
            "long_vector[1]",
        )
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.long_vector[2]", type_=int),
            15,
            test_suite,
            "long_vector[2]",
        )
        print("Expected: the_object.vsip_test.long_vector[3] could not be found.")
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.long_vector[3]", type_=str),
            "BAD_REF",
            test_suite,
            "long_vector[3]:BAD_REF",
        )

        ### Test STL container: std::vector<unsigned long>
        # --- std::vector<unsigned long>: addData pushes 16, 17, 18 ---
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.unsigned_long_vector[0]", type_=int),
            16,
            test_suite,
            "unsigned_long_vector[0]",
        )
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.unsigned_long_vector[1]", type_=int),
            17,
            test_suite,
            "unsigned_long_vector[1]",
        )
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.unsigned_long_vector[2]", type_=int),
            18,
            test_suite,
            "unsigned_long_vector[2]",
        )
        print("Expected: the_object.vsip_test.unsigned_long_vector[3] could not be found.")
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.unsigned_long_vector[3]", type_=str),
            "BAD_REF",
            test_suite,
            "unsigned_long_vector[3]:BAD_REF",
        )

        ### Test STL container: std::vector<long long>
        # --- std::vector<long long>: addData pushes -19, 20, 21 ---
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.long_long_vector[0]", type_=int),
            -19,
            test_suite,
            "long_long_vector[0]",
        )
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.long_long_vector[1]", type_=int),
            20,
            test_suite,
            "long_long_vector[1]",
        )
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.long_long_vector[2]", type_=int),
            21,
            test_suite,
            "long_long_vector[2]",
        )
        print("Expected: the_object.vsip_test.long_long_vector[3] could not be found.")
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.long_long_vector[3]", type_=str),
            "BAD_REF",
            test_suite,
            "long_long_vector[3]:BAD_REF",
        )

        ### Test STL container: std::vector<unsigned long long>
        # --- std::vector<unsigned long long>: addData pushes 22, 23, 24 ---
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.unsigned_long_long_vector[0]", type_=int),
            22,
            test_suite,
            "unsigned_long_long_vector[0]",
        )
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.unsigned_long_long_vector[1]", type_=int),
            23,
            test_suite,
            "unsigned_long_long_vector[1]",
        )
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.unsigned_long_long_vector[2]", type_=int),
            24,
            test_suite,
            "unsigned_long_long_vector[2]",
        )
        print("Expected: the_object.vsip_test.unsigned_long_long_vector[3] could not be found.")
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.unsigned_long_long_vector[3]", type_=str),
            "BAD_REF",
            test_suite,
            "unsigned_long_long_vector[3]:BAD_REF",
        )

        ### Test STL container: std::vector<float>
        # --- std::vector<float>: addData pushes 25.0, 26.0, 27.0 ---
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.float_vector[0]", type_=float),
            25.0,
            test_suite,
            "float_vector[0]",
        )
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.float_vector[1]", type_=float),
            26.0,
            test_suite,
            "float_vector[1]",
        )
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.float_vector[2]", type_=float),
            27.0,
            test_suite,
            "float_vector[2]",
        )
        print("Expected: the_object.vsip_test.float_vector[3] could not be found.")
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.float_vector[3]", type_=str),
            "BAD_REF",
            test_suite,
            "float_vector[3]:BAD_REF",
        )

        ### Test STL container: std::vector<double>
        # --- std::vector<double>: addData pushes 4.0, 5.0, 6.0 ---
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.double_vector[0]", type_=float),
            4.0,
            test_suite,
            "double_vector[0]",
        )
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.double_vector[1]", type_=float),
            5.0,
            test_suite,
            "double_vector[1]",
        )
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.double_vector[2]", type_=float),
            6.0,
            test_suite,
            "double_vector[2]",
        )
        print("Expected: the_object.vsip_test.double_vector[3] could not be found.")
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.double_vector[3]", type_=str),
            "BAD_REF",
            test_suite,
            "double_vector[3]:BAD_REF",
        )

        # --- Use get_values to fetch multiple variables at once ---
        v0 = Variable("the_object.vsip_test.double_vector[0]", type_=float)
        v1 = Variable("the_object.vsip_test.double_vector[1]", type_=float)
        v2 = Variable("the_object.vsip_test.double_vector[2]", type_=float)
        TRICK_EXPECT_EQ(
            vs.get_values(v0, v1, v2),
            [4.0, 5.0, 6.0],
            test_suite,
            "get_values: double_vector",
        )

        ### Test STL container: std::vector<bool>
        # --- std::vector<bool>: addData pushes true, false, true ---
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.bool_vector[0]", type_=int),
            True,
            test_suite,
            "bool_vector[0]",
        )
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.bool_vector[1]", type_=int),
            False,
            test_suite,
            "bool_vector[1]",
        )
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.bool_vector[2]", type_=int),
            True,
            test_suite,
            "bool_vector[2]",
        )
        print("Expected: the_object.vsip_test.bool_vector[3] could not be found.")
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.bool_vector[3]", type_=str),
            "BAD_REF",
            test_suite,
            "bool_vector[3]:BAD_REF",
        )

        ### Test STL container: std::vector<std::string>
        # --- std::vector<std::string>: addData pushes "It", "has", "the", "Wi-Fies " ---
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.string_vector[0]", type_=str),
            "It",
            test_suite,
            "string_vector[0]",
        )
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.string_vector[1]", type_=str),
            "has",
            test_suite,
            "string_vector[1]",
        )
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.string_vector[2]", type_=str),
            "the",
            test_suite,
            "string_vector[2]",
        )
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.string_vector[3]", type_=str),
            "Wi-Fies",
            test_suite,
            "string_vector[3]",
        )
        print("Expected: the_object.vsip_test.string_vector[4] could not be found.")
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.string_vector[4]", type_=str),
            "BAD_REF",
            test_suite,
            "string_vector[4]:BAD_REF",
        )


        ### Test STL container: std::deque<float>
        # --- std::deque<float>: addData pushes 98.7, 65.4, 32.1 ---
        TRICK_EXPECT_NEAR(
            vs.get_value("the_object.vsip_test.float_deque[0]", type_=float),
            98.7,
            1e-4,
            test_suite,
            "float_deque[0]",
        )
        TRICK_EXPECT_NEAR(
            vs.get_value("the_object.vsip_test.float_deque[1]", type_=float),
            65.4,
            1e-4,
            test_suite,
            "float_deque[1]",
        )
        TRICK_EXPECT_NEAR(
            vs.get_value("the_object.vsip_test.float_deque[2]", type_=float),
            32.1,
            1e-4,
            test_suite,
            "float_deque[2]",
        )
        print("Expected: the_object.vsip_test.float_deque[3] could not be found.")
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.float_deque[3]", type_=str),
            "BAD_REF",
            test_suite,
            "float_deque[3]:BAD_REF",
        )

        ### Test STL container: std::deque<std::string>
        # --- std:deque<std::string>: addData pushes "Welcome", "to", "PhoneMart" ---
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.string_deque[0]", type_=str),
            "Welcome",
            test_suite,
            "string_deque[0]",
        )
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.string_deque[1]", type_=str),
            "to",
            test_suite,
            "string_deque[1]",
        )
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.string_deque[2]", type_=str),
            "PhoneMart",
            test_suite,
            "string_deque[2]",
        )
        print("Expected: the_object.vsip_test.string_deque[3] could not be found.")
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.string_deque[3]", type_=str),
            "BAD_REF",
            test_suite,
            "string_deque[3]:BAD_REF",
        )


        ### Test STL container: std::array<int,4>
        # --- std::array<int,4>: addData sets 10, 20, 30, 40 ---
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.int_array[0]", type_=int),
            10,
            test_suite,
            "int_array[0]",
        )
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.int_array[1]", type_=int),
            20,
            test_suite,
            "int_array[1]",
        )
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.int_array[2]", type_=int),
            30,
            test_suite,
            "int_array[2]",
        )
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.int_array[3]", type_=int),
            40,
            test_suite,
            "int_array[3]",
        )
        print("Expected: the_object.vsip_test.int_array[4] could not be found.")
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.int_array[4]", type_=str),
            "BAD_REF",
            test_suite,
            "int_array[4]:BAD_REF",
        )

        # --- Use get_values to fetch multiple variables at once ---
        int_arr = [
            Variable("the_object.vsip_test.int_array[{0}]".format(i), type_=int)
            for i in range(4)
        ]
        arr_vals = vs.get_values(*int_arr)
        TRICK_EXPECT_EQ(arr_vals, [10, 20, 30, 40], test_suite, "int_array")

        ### Test STL container: std::array<std::string,5>
        # --- std::array<std::string,5>: addData sets "Alpha", "Beta", "Gamma", "Delta", "Epsilon" ---
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.string_array[0]", type_=str),
            "Alpha",
            test_suite,
            "string_array[0]",
        )
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.string_array[1]", type_=str),
            "Beta",
            test_suite,
            "string_array[1]",
        )
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.string_array[2]", type_=str),
            "Gamma",
            test_suite,
            "string_array[2]",
        )
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.string_array[3]", type_=str),
            "Delta",
            test_suite,
            "string_array[3]",
        )
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.string_array[4]", type_=str),
            "Epsilon",
            test_suite,
            "string_array[4]",
        )
        print("Expected: the_object.vsip_test.string_array[5] could not be  found.")
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.string_array[5]", type_=str),
            "BAD_REF",
            test_suite,
            "string_array[5]:BAD_REF",
        )


        ### Test STL-element struct member: vec_user_simple is std::vector<UserClass>
        # size of vec_user_simple is 10 starting from 0
        # size of vec_user_simple[i].a is 5 starting from 0
        # vec_user_simple[i].a[j] = i+j, vec_user_simple[i].b = 8888888888
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.vec_user_simple[0].b", type_=int),
            8888888888,
            test_suite,
            "vec_user_simple[0].b",
        )
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.vec_user_simple[0].a[0]", type_=int),
            0,
            test_suite,
            "vec_user_simple[0].a[0]",
        )
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.vec_user_simple[0].a[2]", type_=int),
            2,
            test_suite,
            "vec_user_simple[0].a[2]",
        )
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.vec_user_simple[1].a[4]", type_=int),
            5,
            test_suite,
            "vec_user_simple[1].a[4]",
        )
        TRICK_EXPECT_EQ(
            vs.get_value(
                "the_object.vsip_test.vec_user_simple[0].d[0].a[0]", type_=int
            ),
            0,
            test_suite,
            "vec_user_simple[0].d.a[0]",
        )
        TRICK_EXPECT_EQ(
            vs.get_value(
                "the_object.vsip_test.vec_user_simple[2].d[0].a[2]", type_=int
            ),
            4,
            test_suite,
            "vec_user_simple[2].d[0].a[2]",
        )
        print(
            "Expected: the_object.vsip_test.vec_user_simple[0].d[0].a[5] could not be found."
        )
        TRICK_EXPECT_EQ(
            vs.get_value(
                "the_object.vsip_test.vec_user_simple[0].d[0].a[5]", type_=str
            ),
            "BAD_REF",
            test_suite,
            "vec_user_simple[0].d[0].a[5]:BAD_REF",
        )
        print(
            "Expected: the_object.vsip_test.vec_user_simple[1].d[11].a[2] could not be found."
        )
        TRICK_EXPECT_EQ(
            vs.get_value(
                "the_object.vsip_test.vec_user_simple[1].d[11].a[2]", type_=str
            ),
            "BAD_REF",
            test_suite,
            "vec_user_simple[1].d[11].a[2]:BAD_REF",
        )
        print(
            "Expected: the_object.vsip_test.vec_user_simple[10].d[3].a[2] could not be found."
        )
        TRICK_EXPECT_EQ(
            vs.get_value(
                "the_object.vsip_test.vec_user_simple[10].d[3].a[2]", type_=str
            ),
            "BAD_REF",
            test_suite,
            "vec_user_simple[10].d[3].a[2]:BAD_REF",
        )
        print(
            "Expected: the_object.vsip_test.vec_user_simple[0].a[5] could not be found."
        )
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.vec_user_simple[0].a[5]", type_=str),
            "BAD_REF",
            test_suite,
            "vec_user_simple[0].a[5]:BAD_REF",
        )
        print(
            "Expected: the_object.vsip_test.vec_user_simple[10].a[2] could not be found."
        )
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.vec_user_simple[10].a[2]", type_=str),
            "BAD_REF",
            test_suite,
            "vec_user_simple[10].a[2]:BAD_REF",
        )


        ### Test STL-element struct member: vec_user_defined is std::vector<SimpleWrapper>
        # vec_user_defined[i].a = 888, vec_user_defined[i].vec[j] = i+j  (10 elements each)
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.vec_user_defined[0].a", type_=int),
            888,
            test_suite,
            "vec_user_defined[0].a",
        )
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.vec_user_defined[1].a", type_=int),
            888,
            test_suite,
            "vec_user_defined[1].a",
        )
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.vec_user_defined[2].vec[5]", type_=int),
            (2 + 5),
            test_suite,
            "vec_user_defined[2].vec[5]",
        )
        print(
            "Expected: the_object.vsip_test.vec_user_defined[12].a could not be found."
        )
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.vec_user_defined[12].a", type_=str),
            "BAD_REF",
            test_suite,
            "vec_user_defined[12].a:BAD_REF",
        )
        print(
            "Expected: the_object.vsip_test.vec_user_defined[0].vec[10] could not be found."
        )
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.vec_user_defined[0].vec[10]", type_=str),
            "BAD_REF",
            test_suite,
            "vec_user_defined[0].vec[10]:BAD_REF",
        )
        print(
            "Expected: the_object.vsip_test.vec_user_defined[10].vec[11] could not be found."
        )
        TRICK_EXPECT_EQ(
            vs.get_value(
                "the_object.vsip_test.vec_user_defined[10].vec[11]", type_=str
            ),
            "BAD_REF",
            test_suite,
            "vec_user_defined[10].vec[11]:BAD_REF",
        )


        ### Test STL-element struct member: vec_user_defined_ptr is std::vector<SimpleWrapper*>
        # vec_user_defined_ptr[i]->a = 888, vec_user_defined_ptr[i]->vec[j] = i+j  (10 elements each)
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.vec_user_defined_ptr[0].a", type_=int),
            888,
            test_suite,
            "vec_user_defined_ptr[0].a",
        )
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.vec_user_defined_ptr[1].a", type_=int),
            888,
            test_suite,
            "vec_user_defined_ptr[1].a",
        )
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.vec_user_defined_ptr[2].vec[5]", type_=int),
            (2 + 5),
            test_suite,
            "vec_user_defined_ptr[2].vec[5]",
        )
        print(
            "Expected: the_object.vsip_test.vec_user_defined_ptr[12].a could not be found."
        )
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.vec_user_defined_ptr[12].a", type_=str),
            "BAD_REF",
            test_suite,
            "vec_user_defined_ptr[12].a:BAD_REF",
        )
        print(
            "Expected: the_object.vsip_test.vec_user_defined_ptr[0].vec[10] could not be found."
        )
        TRICK_EXPECT_EQ(
            vs.get_value("the_object.vsip_test.vec_user_defined_ptr[0].vec[10]", type_=str),
            "BAD_REF",
            test_suite,
            "vec_user_defined_ptr[0].vec[10]:BAD_REF",
        )
        print(
            "Expected: the_object.vsip_test.vec_user_defined_ptr[10].vec[11] could not be found."
        )
        TRICK_EXPECT_EQ(
            vs.get_value(
                "the_object.vsip_test.vec_user_defined_ptr[10].vec[11]", type_=str
            ),
            "BAD_REF",
            test_suite,
            "vec_user_defined_ptr[10].vec[11]:BAD_REF",
        )


        ### Test get_stl_size() for various STL containers
        # -- Check the size of the vector using get_stl_size() ---
        TRICK_EXPECT_EQ(
            vs.get_stl_size("the_object.vsip_test.double_vector"),
            3,
            test_suite,
            "double_vector size check",
        )
        TRICK_EXPECT_EQ(
            vs.get_stl_size("the_object.vsip_test.float_deque"),
            3,
            test_suite,
            "float_deque size check",
        )
        TRICK_EXPECT_EQ(
            vs.get_stl_size("the_object.vsip_test.int_array"),
            4,
            test_suite,
            "int_array size check",
        )
        TRICK_EXPECT_EQ(
            vs.get_stl_size("the_object.vsip_test.vec_user_simple"),
            10,
            test_suite,
            "vec_user_simple size check",
        )
        TRICK_EXPECT_EQ(
            vs.get_stl_size("the_object.vsip_test.vec_user_defined"),
            10,
            test_suite,
            "vec_user_defined size check",
        )
        TRICK_EXPECT_EQ(
            vs.get_stl_size("the_object.vsip_test.vec_user_defined[0].vec"),
            10,
            test_suite,
            "vec_user_defined[0].vec size check",
        )
        try:
            vs.get_stl_size("the_object.vsip_test.vec_user_defined[11].vec")
        except Exception as e:
            print(
                "Expected: the_object.vsip_test.vec_user_defined[11].vec out of bound."
            )
            TRICK_EXPECT_EQ(
                str(e),
                "invalid literal for int() with base 10: 'BAD_REF'",
                test_suite,
                "vec_user_defined[11].vec size check: out of bound",
            )

    print("VSIP tests PASSED.")


def main():
    trick.exec_set_job_onoff("the_object.vsip_test.addData", 1, True)

    trick.var_allow_connections()
    trick.var_server_set_port(VS_PORT)

    trick.exec_set_freeze_frame(0.10)
    trick.stop(1.0)

    # Run VS IP tests at t=0.1
    # addData is an initialization job so data is already populated for testing at 0.1
    trick.add_read(0.1, "run_vs_ip_tests()")


if __name__ == "__main__":
    main()
