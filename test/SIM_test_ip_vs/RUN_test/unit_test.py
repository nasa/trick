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

        # --- Dynamic arrays: addData sets dp to [0.0, 1.0, 2.0, 3.0, 4.0] ---
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

        # --- Dynamic arrays: addData sets dap to [[0.0, 2.0], [0.0, 3.0, 6.0], [0.0, 4.0, 8.0, 12.0], [0.0, 5.0, 10.0, 15.0, 20.0]] ---
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

        # --- Dynamic arrays: addData sets dppp to a 5x5x5 array with values i*100 + j*10 + k ---
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

        # --- STL-element struct member: vec_user_simple is std::vector<UserClass>
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

        # --- STL-element struct member: vec_user_defined is std::vector<SimpleWrapper>
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
