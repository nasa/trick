/**
@file

@verbatim
PURPOSE:
    (Template tests)
@endverbatim
*******************************************************************************/

#ifndef TEMPLATETEST_HH
#define TEMPLATETEST_HH

#include <iostream>

namespace TT_NS
{

    template <class A, int B> class TemplateTest
    {
        public:
            TemplateTest()
            {
                for (int i = 0; i < B; ++i)
                {
                    aa[i] = i;
                }
            }

            void printout()
            {
                for (int i = 0; i < B; ++i)
                {
                    std::cout << aa[i] << "\n";
                }
            }

            A aa[B];
    };

}

#ifdef SWIG
%struct_str(TemplateTest)
#endif

#endif /* TEMPLATETEST_HH */
