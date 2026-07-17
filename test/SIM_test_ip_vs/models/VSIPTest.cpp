#include "VSIPTest.hh"

#include "trick/memorymanager_c_intf.h"

#include <array>
#include <deque>
#include <iostream>
#include <vector>

int VSIPTest::addData()
{
    std::cout << "Adding hardcoded data for testing" << std::endl;

    // Dynamic arrays for testing
    dp = (double*)TMM_declare_var_s("double[5]");
    for (int i = 0; i < 5; i++)
    {
        dp[i] = i * 1.0;
    }

    dap[0] = (double*)TMM_declare_var_s("double[2]");
    for (int i = 0; i < 2; i++)
    {
        dap[0][i] = i * 2.0;
    }
    dap[1] = (double*)TMM_declare_var_s("double[3]");
    for (int i = 0; i < 3; i++)
    {
        dap[1][i] = i * 3.0;
    }
    dap[2] = (double*)TMM_declare_var_s("double[4]");
    for (int i = 0; i < 4; i++)
    {
        dap[2][i] = i * 4.0;
    }
    dap[3] = (double*)TMM_declare_var_s("double[5]");
    for (int i = 0; i < 5; i++)
    {
        dap[3][i] = i * 5.0;
    }

    dpp = (double**)TMM_declare_var_s("double*[5]");
    for (int i = 0; i < 5; i++)
    {
        dpp[i] = (double*)TMM_declare_var_s("double");
        for (int j = 0; j < 5; j++)
        {
            dpp[i][j] = i * 10.0 + j;
        }
    }

    dppp = (double***)TMM_declare_var_s("double**[5]");
    for (int i = 0; i < 5; i++)
    {
        dppp[i] = (double**)TMM_declare_var_s("double*[5]");
        for (int j = 0; j < 5; j++)
        {
            dppp[i][j] = (double*)TMM_declare_var_s("double[5]");
            for (int k = 0; k < 5; k++)
            {
                dppp[i][j][k] = i * 100.0 + j * 10.0 + k;
            }
        }
    }

    swp = (SimpleWrapper**)TMM_declare_var_1d("SimpleWrapper*", 5);

    for (int i = 0; i < 5; i++)
    {
        swp[i]    = (SimpleWrapper*)TMM_declare_var_s("SimpleWrapper");
        swp[i]->a = 555;
        for (int j = 0; j < 10; j++)
        {
            swp[i]->vec.push_back(j);
        }
    }

    swpp = (SimpleWrapper***)TMM_declare_var_1d("SimpleWrapper**", 6);
    for (int i = 0; i < 6; i++)
    {
        swpp[i] = (SimpleWrapper**)TMM_declare_var_1d("SimpleWrapper*", 7);
        for (int j = 0; j < 7; j++)
        {
            swpp[i][j]    = (SimpleWrapper*)TMM_declare_var_s("SimpleWrapper");
            swpp[i][j]->a = 666;
            for (int k = 0; k < 10; k++)
            {
                swpp[i][j]->vec.push_back(k);
            }
        }
    }

    /* ========================== */
    // STL containers for testing
    double_vector.push_back(4.0);
    double_vector.push_back(5.0);
    double_vector.push_back(6.0);

    string_vector.push_back("It");
    string_vector.push_back("has");
    string_vector.push_back("the");
    string_vector.push_back("Wi-Fies");

    string_deque.push_back("Welcome");
    string_deque.push_back("to");
    string_deque.push_back("PhoneMart");

    float_deque.push_back(98.7);
    float_deque.push_back(65.4);
    float_deque.push_back(32.1);

    int_array[0] = 10;
    int_array[1] = 20;
    int_array[2] = 30;
    int_array[3] = 40;

    string_array[0] = "Alpha";
    string_array[1] = "Beta";
    string_array[2] = "Gamma";
    string_array[3] = "Delta";
    string_array[4] = "Epsilon";

    for (int i = 0; i < 10; i++)
    {
        UserClass temp;
        for (int j = 0; j < 5; j++)
        {
            temp.a[j] = i + j;
        }
        temp.b = 8888888888;
        temp.c = "Here is a test string";

        // We'll just make a pointer to the same stuff

        UserClass* user_class_ptr = (UserClass*)TMM_declare_var_s("UserClass");
        for (int j = 0; j < 5; j++)
        {
            user_class_ptr->a[j] = i + j;
        }
        user_class_ptr->b = 8888888888;
        user_class_ptr->c = "Here is a test string";

        temp.d = user_class_ptr;

        vec_user_simple.emplace_back(temp);
    }

    for (int i = 0; i < 10; i++)
    {
        SimpleWrapper temp_wrapper;
        temp_wrapper.a = 888;
        for (int j = i; j < i + 10; j++)
        {
            temp_wrapper.vec.push_back(j);
        }
        vec_user_defined.emplace_back(temp_wrapper);
    }

    for (int i = 0; i < 10; i++)
    {
        SimpleWrapper* temp_wrapper = (SimpleWrapper*)TMM_declare_var_s("SimpleWrapper");
        temp_wrapper->a             = 888;
        for (int j = i; j < i + 10; j++)
        {
            temp_wrapper->vec.push_back(j);
        }
        vec_user_defined_ptr.push_back(temp_wrapper);
    }

    return 0;
}
