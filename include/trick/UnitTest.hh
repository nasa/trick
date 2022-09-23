/*
    PURPOSE:
        (Output test results in JUnit format)
*/

#ifndef UNITTEST_HH
#define UNITTEST_HH

#include <string>
#include <fstream>
#include <vector>
#include <map>

namespace Trick {

    /**
     * This class gathers test results during a simulation and writes them to disk following
     * the JUint xml format.
     *
     * @author Alexander S. Lin
     *
     * @date Oct. 2010
     *
     */
    class TestCase {
        public:
            std::string name ;
            std::string status ;
			std::string parent ;
            double elapsed_time ;
            std::string failure_string ;

            TestCase(std::string test_case_name , std::string in_failure_string) ;
    } ;

    class TestSuite {
        public:
            unsigned int num_failures ;
            std::vector <Trick::TestCase *> test_results ;

            TestSuite() ;

            int add_test_results( std::string test_case , std::string failure_string = "") ;
			int add_test_requirements(std::string test_case , std::string par_num) ;
            void delete_test_results() ;

    } ;

    class UnitTest {

        public:

            /** Create test xml output.\n*/
            bool enabled ; /**< trick_units(--) */

            /** Send the unit test exit code up the chain to Executive.\n*/
            bool exit_code_enabled ; /**< trick_units(--) */

            /** Name of Unit test\n*/
            std::string name ;

            /** The file name of a file which the messages goes to. \n*/
            std::string file_name ; /**< trick_units(--) trick_io(*i) */

            /** Keeps lists of test results keyed by test_suite name.\n*/
            std::map< std::string , TestSuite > test_suites ; // ** ignore

            /**
             @brief The constructor.
             */
            UnitTest() ;

            /**
             @brief The destructor.
             */
            ~UnitTest() ;

            /**
             @brief Enable test output.
             */
            bool enable() ;

            /**
             @brief Enable/Disable exit code return feature.
             @return always 0
             */
            int set_exit_code_enabled( bool in_enable ) ;

            /**
             @brief Output message to the file.
             */
            int add_test_results( std::string test_suite_name , std::string test_case , std::string failure_string = "") ;

			int add_test_requirements(std::string in_test_suite_name, std::string in_test_case, std::string par_num);

            /**
             @brief Set the test name
             @return always 0
             */
            int set_test_name(std::string in_name) ;

            /**
             @brief Set a file name
             @return always 0
             */
            int set_file_name(std::string in_name) ;

            /**
             @brief Write output to xml file.
             @return always 0
             */
            int write_output() ;
    } ;

}

#endif

