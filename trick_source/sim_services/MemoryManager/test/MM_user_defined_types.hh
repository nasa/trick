/*
PURPOSE: (Testing)
*/
#include <string>

typedef struct {
    unsigned int a :3;
    unsigned int b :5;
    unsigned int c :7;
    unsigned int d :8;
    unsigned int e :9;
    bool a_boolean;
} FLAGS;

typedef enum {
    APPLE       = 0,
    APRICOT     = 1,
    BANANA      = 2,
    BLACKBERRY  = 3,
    BLUEBERRY   = 4,
    CANTALOPE   = 5,
    CHERRY      = 6,
    CRANBERRY   = 7,
    FIG         = 8,
    GRAPE       = 9,
    GRAPEFRUIT  = 10,
    GUAVA       = 11,
    KIWIFRUIT   = 12,
    LEMON       = 13,
    LIME        = 14,
    MANGO       = 15,
    NECTARINE   = 16,
    ORANGE      = 17,
    PAPAYA      = 18,
    PEACH       = 19,
    PEAR        = 20,
    PERSIMMON   = 21,
    PINEAPPLE   = 22,
    PLUM        = 23,
    RASPBERRY   = 24,
    STRAWBERRY  = 25,
    TANGERINE   = 26,
    WATERMELON  = 27
} FRUIT ;

typedef enum {
    RED = 1,
    BLUE = 2,
    GREEN = 3
} RGB ;

typedef struct {
  double x;
  double y;
  double z;
} UDT1;

class UDT2 {
    public:
    double A;
    double B;
    double C;
    const char* ss;
    UDT1   udt1;
    UDT1*  udt1_p;
};

class UDT3 {
    public:
    double X;
    double Y;
    double Z;
    int    I;
    double M2[3][4];    /* rad */
    double M3[2][3][4]; /* degree */
    char   C[10];
    UDT2   N;
    UDT2   NA[2];
    UDT2*  udt2_p;
    UDT1*  udt1_p;
    double starstar;    /* ** */
    std::string cppstr;
};

class UDT4 : public UDT3 {
    public:
    double XX;
};

class UDT7 {
    public:
    char A[32][4];
    char B[32][4];
};

