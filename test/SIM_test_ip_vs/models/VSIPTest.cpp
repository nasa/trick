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
    color_p = (Color*)TMM_declare_var_s("Color[5]");
    for (int i = 0; i < 5; i++)
    {
        color_p[i] = static_cast<Color>(i % 3);
    }

    color_ap[0] = (Color*)TMM_declare_var_s("Color[2]");
    color_ap[1] = (Color*)TMM_declare_var_s("Color[3]");
    color_ap[2] = (Color*)TMM_declare_var_s("Color[4]");
    color_ap[3] = (Color*)TMM_declare_var_s("Color[5]");
    for (int i = 0; i < 2; i++)
    {
        color_ap[0][i] = static_cast<Color>(i % 3);
    }
    for (int i = 0; i < 3; i++)
    {
        color_ap[1][i] = static_cast<Color>(i % 3);
    }
    for (int i = 0; i < 4; i++)
    {
        color_ap[2][i] = static_cast<Color>(i % 3);
    }
    for (int i = 0; i < 5; i++)
    {
        color_ap[3][i] = static_cast<Color>(i % 3);
    }
    color_pp = (Color**)TMM_declare_var_s("Color*[3]");
    for (int i = 0; i < 3; i++)
    {
        color_pp[i] = (Color*)TMM_declare_var_s("Color[3]");
        for (int j = 0; j < 3; j++)
        {
            color_pp[i][j] = static_cast<Color>(j % 3);
        }
    }
    color_ppp = (Color***)TMM_declare_var_s("Color**[2]");
    for (int i = 0; i < 2; i++)
    {
        color_ppp[i] = (Color**)TMM_declare_var_s("Color*[2]");
        for (int j = 0; j < 2; j++)
        {
            color_ppp[i][j] = (Color*)TMM_declare_var_s("Color[2]");
            for (int k = 0; k < 2; k++)
            {
                color_ppp[i][j][k] = static_cast<Color>(k % 3);
            }
        }
    }

    sp = (short*)TMM_declare_var_s("short[5]");
    for (int i = 0; i < 5; i++)
    {
        sp[i] = -static_cast<short>(i);
    }

    sap[0] = (short*)TMM_declare_var_s("short[2]");
    for (int i = 0; i < 2; i++)
    {
        sap[0][i] = -static_cast<short>(i);
    }
    sap[1] = (short*)TMM_declare_var_s("short[3]");
    for (int i = 0; i < 3; i++)
    {
        sap[1][i] = i;
    }
    sap[2] = (short*)TMM_declare_var_s("short[4]");
    for (int i = 0; i < 4; i++)
    {
        sap[2][i] = i;
    }
    sap[3] = (short*)TMM_declare_var_s("short[5]");
    for (int i = 0; i < 5; i++)
    {
        sap[3][i] = i;
    }
    spp = (short**)TMM_declare_var_s("short*[3]");
    for (int i = 0; i < 3; i++)
    {
        spp[i] = (short*)TMM_declare_var_s("short[3]");
        for (int j = 0; j < 3; j++)
        {
            spp[i][j] = j;
        }
    }
    sppp = (short***)TMM_declare_var_s("short**[2]");
    for (int i = 0; i < 2; i++)
    {
        sppp[i] = (short**)TMM_declare_var_s("short*[2]");
        for (int j = 0; j < 2; j++)
        {
            sppp[i][j] = (short*)TMM_declare_var_s("short[2]");
            for (int k = 0; k < 2; k++)
            {
                sppp[i][j][k] = k;
            }
        }
    }

    usp = (unsigned short*)TMM_declare_var_s("unsigned short[5]");
    for (int i = 0; i < 5; i++)
    {
        usp[i] = i;
    }
    usap[0] = (unsigned short*)TMM_declare_var_s("unsigned short[2]");
    for (int i = 0; i < 2; i++)
    {
        usap[0][i] = i;
    }
    usap[1] = (unsigned short*)TMM_declare_var_s("unsigned short[3]");
    for (int i = 0; i < 3; i++)
    {
        usap[1][i] = i;
    }
    usap[2] = (unsigned short*)TMM_declare_var_s("unsigned short[4]");
    for (int i = 0; i < 4; i++)
    {
        usap[2][i] = i;
    }
    usap[3] = (unsigned short*)TMM_declare_var_s("unsigned short[5]");
    for (int i = 0; i < 5; i++)
    {
        usap[3][i] = i;
    }
    uspp = (unsigned short**)TMM_declare_var_s("unsigned short*[3]");
    for (int i = 0; i < 3; i++)
    {
        uspp[i] = (unsigned short*)TMM_declare_var_s("unsigned short[3]");
        for (int j = 0; j < 3; j++)
        {
            uspp[i][j] = j;
        }
    }
    usppp = (unsigned short***)TMM_declare_var_s("unsigned short**[2]");
    for (int i = 0; i < 2; i++)
    {
        usppp[i] = (unsigned short**)TMM_declare_var_s("unsigned short*[2]");
        for (int j = 0; j < 2; j++)
        {
            usppp[i][j] = (unsigned short*)TMM_declare_var_s("unsigned short[2]");
            for (int k = 0; k < 2; k++)
            {
                usppp[i][j][k] = k;
            }
        }
    }

    ip = (int*)TMM_declare_var_s("int[5]");
    for (int i = 0; i < 5; i++)
    {
        ip[i] = -i;
    }
    iap[0] = (int*)TMM_declare_var_s("int[2]");
    for (int i = 0; i < 2; i++)
    {
        iap[0][i] = -i;
    }
    iap[1] = (int*)TMM_declare_var_s("int[3]");
    for (int i = 0; i < 3; i++)
    {
        iap[1][i] = i;
    }
    iap[2] = (int*)TMM_declare_var_s("int[4]");
    for (int i = 0; i < 4; i++)
    {
        iap[2][i] = i;
    }
    iap[3] = (int*)TMM_declare_var_s("int[5]");
    for (int i = 0; i < 5; i++)
    {
        iap[3][i] = i;
    }
    ipp = (int**)TMM_declare_var_s("int*[3]");
    for (int i = 0; i < 3; i++)
    {
        ipp[i] = (int*)TMM_declare_var_s("int[3]");
        for (int j = 0; j < 3; j++)
        {
            ipp[i][j] = j;
        }
    }
    ippp = (int***)TMM_declare_var_s("int**[2]");
    for (int i = 0; i < 2; i++)
    {
        ippp[i] = (int**)TMM_declare_var_s("int*[2]");
        for (int j = 0; j < 2; j++)
        {
            ippp[i][j] = (int*)TMM_declare_var_s("int[2]");
            for (int k = 0; k < 2; k++)
            {
                ippp[i][j][k] = k;
            }
        }
    }

    uip = (unsigned int*)TMM_declare_var_s("unsigned int[5]");
    for (int i = 0; i < 5; i++)
    {
        uip[i] = i;
    }
    uiap[0] = (unsigned int*)TMM_declare_var_s("unsigned int[2]");
    for (int i = 0; i < 2; i++)
    {
        uiap[0][i] = i;
    }
    uiap[1] = (unsigned int*)TMM_declare_var_s("unsigned int[3]");
    for (int i = 0; i < 3; i++)
    {
        uiap[1][i] = i;
    }
    uiap[2] = (unsigned int*)TMM_declare_var_s("unsigned int[4]");
    for (int i = 0; i < 4; i++)
    {
        uiap[2][i] = i;
    }
    uiap[3] = (unsigned int*)TMM_declare_var_s("unsigned int[5]");
    for (int i = 0; i < 5; i++)
    {
        uiap[3][i] = i;
    }
    uipp = (unsigned int**)TMM_declare_var_s("unsigned int*[3]");
    for (int i = 0; i < 3; i++)
    {
        uipp[i] = (unsigned int*)TMM_declare_var_s("unsigned int[3]");
        for (int j = 0; j < 3; j++)
        {
            uipp[i][j] = j;
        }
    }
    uippp = (unsigned int***)TMM_declare_var_s("unsigned int**[2]");
    for (int i = 0; i < 2; i++)
    {
        uippp[i] = (unsigned int**)TMM_declare_var_s("unsigned int*[2]");
        for (int j = 0; j < 2; j++)
        {
            uippp[i][j] = (unsigned int*)TMM_declare_var_s("unsigned int[2]");
            for (int k = 0; k < 2; k++)
            {
                uippp[i][j][k] = k;
            }
        }
    }

    lp = (long*)TMM_declare_var_s("long[5]");
    for (int i = 0; i < 5; i++)
    {
        lp[i] = -i;
    }
    lap[0] = (long*)TMM_declare_var_s("long[2]");
    for (int i = 0; i < 2; i++)
    {
        lap[0][i] = -i;
    }
    lap[1] = (long*)TMM_declare_var_s("long[3]");
    for (int i = 0; i < 3; i++)
    {
        lap[1][i] = i;
    }
    lap[2] = (long*)TMM_declare_var_s("long[4]");
    for (int i = 0; i < 4; i++)
    {
        lap[2][i] = i;
    }
    lap[3] = (long*)TMM_declare_var_s("long[5]");
    for (int i = 0; i < 5; i++)
    {
        lap[3][i] = i;
    }
    lpp = (long**)TMM_declare_var_s("long*[3]");
    for (int i = 0; i < 3; i++)
    {
        lpp[i] = (long*)TMM_declare_var_s("long[3]");
        for (int j = 0; j < 3; j++)
        {
            lpp[i][j] = j;
        }
    }
    lppp = (long***)TMM_declare_var_s("long**[2]");
    for (int i = 0; i < 2; i++)
    {
        lppp[i] = (long**)TMM_declare_var_s("long*[2]");
        for (int j = 0; j < 2; j++)
        {
            lppp[i][j] = (long*)TMM_declare_var_s("long[2]");
            for (int k = 0; k < 2; k++)
            {
                lppp[i][j][k] = k;
            }
        }
    }

    ulp = (unsigned long*)TMM_declare_var_s("unsigned long[5]");
    for (int i = 0; i < 5; i++)
    {
        ulp[i] = i;
    }
    ulap[0] = (unsigned long*)TMM_declare_var_s("unsigned long[2]");
    for (int i = 0; i < 2; i++)
    {
        ulap[0][i] = i;
    }
    ulap[1] = (unsigned long*)TMM_declare_var_s("unsigned long[3]");
    for (int i = 0; i < 3; i++)
    {
        ulap[1][i] = i;
    }
    ulap[2] = (unsigned long*)TMM_declare_var_s("unsigned long[4]");
    for (int i = 0; i < 4; i++)
    {
        ulap[2][i] = i;
    }
    ulap[3] = (unsigned long*)TMM_declare_var_s("unsigned long[5]");
    for (int i = 0; i < 5; i++)
    {
        ulap[3][i] = i;
    }
    ulpp = (unsigned long**)TMM_declare_var_s("unsigned long*[3]");
    for (int i = 0; i < 3; i++)
    {
        ulpp[i] = (unsigned long*)TMM_declare_var_s("unsigned long[3]");
        for (int j = 0; j < 3; j++)
        {
            ulpp[i][j] = j;
        }
    }
    ulppp = (unsigned long***)TMM_declare_var_s("unsigned long**[2]");
    for (int i = 0; i < 2; i++)
    {
        ulppp[i] = (unsigned long**)TMM_declare_var_s("unsigned long*[2]");
        for (int j = 0; j < 2; j++)
        {
            ulppp[i][j] = (unsigned long*)TMM_declare_var_s("unsigned long[2]");
            for (int k = 0; k < 2; k++)
            {
                ulppp[i][j][k] = k;
            }
        }
    }

    llp = (long long*)TMM_declare_var_s("long long[5]");
    for (int i = 0; i < 5; i++)
    {
        llp[i] = -i;
    }
    llap[0] = (long long*)TMM_declare_var_s("long long[2]");
    for (int i = 0; i < 2; i++)
    {
        llap[0][i] = -i;
    }
    llap[1] = (long long*)TMM_declare_var_s("long long[3]");
    for (int i = 0; i < 3; i++)
    {
        llap[1][i] = i;
    }
    llap[2] = (long long*)TMM_declare_var_s("long long[4]");
    for (int i = 0; i < 4; i++)
    {
        llap[2][i] = i;
    }
    llap[3] = (long long*)TMM_declare_var_s("long long[5]");
    for (int i = 0; i < 5; i++)
    {
        llap[3][i] = i;
    }
    llpp = (long long**)TMM_declare_var_s("long long*[3]");
    for (int i = 0; i < 3; i++)
    {
        llpp[i] = (long long*)TMM_declare_var_s("long long[3]");
        for (int j = 0; j < 3; j++)
        {
            llpp[i][j] = j;
        }
    }
    llppp = (long long***)TMM_declare_var_s("long long**[2]");
    for (int i = 0; i < 2; i++)
    {
        llppp[i] = (long long**)TMM_declare_var_s("long long*[2]");
        for (int j = 0; j < 2; j++)
        {
            llppp[i][j] = (long long*)TMM_declare_var_s("long long[2]");
            for (int k = 0; k < 2; k++)
            {
                llppp[i][j][k] = k;
            }
        }
    }

    ullp = (unsigned long long*)TMM_declare_var_s("unsigned long long[5]");
    for (int i = 0; i < 5; i++)
    {
        ullp[i] = i;
    }
    ullap[0] = (unsigned long long*)TMM_declare_var_s("unsigned long long[2]");
    for (int i = 0; i < 2; i++)
    {
        ullap[0][i] = i;
    }
    ullap[1] = (unsigned long long*)TMM_declare_var_s("unsigned long long[3]");
    for (int i = 0; i < 3; i++)
    {
        ullap[1][i] = i;
    }
    ullap[2] = (unsigned long long*)TMM_declare_var_s("unsigned long long[4]");
    for (int i = 0; i < 4; i++)
    {
        ullap[2][i] = i;
    }
    ullap[3] = (unsigned long long*)TMM_declare_var_s("unsigned long long[5]");
    for (int i = 0; i < 5; i++)
    {
        ullap[3][i] = i;
    }
    ullpp = (unsigned long long**)TMM_declare_var_s("unsigned long long*[3]");
    for (int i = 0; i < 3; i++)
    {
        ullpp[i] = (unsigned long long*)TMM_declare_var_s("unsigned long long[3]");
        for (int j = 0; j < 3; j++)
        {
            ullpp[i][j] = j;
        }
    }
    ullppp = (unsigned long long***)TMM_declare_var_s("unsigned long long**[2]");
    for (int i = 0; i < 2; i++)
    {
        ullppp[i] = (unsigned long long**)TMM_declare_var_s("unsigned long long*[2]");
        for (int j = 0; j < 2; j++)
        {
            ullppp[i][j] = (unsigned long long*)TMM_declare_var_s("unsigned long long[2]");
            for (int k = 0; k < 2; k++)
            {
                ullppp[i][j][k] = k;
            }
        }
    }

    fp = (float*)TMM_declare_var_s("float[5]");
    for (int i = 0; i < 5; i++)
    {
        fp[i] = i * 1.0f;
    }
    fap[0] = (float*)TMM_declare_var_s("float[2]");
    for (int i = 0; i < 2; i++)
    {
        fap[0][i] = i * 2.0f;
    }
    fap[1] = (float*)TMM_declare_var_s("float[3]");
    for (int i = 0; i < 3; i++)
    {
        fap[1][i] = i * 3.0f;
    }
    fap[2] = (float*)TMM_declare_var_s("float[4]");
    for (int i = 0; i < 4; i++)
    {
        fap[2][i] = i * 4.0f;
    }
    fap[3] = (float*)TMM_declare_var_s("float[5]");
    for (int i = 0; i < 5; i++)
    {
        fap[3][i] = i * 5.0f;
    }
    fpp = (float**)TMM_declare_var_s("float*[2]");
    for (int i = 0; i < 2; i++)
    {
        fpp[i] = (float*)TMM_declare_var_s("float[2]");
        for (int j = 0; j < 2; j++)
        {
            fpp[i][j] = i * 1.0f + j;
        }
    }
    fppp = (float***)TMM_declare_var_s("float**[2]");
    for (int i = 0; i < 2; i++)
    {
        fppp[i] = (float**)TMM_declare_var_s("float*[2]");
        for (int j = 0; j < 2; j++)
        {
            fppp[i][j] = (float*)TMM_declare_var_s("float[2]");
            for (int k = 0; k < 2; k++)
            {
                fppp[i][j][k] = i * 1.0f + j + k;
            }
        }
    }

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
        dpp[i] = (double*)TMM_declare_var_s("double[5]");
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

    bp = (bool*)TMM_declare_var_s("bool[5]");
    for (int i = 0; i < 5; i++)
    {
        bp[i] = (i % 2 == 0);
    }
    bap[0] = (bool*)TMM_declare_var_s("bool[2]");
    for (int i = 0; i < 2; i++)
    {
        bap[0][i] = (i % 2 == 0);
    }
    bap[1] = (bool*)TMM_declare_var_s("bool[3]");
    for (int i = 0; i < 3; i++)
    {
        bap[1][i] = (i % 2 == 0);
    }
    bap[2] = (bool*)TMM_declare_var_s("bool[4]");
    for (int i = 0; i < 4; i++)
    {
        bap[2][i] = (i % 2 == 0);
    }
    bap[3] = (bool*)TMM_declare_var_s("bool[5]");
    for (int i = 0; i < 5; i++)
    {
        bap[3][i] = (i % 2 == 0);
    }
    bpp = (bool**)TMM_declare_var_s("bool*[5]");
    for (int i = 0; i < 5; i++)
    {
        bpp[i] = (bool*)TMM_declare_var_s("bool[5]");
        for (int j = 0; j < 5; j++)
        {
            bpp[i][j] = (j % 2 == 0);
        }
    }
    bppp = (bool***)TMM_declare_var_s("bool**[5]");
    for (int i = 0; i < 5; i++)
    {
        bppp[i] = (bool**)TMM_declare_var_s("bool*[5]");
        for (int j = 0; j < 5; j++)
        {
            bppp[i][j] = (bool*)TMM_declare_var_s("bool[5]");
            for (int k = 0; k < 5; k++)
            {
                bppp[i][j][k] = (k % 2 == 0);
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
    color_vector.push_back(Color::RED);
    color_vector.push_back(Color::GREEN);
    color_vector.push_back(Color::BLUE);

    short_vector.push_back(-1);
    short_vector.push_back(2);
    short_vector.push_back(3);

    unsigned_short_vector.push_back(4);
    unsigned_short_vector.push_back(5);
    unsigned_short_vector.push_back(6);

    int_vector.push_back(-7);
    int_vector.push_back(8);
    int_vector.push_back(9);

    unsigned_int_vector.push_back(10);
    unsigned_int_vector.push_back(11);
    unsigned_int_vector.push_back(12);

    long_vector.push_back(-13);
    long_vector.push_back(14);
    long_vector.push_back(15);

    unsigned_long_vector.push_back(16);
    unsigned_long_vector.push_back(17);
    unsigned_long_vector.push_back(18);

    long_long_vector.push_back(-19);
    long_long_vector.push_back(20);
    long_long_vector.push_back(21);

    unsigned_long_long_vector.push_back(22);
    unsigned_long_long_vector.push_back(23);
    unsigned_long_long_vector.push_back(24);

    float_vector.push_back(25.0);
    float_vector.push_back(26.0);
    float_vector.push_back(27.0);

    bool_vector.push_back(true);
    bool_vector.push_back(false);
    bool_vector.push_back(true);

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
