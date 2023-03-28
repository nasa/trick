/*
PURPOSE: (Testing)
*/
#include <vector>
#include <string>

typedef struct {
      signed int signedBF   :5;
    unsigned int unsignedBF :5;
    int c :7;
    int d :8;
    int e :9;
} BF_FLAGS;

typedef struct {
  double x;
  double y;
  double z;
} UDT1;

class Foo {
    public:
    int a;
    double b;
    std::vector<int> c;
};

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