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

/*
 * $Id: Thread2.h 1707 2011-06-20 16:50:19Z wwoodard $
 */

#ifndef THREAD_2_H_
#define THREAD_2_H_

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
