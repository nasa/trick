import math
from trick.unit_test import *

print(f"alloc_test.atwargs = {alloc_test}")

#TRICK_EXPECT_EQ(alloc_test.atwargs.some_int, 0)
#TRICK_EXPECT_NEAR(alloc_test.atwargs.some_double, 0, 1e-6)

trick.add_read(4.0, """TRICK_EXPECT_EQ(alloc_test.atwargs.some_int, 0)""")
trick.add_read(4.0, """TRICK_EXPECT_NEAR(alloc_test.atwargs.some_double, 0, 1e-6)""")
trick.stop(5.0)