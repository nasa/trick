/*
PURPOSE: (Testing)
*/

struct UDT1
{
    double x;
    UDT1 * udt_p;
    double * dbl_p;
};

struct UDT2
{
    UDT1 udt1_var;
    double x;
    UDT1 * udt_p;
    double * dbl_p;
};

struct UDT3
{
    int & i_r;
    bool bl;
    double & dbl_r;

    UDT3() = delete;
};

struct UDT4
{
    UDT3 & udt3_ref;
    bool * bl_p;
    UDT2 & udt2_ref;
    double dbl;

    UDT4() = delete;
};

struct UDT5
{
    double y;
    UDT2 udt2_2d_arr[3][4];
};
