/*
PURPOSE: (Testing)
*/

typedef struct udt1 {
    double x;
    struct udt1 *udt_p;
    double *dbl_p;
} UDT1;

typedef struct udt2 {
    UDT1   udt1_var;
    double x;
    struct udt1 *udt_p;
    double *dbl_p;
} UDT2;
