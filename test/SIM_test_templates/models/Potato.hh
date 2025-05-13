/**
@file

@verbatim
PURPOSE:
    (Template tests)
@endverbatim
*******************************************************************************/

#ifndef POTATO_HH
#define POTATO_HH

namespace potato {
    template <class T>
    class Potato {
        public:
        class Inner {}; 
        Inner inner;
    };
}

#endif /* POTATO_HH */
