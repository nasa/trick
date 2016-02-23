/**
@file

@verbatim
PURPOSE:
    (Test)
LANGUAGE:
    (C++)
LIBRARY DEPENDENCY:
    ((Thread1.o))
@endverbatim
*******************************************************************************/

#ifndef THREAD_1_H
#define THREAD_1_H

class Thread1
{
    public:
        Thread1(int in_usec);
        ~Thread1();
        int Step();
    /*private:*/
        bool m_bBusy;
        int m_iBusyUSecs;
        int m_iCount;
};

#endif
