/*
PURPOSE: (Testing)
*/
#include <vector>
#include <stdlib.h>

typedef enum {
    JANUARY = 1, FEBRUARY = 2, MARCH, APRIL, MAY, JUNE, JULY, AUGUST, SEPTEMBER, OCTOBER, NOVEMBER, DECEMBER
    } MONTH;

typedef struct udt1 {
  double x;
  struct udt1 *udt_p;
  double *dbl_p;
  MONTH *month_p;
} UDT1;

typedef struct udt2 {
    char   pad[8];
    double starStarDbl ; /* ** */
} UDT2;

typedef struct udt3 {
    int a;
    int b;
} UDT3;

class UDT5 {
    public:
    double star_star;    /* -- trick_chkpnt_io(**) */
    double star_aye;     /* -- trick_chkpnt_io(*i) */
    double star_eau;     /* -- trick_chkpnt_io(*o) */
    double star_aye_eau; /* -- trick_chkpnt_io(*io) */
};

class UDT6 {
    public:
    char A[128][4];
    char B[128][4];
};

class UDT7 {
    public:
    double A;
    UDT3** udt3pp;
};

class VectorWrapper {
    public:
    int operator[] (int index) {
        return vec[index];            
    }

    void push_back(int val) {
        vec.push_back(val);
    }

    int size() {
        return vec.size();
    }

    private:
    std::vector<int> vec;
};
