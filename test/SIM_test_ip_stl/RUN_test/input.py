import trick

from trick.unit_test import *
trick_utest.unit_tests.enable()
trick_utest.unit_tests.set_file_name( os.getenv("TRICK_HOME") + "/trick_test/SIM_test_ip_stl.xml" )
trick_utest.unit_tests.set_test_name( "InputFileSTLTests" )



test_suite = "STL input tests"

from RUN_test.test_arrays import *
from RUN_test.test_vectors import *

array_tests(stl_so, test_suite)
vector_tests(stl_so, test_suite)


trick.stop(10)

