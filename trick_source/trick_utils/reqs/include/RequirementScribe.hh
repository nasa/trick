/****************************************************
PURPOSE: ()
**************************************************/

#ifndef REQUIREMENTSCRIBE_HH
#define REQUIREMENTSCRIBE_HH

#include <string>
#include <map>
#include "gtest/gtest.h"
#include "sim_services/UnitTest/include/UnitTest.hh"

namespace Trick {

class RequirementScribe {

    public:
        Trick::UnitTest * the_unit_test_output ;
        static std::map< std::string , unsigned int > num_reqs ;

        RequirementScribe() {}
        ~RequirementScribe() {}

        //Googletest unit tests
        void add_requirement( std::string par_num ) {
            ::testing::Test::RecordProperty("parent", par_num.c_str() ) ;
        }

        //Trick unit tests
        void trick_add_parent(std::string in_test_suite_name, std::string in_test_case, std::string par_num) {
            the_unit_test_output->add_test_requirements(in_test_suite_name, in_test_case, par_num);
        }
};

}

#endif
