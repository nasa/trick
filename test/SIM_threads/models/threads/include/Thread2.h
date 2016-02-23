/**
@file

@verbatim
PURPOSE:
    (Test)
LANGUAGE:
    (C++)
LIBRARY DEPENDENCY:
    ((Thread2.o))
@endverbatim
*******************************************************************************/

#ifndef THREAD_2_H
#define THREAD_2_H

class Thread2
{
    public:
        Thread2();
        ~Thread2();
        int Step();
    /*private:*/
        bool m_bBusy;
        int m_iBusyNSecs;
        int m_iCount;
};

#endif
