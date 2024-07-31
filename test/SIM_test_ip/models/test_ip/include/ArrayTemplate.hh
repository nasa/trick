/**
@file

@verbatim
PURPOSE:
    (Test if we can build with arrays as template parameters)
@endverbatim
*******************************************************************************/

#ifndef ARRAY_TEMPLATE_TESTS_HH
#define ARRAY_TEMPLATE_TESTS_HH

// System include files.
#include <string>
#include <iostream>
#include <sstream>


template <class SourceType>
class ArrayTemplate
{
    public:
        ArrayTemplate(const SourceType& source)
        :  source(source)
        {
        }

        ArrayTemplate(const ArrayTemplate&) = delete;
        ArrayTemplate& operator=(const ArrayTemplate&) = delete;

    private:
        const SourceType& source;
};


#endif 

