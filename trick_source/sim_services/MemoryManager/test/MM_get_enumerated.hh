/*
PURPOSE: (Testing)
*/

typedef enum {
    JANUARY = 1, FEBRUARY = 2, MARCH, APRIL, MAY, JUNE, JULY, AUGUST, SEPTEMBER, OCTOBER, NOVEMBER, DECEMBER
    } MONTH;


typedef enum { ONE = 1, TWO = 2, THREE } MyEnum ;

template <typename T>
class MyTemplateClass {
    public:
    T num;
    MyEnum aaa;
};

class SomeClass {
    public:
    MyTemplateClass<double> foo;

};
