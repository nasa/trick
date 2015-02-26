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

/*
 * $Id: Thread1.h 2200 2012-02-23 22:26:02Z alin $
 */

#ifndef THREAD_1_H_
#define THREAD_1_H_

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
