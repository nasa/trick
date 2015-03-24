/**
@file

@verbatim
PURPOSE:
    (Test)
LANGUAGE:
    (C++)
LIBRARY DEPENDENCY:
    ((Thread3.o))
@endverbatim
*******************************************************************************/

#ifndef THREAD_3_H
#define THREAD_3_H

class Thread3
{
    public:
        Thread3();
        ~Thread3();
        int Step();
    /*private:*/
        bool m_bBusy;
        int m_iBusyNSecs;
        int m_iCount;
};

#endif
