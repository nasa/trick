/*
PURPOSE: (Testing)
*/
#include <stddef.h>

class CountMe
{
public:
    int a;
    static size_t count;

    CountMe()
    {
        count++;
    }

    ~CountMe()
    {
        count--;
    }
};
