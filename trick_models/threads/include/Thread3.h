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

/*
 * $Id: Thread3.h 1707 2011-06-20 16:50:19Z wwoodard $
 */

#ifndef THREAD_3_H_
#define THREAD_3_H_

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
